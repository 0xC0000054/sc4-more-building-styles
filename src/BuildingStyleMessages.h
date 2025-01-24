////////////////////////////////////////////////////////////////////////////
//
// This file is part of sc4-more-building-styles, a DLL Plugin for
// SimCity 4 that adds support for more building styles.
//
// Copyright (c) 2024, 2025 Nicholas Hayes
//
// This file is licensed under terms of the MIT License.
// See LICENSE.txt for more information.
//
////////////////////////////////////////////////////////////////////////////

#pragma once
#include <cstdint>

// Sent when a style is checked on unchecked in the building style list.
// data1: Non-zero if the check box has been checked; otherwise, zero.
// data2: The ID of the style that has been enabled or disabled.
// data3: A cIGZString containing the style name.
static const uint32_t kMessageBuildingStyleCheckboxChanged = 0x573D5E8F;
