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

#include "TractDeveloperHooks.h"
#include "BuildingUtil.h"
#include "cGZPersistResourceKey.h"
#include "cIGZPersistResourceManager.h"
#include "cIGZString.h"
#include "cIGZVariant.h"
#include "cISCProperty.h"
#include "cISCPropertyHolder.h"
#include "cISC4BuildingDevelopmentSimulator.h"
#include "cISC4BuildingOccupant.h"
#include "cISC4GrowthDeveloper.h"
#include "cISC4Lot.h"
#include "cISC4LotConfiguration.h"
#include "cRZAutoRefCount.h"
#include "GlobalPointers.h"
#include "GZServPtrs.h"
#include "Logger.h"
#include "Patcher.h"
#include "PropertyIDs.h"
#include "SC4Rect.h"
#include "SC4String.h"
#include "SC4Vector.h"
#include "SC4VersionDetection.h"
#include "WallToWallOccupantGroups.h"
#include <span>

#include "wil/result.h"

struct cSC4TractDeveloper
{
	struct CandidateLot
	{
		enum class DevelopmentType : uint32_t
		{
			TakeOverAbandoned = 1,
			LotSameSize = 2,
			LotAggregation = 3,
			LotSubdivision = 4,
		};

		DevelopmentType developmentType;			// 0x0
		cISC4Lot* pExistingLot;						// 0x4
		uint32_t lotLocationX;						// 0x8
		uint32_t lotLocationZ;						// 0xc
		uint32_t facing;							// 0x10
		uint8_t lotSizeX;							// 0x14
		uint8_t lotSizeZ;							// 0x15
		uint32_t lotSet[3];							// 0x18 - set<cISC4Lot*>
		int32_t totalNonVacantCapacity;				// 0x24
		int32_t totalVacantCapacity;				// 0x28
		uint32_t growthStage;						// 0x2c
		cISC4BuildingOccupant::WealthType wealth;	// 0x30
		float score;								// 0x34
		SC4Rect<int32_t> lotBounds;					// 0x38
		uint8_t unknown1[0x28];
		// The following 4 vectors appear to be network related.
		// Tracking Road/street connections for each side of the lot?
		SC4Vector<uint32_t> field_0x70;				// 0x70
		SC4Vector<uint32_t> field_0x7c;				// 0x7c
		SC4Vector<uint32_t> field_0x88;				// 0x88
		SC4Vector<uint32_t> field_0x94;				// 0x94
		uint8_t field_0xa0;							// 0xa0
	};

	static_assert(sizeof(CandidateLot) == 0xa4);
	static_assert(offsetof(CandidateLot, pExistingLot) == 0x4);
	static_assert(offsetof(CandidateLot, lotSet) == 0x18);
	static_assert(offsetof(CandidateLot, lotBounds) == 0x38);
	static_assert(offsetof(CandidateLot, field_0x70) == 0x70);
	static_assert(offsetof(CandidateLot, field_0xa0) == 0xa0);

	void* vtable;												// 0x0
	void* messageTarget2;										// 0x4
	void* gzSerialzable;										// 0x8
	int initialized;											// 0xc
	int refCount;												// 0x10
	void* pMS2;													// 0x14
	void* pCity;												// 0x18
	cISC4BuildingDevelopmentSimulator* pBuildingDevelopmentSim; // 0x1c
	void* pLandValueSim;										// 0x20
	void* pLotConfigurationManager;								// 0x24
	void* pLotDeveloper;										// 0x28
	void* pLotManager;											// 0x2c
	void* pPlumbingSim;											// 0x30
	void* pPowerSim;											// 0x34
	void* pTrafficSim;											// 0x38
	void* pZoneManager;											// 0x3c
	// begin tunable values
	float overbuild;											// 0x40
	float reoccupyThresholds[2];								// 0x44
	float redevelopSizeThreshold;								// 0x4c
	float demolitionCosts[2];									// 0x50
	float newLotCosts[2];										// 0x58
	float occupancyVariation;									// 0x60
	float minVacancyRange[2];									// 0x64
	uint8_t kickOutLowerWealth;									// 0x6c
	uint8_t kickOutLowerWealthPadding[3];
	SC4Vector<uint32_t> availableStyles;						// 0x70
	int yearsBetweenStyles;										// 0x7c
	// end tunable values
	cISC4GrowthDeveloper* pCurrentDeveloper;					// 0x80
	uint8_t unknown1[0x94];										// 0x84 - 0x117
	SC4Vector<uint32_t> activeStyles;							// 0x118
	uint32_t currentStyleIndex;									// 0x124
	uint32_t yearsPassed;										// 0x128
	uint8_t changeStylesEveryNYears;							// 0x12c
};

static_assert(offsetof(cSC4TractDeveloper, initialized) == 0xc);
static_assert(offsetof(cSC4TractDeveloper, pZoneManager) == 0x3c);
static_assert(offsetof(cSC4TractDeveloper, kickOutLowerWealth) == 0x6c);
static_assert(offsetof(cSC4TractDeveloper, yearsBetweenStyles) == 0x7c);
static_assert(offsetof(cSC4TractDeveloper, activeStyles) == 0x118);
static_assert(offsetof(cSC4TractDeveloper, currentStyleIndex) == 0x124);
static_assert(offsetof(cSC4TractDeveloper, changeStylesEveryNYears) == 0x12c);

struct cSC4LotConfiguration
{
	void* vtable;
	uint32_t refCount;
	uint32_t id;
	SC4String name;
	uint8_t unknown1[0x60];
	SC4Vector<uint32_t> buildingOccupantGroups;
};

static_assert(offsetof(cSC4LotConfiguration, buildingOccupantGroups) == 0x80);

static uintptr_t IsLotConfigurationSuitable_CompatableStyleFound_Continue;
static uintptr_t IsLotConfigurationSuitable_NoCompatableStyle_Continue;

static bool LotConfigurationHasOccupantGroupValue(const cSC4LotConfiguration* pLotConfiguration, uint32_t value)
{
	const SC4Vector<uint32_t>& buildingOccupantGroups = pLotConfiguration->buildingOccupantGroups;

	for (const auto& occupantGroup : buildingOccupantGroups)
	{
		if (occupantGroup == value)
		{
			return true;
		}
	}

	return false;
}

template<size_t N>
static bool LotConfigurationHasOccupantGroupValue(
	const cSC4LotConfiguration* pLotConfiguration,
	const frozen::unordered_map<uint32_t, const std::string_view, N>& values)
{
	const SC4Vector<uint32_t>& buildingOccupantGroups = pLotConfiguration->buildingOccupantGroups;

	for (const auto& occupantGroup : buildingOccupantGroups)
	{
		if (values.count(occupantGroup) != 0)
		{
			return true;
		}
	}

	return false;
}

struct PropertyData
{
	uint32_t value;
	std::span<uint32_t> values;

	PropertyData(const cIGZVariant& variant)
		: value(0),
		  values()
	{
		uint32_t* pData = variant.RefUint32();
		uint32_t repCount = variant.GetCount();

		if (repCount == 0)
		{
			// If the rep count is zero, the pointer's address is the value.
			value = reinterpret_cast<uint32_t>(pData);
		}
		else
		{
			values = std::span<uint32_t>(pData, repCount);
		}
	}

	bool Contains(uint32_t value) const
	{
		if (values.empty())
		{
			return value == value;
		}
		else
		{
			return std::find(values.begin(), values.end(), value) != values.end();
		}
	}
};

static void LogPurposeTypeDoesNotSupportStyles(
	uint32_t id,
	const char* const name,
	cISC4BuildingOccupant::PurposeType purposeType)
{
	const char* purposeName = "Unknown";

	switch (purposeType)
	{
	case cISC4BuildingOccupant::PurposeType::None:
		purposeName = "None";
		break;
	case cISC4BuildingOccupant::PurposeType::Residence:
		purposeName = "Residence";
		break;
	case cISC4BuildingOccupant::PurposeType::Services:
		purposeName = "Services";
		break;
	case cISC4BuildingOccupant::PurposeType::Office:
		purposeName = "Office";
		break;
	case cISC4BuildingOccupant::PurposeType::Tourism:
		purposeName = "Tourism";
		break;
	case cISC4BuildingOccupant::PurposeType::Agriculture:
		purposeName = "Agriculture";
		break;
	case cISC4BuildingOccupant::PurposeType::Processing:
		purposeName = "Processing";
		break;
	case cISC4BuildingOccupant::PurposeType::Manufacturing:
		purposeName = "Manufacturing";
		break;
	case cISC4BuildingOccupant::PurposeType::HighTech:
		purposeName = "HighTech";
		break;
	case cISC4BuildingOccupant::PurposeType::Other:
		purposeName = "Other";
		break;
	}

	Logger::GetInstance().WriteLineFormatted(
		LogLevel::Info,
		"0x%08x (%s): Purpose type %d (%s) does not support building styles.",
		id,
		name,
		static_cast<uint32_t>(purposeType),
		purposeName);
}

static void LogStyleSupported(
	uint32_t id,
	const char* const name,
	uint32_t style)
{
	Logger::GetInstance().WriteLineFormatted(
		LogLevel::Info,
		"0x%08x (%s) supports building style 0x%x.",
		id,
		name,
		style);
}

static void LogNoSupportedStyles(
	uint32_t id,
	const char* const name)
{
	Logger::GetInstance().WriteLineFormatted(
		LogLevel::Info,
		"0x%08x (%s) doesn't have any supported building styles.",
		id,
		name);
}

static void LogWallToWallStyleOptionFailure(
	uint32_t id,
	const char* const name,
	IBuildingSelectWinContext::WallToWallOption option)
{
	const char* description = nullptr;

	switch (option)
	{
	case IBuildingSelectWinContext::WallToWallOption::Only:
		description = "doesn't have a Wall-to-Wall occupant group.";
		break;
	case IBuildingSelectWinContext::WallToWallOption::Block:
		description = "has a Wall-to-Wall occupant group.";
		break;
	}

	if (description)
	{
		Logger::GetInstance().WriteLineFormatted(
			LogLevel::Info,
			"0x%08x (%s) %s.",
			id,
			name,
			description);
	}
}

static void LogGrowableFunctionResult(const char* const functionName, int32_t result)
{
	Logger::GetInstance().WriteLineFormatted(
		LogLevel::Info,
		"%s returned %d",
		functionName,
		result);
}

static bool DoesLotSupportBuildingStyles(
	const cSC4TractDeveloper* pThis,
	const cSC4LotConfiguration* pLotConfiguration,
	cISC4BuildingOccupant::PurposeType purpose)
{
	if (pThis->activeStyles.empty()
		|| !BuildingUtil::PurposeTypeSupportsBuildingStyles(purpose))
	{
		if (spPreferences->LogLotStyleSelection())
		{
			LogPurposeTypeDoesNotSupportStyles(
				pLotConfiguration->id,
				pLotConfiguration->name.AsIGZString()->ToChar(),
				purpose);
		}
		return false;
	}

	return true;
}

static bool CheckAdditionalLotStyleOptions(const cSC4LotConfiguration* pLotConfiguration)
{
	bool result = true;

	const IBuildingSelectWinContext& context = spBuildingSelectWinManager->GetContext();
	const IBuildingSelectWinContext::WallToWallOption wallToWallOption = context.GetWallToWallOption();

	if (wallToWallOption != IBuildingSelectWinContext::WallToWallOption::Mixed)
	{
		switch (wallToWallOption)
		{
		case IBuildingSelectWinContext::WallToWallOption::Only:
			result = LotConfigurationHasOccupantGroupValue(pLotConfiguration, WallToWallOccupantGroups);
			break;
		case IBuildingSelectWinContext::WallToWallOption::Block:
			result = !LotConfigurationHasOccupantGroupValue(pLotConfiguration, WallToWallOccupantGroups);
			break;
		}

		if (!result && spPreferences->LogLotStyleSelection())
		{
			LogWallToWallStyleOptionFailure(
				pLotConfiguration->id,
				pLotConfiguration->name.AsIGZString()->ToChar(),
				wallToWallOption);
		}
	}

	return result;
}

static void LogLotStyleSupported(const cSC4LotConfiguration* pLotConfiguration, uint32_t style)
{
	if (spPreferences->LogLotStyleSelection())
	{
		LogStyleSupported(
			pLotConfiguration->id,
			pLotConfiguration->name.AsIGZString()->ToChar(),
			style);
	}
}

template<bool isBuildingStyleProperty>
static bool CheckLotCompatibilityWithActiveStyles(
	const cSC4TractDeveloper* pThis,
	const cSC4LotConfiguration* pLotConfiguration,
	cISC4BuildingOccupant::PurposeType purpose)
{
	if (pThis->changeStylesEveryNYears == 0)
	{
		// Use all styles at once.

		const SC4Vector<uint32_t>& activeStyles = pThis->activeStyles;

		for (const auto& style : activeStyles)
		{
			if constexpr (isBuildingStyleProperty)
			{
				if (LotConfigurationHasOccupantGroupValue(pLotConfiguration, style))
				{
					LogLotStyleSupported(pLotConfiguration, style);
					return true;
				}
			}
			else
			{
				if (BuildingUtil::IsIndustrialBuilding(purpose))
				{
					// Industrial buildings without a BuildingStyles property
					// are compatible with all building styles.
					LogLotStyleSupported(pLotConfiguration, style);
					return true;
				}
				else
				{
					if (LotConfigurationHasOccupantGroupValue(pLotConfiguration, style))
					{
						LogLotStyleSupported(pLotConfiguration, style);
						return true;
					}
				}
			}
		}
	}
	else
	{
		// Change style every N years.

		const uint32_t activeStyle = pThis->activeStyles[pThis->currentStyleIndex];

		if constexpr (isBuildingStyleProperty)
		{
			if (LotConfigurationHasOccupantGroupValue(pLotConfiguration, activeStyle))
			{
				LogLotStyleSupported(pLotConfiguration, activeStyle);
				return true;
			}
		}
		else
		{
			if (BuildingUtil::IsIndustrialBuilding(purpose))
			{
				// Industrial buildings without a BuildingStyles property
				// are compatible with all building styles.
				LogLotStyleSupported(pLotConfiguration, activeStyle);
				return true;
			}
			else
			{
				if (LotConfigurationHasOccupantGroupValue(pLotConfiguration, activeStyle))
				{
					LogLotStyleSupported(pLotConfiguration, activeStyle);
					return true;
				}
			}
		}
	}

	if (spPreferences->LogLotStyleSelection())
	{
		LogNoSupportedStyles(
			pLotConfiguration->id,
			pLotConfiguration->name.AsIGZString()->ToChar());
	}
	return false;
}

static bool IsLotCompatibleWithActiveStyles(
	const cSC4TractDeveloper* pThis,
	const cSC4LotConfiguration* pLotConfiguration,
	cISC4BuildingOccupant::PurposeType purpose)
{
	if (!CheckAdditionalLotStyleOptions(pLotConfiguration))
	{
		// CheckAdditionalLotStyleOptions already wrote a failure log message.
		return false;
	}

	const bool hasBuildingStylesProperty = LotConfigurationHasOccupantGroupValue(pLotConfiguration, kBuildingStylesProperty);

	if (hasBuildingStylesProperty)
	{
		// CheckLotCompatibilityWithActiveStyles will write the success/failure log messages.
		return CheckLotCompatibilityWithActiveStyles<true>(pThis, pLotConfiguration, purpose);
	}
	else
	{
		// CheckLotCompatibilityWithActiveStyles will write the success/failure log messages.
		return CheckLotCompatibilityWithActiveStyles<false>(pThis, pLotConfiguration, purpose);
	}
}

static void NAKED_FUN IsLotConfigurationSuitable_BuildingStyleSelectionHook()
{
	__asm
	{
		push eax // store
		push ecx // store
		push edx // store
		push ebp // lot purpose type
		push edi // cISC4LotConfiguration*
		push esi // cSC4TractDeveloper this pointer
		call DoesLotSupportBuildingStyles // (cdecl)
		add esp, 12
		test al, al
		jz compatableStyleFound // If building styles are not supported report that the style is compatible
		push ebp // lot purpose type
		push edi // cISC4LotConfiguration*
		push esi // cSC4TractDeveloper this pointer
		call IsLotCompatibleWithActiveStyles // (cdecl)
		add esp, 12
		test al, al
		jz noCompatableStyleFound
		compatableStyleFound:
		pop edx // restore
		pop ecx // restore
		pop eax // restore
		push IsLotConfigurationSuitable_CompatableStyleFound_Continue
		ret
		noCompatableStyleFound:
		pop edx // restore
		pop ecx // restore
		pop eax // restore
		push IsLotConfigurationSuitable_NoCompatableStyle_Continue
		ret
	}
}

static bool DoesBuildingSupportStyles(
	const cSC4TractDeveloper* pThis,
	uint32_t buildingType,
	cISC4BuildingOccupant::PurposeType purpose)
{
	if (pThis->activeStyles.empty()
		|| !BuildingUtil::PurposeTypeSupportsBuildingStyles(purpose))
	{
		if (spPreferences->LogBuildingStyleSelection())
		{
			LogPurposeTypeDoesNotSupportStyles(
				buildingType,
				pThis->pBuildingDevelopmentSim->GetExemplarName(buildingType)->ToChar(),
				purpose);
		}

		return false;
	}

	return true;
}

static bool CheckAdditionalBuildingStyleOptions(
	const cSC4TractDeveloper* pThis,
	uint32_t buildingType,
	const cISCPropertyHolder* pPropertyHolder)
{
	bool result = true;

	const IBuildingSelectWinContext& context = spBuildingSelectWinManager->GetContext();
	const IBuildingSelectWinContext::WallToWallOption wallToWallOption = context.GetWallToWallOption();

	if (wallToWallOption != IBuildingSelectWinContext::WallToWallOption::Mixed)
	{
		switch (wallToWallOption)
		{
		case IBuildingSelectWinContext::WallToWallOption::Only:
			result = BuildingUtil::IsWallToWall(pPropertyHolder);
			break;
		case IBuildingSelectWinContext::WallToWallOption::Block:
			result = !BuildingUtil::IsWallToWall(pPropertyHolder);
			break;
		}

		if (!result && spPreferences->LogLotStyleSelection())
		{
			LogWallToWallStyleOptionFailure(
				buildingType,
				pThis->pBuildingDevelopmentSim->GetExemplarName(buildingType)->ToChar(),
				wallToWallOption);
		}
	}

	return result;
}

static void LogBuildingStyleSupported(
	const cSC4TractDeveloper* pThis,
	uint32_t buildingType,
	uint32_t style)
{
	if (spPreferences->LogBuildingStyleSelection())
	{
		LogStyleSupported(
			buildingType,
			pThis->pBuildingDevelopmentSim->GetExemplarName(buildingType)->ToChar(),
			style);
	}
}

template <bool isBuildingStylesProperty>
static bool BuildingHasStyleValue(
	const cSC4TractDeveloper* pThis,
	uint32_t buildingType,
	const PropertyData& propertyData,
	cISC4BuildingOccupant::PurposeType purposeType)
{
	if (pThis->changeStylesEveryNYears == 0)
	{
		// Use all styles at once.

		const SC4Vector<uint32_t>& activeStyles = pThis->activeStyles;

		for (const auto& style : activeStyles)
		{
			if constexpr (isBuildingStylesProperty)
			{
				if (propertyData.Contains(style))
				{
					LogBuildingStyleSupported(pThis, buildingType, style);
					return true;
				}
			}
			else
			{
				if (BuildingUtil::IsIndustrialBuilding(purposeType))
				{
					// Industrial buildings without the Building Styles property are
					// compatible with all styles.
					LogBuildingStyleSupported(pThis, buildingType, style);
					return true;
				}
				else
				{
					if (propertyData.Contains(style))
					{
						LogBuildingStyleSupported(pThis, buildingType, style);
						return true;
					}
				}
			}
		}
	}
	else
	{
		// Change style every N years.

		uint32_t activeStyle = pThis->activeStyles[pThis->currentStyleIndex];

		if constexpr (isBuildingStylesProperty)
		{
			if (propertyData.Contains(activeStyle))
			{
				LogBuildingStyleSupported(pThis, buildingType, activeStyle);
				return true;
			}
		}
		else
		{
			if (BuildingUtil::IsIndustrialBuilding(purposeType))
			{
				// Industrial buildings without the Building Styles property are
				// compatible with all styles.
				LogBuildingStyleSupported(pThis, buildingType, activeStyle);
				return true;
			}
			else
			{
				if (propertyData.Contains(activeStyle))
				{
					LogBuildingStyleSupported(pThis, buildingType, activeStyle);
					return true;
				}
			}
		}
	}

	if (spPreferences->LogBuildingStyleSelection())
	{
		LogNoSupportedStyles(
			buildingType,
			pThis->pBuildingDevelopmentSim->GetExemplarName(buildingType)->ToChar());
	}
	return false;
}

static const char* const GetCurrentDeveloperDescription(const cSC4TractDeveloper* pThis)
{
	switch (pThis->pCurrentDeveloper->GetSubType())
	{
	case 0x1010:
		return "R§";
	case 0x1020:
		return "R§§";
	case 0x1030:
		return "R§§§";
	case 0x3110:
		return "Cs§";
	case 0x3120:
		return "Cs§§";
	case 0x3130:
		return "Cs§§§";
	case 0x3320:
		return "Co§§";
	case 0x3330:
		return "Co§§§";
	case 0x4100:
		return "IR";
	case 0x4200:
		return "ID";
	case 0x4300:
		return "IM";
	case 0x4400:
		return "IHT";
	default:
		return "Unknown";
	}
}

static const char* const GetDevelopmentTypeDescription(const cSC4TractDeveloper::CandidateLot* pCandidateLot)
{
	switch (pCandidateLot->developmentType)
	{
	case cSC4TractDeveloper::CandidateLot::DevelopmentType::TakeOverAbandoned:
		return "Take Over Abandoned";
	case cSC4TractDeveloper::CandidateLot::DevelopmentType::LotSameSize:
		return "Same Size Lot";
	case cSC4TractDeveloper::CandidateLot::DevelopmentType::LotAggregation:
		return "Lot Aggregation";
	case cSC4TractDeveloper::CandidateLot::DevelopmentType::LotSubdivision:
		return "Lot Subdivision";
	default:
		return "Unknown";
	}
}

static const char* const GetLotStateString(cISC4Lot::HabitationState state)
{
	switch (state)
	{
	case cISC4Lot::HabitationState::Created:
		return "Created";
	case cISC4Lot::HabitationState::Ready:
		return "Ready";
	case cISC4Lot::HabitationState::Constructing:
		return "Constructing";
	case cISC4Lot::HabitationState::Occupied:
		return "Occupied";
	case cISC4Lot::HabitationState::Vacant:
		return "Vacant";
	case cISC4Lot::HabitationState::Destructing:
		return "Destructing";
	case cISC4Lot::HabitationState::Destroyed:
		return "Destroyed";
	default:
		return "Unknown";
	}
}

static void LogCandidateLotInfo(
	const cSC4TractDeveloper* pThis,
	const cSC4TractDeveloper::CandidateLot* pCandidateLot)
{
	if (spPreferences->LogCandidateLots())
	{
		Logger& logger = Logger::GetInstance();

		if (pCandidateLot->pExistingLot)
		{
			cISC4Lot* pLot = pCandidateLot->pExistingLot;

			const cISC4Lot::HabitationState state = pLot->GetState();

			logger.WriteLineFormatted(
				LogLevel::Info,
				"Developer=%s | Development-type=%s | cellX=%u | cellZ=%u | sizeX=%u | sizeZ=%u"
				" | facing=%u | bounds=[%u, %u, %u, %u] | lot-state=%s (%u) | slope=%f | score=%f",
				GetCurrentDeveloperDescription(pThis),
				GetDevelopmentTypeDescription(pCandidateLot),
				pCandidateLot->lotLocationX,
				pCandidateLot->lotLocationZ,
				pCandidateLot->lotSizeX,
				pCandidateLot->lotSizeZ,
				pCandidateLot->facing,
				pCandidateLot->lotBounds.topLeftX,
				pCandidateLot->lotBounds.topLeftY,
				pCandidateLot->lotBounds.bottomRightX,
				pCandidateLot->lotBounds.bottomRightY,
				GetLotStateString(state),
				static_cast<uint32_t>(state),
				pLot->GetSlope(),
				pCandidateLot->score);

			if (state == cISC4Lot::HabitationState::Occupied || state == cISC4Lot::HabitationState::Vacant)
			{
				cISC4BuildingOccupant* pBuilding = pLot->GetBuilding();
				cISC4LotConfiguration* pLotConfig = pLot->GetLotConfiguration();

				if (pBuilding && pLotConfig)
				{
					cRZBaseString lotName;
					pLotConfig->GetName(lotName);

					const cIGZString* pExemplarName = pBuilding->GetExemplarName();
					const cIGZString* pBuildingName = pBuilding->GetBuildingName();

					if (pExemplarName && pBuildingName)
					{
						logger.WriteLineFormatted(
							LogLevel::Info,
							"  Existing Lot: id=0x%08X | name=%s -- Building: id = 0x%08X"
							" | exemplar-name = %s | display-name = %s",
							pLotConfig->GetID(),
							lotName.ToChar(),
							pBuilding->GetBuildingType(),
							pExemplarName->ToChar(),
							pBuildingName->ToChar());
					}
				}
			}
		}
		else
		{
			logger.WriteLineFormatted(
				LogLevel::Info,
				"Developer=%s | Development-type=%s | cellX=%u | cellZ=%u | sizeX=%u | sizeZ=%u"
				" | facing=%u | bounds=[%u, %u, %u, %u] | score=%f",
				GetCurrentDeveloperDescription(pThis),
				GetDevelopmentTypeDescription(pCandidateLot),
				pCandidateLot->lotLocationX,
				pCandidateLot->lotLocationZ,
				pCandidateLot->lotSizeX,
				pCandidateLot->lotSizeZ,
				pCandidateLot->facing,
				pCandidateLot->lotBounds.topLeftX,
				pCandidateLot->lotBounds.topLeftY,
				pCandidateLot->lotBounds.bottomRightX,
				pCandidateLot->lotBounds.bottomRightY,
				pCandidateLot->score);
		}
	}
}

static bool BuildingHasStyleOccupantGroup(
	const cSC4TractDeveloper* pThis,
	uint32_t buildingType,
	cISC4BuildingOccupant::PurposeType purpose,
	const cSC4TractDeveloper::CandidateLot* pCandidateLot)
{
	LogCandidateLotInfo(pThis, pCandidateLot);

	bool result = false;

	cGZPersistResourceKey key;

	if (pThis->pBuildingDevelopmentSim->GetBuildingKeyFromType(buildingType, key))
	{
		cIGZPersistResourceManagerPtr pRM;

		if (pRM)
		{
			cRZAutoRefCount<cISCPropertyHolder> pPropertyHolder;

			if (pRM->GetResource(key, GZIID_cISCPropertyHolder, pPropertyHolder.AsPPVoid(), 0, nullptr))
			{
				// CheckAdditionalBuildingStyleOptions will write a log message if it fails.
				if (CheckAdditionalBuildingStyleOptions(pThis, buildingType, pPropertyHolder))
				{
					const cISCProperty* pProperty = pPropertyHolder->GetProperty(kBuildingStylesProperty);

					if (pProperty)
					{
						const cIGZVariant* pVariant = pProperty->GetPropertyValue();

						if (pVariant)
						{
							const PropertyData propertyData(*pVariant);

							result = BuildingHasStyleValue<true>(
								pThis,
								buildingType,
								propertyData,
								purpose);
						}
					}
					else
					{
						pProperty = pPropertyHolder->GetProperty(kOccupantGroupsProperty);

						if (pProperty)
						{
							const cIGZVariant* pVariant = pProperty->GetPropertyValue();

							if (pVariant)
							{
								const PropertyData propertyData(*pVariant);

								result = BuildingHasStyleValue<false>(
									pThis,
									buildingType,
									propertyData,
									purpose);
							}
						}
					}
				}
			}
		}
	}

	return result;
}

static uintptr_t IsBuildingCompatible_CompatableStyleFound_Continue;
static uintptr_t IsBuildingCompatible_NoCompatableStyle_Continue;

static void NAKED_FUN IsBuildingCompatible_BuildingStyleSelectionHook()
{
	__asm
	{
		// First we push EBX and XOR it with itself to set it to zero.
		// This is done by code that runs after our inject point, and all of the exit conditions after that point
		// include a POP EBX call before returning.
		push ebx
		xor ebx, ebx
		push eax // store
		push ecx // store
		push edx // store
		mov eax, dword ptr[edi]
		push eax // purpose
		push ebp // building type
		push esi // this pointer
		call DoesBuildingSupportStyles // (cdecl)
		add esp, 12
		test al, al
		jz compatableStyleFound // If building styles are not supported report that the style is compatible
		mov ecx, dword ptr[esp - 0xc + 0x10 + 0x3c]
		push ecx // CandidateLot pointer
		mov eax, dword ptr[edi]
		push eax // purpose
		push ebp // building type
		push esi // this pointer
		call BuildingHasStyleOccupantGroup
		add esp, 16
		test al, al
		jz noCompatableStyleFound
		compatableStyleFound:
		pop edx // restore
		pop ecx // restore
		pop eax // restore
		// EBX is not popped because SC4's code will handle that.
		push IsBuildingCompatible_CompatableStyleFound_Continue
		ret
		noCompatableStyleFound:
		pop edx // restore
		pop ecx // restore
		pop eax // restore
		// EBX is not popped because SC4's code will handle that.
		push IsBuildingCompatible_NoCompatableStyle_Continue
		ret
	}
}

static LotZoningOptions __fastcall GetLotZoningOptions(cISC4BuildingOccupant::PurposeType purposeType, void* edxUnused)
{
	LotZoningOptions result = LotZoningOptionNone;

	// Lot aggregation and subdivision is only disabled for the residential and commercial building purpose types.
	//
	// Lot aggregation and subdivision must always be active for the Processing, Manufacturing and High Tech
	// purpose types, the industrial medium and high density zones require it for anything to grow.
	// Agriculture zones only support aggregation, not subdivision. They are excluded because the game always
	// picks the same items when aggregation is disabled, but it appears to work fine otherwise.

	switch (purposeType)
	{
	case cISC4BuildingOccupant::PurposeType::Residence:
	case cISC4BuildingOccupant::PurposeType::Services:
	case cISC4BuildingOccupant::PurposeType::Office:
		result = spBuildingSelectWinManager->GetContext().GetLotZoningOptions();
		break;
	}

	return result;
}

static bool __fastcall DisableLotSubdivisionForPurposeType(cISC4BuildingOccupant::PurposeType purposeType, void* edxUnused)
{
	bool result = false;

	switch (purposeType)
	{
	case cISC4BuildingOccupant::PurposeType::Residence:
	case cISC4BuildingOccupant::PurposeType::Services:
	case cISC4BuildingOccupant::PurposeType::Office:
		result = (spBuildingSelectWinManager->GetContext().GetLotZoningOptions() & LotZoningOptionDisableSubdivision) != 0;
		break;
	case cISC4BuildingOccupant::PurposeType::Agriculture:
		// Agriculture lots don't support subdivision.
		result = true;
		break;
	}

	return result;
}

typedef int32_t(__thiscall* pfn_ThisParameter_Int32_Return)(cSC4TractDeveloper* pThis);

static pfn_ThisParameter_Int32_Return ListCandidateLots_Aggregation = nullptr;
static pfn_ThisParameter_Int32_Return ListCandidateLots_Subdivision = nullptr;

static uintptr_t Grow_LotAggregationAndSubdivisionHook_Continue;

static int32_t __fastcall ListCandidateLots_Aggregation_Proxy(cSC4TractDeveloper* pThis, void* edxUnused)
{
	int32_t result = ListCandidateLots_Aggregation(pThis);

	if (spPreferences->LogGrowableFunctions())
	{
		LogGrowableFunctionResult("ListCandidateLots_Aggregation", result);
	}

	return result;
}

static int32_t __fastcall ListCandidateLots_Subdivision_Proxy(cSC4TractDeveloper* pThis, void* edxUnused)
{
	int32_t result = ListCandidateLots_Subdivision(pThis);

	if (spPreferences->LogGrowableFunctions())
	{
		LogGrowableFunctionResult("ListCandidateLots_Subdivision", result);
	}

	return result;
}

static void NAKED_FUN Grow_LotAggregationAndSubdivisionHook()
{
	__asm
	{
		// The original code doesn't do any pushes, so we don't either.
		mov ecx, dword ptr[esp + 0x24] // building purpose type
		call GetLotZoningOptions // (fastcall)
		cmp eax, LotZoningOptionDisableAggregationAndSubdivision
		jz afterLotSubdivision
		cmp eax, LotZoningOptionDisableAggregation
		jz lotSubdivision
		mov ecx, esi
		call ListCandidateLots_Aggregation_Proxy // (fastcall)
		cmp eax, dword ptr [esp + 0x14]
		jle lotSubdivision
		mov dword ptr[esp + 0x14], eax

		lotSubdivision:
		mov ecx, dword ptr[esp + 0x24] // building purpose type
		call DisableLotSubdivisionForPurposeType // (fastcall)
		test al, al
		jnz afterLotSubdivision
		mov ecx, esi
		call ListCandidateLots_Subdivision_Proxy // (fastcall)
		cmp eax, dword ptr[esp + 0x14]
		jle afterLotSubdivision
		mov dword ptr[esp + 0x14], eax

		afterLotSubdivision:
		mov ecx, Grow_LotAggregationAndSubdivisionHook_Continue
		jmp ecx
	}
}

pfn_ThisParameter_Int32_Return TakeOverAbandoned = nullptr;
pfn_ThisParameter_Int32_Return ListPossibleLotConfigurations = nullptr;
pfn_ThisParameter_Int32_Return ListCandidateLots_Existing = nullptr;

typedef int32_t(__thiscall* pfn_Build_Functions)(cSC4TractDeveloper* pThis, void* candidateLot);

pfn_Build_Functions Build = nullptr;
pfn_Build_Functions BuildFarm = nullptr;

static int32_t __fastcall TakeOverAbandoned_Trampoline(cSC4TractDeveloper* pThis, void* edxUnused)
{
	int32_t result = TakeOverAbandoned(pThis);

	if (spPreferences->LogGrowableFunctions())
	{
		LogGrowableFunctionResult("TakeOverAbandoned", result);
	}

	return result;
}

static int32_t __fastcall ListPossibleLotConfigurations_Trampoline(cSC4TractDeveloper* pThis, void* edxUnused)
{
	int32_t result = ListPossibleLotConfigurations(pThis);

	if (spPreferences->LogGrowableFunctions())
	{
		LogGrowableFunctionResult("ListPossibleLotConfigurations", result);
	}

	return result;
}

static int32_t __fastcall ListCandidateLots_Existing_Trampoline(cSC4TractDeveloper* pThis, void* edxUnused)
{
	int32_t result = ListCandidateLots_Existing(pThis);

	if (spPreferences->LogGrowableFunctions())
	{
		LogGrowableFunctionResult("ListCandidateLots_Existing", result);
	}

	return result;
}

static int32_t __fastcall Build_Trampoline(cSC4TractDeveloper* pThis, void* edxUnused, void* candidateLot)
{
	int32_t result = Build(pThis, candidateLot);

	if (spPreferences->LogGrowableFunctions())
	{
		LogGrowableFunctionResult("Build", result);
	}

	return result;
}

static int32_t __fastcall BuildFarm_Trampoline(cSC4TractDeveloper* pThis, void* edxUnused, void* candidateLot)
{
	int32_t result = BuildFarm(pThis, candidateLot);

	if (spPreferences->LogGrowableFunctions())
	{
		LogGrowableFunctionResult("BuildFarm", result);
	}

	return result;
}

bool TractDeveloperHooks::GetKickOutLowerWealthValue(const cISC4TractDeveloper* pTractDeveloper)
{
	bool result = true;

	if (pTractDeveloper)
	{
		result = reinterpret_cast<const cSC4TractDeveloper*>(pTractDeveloper)->kickOutLowerWealth != 0;
	}

	return result;
}

void TractDeveloperHooks::SetKickOutLowerWealthValue(cISC4TractDeveloper* pTractDeveloper, bool value)
{
	if (pTractDeveloper)
	{
		reinterpret_cast<cSC4TractDeveloper*>(pTractDeveloper)->kickOutLowerWealth = value;
	}
}

void TractDeveloperHooks::Install(const Preferences& preferences)
{
	Logger& logger = Logger::GetInstance();

	uintptr_t IsLotConfigurationSuitable_BuildingStyleSelectionHook_Inject = 0;
	IsLotConfigurationSuitable_CompatableStyleFound_Continue = 0;
	IsLotConfigurationSuitable_NoCompatableStyle_Continue = 0;
	uintptr_t IsBuildingCompatible_BuildingStyleSelectionHook_Inject = 0;
	IsBuildingCompatible_CompatableStyleFound_Continue = 0;
	IsBuildingCompatible_NoCompatableStyle_Continue = 0;
	uintptr_t Grow_LotAggregationAndSubdivisionHook_Inject = 0;
	Grow_LotAggregationAndSubdivisionHook_Continue = 0;
	ListCandidateLots_Aggregation = nullptr;
	ListCandidateLots_Subdivision = nullptr;
	uintptr_t TakeOverAbandoned_Trampoline_Inject = 0;
	uintptr_t ListPossibleLotConfigurations_Trampoline_Inject = 0;
	uintptr_t ListCandidateLots_Existing_Trampoline_Inject = 0;
	uintptr_t Build_Trampoline_Inject = 0;
	uintptr_t BuildFarm_Trampoline_Inject = 0;
	TakeOverAbandoned = nullptr;
	ListPossibleLotConfigurations = nullptr;
	ListCandidateLots_Existing = nullptr;
	Build = nullptr;
	BuildFarm = nullptr;

	const uint16_t gameVersion = SC4VersionDetection::GetInstance().GetGameVersion();
	bool setCallbacks = false;

	switch (gameVersion)
	{
	case 641:
		IsLotConfigurationSuitable_BuildingStyleSelectionHook_Inject = 0x704ca2;
		IsLotConfigurationSuitable_CompatableStyleFound_Continue = 0x704d22;
		IsLotConfigurationSuitable_NoCompatableStyle_Continue = 0x704cda;
		IsBuildingCompatible_BuildingStyleSelectionHook_Inject = 0x704e78;
		IsBuildingCompatible_CompatableStyleFound_Continue = 0x704ef4;
		IsBuildingCompatible_NoCompatableStyle_Continue = 0x704ed3;
		Grow_LotAggregationAndSubdivisionHook_Inject = 0x70d509;
		Grow_LotAggregationAndSubdivisionHook_Continue = 0x70d532;
		ListCandidateLots_Aggregation = reinterpret_cast<pfn_ThisParameter_Int32_Return>(0x7097e0);
		ListCandidateLots_Subdivision = reinterpret_cast<pfn_ThisParameter_Int32_Return>(0x708c00);
		TakeOverAbandoned_Trampoline_Inject = 0x70d4d2;
		ListPossibleLotConfigurations_Trampoline_Inject = 0x70d4ea;
		ListCandidateLots_Existing_Trampoline_Inject = 0x70d4fa;
		Build_Trampoline_Inject = 0x70d647;
		BuildFarm_Trampoline_Inject = 0x70d63a;
		TakeOverAbandoned = reinterpret_cast<pfn_ThisParameter_Int32_Return>(0x70a3d0);
		ListPossibleLotConfigurations = reinterpret_cast<pfn_ThisParameter_Int32_Return>(0x709650);
		ListCandidateLots_Existing = reinterpret_cast<pfn_ThisParameter_Int32_Return>(0x708110);
		Build = reinterpret_cast<pfn_Build_Functions>(0x70b440);
		BuildFarm = reinterpret_cast<pfn_Build_Functions>(0x70b640);
		setCallbacks = true;
		break;
	}

	if (setCallbacks)
	{
		try
		{
			Patcher::InstallJump(
				IsLotConfigurationSuitable_BuildingStyleSelectionHook_Inject,
				reinterpret_cast<uintptr_t>(&IsLotConfigurationSuitable_BuildingStyleSelectionHook));
			Patcher::InstallJump(
				IsBuildingCompatible_BuildingStyleSelectionHook_Inject,
				reinterpret_cast<uintptr_t>(&IsBuildingCompatible_BuildingStyleSelectionHook));
			Patcher::InstallJump(
				Grow_LotAggregationAndSubdivisionHook_Inject,
				reinterpret_cast<uintptr_t>(&Grow_LotAggregationAndSubdivisionHook));

			if (preferences.LogGrowableFunctions())
			{
				Patcher::InstallCallHook(
					TakeOverAbandoned_Trampoline_Inject,
					reinterpret_cast<uintptr_t>(&TakeOverAbandoned_Trampoline));
				Patcher::InstallCallHook(
					ListPossibleLotConfigurations_Trampoline_Inject,
					reinterpret_cast<uintptr_t>(&ListPossibleLotConfigurations_Trampoline));
				Patcher::InstallCallHook(
					ListCandidateLots_Existing_Trampoline_Inject,
					reinterpret_cast<uintptr_t>(&ListCandidateLots_Existing_Trampoline));
				Patcher::InstallCallHook(
					Build_Trampoline_Inject,
					reinterpret_cast<uintptr_t>(&Build_Trampoline));
				Patcher::InstallCallHook(
					BuildFarm_Trampoline_Inject,
					reinterpret_cast<uintptr_t>(&BuildFarm_Trampoline));
			}

			logger.WriteLine(LogLevel::Info, "Installed the building style algorithm patch.");
		}
		catch (const wil::ResultException& e)
		{
			logger.WriteLineFormatted(
				LogLevel::Error,
				"Failed to install the building style algorithm patch.\n%s",
				e.what());
		}
	}
}
