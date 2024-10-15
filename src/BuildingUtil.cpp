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

#include "BuildingUtil.h"
#include "cISC4BuildingOccupant.h"
#include "cISC4Occupant.h"
#include "cISCPropertyHolder.h"
#include "cRZAutoRefCount.h"

bool BuildingUtil::IsMaxisBuildingStyle(uint32_t style)
{
	// We check the high value first as an optimization, custom building styles
	// should be 0x2004 or higher.
	return style <= 0x2003 && style >= 0x2000;
}

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
