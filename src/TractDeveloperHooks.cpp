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
#include "GlobalPointers.h"
#include "GZServPtrs.h"
#include "Logger.h"
#include "Patcher.h"
#include "SC4String.h"
#include "SC4VersionDetection.h"

#include "frozen/unordered_set.h"
#include "wil/result.h"

static constexpr frozen::unordered_set<uint32_t, 6> WallToWallOccupantGroups =
{
	0xD02C802E, // More Building Styles DLL W2W
	0xB5C00A05, // BTE: Comm. W2W
	0xB5C00B05, // BTE: Res. W2W
	0xB5C00DDE, // BTE: W2W General
	0xB5C00F0A, // SFBT: Hamburg W2W
	0xB5C00F0B, // SFBT: Paris W2W
};

template<typename T> class SC4VectorIterator
{
public:
	using iterator_category = std::forward_iterator_tag;
	using difference_type = std::ptrdiff_t;
	using value_type = T;
	using pointer = value_type*;
	using reference = value_type&;

	SC4VectorIterator(pointer pValue) : mpValue(pValue)
	{
	}

	~SC4VectorIterator()
	{
	}

	reference operator*() const
	{
		return *mpValue;
	}

	pointer operator->()
	{
		return mpValue;
	}

	// Prefix increment
	SC4VectorIterator& operator++()
	{
		mpValue = mpValue + 1;
		return *this;
	}

	// Postfix increment
	SC4VectorIterator operator++(int)
	{
		SC4VectorIterator tmp = *this;
		++(*this);
		return tmp;
	}

	static friend bool operator== (const SC4VectorIterator& a, const SC4VectorIterator& b)
	{
		return a.mpValue == b.mpValue;
	};
	static friend bool operator!= (const SC4VectorIterator& a, const SC4VectorIterator& b)
	{
		return a.mpValue != b.mpValue;
	};
private:
	pointer mpValue;
};

template<typename T> class SC4Vector
{
public:
	typedef SC4VectorIterator<const T> const_iterator;

	const_iterator begin() const
	{
		return const_iterator(mpBegin);
	}

	const_iterator end() const
	{
		return const_iterator(mpEnd);
	}

	const_iterator cbegin() const
	{
		return const_iterator(mpBegin);
	}

	const_iterator cend() const
	{
		return const_iterator(mpEnd);
	}

	bool empty() const
	{
		return mpBegin == mpEnd;
	}

	T operator[](size_t index) const
	{
		return mpBegin[index];
	}

private:
	T* mpBegin;
	T* mpEnd;
	void* pAllocator;
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

	for (const auto& occupantGroup : buildingOccupantGroups)
	{
		if (occupantGroup == style)
		{
			return true;
		}
	}

	return false;
}

template<size_t N>
static bool LotConfigurationHasStyle(
	const cSC4LotConfiguration* pLotConfiguration,
	const frozen::unordered_set<uint32_t, N>& styles)
{
	const SC4Vector<uint32_t>& buildingOccupantGroups = pLotConfiguration->buildingOccupantGroups;

	for (const auto& occupantGroup : buildingOccupantGroups)
	{
		if (styles.find(occupantGroup) != styles.end())
		{
			return true;
		}
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

template<size_t N>
static bool HasOccupantGroupValue(
	const uint32_t* const pOccupantGroupData,
	uint32_t occupantGroupCount,
	const frozen::unordered_set<uint32_t, N>& styles)
{
	for (uint32_t i = 0; i < occupantGroupCount; i++)
	{
		if (styles.find(pOccupantGroupData[i]) != styles.end())
		{
			return true;
		}
	}

	return false;
}

static bool PurposeTypeSupportsBuildingStyles(cISC4BuildingOccupant::PurposeType purpose)
{
	switch (purpose)
	{
	case cISC4BuildingOccupant::PurposeType::Residence:
	case cISC4BuildingOccupant::PurposeType::Services:
	case cISC4BuildingOccupant::PurposeType::Office:
		return true;
	case cISC4BuildingOccupant::PurposeType::Agriculture:
		return spPreferences->AgriculturePurposeTypeSupportsBuildingStyles();
	case cISC4BuildingOccupant::PurposeType::Processing:
		return spPreferences->ProcessingPurposeTypeSupportsBuildingStyles();
	case cISC4BuildingOccupant::PurposeType::Manufacturing:
		return spPreferences->ManufacturingPurposeTypeSupportsBuildingStyles();
	case cISC4BuildingOccupant::PurposeType::HighTech:
		return spPreferences->HighTechPurposeTypeSupportsBuildingStyles();
	case cISC4BuildingOccupant::PurposeType::None:
	case cISC4BuildingOccupant::PurposeType::Tourism:
	case cISC4BuildingOccupant::PurposeType::Other:
	default:
		return false;
	}
}

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

static bool DoesLotSupportBuildingStyles(
	const cSC4TractDeveloper* pThis,
	const cSC4LotConfiguration* pLotConfiguration,
	cISC4BuildingOccupant::PurposeType purpose)
{
	if (pThis->activeStyles.empty()
		|| !PurposeTypeSupportsBuildingStyles(purpose))
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
			result = LotConfigurationHasStyle(pLotConfiguration, WallToWallOccupantGroups);
			break;
		case IBuildingSelectWinContext::WallToWallOption::Block:
			result = !LotConfigurationHasStyle(pLotConfiguration, WallToWallOccupantGroups);
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

static bool IsLotCompatibleWithActiveStyles(
	const cSC4TractDeveloper* pThis,
	const cSC4LotConfiguration* pLotConfiguration)
{
	if (!CheckAdditionalLotStyleOptions(pLotConfiguration))
	{
		// CheckAdditionalLotStyleOptions already wrote a failure log message.
		return false;
	}

	if (pThis->notUsingAllStylesAtOnce == 0)
	{
		// Use all styles at once.

		const SC4Vector<uint32_t>& activeStyles = pThis->activeStyles;

		for (const auto& style : activeStyles)
		{
			if (LotConfigurationHasStyle(pLotConfiguration, style))
			{
				if (spPreferences->LogLotStyleSelection())
				{
					LogStyleSupported(
						pLotConfiguration->id,
						pLotConfiguration->name.AsIGZString()->ToChar(),
						style);
				}
				return true;
			}
		}

		if (spPreferences->LogLotStyleSelection())
		{
			LogNoSupportedStyles(
				pLotConfiguration->id,
				pLotConfiguration->name.AsIGZString()->ToChar());
		}
	}
	else
	{
		// Change style every N years.
		if (LotConfigurationHasStyle(pLotConfiguration, pThis->activeStyles[pThis->currentStyleIndex]))
		{
			if (spPreferences->LogLotStyleSelection())
			{
				LogStyleSupported(
					pLotConfiguration->id,
					pLotConfiguration->name.AsIGZString()->ToChar(),
					pThis->activeStyles[pThis->currentStyleIndex]);
			}
			return true;
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
		push edi // cISC4LotConfiguration*
		push esi // cSC4TractDeveloper this pointer
		call IsLotCompatibleWithActiveStyles // (cdecl)
		add esp, 8
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
		|| !PurposeTypeSupportsBuildingStyles(purpose))
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
	const uint32_t* const pOccupantGroupData,
	uint32_t occupantGroupCount)
{
	bool result = true;

	const IBuildingSelectWinContext& context = spBuildingSelectWinManager->GetContext();
	const IBuildingSelectWinContext::WallToWallOption wallToWallOption = context.GetWallToWallOption();

	if (wallToWallOption != IBuildingSelectWinContext::WallToWallOption::Mixed)
	{
		switch (wallToWallOption)
		{
		case IBuildingSelectWinContext::WallToWallOption::Only:
			result = HasOccupantGroupValue(pOccupantGroupData, occupantGroupCount, WallToWallOccupantGroups);
			break;
		case IBuildingSelectWinContext::WallToWallOption::Block:
			result = !HasOccupantGroupValue(pOccupantGroupData, occupantGroupCount, WallToWallOccupantGroups);
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

static bool BuildingHasStyleOccupantGroup(
	const cSC4TractDeveloper* pThis,
	uint32_t buildingType)
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

						// CheckAdditionalBuildingStyleOptions will write a log message if it fails.
						if (CheckAdditionalBuildingStyleOptions(pThis, buildingType, pOccupantGroupData, count))
						{
							if (pThis->notUsingAllStylesAtOnce == 0)
							{
								// Use all styles at once.

								const SC4Vector<uint32_t>& activeStyles = pThis->activeStyles;

								for (const auto& style : activeStyles)
								{
									if (HasOccupantGroupValue(pOccupantGroupData, count, style))
									{
										if (spPreferences->LogBuildingStyleSelection())
										{
											LogStyleSupported(
												buildingType,
												pThis->pBuildingDevelopmentSim->GetExemplarName(buildingType)->ToChar(),
												style);
										}
										result = true;
										break;
									}
								}

								if (!result && spPreferences->LogBuildingStyleSelection())
								{
									LogNoSupportedStyles(
										buildingType,
										pThis->pBuildingDevelopmentSim->GetExemplarName(buildingType)->ToChar());
								}
							}
							else
							{
								// Change style every N years.

								uint32_t activeStyle = pThis->activeStyles[pThis->currentStyleIndex];

								result = HasOccupantGroupValue(pOccupantGroupData, count, activeStyle);
								if (spPreferences->LogBuildingStyleSelection())
								{
									if (result)
									{
										LogStyleSupported(
											buildingType,
											pThis->pBuildingDevelopmentSim->GetExemplarName(buildingType)->ToChar(),
											activeStyle);
									}
									else
									{
										LogNoSupportedStyles(
											buildingType,
											pThis->pBuildingDevelopmentSim->GetExemplarName(buildingType)->ToChar());
									}
								}
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
		push ebp // building type
		push esi // this pointer
		call BuildingHasStyleOccupantGroup
		add esp, 8
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
