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

#include "DefinedBuildingStyles.h"
#include "BuildingStyleButtons.h"
#include "BuildingStyleExemplars.h"
#include "BuildingStyleIniFile.h"
#include "BuildingStyleWinUtil.h"
#include "cIGZWin.h"
#include "Logger.h"

namespace
{
	bool SupportedUIButtonEnumProc(cIGZWin* parent, uint32_t childID, void* child, void* pState)
	{
		if (childID <= BuildingStyleMappedCheckBoxMaxButtonID)
		{
			std::vector<uint32_t>* buttonIDs = static_cast<std::vector<uint32_t>*>(pState);

			if (std::find(buttonIDs->begin(), buttonIDs->end(), childID) == buttonIDs->end())
			{
				buttonIDs->push_back(childID);
			}
			else
			{
				Logger::GetInstance().WriteLineFormatted(
					LogLevel::Error,
					"The building style UI has more than one check box with the id %d.",
					childID);
			}
		}

		return true;
	}

	std::vector<uint32_t> GetSupportedUIButtons()
	{
		std::vector<uint32_t> buttonIDs;

		BuildingStyleWinUtil::EnumerateBuildingStyleContainerButtons(
			&SupportedUIButtonEnumProc,
			&buttonIDs);

		std::sort(buttonIDs.begin(), buttonIDs.end());

		return buttonIDs;
	}

	struct IniFileOverrideInfo
	{
		std::vector<uint32_t> supportedUIButtons;
		std::unordered_set<uint32_t> iniFileStyles;

		IniFileOverrideInfo()
			: supportedUIButtons(), iniFileStyles()
		{
		}
	};

	std::vector<DefinedBuildingStyleEntryWithButtonID> LoadBuildingStylesFromExemplars(const IniFileOverrideInfo& info)
	{
		std::vector<DefinedBuildingStyleEntryWithButtonID> styles;

		auto exemplarBuildingStyles = BuildingStyleExemplars::GetDefinedStyles(info.iniFileStyles);
		const size_t exemplarStyleCount = exemplarBuildingStyles.size();

		if (exemplarStyleCount > 0)
		{
			if (exemplarStyleCount > info.supportedUIButtons.size())
			{
				Logger::GetInstance().WriteLineFormatted(
					LogLevel::Error,
					"%u building styles are present, but the UI only has %u check boxes.",
					exemplarBuildingStyles.size(),
					info.supportedUIButtons.size());
			}
			else
			{
				// Sort the styles in ascending order by style id.
				// This makes the display order in the UI independent of the
				// order that the exemplars are processed.
				std::sort(
					exemplarBuildingStyles.begin(),
					exemplarBuildingStyles.end(),
					[](const DefinedBuildingStyleEntry& lhs, const DefinedBuildingStyleEntry& rhs)
					{
						return lhs.styleID < rhs.styleID;
					});

				styles.reserve(exemplarStyleCount);

				for (size_t i = 0; i < exemplarStyleCount; i++)
				{
					const uint32_t buttonID = info.supportedUIButtons[i];
					const DefinedBuildingStyleEntry& entry = exemplarBuildingStyles[i];

					styles.emplace_back(buttonID, entry);
				}
			}
		}

		return styles;
	}

	struct MergedStyleData
	{
		std::vector<DefinedBuildingStyleEntryWithButtonID> styles;
		size_t iniStyleCount;
		size_t exemplarStyleCount;

		MergedStyleData()
			: styles(),
			  iniStyleCount(0),
			  exemplarStyleCount(0)
		{
		}
	};

	IniFileOverrideInfo GetIniFileOverrideInfo(
		const std::vector<uint32_t>& supportedUIButtons,
		const std::vector<DefinedBuildingStyleEntryWithButtonID>& iniFileData)
	{
		IniFileOverrideInfo info;

		if (iniFileData.empty())
		{
			info.supportedUIButtons = supportedUIButtons;
		}
		else
		{
			std::unordered_set<uint32_t> iniFileButtons;
			iniFileButtons.reserve(iniFileData.size());

			for (const auto& item : iniFileData)
			{
				iniFileButtons.emplace(item.buttonID);

				uint32_t styleID = item.styleData.styleID;

				if (styleID != DefinedBuildingStyleEntry::InvalidStyleID)
				{
					info.iniFileStyles.emplace(styleID);
				}
			}

			std::copy_if(
				supportedUIButtons.begin(),
				supportedUIButtons.end(),
				std::back_inserter(info.supportedUIButtons),
				[iniFileButtons](uint32_t i)
				{
					return !iniFileButtons.contains(i);
				});
		}

		return info;
	}

	MergedStyleData GetMergedExemplarAndIniStyles(const std::vector<uint32_t>& supportedUIButtons)
	{
		MergedStyleData data;

		auto iniStyles = BuildingStyleIniFile::GetDefinedStyles(supportedUIButtons);
		data.iniStyleCount = iniStyles.size();

		// The styles defined in the INI file take precedence over the ones defined by an exemplar.
		// This is done to preserve the behavior of the INI file allowing for fully custom styles.

		const auto info = GetIniFileOverrideInfo(supportedUIButtons, iniStyles);
		const auto exemplarStyles = LoadBuildingStylesFromExemplars(info);
		data.exemplarStyleCount = exemplarStyles.size();

		data.styles.reserve(data.iniStyleCount + data.exemplarStyleCount);

		for (const auto& item : iniStyles)
		{
			data.styles.push_back(item);
		}

		for (const auto& item : exemplarStyles)
		{
			data.styles.push_back(item);
		}

		return data;
	}
}

DefinedBuildingStyles::DefinedBuildingStyles()
{
}

const std::unordered_map<uint32_t, DefinedBuildingStyleEntry>& DefinedBuildingStyles::GetStyles() const
{
	return styles;
}

void DefinedBuildingStyles::Load()
{
	Logger& logger = Logger::GetInstance();

	const std::vector<uint32_t> supportedUIButtons = GetSupportedUIButtons();

	if (supportedUIButtons.empty())
	{
		logger.WriteLine(
			LogLevel::Error,
			"The building style UI doesn't have any automatic style check boxes.");
	}
	else
	{
		const MergedStyleData mergedStyleData = GetMergedExemplarAndIniStyles(supportedUIButtons);

		for (const auto& item : mergedStyleData.styles)
		{
			styles.emplace(item.buttonID, item.styleData);
		}

		logger.WriteLineFormatted(
			LogLevel::Info,
			"Found %u exemplar building styles and %u INI file building styles.",
			mergedStyleData.exemplarStyleCount,
			mergedStyleData.iniStyleCount);
	}
}



