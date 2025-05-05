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
#include "BuildingStyleCollectionEntry.h"
#include <vector>

class BuildingStyleCollection final
{
public:
	BuildingStyleCollection();
	BuildingStyleCollection(const BuildingStyleCollection& other);
	BuildingStyleCollection(BuildingStyleCollection&& other) noexcept;

	BuildingStyleCollection& operator=(const BuildingStyleCollection& other);
	BuildingStyleCollection& operator=(BuildingStyleCollection&& other) noexcept;

	std::vector<BuildingStyleCollectionEntry>::iterator begin();
	std::vector<BuildingStyleCollectionEntry>::const_iterator begin() const;
	std::vector<BuildingStyleCollectionEntry>::iterator end();
	std::vector<BuildingStyleCollectionEntry>::const_iterator end() const;

	bool contains(const BuildingStyleCollectionEntry& entry) const;
	bool contains_button(uint32_t buttonID) const;
	bool contains_style(uint32_t styleID) const;
	bool empty() const;
	std::vector<BuildingStyleCollectionEntry>::const_iterator find_button(uint32_t buttonID) const;
	std::vector<BuildingStyleCollectionEntry>::const_iterator find_style(uint32_t styleID) const;

	void insert(uint32_t buttonID, uint32_t styleID, const cRZBaseString& styleName);
	void insert(uint32_t buttonID, uint32_t styleID, const cIGZString& styleName);
	void insert(const BuildingStyleCollectionEntry& entry);

	void reserve(size_t size);
	size_t size() const;

private:
	std::vector<BuildingStyleCollectionEntry> entries;
};

