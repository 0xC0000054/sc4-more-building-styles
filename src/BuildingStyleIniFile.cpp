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

#include "BuildingStyleIniFile.h"
#include "BuildingStyleButtons.h"
#include "BuildingStyleUtil.h"
#include "cIGZWin.h"
#include "cIGZWinBtn.h"
#include "cISC4App.h"
#include "cRZAutoRefCount.h"
#include "FileSystem.h"
#include "GZServPtrs.h"
#include "Logger.h"
#include "StringResourceKey.h"
#include "StringResourceManager.h"
#include "StringViewUtil.h"
#include "boost/property_tree/ptree.hpp"
#include "boost/property_tree/ini_parser.hpp"
#include <unordered_set>

using namespace std::string_view_literals;

namespace
{
	bool ParseStyleName(const std::string_view& input, cRZBaseString& styleName)
	{
		static constexpr std::string_view CaptionResPrefix = "CaptionRes:"sv;

		if (StringViewUtil::StartsWithIgnoreCase(input, CaptionResPrefix))
		{
			Logger& logger = Logger::GetInstance();

			const std::string_view captionResDataView = input.substr(CaptionResPrefix.size());

			std::vector<std::string_view> captionResValues;
			captionResValues.reserve(2);

			StringViewUtil::Split(captionResDataView, ',', captionResValues);

			if (captionResValues.size() != 2)
			{
				logger.WriteLine(
					LogLevel::Error,
					"The CaptionRes values must be in the format: groupID,instanceID");
				return false;
			}

			uint32_t group = 0;

			if (!StringViewUtil::TryParse(StringViewUtil::TrimWhiteSpace(captionResValues[0]), group))
			{
				logger.WriteLineFormatted(
					LogLevel::Error,
					"Failed to parse the CaptionRes groupID value: %s",
					std::string(captionResValues[0]).c_str());
				return false;
			}

			uint32_t instance = 0;

			if (!StringViewUtil::TryParse(StringViewUtil::TrimWhiteSpace(captionResValues[1]), instance))
			{
				logger.WriteLineFormatted(
					LogLevel::Error,
					"Failed to parse the CaptionRes instanceID value: %s",
					std::string(captionResValues[1]).c_str());
				return false;
			}

			StringResourceKey key(group, instance);

			cRZAutoRefCount<cIGZString> localizedString;

			if (!StringResourceManager::GetLocalizedString(key, localizedString))
			{
				logger.WriteLineFormatted(
					LogLevel::Error,
					"Failed to load the caption resource from TGI: 0x2026960B,0x%08X,0x%08X",
					group,
					instance);
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
		uint32_t buttonID,
		const std::string_view& input,
		BuildingStyleIniFile::StyleEntry& entry)
	{
		Logger& logger = Logger::GetInstance();

		const size_t styleIDCommaIndex = input.find_first_of(',');

		if (styleIDCommaIndex == std::string_view::npos
			|| (styleIDCommaIndex + 1) >= input.size())
		{
			return false;
		}

		const std::string_view styleIDView = StringViewUtil::TrimWhiteSpace(input.substr(0, styleIDCommaIndex));

		if (!StringViewUtil::EqualsIgnoreCase(styleIDView, "Show"sv))
		{
			if (!StringViewUtil::TryParse(styleIDView, entry.styleID))
			{
				logger.WriteLineFormatted(
					LogLevel::Error,
					"Failed to parse the style number '%s' for button id %u.",
					std::string(styleIDView).c_str(),
					buttonID);
				return false;
			}

			if (BuildingStyleUtil::IsReservedStyleID(entry.styleID))
			{
				logger.WriteLineFormatted(
					LogLevel::Error,
					"The style id cannot have a value of %d.",
					BuildingStyleIniFile::InvalidStyleID);
				return false;
			}
		}

		const std::string_view fontStyleView = StringViewUtil::TrimWhiteSpace(input.substr(styleIDCommaIndex + 1));

		if (fontStyleView.empty())
		{
			logger.WriteLine(LogLevel::Error, "The font style section is empty.");
			return false;
		}

		entry.boldText = false;

		if (fontStyleView[0] != 'N')
		{
			if (fontStyleView[0] == 'B')
			{
				entry.boldText = true;
			}
			else
			{
				logger.WriteLineFormatted(
					LogLevel::Error,
					"Invalid font style character: %c. Must be N or B",
					fontStyleView[0]);
				return false;
			}
		}

		bool result = true;

		if (fontStyleView.size() > 2 && fontStyleView[1] == ',')
		{
			const std::string_view styleNameView = fontStyleView.substr(2);

			result = ParseStyleName(styleNameView, entry.styleName);
		}
		else
		{
			if (entry.styleID == BuildingStyleIniFile::InvalidStyleID)
			{
				entry.styleName.Sprintf("Style Slot %d", buttonID);
			}
			else
			{
				entry.styleName.Sprintf("Style Slot %d (Style 0x%04X)", buttonID, entry.styleID);
			}
		}

		return result;
	}

	struct SupportedUIButtonContext
	{
		std::unordered_set<uint32_t> buttonIDs;

		SupportedUIButtonContext() : buttonIDs()
		{
		}
	};

	bool SupportedUIButtonEnumProc(cIGZWin* parent, uint32_t childID, void* child, void* pState)
	{
		if (childID <= BuildingStyleIniMaxButtonID)
		{
			SupportedUIButtonContext* state = static_cast<SupportedUIButtonContext*>(pState);

			state->buttonIDs.insert(childID);
		}

		return true;
	}

	std::unordered_set<uint32_t> GetSupportedUIButtons()
	{
		std::unordered_set<uint32_t> supportedButtonIDs;

		cISC4AppPtr pSC4App;

		if (pSC4App)
		{
			cIGZWin* mainWindow = pSC4App->GetMainWindow();

			if (mainWindow)
			{
				constexpr uint32_t kGZWin_WinSC4App = 0x6104489a;

				cIGZWin* pSC4AppWin = mainWindow->GetChildWindowFromID(kGZWin_WinSC4App);

				if (pSC4AppWin)
				{
					// Get the child window that contains the building style radio buttons.

					constexpr uint32_t BuildingStyleListContainer = 0x8bca20c3;

					cIGZWin* pStyleListContainer = pSC4AppWin->GetChildWindowFromIDRecursive(BuildingStyleListContainer);

					if (pStyleListContainer)
					{
						SupportedUIButtonContext context;

						pStyleListContainer->EnumChildren(
							GZIID_cIGZWinBtn,
							SupportedUIButtonEnumProc,
							&context);

						supportedButtonIDs.swap(context.buttonIDs);
					}
				}
			}
		}

		return supportedButtonIDs;
	}
}

BuildingStyleIniFile::BuildingStyleIniFile()
{
}

const std::unordered_map<uint32_t, BuildingStyleIniFile::StyleEntry>& BuildingStyleIniFile::GetStyles() const
{
	return entries;
}

void BuildingStyleIniFile::Load()
{
	Logger& logger = Logger::GetInstance();

	try
	{
		const std::unordered_set<uint32_t> supportedButtonIDs = GetSupportedUIButtons();

		if (!supportedButtonIDs.empty())
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
						if (supportedButtonIDs.contains(buttonID))
						{
							StyleEntry entry;

							// ParseStyleData will write an error message if it fails.
							if (ParseStyleData(buttonID, item.second.data(), entry))
							{
								entries.insert(std::make_pair(buttonID, entry));
							}
						}
						else
						{
							logger.WriteLineFormatted(
								LogLevel::Error,
								"BuildingStyles.ini: Skipping unsupported button id %u.",
								buttonID);
						}
					}
					else
					{
						logger.WriteLineFormatted(
							LogLevel::Error,
							"BuildingStyles.ini: Failed to parse button id %s.",
							item.first.c_str());
					}
				}
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

