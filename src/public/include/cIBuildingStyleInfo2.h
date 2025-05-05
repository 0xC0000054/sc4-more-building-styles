/*
* The public interface header for the BuildingStyleInfo2 GZCOM class.
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