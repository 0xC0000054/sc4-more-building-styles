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
#include "cRZBaseString.h"
#include "StringResourceKey.h"

struct DefinedBuildingStyleEntry
{
	// A style id value of 0 is used by some Maxis blockers,
	// so it can never be a valid building style id.
	// We use it as a marker value for unused style
	// check boxes that are not hidden in the menu.
	static constexpr uint32_t InvalidStyleID = 0;

	uint32_t styleID;
	cRZBaseString styleName;
	StringResourceKey toolTipKey;
	bool boldText;

	DefinedBuildingStyleEntry();
	DefinedBuildingStyleEntry(uint32_t styleID, const cIGZString& name, bool boldText);
	DefinedBuildingStyleEntry(uint32_t styleID, const cIGZString& name, const StringResourceKey& toolTipKey, bool boldText);
	DefinedBuildingStyleEntry(const DefinedBuildingStyleEntry& other);
	DefinedBuildingStyleEntry(DefinedBuildingStyleEntry&& other) noexcept;

	DefinedBuildingStyleEntry& operator=(const DefinedBuildingStyleEntry& other);
	DefinedBuildingStyleEntry& operator=(DefinedBuildingStyleEntry&& other) noexcept;

	bool operator==(const DefinedBuildingStyleEntry& other) const;
};
