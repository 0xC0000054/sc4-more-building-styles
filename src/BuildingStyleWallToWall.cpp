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

#include "BuildingStyleWallToWall.h"
#include "BuildingUtil.h"
#include "cIGZString.h"
#include "cIGZVariant.h"
#include "cISC4Occupant.h"
#include "cISCProperty.h"
#include "cISCPropertyHolder.h"
#include "cRZAutoRefCount.h"
#include "WallToWallOccupantGroups.h"

static constexpr uint32_t kOccupantGroupsProperty = 0xAA1DD396;

BuildingStyleWallToWall::BuildingStyleWallToWall()
	: refCount(0)
{
}

bool BuildingStyleWallToWall::QueryInterface(uint32_t riid, void** ppvObj)
{
	if (riid == GZIID_cIBuildingStyleWallToWall)
	{
		*ppvObj = static_cast<cIBuildingStyleWallToWall*>(this);
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

uint32_t BuildingStyleWallToWall::AddRef()
{
	return ++refCount;
}

uint32_t BuildingStyleWallToWall::Release()
{
	if (refCount > 0)
	{
		--refCount;
	}

	return refCount;
}

bool BuildingStyleWallToWall::IsWallToWallOccupantGroup(uint32_t occupantGroup) const
{
	return WallToWallOccupantGroups.count(occupantGroup) != 0;
}

uint32_t BuildingStyleWallToWall::GetWallToWallOccupantGroupIds(uint32_t* pOccupantGroups, uint32_t size) const
{
	uint32_t count = 0;

	if (pOccupantGroups)
	{
		if (size > 0)
		{
			for (const auto& item : WallToWallOccupantGroups)
			{
				pOccupantGroups[count] = item.first;
				count++;

				if (count >= size)
				{
					break;
				}
			}
		}
	}
	else
	{
		// If the array pointer is null and the size is 0, we return the total
		// number of wall to wall occupant groups.
		// If the size is not zero that is an API usage error, and we return 0.
		if (size == 0)
		{
			count = WallToWallOccupantGroups.size();
		}
	}

	return count;
}

bool BuildingStyleWallToWall::GetWallToWallOccupantGroupName(uint32_t occupantGroup, cIGZString& name) const
{
	bool result = false;

	const auto item = WallToWallOccupantGroups.find(occupantGroup);

	if (item != WallToWallOccupantGroups.end())
	{
		const std::string_view& occupantGroupName = item->second;

		name.FromChar(occupantGroupName.data(), occupantGroupName.size());
		result = true;
	}

	return result;
}

bool BuildingStyleWallToWall::HasWallToWallOccupantGroup(cISC4Occupant* pBuildingOccupant) const
{
	return BuildingUtil::IsWallToWall(pBuildingOccupant);
}

bool BuildingStyleWallToWall::GetWallToWallOccupantGroupNames(cISC4Occupant* pBuildingOccupant, cIGZString& destination) const
{
	bool result = false;

	destination.Erase(0, destination.Strlen());

	if (pBuildingOccupant)
	{
		const cISCPropertyHolder* pPropertyHolder = pBuildingOccupant->AsPropertyHolder();

		if (pPropertyHolder)
		{
			const cISCProperty* pOccupantGroupsProperty = pPropertyHolder->GetProperty(kOccupantGroupsProperty);

			if (pOccupantGroupsProperty)
			{
				const cIGZVariant* pVariant = pOccupantGroupsProperty->GetPropertyValue();

				if (pVariant)
				{
					const std::string_view separator(", ");

					const uint32_t* pOccupantGroups = pVariant->RefUint32();
					const size_t count = pVariant->GetCount();

					for (size_t i = 0; i < count; i++)
					{
						const auto item = WallToWallOccupantGroups.find(pOccupantGroups[i]);

						if (item != WallToWallOccupantGroups.end())
						{
							const std::string_view& name = item->second;

							destination.Append(name.data(), name.size());
							destination.Append(separator.data(), separator.size());
						}
					}

					// Check that at least one style name has been written to the destination.
					if (destination.Strlen() > 0)
					{
						// Remove the trailing separator from the last style in the list.
						destination.Resize(destination.Strlen() - separator.size());
						result = true;
					}
				}
			}
		}
	}

	return result;
}
