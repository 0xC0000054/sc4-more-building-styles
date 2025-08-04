/*
 * This file is part of sc4-more-building-styles, a DLL Plugin for
 * SimCity 4 that adds support for more building styles.
 *
 * Copyright (C) 2024, 2025 Nicholas Hayes
 *
 * sc4-more-building-styles is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * sc4-more-building-styles is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with sc4-more-building-styles.
 * If not, see <http://www.gnu.org/licenses/>.
 */

#include "LotConfigurationManagerHooks.h"
#include "BuildingUtil.h"
#include "BuildingStyleUtil.h"
#include "cGZPersistResourceKey.h"
#include "cIGZPersistResourceManager.h"
#include "cIGZVariant.h"
#include "cISCProperty.h"
#include "cISCPropertyHolder.h"
#include "cRZAutoRefCount.h"
#include "GZServPtrs.h"
#include "Logger.h"
#include "Patcher.h"
#include "PropertyData.h"
#include "PropertyIDs.h"
#include "SC4Vector.h"
#include "SC4VersionDetection.h"
#include "WallToWallOccupantGroups.h"
#include "wil/result.h"

#include <algorithm>
#include <functional>
#include <iterator>

static void CopyOccupantGroupValues(
	cISCPropertyHolder* pPropertyHolder,
	SC4Vector<uint32_t>& vector)
{
	const PropertyData<uint32_t> propertyData(pPropertyHolder, kOccupantGroupsProperty);

	if (propertyData)
	{
		vector.push_back(propertyData.data(), propertyData.size());
	}
}

static void CopyIndustryTypeOccupantGroups(
	cISCPropertyHolder* pPropertyHolder,
	SC4Vector<uint32_t>& vector)
{
	const PropertyData<uint32_t> propertyData(pPropertyHolder, kOccupantGroupsProperty);

	if (propertyData)
	{
		constexpr uint32_t kIndustryAnchor = 0x3000;
		constexpr uint32_t kIndustryOut = 0x3002;

		const auto it = std::find_if(
			propertyData.begin(),
			propertyData.end(),
			[](uint32_t value)
			{
				return value >= kIndustryAnchor && value <= kIndustryOut;
			});

		if (it != propertyData.end())
		{
			vector.push_back(*it);
		}
	}
}

static bool ReadBuildingStylePropertyValues(
	cISCPropertyHolder* pPropertyHolder,
	SC4Vector<uint32_t>& vector)
{
	vector.clear();

	PropertyData<uint32_t> propertyData;

	if (BuildingStyleUtil::TryReadBuildingStylesProperty(pPropertyHolder, propertyData))
	{
		std::copy(
			propertyData.begin(),
			propertyData.end(),
			std::back_inserter(vector));
	}

	return !vector.empty();
}

static bool ReadBuildingStyleProperty(
	cISCPropertyHolder* pPropertyHolder,
	SC4Vector<uint32_t>& vector)
{
	bool result = false;

	if (ReadBuildingStylePropertyValues(pPropertyHolder, vector))
	{
		// Add the BuildingStyles property id to indicate that the property is present.
		vector.push_back(kBuildingStylesProperty);

		// Copy over the wall-to-wall (W2W) data, if present.
		if (BuildingUtil::IsWallToWall(pPropertyHolder))
		{
			// The exact W2W style doesn't matter, only the fact
			// that it is present in the occupant groups.

			constexpr uint32_t W2WGeneral = 0xB5C00DDE;

			vector.push_back(W2WGeneral);
		}

		// Copy over the industry type occupant groups.
		// This is checked for industrial buildings in cSC4TractDeveloper::PickBuilding.
		CopyIndustryTypeOccupantGroups(pPropertyHolder, vector);

		result = true;
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
			if (!ReadBuildingStyleProperty(pPropertyHolder, vector))
			{
				CopyOccupantGroupValues(pPropertyHolder, vector);
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
