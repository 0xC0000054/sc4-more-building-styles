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

#pragma once
#include "cISC4BuildingOccupant.h"
#include <array>

class cISC4Occupant;

namespace BuildingUtil
{
	bool IsMaxisBuildingStyle(uint32_t style);

	cISC4BuildingOccupant::PurposeType GetPurposeType(cISC4Occupant* pOccupant);

	bool PurposeTypeSupportsBuildingStyles(cISC4BuildingOccupant::PurposeType purposeType);

	bool IsIndustrialBuilding(cISC4BuildingOccupant::PurposeType purposeType);
}