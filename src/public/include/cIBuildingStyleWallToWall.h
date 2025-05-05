/*
* The public interface header for the BuildingStyleWallToWall GZCOM class.
*
* This header uses the MIT license (https://opensource.org/license/mit).
*
* Copyright (c) 2024, 2025 Nicholas Hayes
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the “Software”),
* to deal in the Software without restriction, including without limitation the
* rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
* sell copies of the Software,* and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*/

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