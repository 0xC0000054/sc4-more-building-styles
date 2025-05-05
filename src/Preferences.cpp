/*
 * This file is part of sc4-more-building-styles, a DLL Plugin for
 * SimCity 4 that adds support for more building styles.
 *
 * Copyright (C) 2024, 2025 Nicholas Hayes
 *
 * sc4-more-building-styles is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * sc4-more-building-styles is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with sc4-more-building-styles.
 * If not, see <http://www.gnu.org/licenses/>.
 */

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

