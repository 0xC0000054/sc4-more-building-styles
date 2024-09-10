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
#include "cGZPersistResourceKey.h"
#include "cIGZPersistResourceManager.h"
#include "cIGZString.h"
#include "cIGZVariant.h"
#include "cISCProperty.h"
#include "cISCPropertyHolder.h"
#include "cISC4BuildingDevelopmentSimulator.h"
#include "cISC4BuildingOccupant.h"
#include "cISC4LotConfiguration.h"
#include "cRZAutoRefCount.h"
#include "GZServPtrs.h"
#include "Logger.h"
#include "Patcher.h"
#include "SC4String.h"
#include "SC4VersionDetection.h"

#include "wil/result.h"

template<typename T> struct SC4Vector
{
	T* mpBegin;
	T* mpEnd;
	void* pAllocator;

	bool empty() const
	{
		return mpBegin == mpEnd;
	}

	T operator[](size_t index) const
	{
		return mpBegin[index];
	}
};

struct cSC4TractDeveloper
{
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
	int kickOutLowerWealth;										// 0x6c
	SC4Vector<uint32_t> availableStyles;						// 0x70
	int yearsBetweenStyles;										// 0x7c
	// end tunable values
	uint8_t unknown1[0x98];										// 0x80 - 0x117
	SC4Vector<uint32_t> activeStyles;							// 0x118
	uint32_t currentStyleIndex;									// 0x124
	uint32_t yearsPassed;										// 0x128
	uint32_t notUsingAllStylesAtOnce;							// 0x12c
};

static_assert(offsetof(cSC4TractDeveloper, initialized) == 0xc);
static_assert(offsetof(cSC4TractDeveloper, pZoneManager) == 0x3c);
static_assert(offsetof(cSC4TractDeveloper, kickOutLowerWealth) == 0x6c);
static_assert(offsetof(cSC4TractDeveloper, yearsBetweenStyles) == 0x7c);
static_assert(offsetof(cSC4TractDeveloper, activeStyles) == 0x118);
static_assert(offsetof(cSC4TractDeveloper, currentStyleIndex) == 0x124);
static_assert(offsetof(cSC4TractDeveloper, notUsingAllStylesAtOnce) == 0x12c);

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

static bool LotConfigurationHasStyle(const cSC4LotConfiguration* pLotConfiguration, uint32_t style)
{
	const SC4Vector<uint32_t>& buildingOccupantGroups = pLotConfiguration->buildingOccupantGroups;

	const uint32_t* pOccupantGroup = buildingOccupantGroups.mpBegin;

	while (pOccupantGroup != buildingOccupantGroups.mpEnd)
	{
		if (*pOccupantGroup == style)
		{
			return true;
		}

		++pOccupantGroup;
	}

	return false;
}


static bool HasOccupantGroupValue(
	const uint32_t* const pOccupantGroupData,
	uint32_t occupantGroupCount,
	uint32_t value)
{
	for (uint32_t i = 0; i < occupantGroupCount; i++)
	{
		if (pOccupantGroupData[i] == value)
		{
			return true;
		}
	}

	return false;
}

static void LotPurposeTypeDoesNotSupportStyles(
	const cSC4LotConfiguration* pLotConfiguration,
	cISC4BuildingOccupant::PurposeType purposeType)
{
	const char* name = "Unknown";

	switch (purposeType)
	{
	case cISC4BuildingOccupant::PurposeType::None:
		name = "None";
		break;
	case cISC4BuildingOccupant::PurposeType::Residence:
		name = "Residence";
		break;
	case cISC4BuildingOccupant::PurposeType::Services:
		name = "Services";
		break;
	case cISC4BuildingOccupant::PurposeType::Office:
		name = "Office";
		break;
	case cISC4BuildingOccupant::PurposeType::Tourism:
		name = "Tourism";
		break;
	case cISC4BuildingOccupant::PurposeType::Agriculture:
		name = "Agriculture";
		break;
	case cISC4BuildingOccupant::PurposeType::Processing:
		name = "Processing";
		break;
	case cISC4BuildingOccupant::PurposeType::Manufacturing:
		name = "Manufacturing";
		break;
	case cISC4BuildingOccupant::PurposeType::HighTech:
		name = "HighTech";
		break;
	case cISC4BuildingOccupant::PurposeType::Other:
		name = "Other";
		break;
	}

	Logger::GetInstance().WriteLineFormatted(
		LogLevel::Info,
		"0x%08x (%s): Purpose type %d (%s) does not support building styles.",
		pLotConfiguration->id,
		pLotConfiguration->name.AsIGZString()->ToChar(),
		static_cast<uint32_t>(purposeType),
		name);
}

static void LogStyleSupported(const cSC4LotConfiguration* pLotConfiguration, uint32_t style)
{
	Logger::GetInstance().WriteLineFormatted(
		LogLevel::Info,
		"0x%08x (%s) supports building style 0x%x.",
		pLotConfiguration->id,
		pLotConfiguration->name.AsIGZString()->ToChar(),
		style);
}

static void LogNoSupportedStyles(const cSC4LotConfiguration* pLotConfiguration)
{
	Logger::GetInstance().WriteLineFormatted(
		LogLevel::Info,
		"0x%08x (%s) doesn't have any supported building styles.",
		pLotConfiguration->id,
		pLotConfiguration->name.AsIGZString()->ToChar());
}

static void NAKED_FUN IsLotConfigurationSuitable_BuildingStyleSelectionHook()
{
	static const cSC4TractDeveloper* pThis;
	static const cSC4LotConfiguration* pLotConfiguration;
	static cISC4BuildingOccupant::PurposeType lotPurpose;
	static const uint32_t* pStyle;

	_asm mov pThis, esi
	_asm mov lotPurpose, ebp
	_asm mov pLotConfiguration, edi
	_asm pushad

	if (pThis->activeStyles.empty()
		|| lotPurpose < cISC4BuildingOccupant::PurposeType::Residence
		|| lotPurpose > cISC4BuildingOccupant::PurposeType::Office)
	{
		LotPurposeTypeDoesNotSupportStyles(pLotConfiguration, lotPurpose);

		_asm popad
		_asm push IsLotConfigurationSuitable_CompatableStyleFound_Continue
		_asm ret
	}

	if (pThis->notUsingAllStylesAtOnce == 0)
	{
		// Use all styles at once.
		pStyle = pThis->activeStyles.mpBegin;

		while (pStyle != pThis->activeStyles.mpEnd)
		{
			if (LotConfigurationHasStyle(pLotConfiguration, *pStyle))
			{
				LogStyleSupported(pLotConfiguration, *pStyle);

				_asm popad
				_asm push IsLotConfigurationSuitable_CompatableStyleFound_Continue
				_asm ret
			}
			++pStyle;
		}

		LogNoSupportedStyles(pLotConfiguration);
	}
	else
	{
		// Change style every N years.
		if (LotConfigurationHasStyle(pLotConfiguration, pThis->activeStyles[pThis->currentStyleIndex]))
		{
			LogStyleSupported(pLotConfiguration, pThis->activeStyles[pThis->currentStyleIndex]);

			_asm popad
			_asm push IsLotConfigurationSuitable_CompatableStyleFound_Continue
			_asm ret
		}

		LogNoSupportedStyles(pLotConfiguration);
	}

	_asm popad
	_asm push IsLotConfigurationSuitable_NoCompatableStyle_Continue
	_asm ret
}

static void BuildingPurposeTypeDoesNotSupportStyles(
	const cSC4TractDeveloper* pThis,
	uint32_t buildingType,
	cISC4BuildingOccupant::PurposeType purposeType)
{
	const char* name = "Unknown";

	switch (purposeType)
	{
	case cISC4BuildingOccupant::PurposeType::None:
		name = "None";
		break;
	case cISC4BuildingOccupant::PurposeType::Residence:
		name = "Residence";
		break;
	case cISC4BuildingOccupant::PurposeType::Services:
		name = "Services";
		break;
	case cISC4BuildingOccupant::PurposeType::Office:
		name = "Office";
		break;
	case cISC4BuildingOccupant::PurposeType::Tourism:
		name = "Tourism";
		break;
	case cISC4BuildingOccupant::PurposeType::Agriculture:
		name = "Agriculture";
		break;
	case cISC4BuildingOccupant::PurposeType::Processing:
		name = "Processing";
		break;
	case cISC4BuildingOccupant::PurposeType::Manufacturing:
		name = "Manufacturing";
		break;
	case cISC4BuildingOccupant::PurposeType::HighTech:
		name = "HighTech";
		break;
	case cISC4BuildingOccupant::PurposeType::Other:
		name = "Other";
		break;
	}

	Logger::GetInstance().WriteLineFormatted(
		LogLevel::Info,
		"0x%08x (%s): Purpose type %u (%s) does not support building styles.",
		buildingType,
		pThis->pBuildingDevelopmentSim->GetExemplarName(buildingType)->ToChar(),
		static_cast<uint32_t>(purposeType),
		name);
}

static void BuildingSupportsStyle(
	const cSC4TractDeveloper* pThis,
	uint32_t buildingType,
	uint32_t style)
{
	Logger::GetInstance().WriteLineFormatted(
		LogLevel::Info,
		"0x%08x (%s) supports building style 0x%x.",
		buildingType,
		pThis->pBuildingDevelopmentSim->GetExemplarName(buildingType)->ToChar(),
		style);
}

static void BuildingDoesNotSupportStyle(
	const cSC4TractDeveloper* pThis,
	uint32_t buildingType)
{
	Logger::GetInstance().WriteLineFormatted(
		LogLevel::Info,
		"0x%08x (%s) doesn't have any supported building styles.",
		buildingType,
		pThis->pBuildingDevelopmentSim->GetExemplarName(buildingType)->ToChar());
}

static bool __stdcall BuildingSupportsStyle(const cSC4TractDeveloper* pThis, uint32_t buildingType)
{
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
				constexpr uint32_t kOccupantGroupsProperty = 0xAA1DD396;

				const cISCProperty* pProperty = pPropertyHolder->GetProperty(kOccupantGroupsProperty);

				if (pProperty)
				{
					const cIGZVariant* pVariant = pProperty->GetPropertyValue();

					if (pVariant)
					{
						const uint32_t* const pOccupantGroupData = pVariant->RefUint32();
						const uint32_t count = pVariant->GetCount();

						if (pThis->notUsingAllStylesAtOnce == 0)
						{
							// Use all styles at once.
							const uint32_t* pStyle = pThis->activeStyles.mpBegin;

							while (pStyle != pThis->activeStyles.mpEnd)
							{
								if (HasOccupantGroupValue(pOccupantGroupData, count, *pStyle))
								{
#ifdef _DEBUG
									BuildingSupportsStyle(pThis, buildingType, *pStyle);
#endif // _DEBUG
									result = true;
									break;
								}
								++pStyle;
							}

#ifdef _DEBUG
							if (!result)
							{
								BuildingDoesNotSupportStyle(pThis, buildingType);
							}
#endif // _DEBUG
						}
						else
						{
							uint32_t activeStyle = pThis->activeStyles[pThis->currentStyleIndex];

							result = HasOccupantGroupValue(pOccupantGroupData, count, activeStyle);
#ifdef _DEBUG
							if (result)
							{
								BuildingSupportsStyle(pThis, buildingType, activeStyle);
							}
							else
							{
								BuildingDoesNotSupportStyle(pThis, buildingType);
							}
#endif // _DEBUG
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
	static const cSC4TractDeveloper* pThis;
	static cISC4BuildingOccupant::PurposeType buildingPurpose;
	static uint32_t buildingType;
	static const uint32_t* pStyle;

	_asm pushad
	_asm mov pThis, esi
	_asm mov eax, dword ptr[edi]
	_asm mov buildingPurpose, eax
	_asm mov eax, dword ptr[esp + 0x38]
	_asm mov buildingType, eax

	if (pThis->activeStyles.empty()
		|| buildingPurpose < cISC4BuildingOccupant::PurposeType::Residence
		|| buildingPurpose > cISC4BuildingOccupant::PurposeType::Office)
	{
		BuildingPurposeTypeDoesNotSupportStyles(pThis, buildingType, buildingPurpose);
		_asm popad
		_asm push IsBuildingCompatible_CompatableStyleFound_Continue
		_asm ret
	}

	if (BuildingSupportsStyle(pThis, buildingType))
	{
		_asm popad
		_asm push IsBuildingCompatible_CompatableStyleFound_Continue
		_asm ret
	}

	_asm popad
	_asm push IsBuildingCompatible_NoCompatableStyle_Continue
	_asm ret
}

void TractDeveloperHooks::Install()
{
	Logger& logger = Logger::GetInstance();

	uintptr_t IsLotConfigurationSuitable_BuildingStyleSelectionHook_Inject = 0;
	IsLotConfigurationSuitable_CompatableStyleFound_Continue = 0;
	IsLotConfigurationSuitable_NoCompatableStyle_Continue = 0;
	uintptr_t IsBuildingCompatible_BuildingStyleSelectionHook_Inject = 0;
	IsBuildingCompatible_CompatableStyleFound_Continue = 0;
	IsBuildingCompatible_NoCompatableStyle_Continue = 0;

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
