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
#include "cIBuildingStyleWallToWall.h"

class BuildingStyleWallToWall final : public cIBuildingStyleWallToWall
{
public:
	BuildingStyleWallToWall();

	// cIGZUnknown

	bool QueryInterface(uint32_t riid, void** ppvObj) override;
	uint32_t AddRef() override;
	uint32_t Release() override;

	// cIBuildingStyleWallToWall

	bool IsWallToWallOccupantGroup(uint32_t occupantGroup) const override;
	uint32_t GetWallToWallOccupantGroupIds(uint32_t* pOccupantGroups, uint32_t size) const override;
	bool GetWallToWallOccupantGroupName(uint32_t occupantGroup, cIGZString& name) const override;
	bool HasWallToWallOccupantGroup(cISC4Occupant* pBuildingOccupant) const override;
	bool GetWallToWallOccupantGroupNames(cISC4Occupant* pBuildingOccupant, cIGZString& destination) const override;

private:
	uint32_t refCount;
};

