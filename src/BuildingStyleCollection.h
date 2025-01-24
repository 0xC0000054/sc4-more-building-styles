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

