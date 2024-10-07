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
#include "IBuildingSelectWinContext.h"

class cIGZPersistDBSegment;
class cIGZWin;

class BuildingSelectWinContext final : public IBuildingSelectWinContext
{
public:
	BuildingSelectWinContext();

	void LoadFromDBSegment(cIGZPersistDBSegment* pSegment);
	void SaveToDBSegment(cIGZPersistDBSegment* pSegment) const;

	bool AutomaticallyMarkBuildingsAsHistorical() const;
	bool AutomaticallyGrowifyPloppedBuildings() const;

private:
	// IBuildingSelectWinContext

	bool GetOptionalCheckBoxState(uint32_t buttonID) const override;
	void UpdateOptionalCheckBoxState(cIGZWin* pWin, uint32_t buttonID) override;

	WallToWallOption GetWallToWallOption() const override;
	void SetWallToWallOption(WallToWallOption value) override;

	bool KeepLotZoneSizes() const override;

	bool automaticallyMarkBuildingsAsHistorical;
	bool automaticallyGrowifyPloppedBuildings;
	WallToWallOption wallToWallOption;
	bool keepLotZoneSizes;
};

