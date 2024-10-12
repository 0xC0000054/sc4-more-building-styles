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
class cISC4TractDeveloper;

class BuildingSelectWinContext final : public IBuildingSelectWinContext
{
public:
	BuildingSelectWinContext();

	void LoadFromDBSegment(cIGZPersistDBSegment* pSegment);
	void SaveToDBSegment(cIGZPersistDBSegment* pSegment) const;

	bool AutomaticallyMarkBuildingsAsHistorical() const;
	bool AutomaticallyGrowifyPloppedBuildings() const;

	void PostCityInitComplete();

	void SetTractDeveloper(cISC4TractDeveloper* pTRactDeveloper);

private:
	// IBuildingSelectWinContext

	bool GetOptionalCheckBoxState(uint32_t buttonID) const override;
	void SetOptionalCheckBoxState(uint32_t buttonID, bool checked) override;

	WallToWallOption GetWallToWallOption() const override;
	void SetWallToWallOption(WallToWallOption value) override;

	bool KeepLotZoneSizes() const override;

	// Private data

	enum class KickOutLowerWealthOption : uint8_t
	{
		// The user's current value is not known and should be
		// queried from the tract developer.
		Unknown = 0,
		// Do not kick out lower wealth occupants when redeveloping.
		False,
		// Kick out lower wealth occupants when redeveloping.
		// This is the game's standard behavior.
		True
	};

	cISC4TractDeveloper* pTractDeveloper;
	bool automaticallyMarkBuildingsAsHistorical;
	bool automaticallyGrowifyPloppedBuildings;
	WallToWallOption wallToWallOption;
	bool keepLotZoneSizes;
	KickOutLowerWealthOption kickOutLowerWealthOption;
};

