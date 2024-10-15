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

#include "Preferences.h"
#include "FileSystem.h"
#include "Logger.h"
#include "boost/property_tree/ptree.hpp"
#include "boost/property_tree/ini_parser.hpp"
#include "cGZPersistResourceKey.h"
#include "cIGZCOM.h"
#include "cIGZPersistDBRecord.h"
#include "cIGZPersistDBSerialRecord.h"
#include "cIGZPersistResourceManager.h"
#include "cIGZDBSerialRecordAsIGZFile.h"
#include "cIGZFile.h"
#include "cRZAutoRefCount.h"
#include "GZServPtrs.h"
#include "StringViewUtil.h"
#include <memory>

Preferences::Preferences()
	: logBuildingStyleSelection(false),
	  logLotStyleSelection(false),
	  logGrowableFunctions(false)
{
}

void Preferences::Load(cIGZCOM* const pCOM)
{
	LoadPreferencesFromDllConfigFile();
}

bool Preferences::LogBuildingStyleSelection() const
{
	return logBuildingStyleSelection;
}

bool Preferences::LogLotStyleSelection() const
{
	return logLotStyleSelection;
}

bool Preferences::LogGrowableFunctions() const
{
	return logGrowableFunctions;
}

void Preferences::LoadPreferencesFromDllConfigFile()
{
	Logger& logger = Logger::GetInstance();

	try
	{
		std::filesystem::path path = FileSystem::GetConfigFilePath();

		std::ifstream stream(path, std::ifstream::in);

		if (stream)
		{
			boost::property_tree::ptree tree;

			boost::property_tree::ini_parser::read_ini(stream, tree);

			logBuildingStyleSelection = tree.get<bool>("DebugLogging.BuildingStyleSelection");
			logLotStyleSelection = tree.get<bool>("DebugLogging.LotStyleSelection");
			logGrowableFunctions = tree.get<bool>("DebugLogging.GrowableFunctions");
		}
		else
		{
			logger.WriteLine(LogLevel::Error, "Failed to open the DLL INI file.");
		}
	}
	catch (const std::exception& e)
	{
		logger.WriteLineFormatted(
			LogLevel::Error,
			"Error reading the DLL INI file: %s",
			e.what());
	}
}

