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
#include "cIGZUnknown.h"

class cIGZString;
class cISC4Occupant;

static const uint32_t GZCLSID_cIBuildingStyleWallToWall = 0x9AEE8F81;
static const uint32_t GZIID_cIBuildingStyleWallToWall = 0x9AEE8F82;

class cIBuildingStyleWallToWall : public cIGZUnknown
{
public:
	/**
	 * @brief Determines whether the specified occupant group value is a wall to wall occupant group.
	 * @param occupantGroup The occupant group.
	 * @return true if the specified occupant group value is a wall to wall occupant group; otherwise, false.
	 */
	virtual bool IsWallToWallOccupantGroup(uint32_t occupantGroup) const = 0;

	/**
	 * @brief Gets the wall to wall occupant group ids.
	 * @param pOccupantGroups A pointer to the destination array.
	 * @param size The size of the destination array.
	 * @return The number of items copied to the destination array.
	 * When pOccupantGroups is NULL and size is 0, the total number of items will be returned.
	 */
	virtual uint32_t GetWallToWallOccupantGroupIds(uint32_t* pOccupantGroups, uint32_t size) const = 0;

	/**
	 * @brief Gets the name of the specified wall to wall occupant group.
	 * @param occupantGroup The wall to wall occupant group.
	 * @param name The destination string.
	 * @return true on success; otherwise, false.
	 */
	virtual bool GetWallToWallOccupantGroupName(uint32_t occupantGroup, cIGZString& name) const = 0;

	/**
	 * @brief Gets a value indicating whether the specified building has a wall to wall occupant group.
	 * @param pBuildingOccupant A pointer to the building occupant.
	 * @return true if the specified building has a wall to wall occupant group; otherwise, false.
	 */
	virtual bool HasWallToWallOccupantGroup(cISC4Occupant* pBuildingOccupant) const = 0;

	/**
	 * @brief Gets a comma-separated list of the wall to wall occupant group names for the specified building.
	 * @param pBuildingOccupant A pointer to the building occupant.
	 * @param destination The destination string.
	 * @return true on success; otherwise, false.
	 */
	virtual bool GetWallToWallOccupantGroupNames(cISC4Occupant* pBuildingOccupant, cIGZString& destination) const = 0;
};