////////////////////////////////////////////////////////////////////////////
//
// This file is part of sc4-more-building-styles, a DLL Plugin for
// SimCity 4 that adds support for more building styles.
//
// Copyright (c) 2024 Nicholas Hayes
//
// This file is licensed under terms of the MIT License.
// See LICENSE.txt for more information.
//
////////////////////////////////////////////////////////////////////////////

#include "AvailableBuildingStyles.h"
#include "BuildingStyleButtons.h"
#include "BuildingStyleIniFile.h"
#include "cGZPersistResourceKey.h"
#include "cIGZWin.h"
#include "cIGZWinBtn.h"
#include "cISC4App.h"
#include "cISC4View3DWin.h"
#include "cRZAutoRefCount.h"
#include "GZServPtrs.h"
#include "GZWinUtil.h"
#include <algorithm>

namespace
{
	typedef void(*BuildingStyleWinEnumCallback)(uint32_t buttonID, cIGZWinBtn* pBtn, void* pState);

	struct BuildingStyleWinEnumContext
	{
		BuildingStyleWinEnumCallback callback;
		void* callbackState;
	};

	bool BuildingStyleWinEnumProc(cIGZWin* parent, uint32_t childID, cIGZWin* child, void* pState)
	{
		constexpr uint32_t titleBarButton = 0x2BC619F3;
		constexpr uint32_t minimizeButton = 0xEBC619FD;

		// The title bar, minimize button, and optional buttons are excluded, every other button
		// in the dialog is a style radio button.

		if (childID != titleBarButton
			&& childID != minimizeButton
			&& OptionalButtonIDs.find(childID) == OptionalButtonIDs.end())
		{
			cRZAutoRefCount<cIGZWinBtn> pBtn;

			if (child->QueryInterface(GZIID_cIGZWinBtn, pBtn.AsPPVoid()))
			{
				BuildingStyleWinEnumContext* state = static_cast<BuildingStyleWinEnumContext*>(pState);

				state->callback(childID, pBtn, state->callbackState);
			}
		}

		return true;
	}

	void ConfigureStyleControlReservedButtons(uint32_t buttonID, cIGZWinBtn* pBtn)
	{
		bool hideCheckBox = true;

		switch (buttonID)
		{
		case KickOutLowerWealthButtonID:
			hideCheckBox = false;
			break;
		}

		if (hideCheckBox)
		{
			GZWinUtil::SetWindowVisible(pBtn->AsIGZWin(), false);
		}
	}

	struct InitializeBuildingStyleContext
	{
		const std::unordered_map<uint32_t, BuildingStyleIniFile::StyleEntry>& iniFileBuildingStyles;
		BuildingStyleCollection availableBuildingStyles;

		InitializeBuildingStyleContext(const BuildingStyleIniFile& buildingStyleIniFile)
			: iniFileBuildingStyles(buildingStyleIniFile.GetStyles()),
			  availableBuildingStyles()
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
		}
	}

	struct UpdateINIFileCheckBoxContext
	{
		const std::unordered_map<uint32_t, BuildingStyleIniFile::StyleEntry>& iniFileBuildingStyles;

		UpdateINIFileCheckBoxContext(const BuildingStyleIniFile& buildingStyleIniFile)
			: iniFileBuildingStyles(buildingStyleIniFile.GetStyles())
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

				if (entry.styleID == BuildingStyleIniFile::InvalidStyleID)
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
	return availableBuildingStyles.contains(buttonID);
}

const BuildingStyleCollection& AvailableBuildingStyles::GetBuildingStyles() const
{
	return availableBuildingStyles;
}
