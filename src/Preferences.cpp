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
#include "GZFileStreamBufAdapter.h"
#include "GZServPtrs.h"
#include "StringViewUtil.h"
#include <memory>

static constexpr uint32_t IniFileTypeID = 0;
// This is one of 2 group ID values that the Reader recognizes as an INI file
static constexpr uint32_t IndustrialBuildingStylesGroupID = 0x8A5971C5;
// Industrial Resource A.K.A. Agriculture, Rural, Farm
static constexpr uint32_t IRBuildingStylesInstanceID = 0xC82CA0DC;
// Industrial Dirty A.K.A. Processing
static constexpr uint32_t IDBuildingStylesInstanceID = 0xC82CA0DD;
// Industrial Manufacturing
static constexpr uint32_t IMBuildingStylesInstanceID = 0xC82CA0DE;
// Industrial High Tech
static constexpr uint32_t IHTBuildingStylesInstanceID = 0xC82CA0DF;

namespace
{
	std::unique_ptr<GZFileStreamBufAdapter> OpenDBPFRecordAsStreamBuf(
		cIGZCOM* const pCOM,
		cIGZPersistResourceManager* pResourceManager,
		const cGZPersistResourceKey& key)
	{
		std::unique_ptr<GZFileStreamBufAdapter> adapter;

		if (pCOM && pResourceManager)
		{
			cRZAutoRefCount<cIGZPersistDBRecord> pRecord;

			if (pResourceManager->OpenDBRecord(key, pRecord.AsPPObj(), false))
			{
				cRZAutoRefCount<cIGZPersistDBSerialRecord> pSerialRecord;

				if (pRecord->QueryInterface(GZIID_cIGZPersistDBSerialRecord, pSerialRecord.AsPPVoid()))
				{
					cRZAutoRefCount<cIGZDBSerialRecordAsIGZFile> pAsIGZFile;

					if (pCOM->GetClassObject(
						GZCLSID_cIGZDBSerialRecordAsIGZFile,
						GZIID_cIGZDBSerialRecordAsIGZFile,
						pAsIGZFile.AsPPVoid()))
					{
						if (pAsIGZFile->SetDBSerialRecord(pSerialRecord))
						{
							cRZAutoRefCount<cIGZFile> pFile;

							if (pAsIGZFile->QueryInterface(GZIID_cIGZFile, pFile.AsPPVoid()))
							{
								adapter = std::make_unique<GZFileStreamBufAdapter>(std::move(pFile));
							}
						}
					}
				}
			}
		}

		return adapter;
	}

	bool TryConvertStringToBoolean(const std::string& input, bool& value)
	{
		bool result = false;

		if (StringViewUtil::EqualsIgnoreCase(input, "False"))
		{
			value = false;
			result = true;
		}
		else if (StringViewUtil::EqualsIgnoreCase(input, "True"))
		{
			value = true;
			result = true;
		}

		return result;
	}

	bool TryReadINIFieldFromDBPFRecord(
		cIGZCOM* const pCOM,
		cIGZPersistResourceManager* pResourceManager,
		uint32_t instanceID,
		const char* const iniEntryPath,
		bool& value)
	{
		bool result = false;

		if (pCOM && pResourceManager)
		{
			cGZPersistResourceKey key(IniFileTypeID, IndustrialBuildingStylesGroupID, instanceID);

			std::unique_ptr<GZFileStreamBufAdapter> adapter(OpenDBPFRecordAsStreamBuf(pCOM, pResourceManager, key));

			if (adapter)
			{
				try
				{
					std::istream file(adapter.get());

					boost::property_tree::ptree tree;

					boost::property_tree::ini_parser::read_ini(file, tree);

					result = TryConvertStringToBoolean(tree.get<std::string>(iniEntryPath), value);
				}
				catch (const std::exception& e)
				{
					Logger::GetInstance().WriteLineFormatted(
						LogLevel::Error,
						"Error reading DBPF INI file 0x%08x-0x%08x-0x%08x: %s",
						key.type,
						key.group,
						key.instance,
						e.what());
				}
			}
		}

		return result;
	}
}


Preferences::Preferences()
	: logBuildingStyleSelection(false),
	  logLotStyleSelection(false),
	  agriculturePurposeTypeSupportsBuildingStyles(false),
	  processingPurposeTypeSupportsBuildingStyles(false),
	  manufacturingPurposeTypeSupportsBuildingStyles(false),
	  highTechPurposeTypeSupportsBuildingStyles(false)
{
}

void Preferences::Load(cIGZCOM* const pCOM)
{
	LoadPreferencesFromDllConfigFile();
	LoadPreferencesFromDBPFFiles(pCOM);
}

bool Preferences::LogBuildingStyleSelection() const
{
	return logBuildingStyleSelection;
}

bool Preferences::LogLotStyleSelection() const
{
	return logLotStyleSelection;
}

bool Preferences::AgriculturePurposeTypeSupportsBuildingStyles() const
{
	return agriculturePurposeTypeSupportsBuildingStyles;
}

bool Preferences::ProcessingPurposeTypeSupportsBuildingStyles() const
{
	return processingPurposeTypeSupportsBuildingStyles;
}

bool Preferences::ManufacturingPurposeTypeSupportsBuildingStyles() const
{
	return manufacturingPurposeTypeSupportsBuildingStyles;
}

bool Preferences::HighTechPurposeTypeSupportsBuildingStyles() const
{
	return highTechPurposeTypeSupportsBuildingStyles;
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

void Preferences::LoadPreferencesFromDBPFFiles(cIGZCOM* const pCOM)
{
	if (pCOM)
	{
		cIGZPersistResourceManagerPtr pRM;

		if (pRM)
		{
			bool value = false;

			if (TryReadINIFieldFromDBPFRecord(
				pCOM,
				pRM,
				IRBuildingStylesInstanceID,
				"IRBuildingStyles.Enabled",
				value))
			{
				agriculturePurposeTypeSupportsBuildingStyles = value;
			}

			if (TryReadINIFieldFromDBPFRecord(
				pCOM,
				pRM,
				IDBuildingStylesInstanceID,
				"IDBuildingStyles.Enabled",
				value))
			{
				processingPurposeTypeSupportsBuildingStyles = value;
			}

			if (TryReadINIFieldFromDBPFRecord(
				pCOM,
				pRM,
				IMBuildingStylesInstanceID,
				"IMBuildingStyles.Enabled",
				value))
			{
				manufacturingPurposeTypeSupportsBuildingStyles = value;
			}

			if (TryReadINIFieldFromDBPFRecord(
				pCOM,
				pRM,
				IHTBuildingStylesInstanceID,
				"IHTBuildingStyles.Enabled",
				value))
			{
				highTechPurposeTypeSupportsBuildingStyles = value;
			}
		}
	}
}
