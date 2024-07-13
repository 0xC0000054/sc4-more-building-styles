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

#include "BuildingSelectWinManager.h"
#include "BuildingStyleMessages.h"
#include "cIGZFrameWork.h"
#include "cIGZMessage2Standard.h"
#include "cIGZMessageServer2.h"
#include "cISC4City.h"
#include "cISC4TractDeveloper.h"
#include "cRZCOMDllDirector.h"
#include "cRZMessage2Standard.h"
#include "GZCLSIDDefs.h"
#include "Logger.h"

#include <array>

static constexpr uint32_t kSC4MessagePostCityInit = 0x26D31EC1;
static constexpr uint32_t kSC4MessagePreCityShutdown = 0x26D31EC2;

static constexpr std::array<uint32_t, 2> RequiredNotifications =
{
	kSC4MessagePostCityInit,
	kSC4MessagePreCityShutdown,
};

constexpr uint32_t GZMessageServer2SysServiceID = 0x4FA845B;
constexpr uint32_t GZIID_cIGZMesageServer2 = 0x652294C7;

BuildingSelectWinManager::BuildingSelectWinManager()
	: refCount(0),
	  pMS2(nullptr),
	  pTractDeveloper(nullptr),
	  initialized(false)
{
}

bool BuildingSelectWinManager::Initialize()
{
	if (!initialized)
	{
		initialized = true;

		cIGZFrameWork* const pFramework = RZGetFramework();

		if (pFramework->GetSystemService(
			GZMessageServer2SysServiceID,
			GZIID_cIGZMesageServer2,
			reinterpret_cast<void**>(&pMS2)))
		{
			for (uint32_t messageID : RequiredNotifications)
			{
				if (!pMS2->AddNotification(this, messageID))
				{
					return false;
				}
			}
		}
		else
		{
			pMS2 = nullptr;
			return false;
		}
	}

	return true;
}

bool BuildingSelectWinManager::Shutdown()
{
	if (initialized)
	{
		initialized = false;

		cIGZMessageServer2* pMsgServ = pMS2;
		pMS2 = nullptr;

		if (pMsgServ)
		{
			for (uint32_t messageID : RequiredNotifications)
			{
				if (!pMsgServ->RemoveNotification(this, messageID))
				{
					return false;
				}
			}
			pMsgServ->Release();
		}
	}

	return true;
}

bool BuildingSelectWinManager::QueryInterface(uint32_t riid, void** ppvObj)
{
	if (riid == GZCLSID::kcIGZMessageTarget2)
	{
		*ppvObj = static_cast<cIGZMessageTarget2*>(this);
		AddRef();

		return true;
	}
	else if (riid == GZIID_cIGZUnknown)
	{
		*ppvObj = static_cast<cIGZUnknown*>(this);
		AddRef();

		return true;
	}

	return false;
}

uint32_t BuildingSelectWinManager::AddRef()
{
	return ++refCount;
}

uint32_t BuildingSelectWinManager::Release()
{
	if (refCount > 0)
	{
		--refCount;
	}

	return refCount;
}

bool BuildingSelectWinManager::DoMessage(cIGZMessage2* pMsg)
{
	cIGZMessage2Standard* pStandardMsg = static_cast<cIGZMessage2Standard*>(pMsg);

	switch (pMsg->GetType())
	{
	case kSC4MessagePostCityInit:
		PostCityInit(pStandardMsg);
		break;
	case kSC4MessagePreCityShutdown:
		PreCityShutdown();
		break;
	}

	return true;
}

void BuildingSelectWinManager::PreCityShutdown()
{
	pTractDeveloper = nullptr;
}

void BuildingSelectWinManager::PostCityInit(cIGZMessage2Standard* pStandardMsg)
{
	// Gather the list of available styles from the "Building Select" dialog radio buttons.
	// This must by done in PostCityInit because SC4 doesn't load the "Building Select"
	// dialog until it is loading a city.
	availableBuildingStyles.Initialize();

	cISC4City* pCity = reinterpret_cast<cISC4City*>(pStandardMsg->GetIGZUnknown());

	if (pCity)
	{
		pTractDeveloper = pCity->GetTractDeveloper();
	}
}

cISC4TractDeveloper* BuildingSelectWinManager::GetTractDeveloper() const
{
	return pTractDeveloper;
}

bool BuildingSelectWinManager::IsBuildingStyleAvailable(uint32_t style) const
{
	return availableBuildingStyles.ContainsBuildingStyle(style);
}

const std::map<uint32_t, std::string>& BuildingSelectWinManager::GetAvailableBuildingStyles() const
{
	return availableBuildingStyles.GetBuildingStyles();
}

void BuildingSelectWinManager::SendActiveBuildingStyleCheckboxChangedMessage(bool checked, uint32_t styleID)
{
	cRZMessage2Standard message;
	message.SetType(kMessageBuildingStyleCheckboxChanged);
	message.SetData1(checked);
	message.SetData2(styleID);

	const std::map<uint32_t, std::string>& allStyles = availableBuildingStyles.GetBuildingStyles();
	cRZBaseString name(allStyles.find(styleID)->second);

	message.SetVoid3(static_cast<cIGZString*>(&name));

	// We have to use MesageSend because the message is allocated on the stack.
	pMS2->MessageSend(static_cast<cIGZMessage2*>(static_cast<cIGZMessage2Standard*>(&message)));
}
