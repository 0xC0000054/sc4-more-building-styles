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

#include "BuildingStyleWinUtil.h"
#include "cIGZWinBtn.h"
#include "cISC4App.h"
#include "GZServPtrs.h"

void BuildingStyleWinUtil::EnumerateBuildingStyleContainerButtons(cIGZWin::EnumChildrenCallback callback, void* pCallbackState)
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
						pStyleListContainer->EnumChildren(
							GZIID_cIGZWinBtn,
							callback,
							pCallbackState);
					}
				}
			}
		}
	}
}

