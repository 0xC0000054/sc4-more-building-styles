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
#include "cIBuildingStyleInfo2.h"

class IBuildingSelectWinManager;

class BuildingStyleInfo final : public cIBuildingStyleInfo2
{
public:
	BuildingStyleInfo(const IBuildingSelectWinManager& buildingWinManager);

	// cIGZUnknown

	bool QueryInterface(uint32_t riid, void** ppvObj) override;
	uint32_t AddRef() override;
	uint32_t Release() override;

private:
	// cIBuildingStyleInfo

	uint32_t GetAvailableBuildingStyleIds(uint32_t* pStyles, uint32_t size) const override;
	bool GetBuildingStyleName(uint32_t style, cIGZString& name) const override;
	bool GetBuildingStyleNames(cISC4Occupant* pBuildingOccupant, cIGZString& destination) const override;
	bool IsBuildingStyleAvailable(uint32_t style) const override;

	// cIBuildingStyleInfo2

	bool GetBuildingStyleNamesEx(
		cISC4Occupant* pBuildingOccupant,
		cIGZString& destination,
		const cIGZString& separator) const override;

	bool IsWallToWall(cISC4Occupant* pBuildingOccupant) const override;

	// Private members

	uint32_t refCount;
	const IBuildingSelectWinManager& buildingWinManager;
};

