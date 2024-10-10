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
#include "frozen/unordered_map.h"
#include <cstdint>
#include <string_view>

static constexpr frozen::unordered_map<uint32_t, const std::string_view, 6> WallToWallOccupantGroups =
{
	{ 0xD02C802E, "BTE: Ind. W2W" },
	{ 0xB5C00A05, "BTE: Comm. W2W" },
	{ 0xB5C00B05, "BTE: Res. W2W" },
	{ 0xB5C00DDE, "BTE: W2W General" },
	{ 0xB5C00F0A, "SFBT: Hamburg W2W" },
	{ 0xB5C00F0B, "SFBT: Paris W2W" }
};