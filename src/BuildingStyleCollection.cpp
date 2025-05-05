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

bool BuildingStyleCollection::contains(const BuildingStyleCollectionEntry& entry) const
{
	return contains_button(entry.buttonID);
}

bool BuildingStyleCollection::contains_button(uint32_t buttonID) const
{
	return find_button(buttonID) != end();
}

bool BuildingStyleCollection::contains_style(uint32_t styleID) const
{
	return find_style(styleID) != end();
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
	if (!contains_button(buttonID))
	{
		entries.emplace_back(buttonID, styleID, styleName);
	}
}

void BuildingStyleCollection::insert(
	uint32_t buttonID,
	uint32_t styleID,
	const cIGZString& styleName)
{
	if (!contains_button(buttonID))
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

