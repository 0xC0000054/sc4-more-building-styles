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
#include "BuildingStyleIniFile.h"
#include "cGZPersistResourceKey.h"
#include "cIGZLanguageManager.h"
#include "cIGZPersistResourceManager.h"
#include "cIGZWin.h"
#include "cIGZWinBtn.h"
#include "cISC4App.h"
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

	struct InitializeBuildingStyleContext
	{
		const std::unordered_map<uint32_t, BuildingStyleIniFile::StyleEntry>& iniFileBuildingStyles;
		BuildingStyleCollection availableBuildingStyles;
		cIGZLanguageManagerPtr languageManager;
		cIGZPersistResourceManagerPtr resourceManager;

		InitializeBuildingStyleContext(const BuildingStyleIniFile& buildingStyleIniFile)
			: iniFileBuildingStyles(buildingStyleIniFile.GetStyles()),
			  availableBuildingStyles(),
			  languageManager(),
			  resourceManager()
		{
		}
	};

	void InitializeBuildingStylesCallback(uint32_t childID, cIGZWinBtn* pBtn, void* pState)
	{
		InitializeBuildingStyleContext* state = static_cast<InitializeBuildingStyleContext*>(pState);

		if (childID <= BuildingStyleIniMaxButtonID)
		{
			const auto& styles = state->iniFileBuildingStyles;

			const auto item = styles.find(childID);

			if (item != styles.end())
			{
				const BuildingStyleIniFile::StyleEntry& entry = item->second;

				if (entry.styleID != BuildingStyleIniFile::InvalidStyleID)
				{
					state->availableBuildingStyles.insert(item->first, entry.styleID, entry.styleName);
					SetStyleRadioButtonToolTip(pBtn, entry.styleID, state->languageManager, state->resourceManager);
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

	struct UpdateINIFileCheckBoxContext
	{
		const std::unordered_map<uint32_t, BuildingStyleIniFile::StyleEntry>& iniFileBuildingStyles;
		cIGZLanguageManagerPtr languageManager;
		cIGZPersistResourceManagerPtr resourceManager;

		UpdateINIFileCheckBoxContext(const BuildingStyleIniFile& buildingStyleIniFile)
			: iniFileBuildingStyles(buildingStyleIniFile.GetStyles()),
			  languageManager(),
			  resourceManager()
		{
		}
	};

	void UpdateINIFileCheckBoxNamesCallback(uint32_t childID, cIGZWinBtn* pBtn, void* pState)
	{
		if (childID <= BuildingStyleIniMaxButtonID)
		{
			const UpdateINIFileCheckBoxContext* state = static_cast<UpdateINIFileCheckBoxContext*>(pState);
			const auto& styles = state->iniFileBuildingStyles;

			const auto item = styles.find(childID);

			if (item != styles.end())
			{
				const BuildingStyleIniFile::StyleEntry& entry = item->second;

				if (entry.styleID != BuildingStyleIniFile::InvalidStyleID)
				{
					SetStyleRadioButtonToolTip(pBtn, entry.styleID, state->languageManager, state->resourceManager);
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
			cISC4AppPtr pSC4App;

			if (pSC4App)
			{
				cIGZWin* mainWindow = pSC4App->GetMainWindow();

				if (mainWindow)
				{
					constexpr uint32_t kGZWin_WinSC4App = 0x6104489a;

					cIGZWin* pSC4AppWin = mainWindow->GetChildWindowFromID(kGZWin_WinSC4App);

					if (pSC4AppWin)
					{
						// Get the child window that contains the building style radio buttons.

						constexpr uint32_t BuildingStyleListContainer = 0x8bca20c3;

						cIGZWin* pStyleListContainer = pSC4AppWin->GetChildWindowFromIDRecursive(BuildingStyleListContainer);

						if (pStyleListContainer)
						{
							BuildingStyleWinEnumContext context(callback, pCallbackState);

							pStyleListContainer->EnumChildren(
								GZIID_cIGZWinBtn,
								&BuildingStyleWinEnumProc,
								&context);
						}
					}
				}
			}
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
		UpdateINIFileCheckBoxContext context(buildingStyleIniFile);

		EnumerateBuildingStyleCheckBoxes(UpdateINIFileCheckBoxNamesCallback, &context);
	}
	else
	{
		firstCityLoaded = true;

		buildingStyleIniFile.Load();

		InitializeBuildingStyleContext context(buildingStyleIniFile);

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
