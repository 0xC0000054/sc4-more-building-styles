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
#include "BuildingStyleCollection.h"
#include "BuildingStyleIniFile.h"

class cIGZWin;

class AvailableBuildingStyles
{
public:
	AvailableBuildingStyles();

	// Initializes the list of available building styles.
	void Initialize();

	bool IsStyleButtonIDValid(uint32_t buttonID) const;

	const BuildingStyleCollection& GetBuildingStyles() const;

private:
	BuildingStyleCollection availableBuildingStyles;
	BuildingStyleIniFile buildingStyleIniFile;
	bool firstCityLoaded;
};

