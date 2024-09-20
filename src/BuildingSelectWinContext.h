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

class cIGZPersistDBSegment;
class cIGZWin;

class BuildingSelectWinContext final
{
public:

	BuildingSelectWinContext();

	void LoadFromDBSegment(cIGZPersistDBSegment* pSegment);
	void SaveToDBSegment(cIGZPersistDBSegment* pSegment) const;

	bool GetOptionalCheckBoxState(uint32_t buttonID) const;
	void UpdateOptionalCheckBoxState(cIGZWin* pWin, uint32_t buttonID);

	bool AutomaticallyMarkBuildingsAsHistorical() const;
	bool AutomaticallyGrowifyPloppedBuildings() const;

private:

	bool automaticallyMarkBuildingsAsHistorical;
	bool automaticallyGrowifyPloppedBuildings;
};

