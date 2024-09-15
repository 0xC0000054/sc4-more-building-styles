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

	bool AgriculturePurposeTypeSupportsBuildingStyles() const;
	bool ProcessingPurposeTypeSupportsBuildingStyles() const;
	bool ManufacturingPurposeTypeSupportsBuildingStyles() const;
	bool HighTechPurposeTypeSupportsBuildingStyles() const;

private:
	void LoadPreferencesFromDllConfigFile();
	void LoadPreferencesFromDBPFFiles(cIGZCOM* const pCOM);

	bool logBuildingStyleSelection;
	bool logLotStyleSelection;
	bool agriculturePurposeTypeSupportsBuildingStyles;
	bool processingPurposeTypeSupportsBuildingStyles;
	bool manufacturingPurposeTypeSupportsBuildingStyles;
	bool highTechPurposeTypeSupportsBuildingStyles;
};

