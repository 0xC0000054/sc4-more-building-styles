////////////////////////////////////////////////////////////////////////////
//
// This file is part of sc4-more-building-styles, a DLL Plugin for
// SimCity 4 that adds support for more building styles.
//
// Copyright (c) 2024, 2025 Nicholas Hayes
//
// This file is licensed under terms of the MIT License.
// See LICENSE.txt for more information.
//
////////////////////////////////////////////////////////////////////////////

#pragma once
#include "cIBuildingStyleInfo.h"

static const uint32_t GZIID_cIBuildingStyleInfo2 = 0xC6C058F1;

class cIBuildingStyleInfo2 : public cIBuildingStyleInfo
{
public:
	/**
	 * @brief Gets a list of the style names for the specified building.
	 * @param pBuildingOccupant A pointer to the building occupant.
	 * @param destination The destination string.
	 * @param separator The separator between items in the list.
	 * @return true on success; otherwise, false.
	 */
	virtual bool GetBuildingStyleNamesEx(
		cISC4Occupant* pBuildingOccupant,
		cIGZString& destination,
		cIGZString const& separator) const = 0;

	/**
	 * @brief Gets a value indicating whether the specified building is wall to wall.
	 * @param pBuildingOccupant A pointer to the building occupant.
	 * @return true if the specified building is wall to wall; otherwise, false.
	 */
	virtual bool IsWallToWall(cISC4Occupant* pBuildingOccupant) const = 0;
};