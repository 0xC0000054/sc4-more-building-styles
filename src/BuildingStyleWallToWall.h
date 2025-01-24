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

