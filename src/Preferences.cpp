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


Preferences::Preferences()
	: logBuildingStyleSelection(false),
	  logLotStyleSelection(false)
{
}

void Preferences::Load()
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

			logBuildingStyleSelection = tree.get<bool>("GrowableBuildingStyleDebugLogging.LogBuildingStyleSelection");
			logLotStyleSelection = tree.get<bool>("GrowableBuildingStyleDebugLogging.LogLotStyleSelection");
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
			"Failed to read the DLL INI file: %s",
			e.what());
	}
}

bool Preferences::LogBuildingStyleSelection() const
{
	return logBuildingStyleSelection;
}

bool Preferences::LogLotStyleSelection() const
{
	return logLotStyleSelection;
}
