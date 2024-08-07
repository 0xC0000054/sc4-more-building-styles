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
#include "cIGZWinMgr.h"
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

static constexpr uint32_t kMessageCheatIssued = 0x230E27AC;

static constexpr uint32_t kMoreBuildingStylesDirectorID = 0x3BF9E52C;

static constexpr uint32_t kDebugActiveStyles = 0x730FF429;
static constexpr uint32_t kActiveStyle = 0x4580A54D;

static constexpr std::string_view PluginLogFileName = "SC4MoreBuildingStyles.log";

IBuildingSelectWinManager* spBuildingSelectWinManager = nullptr;

class MoreBuildingStylesDllDirector : public cRZMessage2COMDirector
{
public:

	MoreBuildingStylesDllDirector()
		: buildingSelectWinManager(),
		  buildingStyleInfo(buildingSelectWinManager)
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

	bool GetClassObject(uint32_t rclsid, uint32_t riid, void** ppvObj)
	{
		bool result = false;

		if (rclsid == GZCLSID_cIBuildingStyleInfo)
		{
			result = buildingStyleInfo.QueryInterface(riid, ppvObj);
		}

		return result;
	}

	void EnumClassObjects(ClassObjectEnumerationCallback pCallback, void* pContext)
	{
		pCallback(GZCLSID_cIBuildingStyleInfo, 0, pContext);
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
		else if (cheatID == kActiveStyle)
		{
			cISC4AppPtr pSC4App;
			cIGZWinMgrPtr pWinMgr;

			if (pSC4App && pWinMgr)
			{
				cISC4City* pCity = pSC4App->GetCity();

				if (pCity)
				{
					const cISC4TractDeveloper* pTractDeveloper = pCity->GetTractDeveloper();

					if (pTractDeveloper)
					{
						if (pTractDeveloper->IsUsingAllStylesAtOnce())
						{
							pWinMgr->GZMsgBox(
								cRZBaseString("All styles are being built at once."),
								cRZBaseString("ActiveStyle"),
								0,
								false,
								0);
						}
						else
						{
							uint32_t currentStyle = pTractDeveloper->GetCurrentStyle();

							char buffer[128]{};

							std::snprintf(buffer, sizeof(buffer), "0x%X", currentStyle);

							pWinMgr->GZMsgBox(
								cRZBaseString(buffer),
								cRZBaseString("ActiveStyle"),
								0,
								false,
								0);
						}
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

		spBuildingSelectWinManager = &buildingSelectWinManager;
		BuildingSelectWinProcHooks::Install();

		cISC4AppPtr pSC4App;

		if (pSC4App)
		{
			cIGZCheatCodeManager* pCheatMgr = pSC4App->GetCheatCodeManager();

			if (pCheatMgr)
			{
				pCheatMgr->AddNotification2(this, 0);
				pCheatMgr->RegisterCheatCode(kDebugActiveStyles, cRZBaseString("DebugActiveStyles"));
				pCheatMgr->RegisterCheatCode(kActiveStyle, cRZBaseString("ActiveStyle"));
			}
		}

		if (!buildingSelectWinManager.Initialize())
		{
			logger.WriteLine(LogLevel::Error, "Failed to initialize the building select window manager.");
			return false;
		}

#ifdef _DEBUG
		cIGZMessageServer2Ptr pMS2;

		if (pMS2)
		{
			pMS2->AddNotification(this, kMessageBuildingStyleCheckboxChanged);
		}
#endif // _DEBUG

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

	std::filesystem::path GetDllFolderPath()
	{
		wil::unique_cotaskmem_string modulePath = wil::GetModuleFileNameW(wil::GetModuleInstanceHandle());

		std::filesystem::path temp(modulePath.get());

		return temp.parent_path();
	}

	BuildingSelectWinManager buildingSelectWinManager;
	BuildingStyleInfo buildingStyleInfo;
};

cRZCOMDllDirector* RZGetCOMDllDirector() {
	static MoreBuildingStylesDllDirector sDirector;
	return &sDirector;
}