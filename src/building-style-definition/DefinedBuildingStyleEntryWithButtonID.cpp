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

#include "DefinedBuildingStyleEntryWithButtonID.h"

DefinedBuildingStyleEntryWithButtonID::DefinedBuildingStyleEntryWithButtonID()
	: buttonID(0), styleData()
{
}

DefinedBuildingStyleEntryWithButtonID::DefinedBuildingStyleEntryWithButtonID(uint32_t button)
	: buttonID(button), styleData()
{
}

DefinedBuildingStyleEntryWithButtonID::DefinedBuildingStyleEntryWithButtonID(uint32_t button, const DefinedBuildingStyleEntry& entry)
	: buttonID(button), styleData(entry)
{
}

DefinedBuildingStyleEntryWithButtonID::DefinedBuildingStyleEntryWithButtonID(const DefinedBuildingStyleEntryWithButtonID& other)
	: buttonID(other.buttonID), styleData(other.styleData)
{
}

DefinedBuildingStyleEntryWithButtonID::DefinedBuildingStyleEntryWithButtonID(DefinedBuildingStyleEntryWithButtonID&& other) noexcept
	: buttonID(other.buttonID), styleData(std::move(other.styleData))
{
}

DefinedBuildingStyleEntryWithButtonID& DefinedBuildingStyleEntryWithButtonID::operator=(const DefinedBuildingStyleEntryWithButtonID& other)
{
	this->buttonID = other.buttonID;
	this->styleData = other.styleData;

	return *this;
}

DefinedBuildingStyleEntryWithButtonID& DefinedBuildingStyleEntryWithButtonID::operator=(DefinedBuildingStyleEntryWithButtonID&& other) noexcept
{
	this->buttonID = other.buttonID;
	this->styleData = std::move(other.styleData);

	return *this;
}

bool DefinedBuildingStyleEntryWithButtonID::operator==(const DefinedBuildingStyleEntryWithButtonID& other) const
{
	return this->styleData.styleID == other.styleData.styleID;
}
