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
#include "cISC4TractDeveloper.h"
#include "IBuildingSelectWinContext.h"

#include <map>
#include <string>

class cIGZWin;

class IBuildingSelectWinManager
{
public:
	virtual cISC4TractDeveloper* GetTractDeveloper() const = 0;

	virtual bool IsBuildingStyleAvailable(uint32_t style) const = 0;
	virtual const std::map<uint32_t, std::string>& GetAvailableBuildingStyles() const = 0;

	virtual IBuildingSelectWinContext& GetContext() = 0;
	virtual const IBuildingSelectWinContext& GetContext() const = 0;

	virtual void SendActiveBuildingStyleCheckboxChangedMessage(bool checked, uint32_t styleID) = 0;
};