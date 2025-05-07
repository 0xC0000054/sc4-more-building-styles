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
#include "frozen/unordered_set.h"

constexpr uint32_t StyleControlRadio_CycleEveryNYears = 0xCBC61559;
constexpr uint32_t StyleControlRadio_UseAllStylesAtOnce = 0xEBC61560;
constexpr uint32_t StylePanel_Collapsed_TitleBar = 0xBC61548;
constexpr uint32_t StylePanel_Expanded_TitleBar = 0x2BC619F3;
constexpr uint32_t StylePanel_Collapsed_ToggleButton = 0xCBC61567;
constexpr uint32_t StylePanel_Expanded_ToggleButton = 0xEBC619FD;

static constexpr frozen::unordered_set<uint32_t, 6> MaxisUIControlIDs =
{
	StyleControlRadio_CycleEveryNYears,
	StyleControlRadio_UseAllStylesAtOnce,
	StylePanel_Collapsed_TitleBar,
	StylePanel_Expanded_TitleBar,
	StylePanel_Collapsed_ToggleButton,
	StylePanel_Expanded_ToggleButton
};