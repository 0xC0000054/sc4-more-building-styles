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

// The button ids in this file are used to provide additional functionality in
// the building style dialog beyond just allowing the user to add new styles.
// All of the buttons are optional for users that are creating a building
// style dialog.

// A check box that controls whether growable buildings will be automatically
// marked has historical when they are created by the game.
static constexpr uint32_t AutoHistoricalButtonID = 0x9476D8DA;