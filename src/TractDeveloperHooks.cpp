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
#include "cISC4BuildingOccupant.h"
#include "cISC4LotConfiguration.h"
#include "cISC4TractDeveloper.h"
#include "Logger.h"
#include "Patcher.h"
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
	void* vtable;                        // 0x0
	void* messageTarget2;                // 0x4
	void* gzSerialzable;                 // 0x8
	int initialized;                     // 0xc
	int refCount;                        // 0x10
	void* pMS2;                          // 0x14
	void* pCity;                         // 0x18
	void* pBuildingDevelopmentSim;       // 0x1c
	void* pLandValueSim;                 // 0x20
	void* pLotConfigurationManager;      // 0x24
	void* pLotDeveloper;                 // 0x28
	void* pLotManager;                   // 0x2c
	void* pPlumbingSim;                  // 0x30
	void* pPowerSim;                     // 0x34
	void* pTrafficSim;                   // 0x38
	void* pZoneManager;                  // 0x3c
	// begin tunable values
	float overbuild;                     // 0x40
	float reoccupyThresholds[2];         // 0x44
	float redevelopSizeThreshold;        // 0x4c
	float demolitionCosts[2];            // 0x50
	float newLotCosts[2];                // 0x58
	float occupancyVariation;            // 0x60
	float minVacancyRange[2];            // 0x64
	int kickOutLowerWealth;              // 0x6c
	SC4Vector<uint32_t> availableStyles; // 0x70
	int yearsBetweenStyles;              // 0x7c
	// end tunable values
	uint8_t unknown1[0x98];              // 0x80 - 0x117
	SC4Vector<uint32_t> activeStyles;    // 0x118
	uint32_t currentStyleIndex;          // 0x124
	uint32_t yearsPassed;                // 0x128
	uint32_t notUsingAllStylesAtOnce;    // 0x12c
};

static_assert(offsetof(cSC4TractDeveloper, initialized) == 0xc);
static_assert(offsetof(cSC4TractDeveloper, pZoneManager) == 0x3c);
static_assert(offsetof(cSC4TractDeveloper, kickOutLowerWealth) == 0x6c);
static_assert(offsetof(cSC4TractDeveloper, yearsBetweenStyles) == 0x7c);
static_assert(offsetof(cSC4TractDeveloper, activeStyles) == 0x118);
static_assert(offsetof(cSC4TractDeveloper, currentStyleIndex) == 0x124);
static_assert(offsetof(cSC4TractDeveloper, notUsingAllStylesAtOnce) == 0x12c);

static uintptr_t IsLotConfigurationSuitable_CompatableStyleFound_Continue;
static uintptr_t IsLotConfigurationSuitable_NoCompatableStyle_Continue;

//#define HAVE_LOT_CONFIG

static void NAKED_FUN IsLotConfigurationSuitable_BuildingStyleSelectionHook()
{
	static const cSC4TractDeveloper* pThis;
#ifdef HAVE_LOT_CONFIG
	static cISC4LotConfiguration* pLotConfiguration;
#endif // HAVE_LOT_CONFIG

	static cISC4BuildingOccupant::PurposeType lotPurpose;
	static const uint32_t* pStyle;

	_asm mov pThis, esi
	_asm mov lotPurpose, ebp
#ifdef HAVE_LOT_CONFIG
	_asm mov pLotConfiguration, edi
#endif // HAVE_LOT_CONFIG
	_asm pushad

	if (pThis->activeStyles.empty()
		|| lotPurpose < cISC4BuildingOccupant::PurposeType::Residence
		|| lotPurpose > cISC4BuildingOccupant::PurposeType::Office)
	{
		_asm popad
		_asm push IsLotConfigurationSuitable_CompatableStyleFound_Continue
		_asm ret
	}

	if (pThis->notUsingAllStylesAtOnce == 0)
	{
		// Use all styles at once.
#ifdef HAVE_LOT_CONFIG
		pStyle = pThis->activeStyles.mpBegin;

		while (pStyle != pThis->activeStyles.mpEnd)
		{
			if (pLotConfiguration->IsCompatibleWithStyleType(*pStyle))
			{
				_asm popad
				_asm push IsLotConfigurationSuitable_CompatableStyleFound_Continue
				_asm ret
			}
			++pStyle;
		}
#else
		_asm popad
		_asm push 0x704cf6
		_asm ret
#endif // HAVE_LOT_CONFIG

		}
	else
	{
		// Change style every N years.
#ifdef HAVE_LOT_CONFIG
		if (pLotConfiguration->IsCompatibleWithStyleType(pThis->activeStyles[pThis->currentStyleIndex]))
		{
			_asm popad
			_asm push IsLotConfigurationSuitable_CompatableStyleFound_Continue
			_asm ret
		}
#else
		_asm popad
		_asm push 0x704cc3
		_asm ret
#endif // HAVE_LOT_CONFIG
	}

	_asm popad
	_asm push IsLotConfigurationSuitable_NoCompatableStyle_Continue
	_asm ret
}

void TractDeveloperHooks::Install()
{
	Logger& logger = Logger::GetInstance();

	uintptr_t IsLotConfigurationSuitable_BuildingStyleSelectionHook_Inject = 0;
	IsLotConfigurationSuitable_CompatableStyleFound_Continue = 0;
	IsLotConfigurationSuitable_NoCompatableStyle_Continue = 0;

	const uint16_t gameVersion = SC4VersionDetection::GetInstance().GetGameVersion();
	bool setCallbacks = false;

	switch (gameVersion)
	{
	case 641:
		IsLotConfigurationSuitable_BuildingStyleSelectionHook_Inject = 0x704ca2;
		IsLotConfigurationSuitable_CompatableStyleFound_Continue = 0x704d22;
		IsLotConfigurationSuitable_NoCompatableStyle_Continue = 0x704cda;
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
