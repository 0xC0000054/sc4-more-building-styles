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

class AvailableBuildingStyles
{
public:

	static AvailableBuildingStyles& GetInstance();

	// Initializes the list of
	void Initialize();

	bool ContainsBuildingStyle(uint32_t style) const;

	const std::map<uint32_t, std::string>& GetBuildingStyles() const;

private:

	AvailableBuildingStyles();

	std::map<uint32_t, std::string> availableBuildingStyles;
	bool initialized;
};

