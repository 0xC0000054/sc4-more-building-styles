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

#include "BuildingStyleIniFile.h"
#include "cIGZWin.h"
#include "cIGZWinBtn.h"
#include "cRZAutoRefCount.h"
#include "FileSystem.h"
#include "Logger.h"
#include "StringResourceKey.h"
#include "StringResourceManager.h"
#include "StringViewUtil.h"
#include "boost/property_tree/ptree.hpp"
#include "boost/property_tree/ini_parser.hpp"

using namespace std::string_view_literals;

namespace
{
	bool ParseStyleName(const std::string_view& input, cRZBaseString& styleName)
	{
		static constexpr std::string_view LtextPrefix = "LTEXT:"sv;

		if (StringViewUtil::StartsWithIgnoreCase(input, LtextPrefix))
		{
			const std::string_view tgiDataView = input.substr(LtextPrefix.size());

			std::vector<std::string_view> tgiValues;
			tgiValues.reserve(3);

			StringViewUtil::Split(tgiDataView, ',', tgiValues);

			if (tgiValues.size() != 3)
			{
				return false;
			}

			constexpr uint32_t LTEXTTypeID = 0x2026960B;

			uint32_t type = 0;
			uint32_t group = 0;
			uint32_t instance = 0;

			if (!StringViewUtil::TryParse(StringViewUtil::TrimWhiteSpace(tgiValues[0]), type)
				|| type != LTEXTTypeID
				|| !StringViewUtil::TryParse(StringViewUtil::TrimWhiteSpace(tgiValues[1]), group)
				|| !StringViewUtil::TryParse(StringViewUtil::TrimWhiteSpace(tgiValues[2]), instance))
			{
				return false;
			}

			StringResourceKey key(group, instance);

			cRZAutoRefCount<cIGZString> localizedString;

			if (!StringResourceManager::GetLocalizedString(key, localizedString.AsPPObj()))
			{
				return false;
			}

			styleName.Copy(*localizedString);
		}
		else
		{
			styleName = input;
		}

		return true;
	}

	bool ParseStyleData(
		const std::string_view& input,
		uint32_t& styleID,
		cRZBaseString& styleName)
	{
		const size_t commaIndex = input.find_first_of(',');

		if (commaIndex == std::string_view::npos
			|| (commaIndex + 1) >= input.size())
		{
			return false;
		}

		const std::string_view styleIDView = StringViewUtil::TrimWhiteSpace(input.substr(0, commaIndex));

		if (!StringViewUtil::TryParse(styleIDView, styleID))
		{
			return false;
		}

		const std::string_view styleNameView = StringViewUtil::TrimWhiteSpace(input.substr(commaIndex + 1));

		return ParseStyleName(styleNameView, styleName);
	}

	bool CountStyleButtonsEnumProc(cIGZWin* parent, uint32_t childID, cIGZWin* child, void* pState)
	{
		if (childID <= BuildingStyleIniFile::MaxStyleButtonID)
		{
			int64_t* maxIndex = static_cast<int64_t*>(pState);

			if (childID > *maxIndex)
			{
				*maxIndex = childID;
			}
		}

		return true;
	}

	uint32_t GetMaxStyleButtonIndex(cIGZWin& styleListContainer)
	{
		int64_t maxIndex = -1;

		styleListContainer.EnumChildren(
			GZIID_cIGZWinBtn,
			CountStyleButtonsEnumProc,
			&maxIndex);

		uint32_t result = std::numeric_limits<uint32_t>::max();

		if (maxIndex >= 0 && maxIndex < std::numeric_limits<uint32_t>::max())
		{
			result = static_cast<uint32_t>(maxIndex);
		}

		return result;
	}
}

BuildingStyleIniFile::BuildingStyleIniFile(cIGZWin& styleListContainer)
{
	Load(styleListContainer);
}

const BuildingStyleCollection& BuildingStyleIniFile::GetStyles() const
{
	return entries;
}

void BuildingStyleIniFile::Load(cIGZWin& styleListContainer)
{
	Logger& logger = Logger::GetInstance();

	try
	{
		const uint32_t maxStyleButtonIndex = GetMaxStyleButtonIndex(styleListContainer);

		if (maxStyleButtonIndex != std::numeric_limits<uint32_t>::max())
		{
			std::filesystem::path path = FileSystem::GetBuildingStylesIniFilePath();

			std::ifstream stream(path, std::ifstream::in);

			if (stream)
			{
				boost::property_tree::ptree tree;

				boost::property_tree::ini_parser::read_ini(stream, tree);

				const boost::property_tree::ptree buildingStylesSection = tree.get_child("BuildingStyles");

				entries.reserve(buildingStylesSection.size());

				for (const auto& item : buildingStylesSection)
				{
					uint32_t buttonID = 0;

					if (StringViewUtil::TryParse(item.first, buttonID))
					{
						if (buttonID <= maxStyleButtonIndex)
						{
							uint32_t styleID = 0;
							cRZBaseString styleName;

							if (ParseStyleData(item.second.data(), styleID, styleName))
							{
								entries.insert(
									buttonID,
									styleID,
									styleName);
							}
							else
							{
								logger.WriteLineFormatted(
									LogLevel::Error,
									"Failed to parse the style data for button id %d.",
									buttonID);
							}
						}
						else
						{
							logger.WriteLineFormatted(
								LogLevel::Error,
								"Skipping invalid button id %u. The button ids must be between 0 and %u.",
								buttonID,
								maxStyleButtonIndex);
						}
					}
					else
					{
						logger.WriteLineFormatted(
							LogLevel::Error,
							"Failed to parse the button id %s.",
							item.first.c_str());
					}
				}

				// Sort the items in ascending order.
				std::sort(entries.begin(), entries.end());
			}
			else
			{
				logger.WriteLine(LogLevel::Error, "Failed to open the BuildingStyles INI file.");
			}
		}
	}
	catch (const std::exception& e)
	{
		logger.WriteLineFormatted(
			LogLevel::Error,
			"Failed to read the BuildingStyles INI file: %s",
			e.what());
	}
}

