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

#include "UnavailableUIBuildingStyles.h"
#include "BuildingStyleUtil.h"
#include "cGZPersistResourceKey.h"
#include "cIGZPersistDBSegment.h"
#include "cIGZPersistDBSegmentMultiPackedFiles.h"
#include "cIGZPersistResourceManager.h"
#include "cISC4BuildingDevelopmentSimulator.h"
#include "cISC4City.h"
#include "cISCProperty.h"
#include "cISCPropertyHolder.h"
#include "cISCResExemplar.h"
#include "cRZAutoRefCount.h"
#include "GZServPtrs.h"
#include "Logger.h"
#include "PropertyData.h"
#include "PropertyIDs.h"
#include <algorithm>
#include <functional>
#include <iterator>
#include <set>

namespace
{
	std::vector<uint32_t> GetAllBuildingTypes(cISC4BuildingDevelopmentSimulator& buildingDevelopmentSim)
	{
		uint32_t dummy = 0;

		// GetAllBuildingTypes doesn't follow the pattern in many other SC4 APIs of
		// returning the size in the count field when the data pointer is NULL, instead
		// it only uses the return value.

		const uint32_t count = buildingDevelopmentSim.GetAllBuildingTypes(nullptr, dummy);

		std::vector<uint32_t> buildingTypes(count);

		uint32_t vectorSize = count;

		buildingDevelopmentSim.GetAllBuildingTypes(buildingTypes.data(), vectorSize);

		return buildingTypes;
	}

	std::vector<uint32_t> GetAvailableStylesAsSortedVector(const BuildingStyleCollection& availableUIStyles)
	{
		std::vector<uint32_t> styles;
		styles.reserve(availableUIStyles.size());

		std::transform(
			availableUIStyles.begin(),
			availableUIStyles.end(),
			std::back_inserter(styles),
			[](const BuildingStyleCollectionEntry& entry) { return entry.styleID; });

		// Sort the items in ascending order.
		std::sort(styles.begin(), styles.end());

		return styles;
	}

	bool TryReadBuildingStylesPropertyAsSortedVector(
		cISCPropertyHolder* pPropertyHolder,
		std::vector<uint32_t>& output)
	{
		output.clear();

		PropertyData<uint32_t> propertyData(pPropertyHolder, kBuildingStylesProperty);

		if (propertyData)
		{
			// A Building Styles property set to the PIM-X placeholder style is currently
			// used on over 100 released buildings.
			// This style acts as a blocker when the DLL is installed with one of these
			// updated buildings.
			// Additionally, there are a number of other values that can't be used as a
			// style id such as the control ids in the Building Style Control UI.
			//
			// Buildings that have only the reserved style ids in their Building Styles
			// property will be made to use the legacy Maxis styles in the
			// Occupant Groups property.

			std::copy_if(
				propertyData.begin(),
				propertyData.end(),
				std::back_inserter(output),
				std::not_fn(BuildingStyleUtil::IsReservedStyleID));

			if (output.size() > 1)
			{
				// Sort the items in ascending order.
				std::sort(output.begin(), output.end());
			}
		}

		return !output.empty();
	}

	struct UnavailableBuildingStyleInfo
	{
		cGZPersistResourceKey buildingResoureKey;
		std::vector<uint32_t> unavailableStyles;

		UnavailableBuildingStyleInfo(const cGZPersistResourceKey& key, const std::vector<uint32_t>& styles)
			: buildingResoureKey(key), unavailableStyles(styles)
		{
		}
	};

	std::vector<UnavailableBuildingStyleInfo> GetUnavailableBuildingStyles(
		cISC4City& city,
		cIGZPersistResourceManager& resMan,
		const std::vector<uint32_t>& sortedUIStyles)
	{
		std::vector<UnavailableBuildingStyleInfo> styles;

		cISC4BuildingDevelopmentSimulator* pBuildingDevelopmentSim = city.GetBuildingDevelopmentSimulator();

		if (pBuildingDevelopmentSim)
		{
			const std::vector<uint32_t> allBuildingTypes = GetAllBuildingTypes(*pBuildingDevelopmentSim);

			std::vector<uint32_t> buildingStyles;
			std::vector<uint32_t> unavailableStyles;

			for (const uint32_t& buildingType : allBuildingTypes)
			{
				cGZPersistResourceKey key;

				if (pBuildingDevelopmentSim->GetBuildingKeyFromType(buildingType, key))
				{
					cRZAutoRefCount<cISCResExemplar> exemplar;

					if (resMan.GetResource(key, GZIID_cISCResExemplar, exemplar.AsPPVoid(), 0, nullptr))
					{
						if (TryReadBuildingStylesPropertyAsSortedVector(
							exemplar->AsISCPropertyHolder(),
							buildingStyles))
						{
							// std::set_difference extracts any styles that are not present in the UI into
							// the unavailable styles vector.
							//
							// To allow for the case where a building is compatible with multiple overlapping
							// styles that are not all present in the UI, we only treat a building being
							// incompatible with all UI styles as an error.
							// A building that has a Building Styles property without any of the available
							// UI styles will never grow, so we make those buildings fall back to using
							// the Maxis styles in the Occupant Groups property.

							unavailableStyles.clear();
							std::set_difference(
								buildingStyles.begin(),
								buildingStyles.end(),
								sortedUIStyles.begin(),
								sortedUIStyles.end(),
								std::back_inserter(unavailableStyles));

							if (unavailableStyles.size() == buildingStyles.size())
							{
								// None of the building's styles are supported in the UI.
								styles.emplace_back(key, unavailableStyles);
							}
						}
					}
				}
			}
		}

		return styles;
	}

	bool TryGetDBPFFilePathFromResourceKey(
		cIGZPersistResourceManager& resMan,
		const cGZPersistResourceKey& key,
		cRZBaseString& path)
	{
		bool result = false;

		cRZAutoRefCount<cIGZPersistDBSegment> segment;

		if (resMan.FindDBSegment(key, segment.AsPPObj()))
		{
			cRZAutoRefCount<cIGZPersistDBSegmentMultiPackedFiles> multiPackedFile;

			if (segment->QueryInterface(GZIID_cIGZPersistDBSegmentMultiPackedFiles, multiPackedFile.AsPPVoid()))
			{
				// cIGZPersistDBSegmentMultiPackedFiles is a collection of DBPF files in a specific folder
				// and its sub-folders.
				// Call its FindDBSegment method to get the actual file.

				cRZAutoRefCount<cIGZPersistDBSegment> multiPackedSegment;

				if (multiPackedFile->FindDBSegment(key, multiPackedSegment.AsPPObj()))
				{
					multiPackedSegment->GetPath(path);
					result = true;
				}
			}
			else
			{
				segment->GetPath(path);
				result = true;
			}
		}

		return result;
	}

	void WriteUnavailableStyleErrorLogHeader(
		Logger& logger,
		const std::vector<uint32_t>& sortedUIStyles,
		const BuildingStyleCollection& availableUIStyles)
	{
		logger.WriteLine(
			LogLevel::Error,
			"The Building Styles property in the following building exemplar(s) only has building"
			" styles that are not present in the UI, the building(s) will use the Maxis styles.");

		if (sortedUIStyles.empty())
		{
			logger.Write(LogLevel::Error, "\n\n");
		}
		else
		{
			logger.Write(LogLevel::Error, "Available UI styles: ");

			const size_t styleCount = sortedUIStyles.size();
			const size_t lastStyleIndex = styleCount - 1;

			for (size_t i = 0; i < styleCount; i++)
			{
				const uint32_t styleID = sortedUIStyles[i];

				const auto& entry = availableUIStyles.find_style(styleID);

				if (entry != availableUIStyles.end() && entry->styleName.Strlen() > 0)
				{
					logger.WriteFormatted(
						LogLevel::Error,
						"0x%X (%s)",
						styleID,
						entry->styleName.ToChar());
				}
				else
				{
					logger.WriteFormatted(LogLevel::Error, "0x%X", styleID);
				}

				if (i < lastStyleIndex)
				{
					logger.Write(LogLevel::Error, ", ");
				}
			}

			logger.Write(LogLevel::Error, ".\n\n");
		}
	}

	void LogUnavailableStyleError(
		Logger& logger,
		cIGZPersistResourceManager& resMan,
		const UnavailableBuildingStyleInfo& info)
	{
		const cGZPersistResourceKey& key = info.buildingResoureKey;
		const std::vector<uint32_t>& styles = info.unavailableStyles;

		cRZBaseString filePath;

		if (TryGetDBPFFilePathFromResourceKey(resMan, key, filePath))
		{
			logger.WriteFormatted(LogLevel::Error, filePath.ToChar());
			logger.Write(LogLevel::Error, ": ");
		}

		logger.WriteFormatted(
			LogLevel::Error,
			"Building exemplar 0x%08X, 0x%08X, 0x%08X only has unsupported style id(s): ",
			key.type,
			key.group,
			key.instance);

		const size_t styleCount = styles.size();
		const size_t lastStyleIndex = styleCount - 1;

		for (size_t i = 0; i < styleCount; i++)
		{
			logger.WriteFormatted(LogLevel::Error, "0x%X", styles[i]);

			if (i < lastStyleIndex)
			{
				logger.Write(LogLevel::Error, ", ");
			}
		}

		logger.Write(LogLevel::Error, ".\n");
	}
}

IUnavailableUIBuildingStyles* spUnavailableUIBuildingStyles;

UnavailableUIBuildingStyles::UnavailableUIBuildingStyles()
	: initialized(false),
	  unavailableBuildingStyleIDs()
{
	spUnavailableUIBuildingStyles = this;
}

void UnavailableUIBuildingStyles::Initialize(
	cISC4City& city,
	const BuildingStyleCollection& availableUIStyles)
{
	if (!initialized)
	{
		initialized = true;

		cIGZPersistResourceManagerPtr resMan;

		if (resMan)
		{
			const std::vector<uint32_t> sortedUIStyles = GetAvailableStylesAsSortedVector(availableUIStyles);
			const auto unavailableStyles = GetUnavailableBuildingStyles(city, *resMan, sortedUIStyles);

			if (!unavailableStyles.empty())
			{
				std::set<uint32_t> unavailableStylesSet;

				Logger& logger = Logger::GetInstance();

				logger.Write(LogLevel::Error, "\n");

				WriteUnavailableStyleErrorLogHeader(logger, sortedUIStyles, availableUIStyles);

				for (const auto& item : unavailableStyles)
				{
					LogUnavailableStyleError(logger, *resMan, item);
					unavailableStylesSet.insert(item.unavailableStyles.begin(), item.unavailableStyles.end());
				}

				logger.Write(LogLevel::Error, "\n");
				logger.Flush();

				unavailableBuildingStyleIDs.reserve(unavailableStylesSet.size());
				unavailableBuildingStyleIDs.assign(unavailableStylesSet.begin(), unavailableStylesSet.end());
			}
		}
	}
}

bool UnavailableUIBuildingStyles::Contains(uint32_t styleID) const
{
	return std::find(
		unavailableBuildingStyleIDs.begin(),
		unavailableBuildingStyleIDs.end(),
		styleID) != unavailableBuildingStyleIDs.end();
}
