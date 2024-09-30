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