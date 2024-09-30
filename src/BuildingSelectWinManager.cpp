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
#include "cIGZPersistDBSegment.h"
#include "cISC4BuildingOccupant.h"
#include "cISC4City.h"
#include "cISC4Lot.h"
#include "cISC4LotConfiguration.h"
#include "cISC4LotManager.h"
#include "cISC4SimGrid.h"
#include "cISC4TractDeveloper.h"
#include "cRZAutoRefCount.h"
#include "cRZCOMDllDirector.h"
#include "cRZMessage2Standard.h"
#include "GlobalPointers.h"
#include "GZCLSIDDefs.h"
#include "Logger.h"

#include <array>

static constexpr uint32_t kSC4MessagePostCityInit = 0x26D31EC1;
static constexpr uint32_t kSC4MessagePreCityShutdown = 0x26D31EC2;
static constexpr uint32_t kSC4MessageLoad = 0x26C63341;
static constexpr uint32_t kSC4MessageSave = 0x26C63344;
static constexpr uint32_t kMessageStateChanged = 0xE8BDA2E7;

static constexpr std::array<uint32_t, 5> RequiredNotifications =
{
	kSC4MessagePostCityInit,
	kSC4MessagePreCityShutdown,
	kSC4MessageLoad,
	kSC4MessageSave,
	kMessageStateChanged,
};

constexpr uint32_t GZMessageServer2SysServiceID = 0x4FA845B;
constexpr uint32_t GZIID_cIGZMesageServer2 = 0x652294C7;

namespace
{
	cISC4ZoneManager::ZoneType GetFirstCompatibleZoneType(
		const cISC4LotConfiguration* pLotConfiguration,
		cISC4ZoneManager::ZoneType start,
		cISC4ZoneManager::ZoneType end)
	{
		using T = std::underlying_type<cISC4ZoneManager::ZoneType>::type;

		const T startIndex = static_cast<T>(start);
		const T endIndex = static_cast<T>(end);

		for (T i = startIndex; i <= endIndex; i++)
		{
			cISC4ZoneManager::ZoneType value = static_cast<cISC4ZoneManager::ZoneType>(i);

			if (pLotConfiguration->IsCompatibleWithZoneType(value))
			{
				return value;
			}
		}

		return cISC4ZoneManager::ZoneType::None;
	}

	cISC4ZoneManager::ZoneType GetGrowifyZoneType(cISC4Lot* pLotCopy)
	{
		cISC4ZoneManager::ZoneType zoneType = cISC4ZoneManager::ZoneType::None;

		const cISC4BuildingOccupant* pBuildingOccupant = pLotCopy->GetBuilding();
		const cISC4LotConfiguration* pLotConfiguration = pLotCopy->GetLotConfiguration();

		if (pBuildingOccupant && pLotConfiguration)
		{
			const cISC4BuildingOccupant::BuildingProfile& profile = pBuildingOccupant->GetBuildingProfile();

			switch (profile.purpose)
			{
			case cISC4BuildingOccupant::PurposeType::Residence:
				zoneType = GetFirstCompatibleZoneType(
					pLotConfiguration,
					cISC4ZoneManager::ZoneType::ResidentialLowDensity,
					cISC4ZoneManager::ZoneType::ResidentialHighDensity);
				break;
			case cISC4BuildingOccupant::PurposeType::Services:
			case cISC4BuildingOccupant::PurposeType::Office:
				zoneType = GetFirstCompatibleZoneType(
					pLotConfiguration,
					cISC4ZoneManager::ZoneType::CommercialLowDensity,
					cISC4ZoneManager::ZoneType::CommercialHighDensity);
				break;
			case cISC4BuildingOccupant::PurposeType::Agriculture:
				zoneType = GetFirstCompatibleZoneType(
					pLotConfiguration,
					cISC4ZoneManager::ZoneType::Agriculture,
					cISC4ZoneManager::ZoneType::Agriculture);
				break;
			case cISC4BuildingOccupant::PurposeType::Processing:
			case cISC4BuildingOccupant::PurposeType::Manufacturing:
			case cISC4BuildingOccupant::PurposeType::HighTech:
				zoneType = GetFirstCompatibleZoneType(
					pLotConfiguration,
					cISC4ZoneManager::ZoneType::IndustrialMediumDensity,
					cISC4ZoneManager::ZoneType::IndustrialHighDensity);
				break;
			}
		}

		return zoneType;
	}

	bool IndustrialBuildingSupportsBuildingStyles(const cISC4BuildingOccupant* pBuildingOccupant)
	{
		if (pBuildingOccupant)
		{
			const cISC4BuildingOccupant::BuildingProfile& profile = pBuildingOccupant->GetBuildingProfile();

			switch (profile.purpose)
			{
			case cISC4BuildingOccupant::PurposeType::Agriculture:
				return spPreferences->AgriculturePurposeTypeSupportsBuildingStyles();
			case cISC4BuildingOccupant::PurposeType::Processing:
				return spPreferences->ProcessingPurposeTypeSupportsBuildingStyles();
			case cISC4BuildingOccupant::PurposeType::Manufacturing:
				return spPreferences->ManufacturingPurposeTypeSupportsBuildingStyles();
			case cISC4BuildingOccupant::PurposeType::HighTech:
				return spPreferences->HighTechPurposeTypeSupportsBuildingStyles();
			}
		}

		return false;
	}

	bool BuildingStylesAreSupported(cISC4ZoneManager::ZoneType zoneType, cISC4Lot* pLotCopy)
	{
		bool result = false;

		switch (zoneType)
		{
		case cISC4ZoneManager::ZoneType::ResidentialLowDensity:
		case cISC4ZoneManager::ZoneType::ResidentialMediumDensity:
		case cISC4ZoneManager::ZoneType::ResidentialHighDensity:
		case cISC4ZoneManager::ZoneType::CommercialLowDensity:
		case cISC4ZoneManager::ZoneType::CommercialMediumDensity:
		case cISC4ZoneManager::ZoneType::CommercialHighDensity:
			result = true;
			break;
		case cISC4ZoneManager::ZoneType::Agriculture:
		case cISC4ZoneManager::ZoneType::IndustrialMediumDensity:
		case cISC4ZoneManager::ZoneType::IndustrialHighDensity:
			result = IndustrialBuildingSupportsBuildingStyles(pLotCopy->GetBuilding());
			break;
		}

		return result;
	}
}

BuildingSelectWinManager::BuildingSelectWinManager()
	: refCount(0),
	  pMS2(nullptr),
	  pLotManager(nullptr),
	  pTractDeveloper(nullptr),
	  pZoneManager(nullptr),
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
	case kSC4MessageLoad:
		Load(pStandardMsg);
		break;
	case kSC4MessageSave:
		Save(pStandardMsg);
		break;
	case kMessageStateChanged:
		StateChanged(pStandardMsg);
		break;
	}

	return true;
}

void BuildingSelectWinManager::PreCityShutdown()
{
	pLotManager = nullptr;
	pTractDeveloper = nullptr;
	pZoneManager = nullptr;
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
		pLotManager = pCity->GetLotManager();
		pTractDeveloper = pCity->GetTractDeveloper();
		pZoneManager = pCity->GetZoneManager();
	}
}

void BuildingSelectWinManager::Load(cIGZMessage2Standard* pStandardMsg)
{
	cIGZPersistDBSegment* pSegment = static_cast<cIGZPersistDBSegment*>(pStandardMsg->GetVoid1());

	context.LoadFromDBSegment(pSegment);
}

void BuildingSelectWinManager::Save(cIGZMessage2Standard* pStandardMsg)
{
	cIGZPersistDBSegment* pSegment = static_cast<cIGZPersistDBSegment*>(pStandardMsg->GetVoid1());

	context.SaveToDBSegment(pSegment);
}

void BuildingSelectWinManager::StateChanged(cIGZMessage2Standard* pStandardMsg)
{
	cRZAutoRefCount<cISC4Lot> pLotCopy;

	// The lot provided by the state changed message is a temporary copy, modifying
	// it will have no effect.
	if (pStandardMsg->GetIGZUnknown()->QueryInterface(GZIID_cISC4Lot, pLotCopy.AsPPVoid()))
	{
		if (pStandardMsg->GetData2() == 3)
		{
			LotActivated(pLotCopy);
		}
	}
}

void BuildingSelectWinManager::LotActivated(cISC4Lot* pLotCopy)
{
	const cISC4ZoneManager::ZoneType zoneType = pLotCopy->GetZoneType();

	if (zoneType == cISC4ZoneManager::ZoneType::Plopped)
	{
		if (context.AutomaticallyGrowifyPloppedBuildings())
		{
			const cISC4ZoneManager::ZoneType growifyZone = GetGrowifyZoneType(pLotCopy);

			if (growifyZone != cISC4ZoneManager::ZoneType::None)
			{
				// The lot that is sent in the lot state changed message is a
				// temporary copy, so modifying it will have no effect.
				//
				// To work around this limitation we get the lot's location
				// and use that to get the real lot from the lot manager.

				int32_t lotX = 0;
				int32_t lotZ = 0;

				pLotCopy->GetLocation(lotX, lotZ);

				GrowifyLot(pLotManager->GetLot(lotX, lotZ, false), growifyZone);
			}
		}
	}
	else if (zoneType >= cISC4ZoneManager::ZoneType::ResidentialLowDensity
		  && zoneType <= cISC4ZoneManager::ZoneType::IndustrialHighDensity)
	{
		if (context.AutomaticallyMarkBuildingsAsHistorical()
			&& BuildingStylesAreSupported(zoneType, pLotCopy))
		{
			// The lot that is sent in the lot state changed message is a
			// temporary copy, so modifying it will have no effect.
			//
			// To work around this limitation we get the lot's location
			// and use that to get the real lot from the lot manager.

			int32_t lotX = 0;
			int32_t lotZ = 0;

			pLotCopy->GetLocation(lotX, lotZ);

			cISC4Lot* pRealLot = pLotManager->GetLot(lotX, lotZ, false);

			if (pRealLot)
			{
				pRealLot->SetHistorical(true);
			}
		}
	}
}

void BuildingSelectWinManager::GrowifyLot(
	cISC4Lot* pLot,
	cISC4ZoneManager::ZoneType zoneType)
{
	if (pLot && pZoneManager)
	{
		cISC4SimGrid<int8_t>* pZoneGrid = pZoneManager->GetZoneGrid();

		SC4Rect<int32_t> lotBounds{};
		pLot->GetBoundingRect(lotBounds);

		for (int32_t x = lotBounds.topLeftX; x <= lotBounds.bottomRightX; x++)
		{
			for (int32_t z = lotBounds.topLeftY; z <= lotBounds.bottomRightY; z++)
			{
				// Update the zone type for each cell that the lot occupies.
				pZoneGrid->SetTractValue(x, z, static_cast<int8_t>(zoneType));
			}
		}

		// Update the cached zone data for the lot.
		pLot->UpdateZoneType();

		if (context.AutomaticallyMarkBuildingsAsHistorical())
		{
			pLot->SetHistorical(true);
		}
	}
}

cISC4TractDeveloper* BuildingSelectWinManager::GetTractDeveloper() const
{
	return pTractDeveloper;
}

bool BuildingSelectWinManager::IsStyleButtonIDValid(uint32_t buttonID) const
{
	return availableBuildingStyles.IsStyleButtonIDValid(buttonID);
}

const BuildingStyleCollection& BuildingSelectWinManager::GetAvailableBuildingStyles() const
{
	return availableBuildingStyles.GetBuildingStyles();
}

IBuildingSelectWinContext& BuildingSelectWinManager::GetContext()
{
	return context;
}

const IBuildingSelectWinContext& BuildingSelectWinManager::GetContext() const
{
	return context;
}

void BuildingSelectWinManager::SendActiveBuildingStyleCheckboxChangedMessage(
	bool checked,
	const BuildingStyleCollectionEntry& entry)
{
	cRZMessage2Standard message;
	message.SetType(kMessageBuildingStyleCheckboxChanged);
	message.SetData1(checked);
	message.SetData2(entry.styleID);

	// Make a local copy of the style name to prevent callers from being able to modify it.

	cRZBaseString name(entry.styleName);

	message.SetVoid3(static_cast<cIGZString*>(&name));

	// We have to use MesageSend because the message is allocated on the stack.
	pMS2->MessageSend(static_cast<cIGZMessage2*>(static_cast<cIGZMessage2Standard*>(&message)));
}
