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

#include "BuildingStyleInfo.h"
#include "BuildingUtil.h"
#include "cIGZString.h"
#include "cIGZVariant.h"
#include "cISC4BuildingOccupant.h"
#include "cISC4Occupant.h"
#include "cISCProperty.h"
#include "cISCPropertyHolder.h"
#include "cRZAutoRefCount.h"
#include "cRZBaseString.h"
#include "GlobalPointers.h"
#include "IBuildingSelectWinManager.h"
#include "PropertyIDs.h"
#include "StringResourceKey.h"
#include "StringResourceManager.h"

namespace
{
	void AppendBuildingStyleName(
		const BuildingStyleCollection& availableBuildingStyles,
		uint32_t styleID,
		cIGZString& destination)
	{
		auto item = availableBuildingStyles.find_style(styleID);

		if (item != availableBuildingStyles.end())
		{
			destination.Append(item->styleName);
		}
		else
		{
			// The specified style is not present in the UI, display a fall back string.

			char buffer[128]{};

			int length = std::snprintf(buffer, sizeof(buffer), "Unknown (0x%X)", styleID);

			if (length > 0)
			{
				destination.Append(buffer, static_cast<uint32_t>(length));
			}
		}
	}

	void GetStyleNamesFromVariant(
		const cIGZVariant& variant,
		const BuildingStyleCollection& availableBuildingStyles,
		cIGZString& destination,
		const cIGZString& separator)
	{
		const uint32_t* pData = variant.RefUint32();
		const uint32_t repCount = variant.GetCount();

		if (repCount > 1)
		{
			bool firstStyle = true;

			// The styles are printed in the order that they are listed in the exemplar property.
			// Previous versions of the DLL did this as a side effect of the implementation, and
			// we have to preserve the behavior because users came to depend on it.

			for (size_t i = 0; i < repCount; i++)
			{
				AppendBuildingStyleName(availableBuildingStyles, pData[i], destination);

				if (firstStyle)
				{
					firstStyle = false;
				}
				else
				{
					destination.Append(separator);
				}
			}
		}
		else
		{
			const uint32_t targetStyleID = repCount == 0 ? reinterpret_cast<uint32_t>(pData) : pData[0];

			AppendBuildingStyleName(availableBuildingStyles, targetStyleID, destination);
		}
	}
}

BuildingStyleInfo::BuildingStyleInfo(const IBuildingSelectWinManager& buildingWinManager)
	: refCount(0),
	  buildingWinManager(buildingWinManager)
{
}

bool BuildingStyleInfo::QueryInterface(uint32_t riid, void** ppvObj)
{
	if (riid == GZIID_cIBuildingStyleInfo)
	{
		*ppvObj = static_cast<cIBuildingStyleInfo*>(this);
		AddRef();

		return true;
	}
	else if (riid == GZIID_cIBuildingStyleInfo2)
	{
		*ppvObj = static_cast<cIBuildingStyleInfo2*>(this);
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

uint32_t BuildingStyleInfo::AddRef()
{
	return ++refCount;
}

uint32_t BuildingStyleInfo::Release()
{
	if (refCount > 0)
	{
		--refCount;
	}

	return refCount;
}

uint32_t BuildingStyleInfo::GetAvailableBuildingStyleIds(uint32_t* pStyles, uint32_t size) const
{
	uint32_t styleCount = 0;

	if (pStyles)
	{
		if (size > 0)
		{
			const BuildingStyleCollection& availableBuildingStyles = buildingWinManager.GetAvailableBuildingStyles();

			for (const auto& item : availableBuildingStyles)
			{
				pStyles[styleCount] = item.styleID;
				styleCount++;

				if (styleCount >= size)
				{
					break;
				}
			}
		}
	}
	else
	{
		// If the array pointer is null and the size is 0, we return the total
		// number of available styles.
		// If the size is not zero that is an API usage error, and we return 0.
		if (size == 0)
		{
			styleCount = buildingWinManager.GetAvailableBuildingStyles().size();
		}
	}

	return styleCount;
}

bool BuildingStyleInfo::GetBuildingStyleName(uint32_t style, cIGZString& name) const
{
	const BuildingStyleCollection& availableBuildingStyles = buildingWinManager.GetAvailableBuildingStyles();

	name.Erase(0, UINT_MAX);
	AppendBuildingStyleName(availableBuildingStyles, style, name);

	return name.Strlen() > 0;
}

bool BuildingStyleInfo::GetBuildingStyleNames(cISC4Occupant* pBuildingOccupant, cIGZString& destination) const
{
	return GetBuildingStyleNamesEx(pBuildingOccupant, destination, cRZBaseString(", "));
}

bool BuildingStyleInfo::IsBuildingStyleAvailable(uint32_t style) const
{
	const BuildingStyleCollection& availableBuildingStyles = buildingWinManager.GetAvailableBuildingStyles();

	return availableBuildingStyles.find_style(style) != availableBuildingStyles.end();
}

bool BuildingStyleInfo::GetBuildingStyleNamesEx(
	cISC4Occupant* pBuildingOccupant,
	cIGZString& destination,
	cIGZString const& separator) const
{
	bool result = false;

	destination.Erase(0, destination.Strlen());

	const cISC4BuildingOccupant::PurposeType purpose = BuildingUtil::GetPurposeType(pBuildingOccupant);

	if (BuildingUtil::PurposeTypeSupportsBuildingStyles(purpose))
	{
		const BuildingStyleCollection& availableBuildingStyles = buildingWinManager.GetAvailableBuildingStyles();

		if (availableBuildingStyles.size() > 0)
		{
			const cISCPropertyHolder* pPropertyHolder = pBuildingOccupant->AsPropertyHolder();

			if (pPropertyHolder)
			{
				const cISCProperty* pProperty = pPropertyHolder->GetProperty(kBuildingStylesProperty);

				if (pProperty)
				{
					const cIGZVariant* pVariant = pProperty->GetPropertyValue();

					if (pVariant)
					{
						GetStyleNamesFromVariant(
							*pVariant,
							availableBuildingStyles,
							destination,
							separator);
					}
				}
				else
				{
					if (BuildingUtil::IsIndustrialBuilding(purpose))
					{
						// Industrial buildings without a BuildingStyles property
						// are compatible with all building styles.
						// This is represented by the localized text 'Build all styles at once'.

						cRZAutoRefCount<cIGZString> temp;

						const StringResourceKey key(0x6A231EAA, 0x2BBBD89B);

						// Load the string with the specified group and instance id.
						// The strings in SimCityLocale.dat don't use the per-language group id
						// offset system that is used by DBPF plug-ins, so we use GetString
						// instead of GetLocalizedString.
						if (StringResourceManager::GetString(key, temp))
						{
							destination.Copy(*temp);
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
								GetStyleNamesFromVariant(
									*pVariant,
									availableBuildingStyles,
									destination,
									separator);
							}
						}
					}
				}
			}

			// Check that at least one style name has been written to the destination.
			result = destination.Strlen() > 0;
		}
	}

	return result;
}

bool BuildingStyleInfo::IsWallToWall(cISC4Occupant* pBuildingOccupant) const
{
	return BuildingUtil::IsWallToWall(pBuildingOccupant);
}
