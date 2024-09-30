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

#include "BuildingStyleCollectionEntry.h"

BuildingStyleCollectionEntry::BuildingStyleCollectionEntry(uint32_t buttonID)
	: buttonID(buttonID),
	  styleID(0),
	  styleName()
{
}

BuildingStyleCollectionEntry::BuildingStyleCollectionEntry(
	uint32_t buttonID,
	uint32_t styleID,
	const cRZBaseString& styleName)
	: buttonID(buttonID),
	  styleID(styleID),
	  styleName(styleName)
{
}

BuildingStyleCollectionEntry::BuildingStyleCollectionEntry(
	uint32_t buttonID,
	uint32_t styleID,
	const cIGZString& styleName)
	: buttonID(buttonID),
	  styleID(styleID),
	  styleName(styleName)
{
}

BuildingStyleCollectionEntry::BuildingStyleCollectionEntry(const BuildingStyleCollectionEntry& other)
	: buttonID(other.buttonID),
	  styleID(other.styleID),
	  styleName(other.styleName)
{
}

BuildingStyleCollectionEntry::BuildingStyleCollectionEntry(BuildingStyleCollectionEntry&& other) noexcept
	: buttonID(std::move(other.buttonID)),
	  styleID(std::move(other.styleID)),
	  styleName(std::move(other.styleName))
{
}

BuildingStyleCollectionEntry& BuildingStyleCollectionEntry::operator=(const BuildingStyleCollectionEntry& other)
{
	buttonID = other.buttonID;
	styleID = other.styleID;
	styleName = other.styleName;

	return *this;
}

BuildingStyleCollectionEntry& BuildingStyleCollectionEntry::operator=(BuildingStyleCollectionEntry&& other) noexcept
{
	buttonID = std::move(other.buttonID);
	styleID = std::move(other.styleID);
	styleName = std::move(other.styleName);

	return *this;
}

bool BuildingStyleCollectionEntry::operator==(const BuildingStyleCollectionEntry& other) const
{
	return buttonID == other.buttonID;
}

bool BuildingStyleCollectionEntry::operator!=(const BuildingStyleCollectionEntry& other) const
{
	return !operator==(other);
}

bool BuildingStyleCollectionEntry::operator<(const BuildingStyleCollectionEntry& other) const
{
	return buttonID < other.buttonID;
}

bool BuildingStyleCollectionEntry::operator<=(const BuildingStyleCollectionEntry& other) const
{
	return buttonID <= other.buttonID;
}

bool BuildingStyleCollectionEntry::operator>(const BuildingStyleCollectionEntry& other) const
{
	return buttonID > other.buttonID;
}

bool BuildingStyleCollectionEntry::operator>=(const BuildingStyleCollectionEntry& other) const
{
	return buttonID >= other.buttonID;
}
