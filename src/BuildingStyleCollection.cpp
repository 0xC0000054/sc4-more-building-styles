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

#include "BuildingStyleCollection.h"
#include <algorithm>

BuildingStyleCollection::BuildingStyleCollection()
{
}

BuildingStyleCollection::BuildingStyleCollection(const BuildingStyleCollection& other)
	: entries(other.entries)
{
}

BuildingStyleCollection::BuildingStyleCollection(BuildingStyleCollection&& other) noexcept
	: entries(std::move(other.entries))
{
}

BuildingStyleCollection& BuildingStyleCollection::operator=(const BuildingStyleCollection& other)
{
	entries = other.entries;

	return *this;
}

BuildingStyleCollection& BuildingStyleCollection::operator=(BuildingStyleCollection&& other) noexcept
{
	entries = std::move(other.entries);

	return *this;
}

std::vector<BuildingStyleCollectionEntry>::iterator BuildingStyleCollection::begin()
{
	return entries.begin();
}

std::vector<BuildingStyleCollectionEntry>::const_iterator BuildingStyleCollection::begin() const
{
	return entries.begin();
}

std::vector<BuildingStyleCollectionEntry>::iterator BuildingStyleCollection::end()
{
	return entries.end();
}

std::vector<BuildingStyleCollectionEntry>::const_iterator BuildingStyleCollection::end() const
{
	return entries.end();
}

bool BuildingStyleCollection::contains(uint32_t buttonID) const
{
	const size_t count = entries.size();

	for (size_t i = 0; i < count; i++)
	{
		if (entries[i].buttonID == buttonID)
		{
			return true;
		}
	}

	return false;
}

bool BuildingStyleCollection::contains(const BuildingStyleCollectionEntry& entry) const
{
	return contains(entry.buttonID);
}

bool BuildingStyleCollection::empty() const
{
	return entries.empty();
}

std::vector<BuildingStyleCollectionEntry>::const_iterator BuildingStyleCollection::find_button(uint32_t buttonID) const
{
	return std::find_if(
		entries.begin(),
		entries.end(),
		[buttonID](const BuildingStyleCollectionEntry& e)
		{
			return e.buttonID == buttonID;
		});
}

std::vector<BuildingStyleCollectionEntry>::const_iterator BuildingStyleCollection::find_style(uint32_t styleID) const
{
	return std::find_if(
		entries.begin(),
		entries.end(),
		[styleID](const BuildingStyleCollectionEntry& e)
		{
			return e.styleID == styleID;
		});
}

void BuildingStyleCollection::insert(
	uint32_t buttonID,
	uint32_t styleID,
	const cRZBaseString& styleName)
{
	if (!contains(buttonID))
	{
		entries.emplace_back(buttonID, styleID, styleName);
	}
}

void BuildingStyleCollection::insert(
	uint32_t buttonID,
	uint32_t styleID,
	const cIGZString& styleName)
{
	if (!contains(buttonID))
	{
		entries.emplace_back(buttonID, styleID, styleName);
	}
}

void BuildingStyleCollection::insert(const BuildingStyleCollectionEntry& entry)
{
	if (!contains(entry))
	{
		entries.push_back(entry);
	}
}

void BuildingStyleCollection::reserve(size_t size)
{
	entries.reserve(size);
}

size_t BuildingStyleCollection::size() const
{
	return entries.size();
}

