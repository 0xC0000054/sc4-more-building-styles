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
#include "cGZPersistResourceKey.h"
#include "cIGZWin.h"
#include "cIGZWinBtn.h"
#include "cISC4App.h"
#include "cISC4View3DWin.h"
#include "GZServPtrs.h"
#include <map>

static const uint32_t kGZWin_WinSC4App = 0x6104489a;
static const uint32_t kGZWin_SC4View3DWin = 0x9a47b417;

static const uint32_t kGZIID_cISC4View3DWin = 0xFA47B3F9;

AvailableBuildingStyles::AvailableBuildingStyles()
	: initialized(false)
{
}

void AvailableBuildingStyles::Initialize()
{
	if (!initialized)
	{
		cISC4AppPtr pSC4App;

		if (pSC4App)
		{
			cIGZWin* mainWindow = pSC4App->GetMainWindow();

			if (mainWindow)
			{
				cIGZWin* pSC4AppWin = mainWindow->GetChildWindowFromID(kGZWin_WinSC4App);

				if (pSC4AppWin)
				{
					// Get the child window that contains the building style radio buttons.

					constexpr uint32_t BuildingStyleListContainer = 0x8bca20c3;

					cIGZWin* pStyleListContainer = pSC4AppWin->GetChildWindowFromIDRecursive(BuildingStyleListContainer);

					if (pStyleListContainer)
					{
						constexpr uint32_t GZIID_cIGZWinBtn = 0x8810;

						// Enumerate the buttons in the child window to get the list of available styles.

						pStyleListContainer->EnumChildren(
							GZIID_cIGZWinBtn,
							&BuildingSelectWinEnumProc,
							this);
					}
				}
			}
		}

		initialized = true;
	}
}

bool AvailableBuildingStyles::ContainsBuildingStyle(uint32_t style) const
{
	return availableBuildingStyles.contains(style);
}

const std::map<uint32_t, std::string>& AvailableBuildingStyles::GetBuildingStyles() const
{
	return availableBuildingStyles;
}

bool AvailableBuildingStyles::BuildingSelectWinEnumProc(cIGZWin* parent, uint32_t childID, cIGZWin* child, void* pState)
{
	constexpr uint32_t titleBarButton = 0x2BC619F3;
	constexpr uint32_t minimizeButton = 0xEBC619FD;

	// The title bar and minimize buttons are excluded, every other button
	// in the dialog is a style radio button.

	if (childID != titleBarButton && childID != minimizeButton)
	{
		AvailableBuildingStyles* state = static_cast<AvailableBuildingStyles*>(pState);

		cIGZWinBtn* pBtn = nullptr;

		if (child->QueryInterface(GZIID_cIGZWinBtn, reinterpret_cast<void**>(&pBtn)))
		{
			cIGZString* caption = pBtn->GetCaption();

			state->availableBuildingStyles.try_emplace(childID, caption->ToChar());

			pBtn->Release();
		}
	}

	return true;
}

