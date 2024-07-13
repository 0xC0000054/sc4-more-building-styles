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
#include <map>
#include <string>

class cIGZWin;

class AvailableBuildingStyles
{
public:
	AvailableBuildingStyles();

	// Initializes the list of available building styles.
	void Initialize();

	bool ContainsBuildingStyle(uint32_t style) const;

	const std::map<uint32_t, std::string>& GetBuildingStyles() const;

private:

	static bool BuildingSelectWinEnumProc(cIGZWin* parent, uint32_t childID, cIGZWin* child, void* pState);

	std::map<uint32_t, std::string> availableBuildingStyles;
	bool initialized;
};

