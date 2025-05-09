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

#include "BuildingUtil.h"
#include "cIGZVariant.h"
#include "cISC4BuildingOccupant.h"
#include "cISC4Occupant.h"
#include "cISCProperty.h"
#include "cISCPropertyHolder.h"
#include "cRZAutoRefCount.h"
#include "PropertyIDs.h"
#include "WallToWallOccupantGroups.h"

cISC4BuildingOccupant::PurposeType BuildingUtil::GetPurposeType(cISC4Occupant* pOccupant)
{
	cISC4BuildingOccupant::PurposeType purposeType = cISC4BuildingOccupant::PurposeType::None;

	if (pOccupant)
	{
		cRZAutoRefCount<cISC4BuildingOccupant> pBuildingOccupant;

		if (pOccupant->QueryInterface(GZIID_cISC4BuildingOccupant, pBuildingOccupant.AsPPVoid()))
		{
			const cISC4BuildingOccupant::BuildingProfile& profile = pBuildingOccupant->GetBuildingProfile();

			purposeType = profile.purpose;
		}
	}

	return purposeType;
}

bool BuildingUtil::PurposeTypeSupportsBuildingStyles(cISC4BuildingOccupant::PurposeType purposeType)
{
	switch (purposeType)
	{
	case cISC4BuildingOccupant::PurposeType::Residence:
	case cISC4BuildingOccupant::PurposeType::Services:
	case cISC4BuildingOccupant::PurposeType::Office:
	case cISC4BuildingOccupant::PurposeType::Agriculture:
	case cISC4BuildingOccupant::PurposeType::Processing:
	case cISC4BuildingOccupant::PurposeType::Manufacturing:
	case cISC4BuildingOccupant::PurposeType::HighTech:
		return true;
	case cISC4BuildingOccupant::PurposeType::None:
	case cISC4BuildingOccupant::PurposeType::Tourism:
	case cISC4BuildingOccupant::PurposeType::Other:
	default:
		return false;
	}
}

bool BuildingUtil::IsIndustrialBuilding(cISC4BuildingOccupant::PurposeType purposeType)
{
	return purposeType >= cISC4BuildingOccupant::PurposeType::Agriculture
		&& purposeType <= cISC4BuildingOccupant::PurposeType::HighTech;
}

bool BuildingUtil::IsWallToWall(const cISCPropertyHolder* pPropertyHolder)
{
	bool buildingIsWallToWall = false;

	if (pPropertyHolder)
	{
		const cISCProperty* pProperty = pPropertyHolder->GetProperty(kBuildingIsWallToWallProperty);

		if (pProperty)
		{
			const cIGZVariant* pVariant = pProperty->GetPropertyValue();

			if (pVariant)
			{
				const uint16_t type = pVariant->GetType();

				if (type == cIGZVariant::Bool)
				{
					buildingIsWallToWall = pVariant->GetValBool();
				}
				else if (type == cIGZVariant::BoolArray)
				{
					const uint32_t count = pVariant->GetCount();

					if (count == 1)
					{
						buildingIsWallToWall = *pVariant->RefBool();
					}
				}
			}
		}
		else
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
							buildingIsWallToWall = true;
							break;
						}
					}
				}
			}
		}
	}

	return buildingIsWallToWall;
}

bool BuildingUtil::IsWallToWall(cISC4Occupant* pOccupant)
{
	bool result = false;

	if (pOccupant)
	{
		result = IsWallToWall(pOccupant->AsPropertyHolder());
	}

	return result;
}
