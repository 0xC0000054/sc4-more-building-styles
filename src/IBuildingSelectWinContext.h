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
#include <cstdint>

class cIGZWin;

class IBuildingSelectWinContext
{
public:
	enum class WallToWallOption : uint8_t
	{
		// Wall-to-Wall buildings will be built alongside other buildings.
		// This is the game's standard behavior.
		Mixed = 0,
		// Only Wall-to-Wall buildings will be built.
		Only,
		// Wall-to-Wall buildings will never be built.
		Block
	};

	virtual bool GetOptionalCheckBoxState(uint32_t buttonID) const = 0;
	virtual void UpdateOptionalCheckBoxState(cIGZWin* pWin, uint32_t buttonID) = 0;

	virtual WallToWallOption GetWallToWallOption() const = 0;
	virtual void SetWallToWallOption(WallToWallOption value) = 0;
};