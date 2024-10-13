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
#include <cstdint>

// The BuildingStyles property is used to indicate the custom styles
// that the building/lot is compatible with.
// When present, the building/lot will not grow under the 4 Maxis styles.
//
// For industrial lots, items without this property will be considered
// compatible with the 4 Maxis styles.
static constexpr uint32_t kBuildingStylesProperty = 0xAA1DD400;

// The Maxis occupant groups property.
constexpr uint32_t kOccupantGroupsProperty = 0xAA1DD396;
