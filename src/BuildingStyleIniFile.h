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
#include "BuildingStyleCollection.h"

class cIGZWin;

class BuildingStyleIniFile
{
public:
	// We currently support a maximum of 128 custom style
	// check boxes, with a button id range of 0-127.
	static constexpr uint32_t MaxStyleButtonID = 127;

	BuildingStyleIniFile(cIGZWin& styleListContainer);

	const BuildingStyleCollection& GetStyles() const;

private:
	void Load(cIGZWin& styleListContainer);

	BuildingStyleCollection entries;
};

