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

// The BuildingStyles property is used to indicate the custom styles
// that the building/lot is compatible with.
// When present, the building/lot will not grow under the 4 Maxis styles.
//
// For industrial lots, items without this property will be considered
// compatible with the 4 Maxis styles.
static constexpr uint32_t kBuildingStylesProperty = 0xAA1DD400;
// A Boolean property that indicates if the building is
// Wall to Wall (W2W) compatible.
static constexpr uint32_t kBuildingIsWallToWallProperty = 0xAA1DD401;

// The Maxis occupant groups property.
constexpr uint32_t kOccupantGroupsProperty = 0xAA1DD396;
