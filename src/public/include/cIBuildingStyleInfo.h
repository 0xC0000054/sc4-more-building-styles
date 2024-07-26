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
	 * @param pOccupant A pointer to the building occupant.
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