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

#include "version.h"
#include "GlobalPointers.h"
#include "BuildingSelectWinProcHooks.h"
#include "BuildingSelectWinManager.h"
#include "BuildingStyleInfo.h"
#include "BuildingStyleMessages.h"
#include "BuildingStyleWallToWall.h"
#include "AvailableBuildingStyles.h"
#include "FileSystem.h"
#include "Logger.h"
#include "Preferences.h"
#include "TractDeveloperHooks.h"
#include "cIGZCOM.h"
#include "cIGZCheatCodeManager.h"
#include "cIGZFrameWork.h"
#include "cISC4App.h"
#include "cISC4City.h"
#include "cIGZMessageServer2.h"
#include "cIGZMessageTarget.h"
#include "cIGZMessageTarget2.h"
#include "cIGZString.h"
#include "cIGZWinMgr.h"
#include "cRZMessage2COMDirector.h"
#include "cRZMessage2Standard.h"
#include "cRZBaseString.h"
#include "GZServPtrs.h"
#include "SC4NotificationDialog.h"

#include <array>
#include <filesystem>
#include <fstream>
#include <memory>
#include <string>
#include <Windows.h>
#include "wil/resource.h"
#include "wil/win32_helpers.h"

#include "EASTLConfigSC4.h"
#include "EASTL\vector.h"

static constexpr uint32_t kMessageCheatIssued = 0x230E27AC;
static constexpr uint32_t kSC4MessagePostCityInitComplete = 0xEA8AE29A;
static constexpr uint32_t kSC4MessagePostCityShutdown = 0x26D31EC3;

static constexpr uint32_t kMoreBuildingStylesDirectorID = 0x3BF9E52C;

static constexpr uint32_t kDebugActiveStyles = 0x730FF429;
static constexpr uint32_t kActiveStyle = 0x4580A54D;

IBuildingSelectWinManager* spBuildingSelectWinManager = nullptr;
const Preferences* spPreferences = nullptr;

class MoreBuildingStylesDllDirector : public cRZMessage2COMDirector
{
public:

	MoreBuildingStylesDllDirector()
		: pCity(nullptr),
		  buildingSelectWinManager(),
		  buildingStyleInfo(buildingSelectWinManager)
	{
		std::filesystem::path logFilePath = FileSystem::GetLogFilePath();

		Logger& logger = Logger::GetInstance();
		logger.Init(logFilePath, LogLevel::Error);
		logger.WriteLogFileHeader("SC4MoreBuildingStyles v" PLUGIN_VERSION_STR);
	}

	uint32_t GetDirectorID() const
	{
		return kMoreBuildingStylesDirectorID;
	}

	bool GetClassObject(uint32_t rclsid, uint32_t riid, void** ppvObj)
	{
		bool result = false;

		if (rclsid == GZCLSID_cIBuildingStyleInfo)
		{
			result = buildingStyleInfo.QueryInterface(riid, ppvObj);
		}
		else if (rclsid == GZCLSID_cIBuildingStyleWallToWall)
		{
			result = buildingStyleWallToWall.QueryInterface(riid, ppvObj);
		}

		return result;
	}

	void EnumClassObjects(ClassObjectEnumerationCallback pCallback, void* pContext)
	{
		pCallback(GZCLSID_cIBuildingStyleInfo, 0, pContext);
		pCallback(GZCLSID_cIBuildingStyleWallToWall, 0, pContext);
	}

	void ActiveBuildingStyleCheckboxChanged(cIGZMessage2Standard* pStandardMsg)
	{
		bool added = pStandardMsg->GetData1() != 0;
		uint32_t styleID = static_cast<uint32_t>(pStandardMsg->GetData2());
		cIGZString* styleButtonText = static_cast<cIGZString*>(pStandardMsg->GetVoid3());

		// Note that if the style button text contains characters outside the US-ASCII range (0x00-0x7F),
		// the value that is written to the log and/or displayed in the debugger may not match the
		// in-game text due to character set differences. This may also be dependent on the user's
		// operating system language.
		//
		// I am not sure if there is an easy way to fix this problem. The game's language manager service
		// has functions to get the current country, language and Windows code page, but it does not have
		// any functions to convert strings between different encodings.

		Logger::GetInstance().WriteLineFormatted(
			LogLevel::Info,
			"Style 0x%08X (%s) %s",
			styleID,
			styleButtonText->ToChar(),
			added ? "added" : "removed");
	}

	void PostCityInitComplete(cIGZMessage2Standard* pStandardMsg)
	{
		pCity = static_cast<cISC4City*>(pStandardMsg->GetVoid1());

		if (pCity)
		{
			cISC4AppPtr pSC4App;

			if (pSC4App)
			{
				cIGZCheatCodeManager* pCheatCodeManager = pSC4App->GetCheatCodeManager();

				if (pCheatCodeManager)
				{
					pCheatCodeManager->AddNotification2(this, 0);
					pCheatCodeManager->RegisterCheatCode(kDebugActiveStyles, cRZBaseString("DebugActiveStyles"));
					pCheatCodeManager->RegisterCheatCode(kActiveStyle, cRZBaseString("ActiveStyle"));
				}
			}
		}
	}

	void PostCityShutdown()
	{
		pCity = nullptr;

		cISC4AppPtr pSC4App;

		if (pSC4App)
		{
			cIGZCheatCodeManager* pCheatCodeManager = pSC4App->GetCheatCodeManager();

			if (pCheatCodeManager)
			{
				pCheatCodeManager->UnregisterCheatCode(kDebugActiveStyles);
				pCheatCodeManager->UnregisterCheatCode(kActiveStyle);
				pCheatCodeManager->RemoveNotification2(this, 0);
			}
		}
	}

	void ProcessCheat(cIGZMessage2Standard* pStandardMsg)
	{
		uint32_t cheatID = static_cast<uint32_t>(pStandardMsg->GetData1());

		if (cheatID == kDebugActiveStyles)
		{
			if (pCity)
			{
				const cISC4TractDeveloper* pTractDeveloper = pCity->GetTractDeveloper();

				if (pTractDeveloper)
				{
					const eastl::vector<uint32_t>& activeStyles = pTractDeveloper->GetActiveStyles();

					Logger& logger = Logger::GetInstance();

					logger.WriteLineFormatted(LogLevel::Info, "%d active styles:", activeStyles.size());

					for (uint32_t style : activeStyles)
					{
						logger.WriteLineFormatted(LogLevel::Info, "0x%08X", style);
					}
				}
			}
		}
		else if (cheatID == kActiveStyle)
		{
			if (pCity)
			{
				const cISC4TractDeveloper* pTractDeveloper = pCity->GetTractDeveloper();

				if (pTractDeveloper)
				{
					if (pTractDeveloper->IsUsingAllStylesAtOnce())
					{
						SC4NotificationDialog::ShowDialog(
							cRZBaseString("All styles are being built at once."),
							cRZBaseString("ActiveStyle"));
					}
					else
					{
						uint32_t currentStyle = pTractDeveloper->GetCurrentStyle();

						char buffer[128]{};

						std::snprintf(buffer, sizeof(buffer), "0x%X", currentStyle);

						SC4NotificationDialog::ShowDialog(
							cRZBaseString(buffer),
							cRZBaseString("ActiveStyle"));
					}
				}
			}
		}
	}

	bool DoMessage(cIGZMessage2* pMessage)
	{
		cIGZMessage2Standard* pStandardMsg = static_cast<cIGZMessage2Standard*>(pMessage);

		uint32_t dwType = pMessage->GetType();

		switch (dwType)
		{
		case kMessageCheatIssued:
			ProcessCheat(pStandardMsg);
			break;
		case kSC4MessagePostCityInitComplete:
			PostCityInitComplete(pStandardMsg);
			break;
		case kSC4MessagePostCityShutdown:
			PostCityShutdown();
			break;
		case kMessageBuildingStyleCheckboxChanged:
#if _DEBUG
			ActiveBuildingStyleCheckboxChanged(pStandardMsg);
#endif // _DEBUG
			break;
		}

		return true;
	}

	bool PostAppInit()
	{
		Logger& logger = Logger::GetInstance();

		preferences.Load(mpCOM);
		spPreferences = &preferences;

		spBuildingSelectWinManager = &buildingSelectWinManager;
		BuildingSelectWinProcHooks::Install();
		TractDeveloperHooks::Install(preferences);

		if (!buildingSelectWinManager.Initialize())
		{
			logger.WriteLine(LogLevel::Error, "Failed to initialize the building select window manager.");
			return false;
		}

		cIGZMessageServer2Ptr pMS2;

		if (pMS2)
		{
			std::vector<uint32_t> requiredNotifications;
			requiredNotifications.push_back(kSC4MessagePostCityInitComplete);
			requiredNotifications.push_back(kSC4MessagePostCityShutdown);
#ifdef _DEBUG
			requiredNotifications.push_back(kMessageBuildingStyleCheckboxChanged);
#endif // _DEBUG

			for (const uint32_t& messageID : requiredNotifications)
			{
				pMS2->AddNotification(this, messageID);
			}
		}

		return true;
	}

	bool PreAppShutdown()
	{
		buildingSelectWinManager.Shutdown();

		return true;
	}

	bool OnStart(cIGZCOM * pCOM)
	{
		cIGZFrameWork* const pFramework = RZGetFrameWork();

		const cIGZFrameWork::FrameworkState state = pFramework->GetState();

		if (state < cIGZFrameWork::kStatePreAppInit)
		{
			pFramework->AddHook(this);
		}
		else
		{
			PreAppInit();
		}
		return true;
	}

private:
	cISC4City* pCity;
	BuildingSelectWinManager buildingSelectWinManager;
	BuildingStyleInfo buildingStyleInfo;
	BuildingStyleWallToWall buildingStyleWallToWall;
	Preferences preferences;
};

cRZCOMDllDirector* RZGetCOMDllDirector() {
	static MoreBuildingStylesDllDirector sDirector;
	return &sDirector;
}