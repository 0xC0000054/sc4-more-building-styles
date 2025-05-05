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

#pragma once
#include <cstdint>

enum LotZoningOptions : uint8_t
{
	LotZoningOptionNone = 0,
	// Prevent SC4 from aggregating multiple smaller
	// lots into a bigger lot when redeveloping.
	LotZoningOptionDisableAggregation = 1 << 0,
	// Prevent SC4 from subdividing a bigger lot
	// into multiple smaller lots when redeveloping.
	LotZoningOptionDisableSubdivision = 1 << 1,
	LotZoningOptionDisableAggregationAndSubdivision = LotZoningOptionDisableAggregation | LotZoningOptionDisableSubdivision
};

class IBuildingSelectWinContext
{
public:
	enum class WallToWallOption : uint8_t
	{
		// Wall-to-Wall buildings will be built alongside other buildings.
		// This is the game's standard behavior.
		Mixed = 0,
		// Only Wall-to-Wall buildings will be built.
		Only,
		// Wall-to-Wall buildings will never be built.
		Block
	};

	virtual bool GetOptionalCheckBoxState(uint32_t buttonID) const = 0;
	virtual void SetOptionalCheckBoxState(uint32_t buttonID, bool checked) = 0;

	virtual WallToWallOption GetWallToWallOption() const = 0;
	virtual void SetWallToWallOption(WallToWallOption value) = 0;

	virtual LotZoningOptions GetLotZoningOptions() const = 0;

	virtual bool PreventCrossStyleRedevelopment() const = 0;
};