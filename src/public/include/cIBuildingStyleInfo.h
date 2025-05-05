/*
* The public interface header for the BuildingStyleInfo GZCOM class.
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

static const uint32_t GZCLSID_cIBuildingStyleInfo = 0x683B30D3;
static const uint32_t GZIID_cIBuildingStyleInfo = 0xC6C058F0;

class cIBuildingStyleInfo : public cIGZUnknown
{
public:
	/**
	 * @brief Gets the style ids that are available in the Building Style Control.
	 * @param pStyles A pointer to the destination array.
	 * @param size The size of the destination array.
	 * @return The number of styles copied to the destination array.
	 * When pStyles is NULL and size is 0, the total number of style ids will be returned.
	 */
	virtual uint32_t GetAvailableBuildingStyleIds(uint32_t* pStyles, uint32_t size) const = 0;

	/**
	 * @brief Gets the name of the specified style.
	 * @param style The style id.
	 * @param name The destination string.
	 * @return true on success; otherwise, false.
	 */
	virtual bool GetBuildingStyleName(uint32_t style, cIGZString& name) const = 0;

	/**
	 * @brief Gets a comma-separated list of the style names for the specified building.
	 * @param pBuildingOccupant A pointer to the building occupant.
	 * @param destination The destination string.
	 * @return true on success; otherwise, false.
	 */
	virtual bool GetBuildingStyleNames(cISC4Occupant* pBuildingOccupant, cIGZString& destination) const = 0;

	/**
	 * @brief Determines whether the specified style is available in the Building Style Control.
	 * @param style The style id.
	 * @return true if the building style is available; otherwise, false.
	 */
	virtual bool IsBuildingStyleAvailable(uint32_t style) const = 0;
};