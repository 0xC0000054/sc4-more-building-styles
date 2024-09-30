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
#include "BuildingStyleCollection.h"
#include "cISC4TractDeveloper.h"
#include "IBuildingSelectWinContext.h"

class cIGZWin;

class IBuildingSelectWinManager
{
public:
	virtual cISC4TractDeveloper* GetTractDeveloper() const = 0;

	virtual bool IsStyleButtonIDValid(uint32_t buttonID) const = 0;
	virtual const BuildingStyleCollection& GetAvailableBuildingStyles() const = 0;

	virtual IBuildingSelectWinContext& GetContext() = 0;
	virtual const IBuildingSelectWinContext& GetContext() const = 0;

	virtual void SendActiveBuildingStyleCheckboxChangedMessage(
		bool checked,
		const BuildingStyleCollectionEntry& entry) = 0;
};