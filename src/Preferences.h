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

class cIGZCOM;

class Preferences final
{
public:
	Preferences();

	void Load(cIGZCOM* const pCOM);

	bool LogBuildingStyleSelection() const;
	bool LogLotStyleSelection() const;
	bool LogGrowableFunctions() const;

private:
	void LoadPreferencesFromDllConfigFile();

	bool logBuildingStyleSelection;
	bool logLotStyleSelection;
	bool logGrowableFunctions;
};

