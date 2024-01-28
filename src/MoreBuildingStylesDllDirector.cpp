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
#include "BuildingStyleMessages.h"
#include "AvailableBuildingStyles.h"
#include "Logger.h"
#include "cIGZCOM.h"
#include "cIGZCheatCodeManager.h"
#include "cIGZFrameWork.h"
#include "cISC4App.h"
#include "cISC4City.h"
#include "cIGZMessageServer2.h"
#include "cIGZMessageTarget.h"
#include "cIGZMessageTarget2.h"
#include "cIGZString.h"
#include "cIGZSystemService.h"
#include "cRZMessage2COMDirector.h"
#include "cRZMessage2Standard.h"
#include "cRZBaseString.h"
#include "GZServPtrs.h"

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

static constexpr uint32_t kSC4MessagePostCityInit = 0x26D31EC1;
static constexpr uint32_t kSC4MessagePreCityShutdown = 0x26D31EC2;
static constexpr uint32_t kMessageCheatIssued = 0x230E27AC;

static constexpr uint32_t kMoreBuildingStylesDirectorID = 0x3BF9E52C;

static constexpr uint32_t kDebugActiveStyles = 0x730FF429;

static constexpr std::string_view PluginLogFileName = "SC4MoreBuildingStyles.log";

cIGZMessageServer2* spMessageServer2 = nullptr;
cISC4TractDeveloper* spTractDeveloper = nullptr;

class MoreBuildingStylesDllDirector : public cRZMessage2COMDirector
{
public:

	MoreBuildingStylesDllDirector()
	{
		std::filesystem::path dllFolderPath = GetDllFolderPath();

		std::filesystem::path logFilePath = dllFolderPath;
		logFilePath /= PluginLogFileName;

		Logger& logger = Logger::GetInstance();
		logger.Init(logFilePath, LogLevel::Error);
		logger.WriteLogFileHeader("SC4MoreBuildingStyles v" PLUGIN_VERSION_STR);
	}

	uint32_t GetDirectorID() const
	{
		return kMoreBuildingStylesDirectorID;
	}

	void ActiveBuildingStyleCheckboxChanged(cIGZMessage2Standard* pStandardMsg)
	{
		bool added = pStandardMsg->GetData1() != 0;
		uint32_t styleID = static_cast<uint32_t>(pStandardMsg->GetData2());

		char buffer[256]{};

		snprintf(
			buffer,
			sizeof(buffer),
			"Style 0x%08X %s",
			styleID,
			added ? "added" : "removed");

		Logger::GetInstance().WriteLine(LogLevel::Info, buffer);
	}

	void ProcessCheat(cIGZMessage2Standard* pStandardMsg)
	{
		uint32_t cheatID = static_cast<uint32_t>(pStandardMsg->GetData1());

		if (cheatID == kDebugActiveStyles)
		{
			cISC4AppPtr pSC4App;

			if (pSC4App)
			{
				cISC4City* pCity = pSC4App->GetCity();

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
		}
	}

	void PostCityInit(cIGZMessage2Standard* pStandardMsg)
	{
		// Gather the list of available styles from the "Building Select" dialog radio buttons.
		AvailableBuildingStyles::GetInstance().Initialize();

		cISC4City* pCity = reinterpret_cast<cISC4City*>(pStandardMsg->GetIGZUnknown());

		if (pCity)
		{
			spTractDeveloper = pCity->GetTractDeveloper();
		}
	}

	void PreCityShutdown()
	{
		spTractDeveloper = nullptr;
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
		case kSC4MessagePostCityInit:
			PostCityInit(pStandardMsg);
			break;
		case kSC4MessagePreCityShutdown:
			PreAppShutdown();
			break;
		case kMessageBuildingStyleCheckboxChanged:
			ActiveBuildingStyleCheckboxChanged(pStandardMsg);
			break;
		}

		return true;
	}

	bool PostAppInit()
	{
		Logger& logger = Logger::GetInstance();

		BuildingSelectWinProcHooks::Install();

		cISC4AppPtr pSC4App;

		if (pSC4App)
		{
			cIGZCheatCodeManager* pCheatMgr = pSC4App->GetCheatCodeManager();

			if (pCheatMgr)
			{
				pCheatMgr->AddNotification2(this, 0);
				pCheatMgr->RegisterCheatCode(kDebugActiveStyles, cRZBaseString("DebugActiveStyles"));
			}
		}

		cIGZFrameWork* const pFramework = RZGetFrameWork();

		constexpr uint32_t GZMessageServer2SysServiceID = 0x4FA845B;
		constexpr uint32_t GZIID_cIGZMesageServer2 = 0x652294C7;

		if (pFramework->GetSystemService(
			GZMessageServer2SysServiceID,
			GZIID_cIGZMesageServer2,
			reinterpret_cast<void**>(&spMessageServer2)))
		{
			std::vector<uint32_t> requiredNotifications;
			requiredNotifications.push_back(kSC4MessagePostCityInit);
			requiredNotifications.push_back(kSC4MessagePreCityShutdown);
			requiredNotifications.push_back(kMessageBuildingStyleCheckboxChanged);

			for (uint32_t messageID : requiredNotifications)
			{
				if (!spMessageServer2->AddNotification(this, messageID))
				{
					logger.WriteLine(LogLevel::Error, "Failed to subscribe to the required notifications.");
					return false;
				}
			}
		}
		else
		{
			spMessageServer2 = nullptr;
			logger.WriteLine(LogLevel::Error, "Failed to subscribe to the required notifications.");
			return false;
		}

		return true;
	}

	bool PreAppShutdown()
	{
		cIGZMessageServer2* pMsgServ = spMessageServer2;
		spMessageServer2 = nullptr;

		if (pMsgServ)
		{
			pMsgServ->Release();
		}

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

	std::filesystem::path GetDllFolderPath()
	{
		wil::unique_cotaskmem_string modulePath = wil::GetModuleFileNameW(wil::GetModuleInstanceHandle());

		std::filesystem::path temp(modulePath.get());

		return temp.parent_path();
	}
};

cRZCOMDllDirector* RZGetCOMDllDirector() {
	static MoreBuildingStylesDllDirector sDirector;
	return &sDirector;
}