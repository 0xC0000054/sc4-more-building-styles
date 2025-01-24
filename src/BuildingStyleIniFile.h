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
#include "cRZBaseString.h"
#include <unordered_map>

class cIGZWin;

class BuildingStyleIniFile
{
public:
	// A style id value of 0 is used by some Maxis blockers,
	// so it can never be a valid building style id.
	// We use it as a marker value for unused style
	// check boxes that are not hidden in the menu.
	static constexpr uint32_t InvalidStyleID = 0;

	struct StyleEntry
	{
		uint32_t styleID;
		cRZBaseString styleName;
		bool boldText;

		StyleEntry()
			: styleID(InvalidStyleID),
			  styleName(),
			  boldText(false)
		{
		}
	};

	BuildingStyleIniFile();

	const std::unordered_map<uint32_t, StyleEntry>& GetStyles() const;

	void Load();
private:

	std::unordered_map<uint32_t, StyleEntry> entries;
};

