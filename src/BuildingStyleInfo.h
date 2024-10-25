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

