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

#include "LotConfigurationManagerHooks.h"
#include "cGZPersistResourceKey.h"
#include "cIGZPersistResourceManager.h"
#include "cIGZVariant.h"
#include "cISCProperty.h"
#include "cISCPropertyHolder.h"
#include "cRZAutoRefCount.h"
#include "GZServPtrs.h"
#include "Logger.h"
#include "Patcher.h"
#include "PropertyIDs.h"
#include "SC4Vector.h"
#include "SC4VersionDetection.h"
#include "WallToWallOccupantGroups.h"
#include "wil/result.h"

#include <array>


static void CopyWallToWallOccupantGroupValues(
	const cISCPropertyHolder* pPropertyHolder,
	SC4Vector<uint32_t>& vector)
{
	const cISCProperty* pProperty = pPropertyHolder->GetProperty(kOccupantGroupsProperty);

	if (pProperty)
	{
		const cIGZVariant* pVariant = pProperty->GetPropertyValue();

		if (pVariant)
		{
			const uint32_t* pData = pVariant->RefUint32();
			const uint32_t count = pVariant->GetCount();

			for (uint32_t i = 0; i < count; i++)
			{
				const uint32_t occupantGroup = pData[i];

				if (WallToWallOccupantGroups.count(occupantGroup) != 0)
				{
					vector.push_back(occupantGroup);
					// Only one value is needed to identify the building as wall-to-wall.
					break;
				}
			}
		}
	}
}

static bool InsertPropertyValues(
	const cISCPropertyHolder* pPropertyHolder,
	uint32_t propertyID,
	SC4Vector<uint32_t>& vector,
	size_t reserveExtraSize = 0)
{
	bool result = false;

	const cISCProperty* pProperty = pPropertyHolder->GetProperty(propertyID);

	if (pProperty)
	{
		const cIGZVariant* pVariant = pProperty->GetPropertyValue();

		if (pVariant)
		{
			const uint16_t type = pVariant->GetType();

			if (type == cIGZVariant::Uint32Array)
			{
				const uint32_t count = pVariant->GetCount();

				if (count > 0)
				{
					vector.reserve(vector.size() + count + reserveExtraSize);
					vector.push_back(pVariant->RefUint32(), count);
					result = true;
				}
			}
			else if (type == cIGZVariant::Uint32)
			{
				vector.reserve(vector.size() + 1 + reserveExtraSize);
				vector.push_back(pVariant->GetValUint32());
				result = true;
			}
		}
	}

	return result;
}

static void __cdecl GetBuildingStyles(cGZPersistResourceKey const& key, SC4Vector<uint32_t>& vector)
{
	// This method replaces a call the gets the building's occupant groups.
	// The lot configuration class uses this information allow callers to check if any of the
	// lot's possible buildings have a specific building style.

	vector.clear();

	cIGZPersistResourceManagerPtr pRM;

	if (pRM)
	{
		cRZAutoRefCount<cISCPropertyHolder> pPropertyHolder;

		if (pRM->GetResource(key, GZIID_cISCPropertyHolder, pPropertyHolder.AsPPVoid(), 0, nullptr))
		{
			if (InsertPropertyValues(pPropertyHolder, kBuildingStylesProperty, vector, 1))
			{
				// Add the BuildingStyles property id to indicate that the property is present.
				vector.push_back(kBuildingStylesProperty);

				// Copy over the wall-to-wall (W2W) occupant group value, if present.
				// No other occupant group values are required for the lot style checks.
				CopyWallToWallOccupantGroupValues(pPropertyHolder, vector);
			}
			else
			{
				InsertPropertyValues(pPropertyHolder, kOccupantGroupsProperty, vector);
			}
		}
	}
}

void LotConfigurationManagerHooks::Install()
{
	Logger& logger = Logger::GetInstance();

	uintptr_t InitBuildingInformation_Inject = 0;

	const uint16_t gameVersion = SC4VersionDetection::GetInstance().GetGameVersion();
	bool setCallbacks = false;

	switch (gameVersion)
	{
	case 641:
		InitBuildingInformation_Inject = 0x48AE4B;
		setCallbacks = true;
		break;
	}

	if (setCallbacks)
	{
		try
		{
			Patcher::InstallCallHook(InitBuildingInformation_Inject, reinterpret_cast<uintptr_t>(&GetBuildingStyles));

			logger.WriteLine(LogLevel::Info, "Installed the lot configuration manager patch.");
		}
		catch (const wil::ResultException& e)
		{
			logger.WriteLineFormatted(
				LogLevel::Error,
				"Failed to install the lot configuration manager patch.\n%s",
				e.what());
		}
	}
}
