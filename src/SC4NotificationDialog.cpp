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

#include "SC4NotificationDialog.h"
#include "SC4VersionDetection.h"
#include "cGZPersistResourceKey.h"
#include "SC4UI.h"
#include "StringResourceKey.h"
#include "StringResourceManager.h"
#include "cIGZWinMgr.h"
#include "cISC4App.h"
#include "cISC4City.h"
#include "cISC424HourClock.h"
#include "cISC4Simulator.h"
#include "GZServPtrs.h"

namespace
{
	void PauseGame(cISC4App* pSC4App)
	{
		cISC4City* pCity = pSC4App->GetCity();

		if (pCity)
		{
			cISC424HourClock* pClock = pCity->Get24HourClock();
			cISC4Simulator* pSim = pCity->GetSimulator();

			if (pClock && pSim)
			{
				pClock->HiddenPause(true);
				pSim->HiddenPause();
			}
		}
	}

	void ResumeGame(cISC4App* pSC4App)
	{
		cISC4City* pCity = pSC4App->GetCity();

		if (pCity)
		{
			cISC424HourClock* pClock = pCity->Get24HourClock();
			cISC4Simulator* pSim = pCity->GetSimulator();

			if (pClock && pSim)
			{
				pClock->HiddenPause(false);
				pSim->HiddenResume();
			}
		}
	}

	typedef bool(__cdecl* pfnCreateSC4GenericNotificationDialog)(cIGZString const& caption, cIGZString const& message);

	pfnCreateSC4GenericNotificationDialog GetCreateSC4GenericNotificationDialogPointer()
	{
		const uint16_t gameVersion = SC4VersionDetection::GetInstance().GetGameVersion();

		pfnCreateSC4GenericNotificationDialog pfnCreateDialog = nullptr;

		switch (gameVersion)
		{
		case 641:
			pfnCreateDialog = reinterpret_cast<pfnCreateSC4GenericNotificationDialog>(0x78dd80);
			break;
		}

		return pfnCreateDialog;
	}

	void ShowGZCOMMessageBox(cIGZString const& message, cIGZString const& caption)
	{
		cISC4AppPtr pSC4App;

		if (pSC4App)
		{
			if (pSC4App->GetPopupDialogsEnabled())
			{
				PauseGame(pSC4App);

				cIGZWinMgrPtr pWinMgr;

				if (pWinMgr)
				{
					pWinMgr->GZMsgBox(message, caption, 0, false, 0);
				}

				ResumeGame(pSC4App);
			}
		}
	}

	void ShowNotificationDialog(cIGZString const& message, cIGZString const& caption)
	{
		const pfnCreateSC4GenericNotificationDialog pfnCreateDialog = GetCreateSC4GenericNotificationDialogPointer();

		if (pfnCreateDialog)
		{
			pfnCreateDialog(caption, message);
		}
		else
		{
			// If we do not have a function pointer to the internal method that creates SC4's notification
			// dialog box, we fall back to using the basic message box that the GZCOM framework provides.
			// The GZCOM message box has a fairly low message character limit (possibly 256?) beyond which
			// it truncates the string, but it is still more user friendly than not showing a notification.

			ShowGZCOMMessageBox(message, caption);
		}
	}
}

void SC4NotificationDialog::ShowDialog(cIGZString const& message, cIGZString const& caption)
{
	ShowNotificationDialog(message, caption);
}

void SC4NotificationDialog::ShowDialog(StringResourceKey const& messageKey, StringResourceKey const& captionKey)
{
	cIGZString* pMessage = nullptr;

	if (StringResourceManager::GetLocalizedString(messageKey, &pMessage))
	{
		cIGZString* pCaption = nullptr;

		if (StringResourceManager::GetLocalizedString(captionKey, &pCaption))
		{
			ShowNotificationDialog(*pMessage, *pCaption);

			pCaption->Release();
		}

		pMessage->Release();
	}
}
