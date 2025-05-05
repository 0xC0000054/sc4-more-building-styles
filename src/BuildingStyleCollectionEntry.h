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
#include <cstdint>

struct BuildingStyleCollectionEntry
{
	uint32_t buttonID;
	uint32_t styleID;
	cRZBaseString styleName;

	BuildingStyleCollectionEntry(uint32_t buttonID);
	BuildingStyleCollectionEntry(uint32_t buttonID, uint32_t styleID, const cRZBaseString& styleName);
	BuildingStyleCollectionEntry(uint32_t buttonID, uint32_t styleID, const cIGZString& styleName);
	BuildingStyleCollectionEntry(const BuildingStyleCollectionEntry& other);
	BuildingStyleCollectionEntry(BuildingStyleCollectionEntry&& other) noexcept;

	BuildingStyleCollectionEntry& operator=(const BuildingStyleCollectionEntry& other);
	BuildingStyleCollectionEntry& operator=(BuildingStyleCollectionEntry&& other) noexcept;

	bool operator==(const BuildingStyleCollectionEntry& other) const;
	bool operator!=(const BuildingStyleCollectionEntry& other) const;
	bool operator<(const BuildingStyleCollectionEntry& other) const;
	bool operator<= (const BuildingStyleCollectionEntry& other) const;
	bool operator>(const BuildingStyleCollectionEntry& other) const;
	bool operator>=(const BuildingStyleCollectionEntry& other) const;
};