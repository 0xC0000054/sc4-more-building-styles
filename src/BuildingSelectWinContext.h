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

	LotZoningOptions GetLotZoningOptions() const override;

	bool PreventCrossStyleRedevelopment() const override;

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

	void SetLotZoningOption(LotZoningOptions option, bool value);

	cISC4TractDeveloper* pTractDeveloper;
	bool automaticallyMarkBuildingsAsHistorical;
	bool automaticallyGrowifyPloppedBuildings;
	WallToWallOption wallToWallOption;
	LotZoningOptions lotZoningOption;
	KickOutLowerWealthOption kickOutLowerWealthOption;
	bool preventCrossStyleRedevelopment;
};

