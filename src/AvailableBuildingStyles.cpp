/*
 * This file is part of sc4-more-building-styles, a DLL Plugin for
 * SimCity 4 that adds support for more building styles.
 *
 * Copyright (C) 2024, 2025 Nicholas Hayes
 *
 * sc4-more-building-styles is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * sc4-more-building-styles is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with sc4-more-building-styles.
 * If not, see <http://www.gnu.org/licenses/>.
 */

#include "AvailableBuildingStyles.h"
#include "BuildingStyleButtons.h"
#include "BuildingStyleWinUtil.h"
#include "cGZPersistResourceKey.h"
#include "cIGZLanguageManager.h"
#include "cIGZPersistResourceManager.h"
#include "cIGZWinBtn.h"
#include "cISC4View3DWin.h"
#include "cRZAutoRefCount.h"
#include "GZServPtrs.h"
#include "GZWinUtil.h"
#include "MaxisBuildingStyleUIControlIDs.h"
#include "ReservedStyleIDs.h"
#include "StringResourceKey.h"
#include "StringResourceManager.h"
#include <algorithm>

namespace
{
	typedef void(*BuildingStyleWinEnumCallback)(uint32_t buttonID, cIGZWinBtn* pBtn, void* pState);

	struct BuildingStyleWinEnumContext
	{
		BuildingStyleWinEnumCallback callback;
		void* callbackState;
	};

	bool BuildingStyleWinEnumProc(cIGZWin* parent, uint32_t childID, void* child, void* pState)
	{
		// The Maxis UI control ids, optional UI control ids, and PIM-X placeholder id are excluded,
		// every other button in the dialog is a style radio button.

		if (MaxisUIControlIDs.count(childID) == 0
			&& OptionalButtonIDs.count(childID) == 0
			&& childID != PIMXPlaceholderStyleID)
		{
			BuildingStyleWinEnumContext* state = static_cast<BuildingStyleWinEnumContext*>(pState);

			state->callback(childID, static_cast<cIGZWinBtn*>(child), state->callbackState);
		}

		return true;
	}

	void ConfigureStyleControlReservedButtons(uint32_t buttonID, cIGZWinBtn* pBtn)
	{
		bool hideCheckBox = true;

		switch (buttonID)
		{
		case KickOutLowerWealthButtonID:
		case NoKickOutLowerWealthButtonID:
		case DisableLotAggregationButtonID:
		case DisableLotSubdivisionButtonID:
		case PreventCrossStyleRedevelopmentButtonID:
			hideCheckBox = false;
			break;
		}

		if (hideCheckBox)
		{
			GZWinUtil::SetWindowVisible(pBtn->AsIGZWin(), false);
		}
	}

	void SetStyleRadioButtonToolTip(
		cIGZWinBtn* pBtn,
		uint32_t styleID,
		cIGZLanguageManager* pLM,
		cIGZPersistResourceManager* pRM)
	{
		constexpr uint32_t BuildingStyleLTEXTGroupID = 0x3EE5B610;

		const StringResourceKey key(BuildingStyleLTEXTGroupID, styleID);

		cRZAutoRefCount<cIGZString> toolTipText;

		if (StringResourceManager::GetLocalizedString(pLM, pRM, key, toolTipText))
		{
			pBtn->SetTipText(*toolTipText);
		}
	}

	void SetStyleRadioButtonToolTip(
		cIGZWinBtn* pBtn,
		const DefinedBuildingStyleEntry& entry,
		cIGZLanguageManager* pLM,
		cIGZPersistResourceManager* pRM)
	{
		const StringResourceKey& key = entry.toolTipKey;

		if (key.groupID != 0 && key.instanceID != 0)
		{
			cRZAutoRefCount<cIGZString> toolTipText;

			if (StringResourceManager::GetLocalizedString(pLM, pRM, key, toolTipText))
			{
				pBtn->SetTipText(*toolTipText);
			}
		}
		else
		{
			SetStyleRadioButtonToolTip(pBtn, entry.styleID, pLM, pRM);
		}		
	}

	struct InitializeBuildingStyleContext
	{
		const std::unordered_map<uint32_t, DefinedBuildingStyleEntry>& styles;
		BuildingStyleCollection availableBuildingStyles;
		cIGZLanguageManagerPtr languageManager;
		cIGZPersistResourceManagerPtr resourceManager;

		InitializeBuildingStyleContext(const DefinedBuildingStyles& definedBuildingStyles)
			: styles(definedBuildingStyles.GetStyles()),
			  availableBuildingStyles(),
			  languageManager(),
			  resourceManager()
		{
		}
	};

	void InitializeBuildingStylesCallback(uint32_t childID, cIGZWinBtn* pBtn, void* pState)
	{
		InitializeBuildingStyleContext* state = static_cast<InitializeBuildingStyleContext*>(pState);

		if (childID <= BuildingStyleMappedCheckBoxMaxButtonID)
		{
			const auto& styles = state->styles;

			const auto item = styles.find(childID);

			if (item != styles.end())
			{
				const DefinedBuildingStyleEntry& entry = item->second;

				if (entry.styleID != DefinedBuildingStyleEntry::InvalidStyleID)
				{
					state->availableBuildingStyles.insert(item->first, entry.styleID, entry.styleName);
					SetStyleRadioButtonToolTip(pBtn, entry, state->languageManager, state->resourceManager);
				}
				else
				{
					// The check box is a placeholder, visible but disabled.
					GZWinUtil::SetWindowEnabled(pBtn->AsIGZWin(), false);
				}

				pBtn->SetCaption(entry.styleName);

				constexpr uint32_t LedgerHeaderFontStyle = 0xE9C86B5A;
				constexpr uint32_t DefaultFontStyle = 0x68963C4C;

				pBtn->SetFontStyle(entry.boldText ? LedgerHeaderFontStyle : DefaultFontStyle);
			}
			else
			{
				GZWinUtil::SetWindowVisible(pBtn->AsIGZWin(), false);
			}
		}
		else if (childID >= StyleControlReservedButtonRangeStart && childID <= StyleControlReservedButtonRangeEnd)
		{
			ConfigureStyleControlReservedButtons(childID, pBtn);
		}
		else
		{
			cIGZString* caption = pBtn->GetCaption();

			state->availableBuildingStyles.insert(childID, childID, *caption);
			SetStyleRadioButtonToolTip(pBtn, childID, state->languageManager, state->resourceManager);
		}
	}

	struct UpdateAutomaticCheckBoxContext
	{
		const std::unordered_map<uint32_t, DefinedBuildingStyleEntry>& styles;
		cIGZLanguageManagerPtr languageManager;
		cIGZPersistResourceManagerPtr resourceManager;

		UpdateAutomaticCheckBoxContext(const DefinedBuildingStyles& definedBuildingStyles)
			: styles(definedBuildingStyles.GetStyles()),
			  languageManager(),
			  resourceManager()
		{
		}
	};

	void UpdateAutomaticCheckBoxNamesCallback(uint32_t childID, cIGZWinBtn* pBtn, void* pState)
	{
		if (childID <= BuildingStyleMappedCheckBoxMaxButtonID)
		{
			const UpdateAutomaticCheckBoxContext* state = static_cast<UpdateAutomaticCheckBoxContext*>(pState);
			const auto& styles = state->styles;

			const auto item = styles.find(childID);

			if (item != styles.end())
			{
				const DefinedBuildingStyleEntry& entry = item->second;

				if (entry.styleID != DefinedBuildingStyleEntry::InvalidStyleID)
				{
					SetStyleRadioButtonToolTip(pBtn, entry, state->languageManager, state->resourceManager);
				}
				else
				{
					// The check box is a placeholder, visible but disabled.
					GZWinUtil::SetWindowEnabled(pBtn->AsIGZWin(), false);
				}

				pBtn->SetCaption(entry.styleName);

				constexpr uint32_t LedgerHeaderFontStyle = 0xE9C86B5A;
				constexpr uint32_t DefaultFontStyle = 0x68963C4C;

				pBtn->SetFontStyle(entry.boldText ? LedgerHeaderFontStyle : DefaultFontStyle);
			}
			else
			{
				GZWinUtil::SetWindowVisible(pBtn->AsIGZWin(), false);
			}
		}
		else if (childID >= StyleControlReservedButtonRangeStart && childID <= StyleControlReservedButtonRangeEnd)
		{
			ConfigureStyleControlReservedButtons(childID, pBtn);
		}
	}

	void EnumerateBuildingStyleCheckBoxes(BuildingStyleWinEnumCallback callback, void* pCallbackState)
	{
		if (callback)
		{
			BuildingStyleWinEnumContext context(callback, pCallbackState);

			BuildingStyleWinUtil::EnumerateBuildingStyleContainerButtons(
				&BuildingStyleWinEnumProc,
				&context);
		}
	}
}

AvailableBuildingStyles::AvailableBuildingStyles()
	: firstCityLoaded(false)
{
}

void AvailableBuildingStyles::Initialize()
{
	if (firstCityLoaded)
	{
		UpdateAutomaticCheckBoxContext context(definedBuildingStyles);

		EnumerateBuildingStyleCheckBoxes(UpdateAutomaticCheckBoxNamesCallback, &context);
	}
	else
	{
		firstCityLoaded = true;

		definedBuildingStyles.Load();

		InitializeBuildingStyleContext context(definedBuildingStyles);

		EnumerateBuildingStyleCheckBoxes(InitializeBuildingStylesCallback, &context);

		// Sort the items in ascending order.
		std::sort(
			context.availableBuildingStyles.begin(),
			context.availableBuildingStyles.end());
		std::swap(availableBuildingStyles, context.availableBuildingStyles);
	}
}

bool AvailableBuildingStyles::IsStyleButtonIDValid(uint32_t buttonID) const
{
	return availableBuildingStyles.contains_button(buttonID);
}

const BuildingStyleCollection& AvailableBuildingStyles::GetBuildingStyles() const
{
	return availableBuildingStyles;
}
