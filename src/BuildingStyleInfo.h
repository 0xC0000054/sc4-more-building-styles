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
#include "cIBuildingStyleInfo.h"

class IBuildingSelectWinManager;

class BuildingStyleInfo : public cIBuildingStyleInfo
{
public:
	BuildingStyleInfo(const IBuildingSelectWinManager& buildingWinManager);

	bool QueryInterface(uint32_t riid, void** ppvObj) override;
	uint32_t AddRef() override;
	uint32_t Release() override;

	uint32_t GetAvailableBuildingStyleIds(uint32_t* pStyles, uint32_t size) const;
	bool GetBuildingStyleName(uint32_t style, cIGZString& name) const;
	bool GetBuildingStyleNames(cISC4Occupant* pBuildingOccupant, cIGZString& destination) const;
	bool IsBuildingStyleAvailable(uint32_t style) const;

private:
	uint32_t refCount;
	const IBuildingSelectWinManager& buildingWinManager;
};

