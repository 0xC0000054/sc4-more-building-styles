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
#include "frozen/unordered_set.h"

// The button ids in this file are used to provide additional functionality in
// the building style dialog beyond just allowing the user to add new styles.
// All of the buttons are optional for users that are creating a building
// style dialog.

// A check box that controls whether growable buildings will be automatically
// marked has historical when they are created by the game.
static constexpr uint32_t AutoHistoricalButtonID = 0x9476D8DA;
// A check box that controls whether plopped growable buildings will be automatically
// changed to use a growable zone type.
// In this case, the auto historical check box controls whether the growified lot
// will be marked has historical.
static constexpr uint32_t AutoGrowifyButtonID = 0xB510A368;
// A radio button that makes the growable building algorithm build wall to wall
// items alongside other building and lot types.
// This is the game's standard behavior.
static constexpr uint32_t WallToWallMixedRadioButtonID = 0x31150389;
// A radio button that makes the growable building algorithm build only
// wall to wall building and lots.
static constexpr uint32_t WallToWallOnlyRadioButtonID = 0x3115038A;
// A radio button that makes the growable building algorithm never build
// wall to wall building and lots.
static constexpr uint32_t WallToWallBlockRadioButtonID = 0x3115038B;
// A check box that controls whether the game will only use the lot sizes the user
// zoned when developing residential and commercial lots.
// When this option is disabled, the game will use its standard behavior of resizing
// the zoned area to fit the lot it picks for that location.
static constexpr uint32_t KeepLotZoneSizesButtonID = 0x3621731B;

constexpr frozen::unordered_set<uint32_t, 5> OptionalButtonIDs =
{
	AutoHistoricalButtonID,
	AutoGrowifyButtonID,
	WallToWallMixedRadioButtonID,
	WallToWallOnlyRadioButtonID,
	WallToWallBlockRadioButtonID,
	KeepLotZoneSizesButtonID
};