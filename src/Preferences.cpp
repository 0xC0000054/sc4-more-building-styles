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

#include "Preferences.h"
#include "FileSystem.h"
#include "Logger.h"
#include "boost/property_tree/ptree.hpp"
#include "boost/property_tree/ini_parser.hpp"
#include "cIGZCOM.h"

Preferences::Preferences()
	: logBuildingStyleSelection(false),
	  logLotStyleSelection(false),
	  logGrowableFunctions(false),
	  logCandidateLots(false)
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

bool Preferences::LogCandidateLots() const
{
	return logCandidateLots;
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
			logCandidateLots = tree.get<bool>("DebugLogging.CandidateLots");
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

