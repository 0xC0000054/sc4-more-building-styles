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

#include "DefinedBuildingStyleEntry.h"

DefinedBuildingStyleEntry::DefinedBuildingStyleEntry()
	: styleID(InvalidStyleID),
	  styleName(),
	  boldText(false)
{
}

DefinedBuildingStyleEntry::DefinedBuildingStyleEntry(
	uint32_t styleID,
	const cIGZString& name,
	bool boldText)
	: styleID(styleID),
	  styleName(name),
	  toolTipKey(),
	  boldText(boldText)
{
}

DefinedBuildingStyleEntry::DefinedBuildingStyleEntry(
	uint32_t styleID,
	const cIGZString& name,
	const StringResourceKey& toolTipKey,
	bool boldText)
	: styleID(styleID),
	  styleName(name),
	  toolTipKey(toolTipKey),
	  boldText(boldText)
{
}

DefinedBuildingStyleEntry::DefinedBuildingStyleEntry(const DefinedBuildingStyleEntry& other)
	: styleID(other.styleID),
	  styleName(other.styleName),
	  toolTipKey(other.toolTipKey),
	  boldText(other.boldText)
{
}

DefinedBuildingStyleEntry::DefinedBuildingStyleEntry(DefinedBuildingStyleEntry&& other) noexcept
	: styleID(other.styleID),
	  styleName(std::move(other.styleName)),
	  toolTipKey(other.toolTipKey),
	  boldText(other.boldText)
{
	other.styleID = InvalidStyleID;
}

DefinedBuildingStyleEntry& DefinedBuildingStyleEntry::operator=(const DefinedBuildingStyleEntry& other)
{
	this->styleID = other.styleID;
	this->styleName = other.styleName;
	this->toolTipKey = other.toolTipKey;
	this->boldText = other.boldText;

	return *this;
}

DefinedBuildingStyleEntry& DefinedBuildingStyleEntry::operator=(DefinedBuildingStyleEntry&& other) noexcept
{
	this->styleID = other.styleID;
	this->styleName = std::move(other.styleName);
	this->toolTipKey = other.toolTipKey;
	this->boldText = other.boldText;

	other.styleID = InvalidStyleID;

	return *this;
}

bool DefinedBuildingStyleEntry::operator==(const DefinedBuildingStyleEntry& other) const
{
	return this->styleID == other.styleID;
}

