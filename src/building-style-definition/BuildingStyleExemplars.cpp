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

#include "BuildingStyleExemplars.h"
#include "BuildingStyleUtil.h"
#include "cGZPersistResourceKey.h"
#include "cIGZPersistResourceKeyList.h"
#include "cIGZPersistResourceManager.h"
#include "cISCResExemplar.h"
#include "cRZAutoRefCount.h"
#include "GZServPtrs.h"
#include "Logger.h"
#include "PersistResourceKeyFilterByTypeAndGroup.h"
#include "SCPropertyUtil.h"
#include "StringResourceKey.h"
#include "StringResourceManager.h"

namespace
{
	bool LoadStringResourceFromProperty(
		const cISCPropertyHolder* pPropertyHolder,
		uint32_t id,
		cRZAutoRefCount<cIGZString>& outString)
	{
		StringResourceKey key;

		return SCPropertyUtil::GetPropertyValue(pPropertyHolder, id, key)
			&& StringResourceManager::GetLocalizedString(key, outString);
	}

	bool ReadBooleanPropertyWithDefault(
		const cISCPropertyHolder* pPropertyHolder,
		uint32_t id,
		bool defaultValue)
	{
		bool value;

		return SCPropertyUtil::GetPropertyValue(pPropertyHolder, id, value) ? value : defaultValue;
	}
}

std::vector<DefinedBuildingStyleEntry> BuildingStyleExemplars::GetDefinedStyles(const std::unordered_set<uint32_t>& iniFileStyles)
{
	constexpr uint32_t kExemplarType = 0x6534284A;
	constexpr uint32_t kBuildingStyleExemplarGroup = 0xB06361D6;

	// The name of the building style that is associated with this exemplar.
	constexpr uint32_t kBuildingStyleNamePropertyID = 0x9CCFAD35;
	// Indicates if the check box text should be bold, like the Maxis styles.
	// The default is false.
	constexpr uint32_t kBuildingStyleNameUseBoldTextPropertyID = 0x9CCFAD36;
	// Specifies the resource key for the building style check box tool tip.
	// This is an alternative to the existing
	constexpr uint32_t kBuildingStyleNameToolTipKeyPropertyID = 0x9CCFAD37;

	Logger& logger = Logger::GetInstance();

	std::vector<DefinedBuildingStyleEntry> entries;

	cIGZPersistResourceManagerPtr rm;

	if (rm)
	{
		cRZAutoRefCount<PersistResourceKeyFilterByTypeAndGroup> filter(
			new PersistResourceKeyFilterByTypeAndGroup(kExemplarType, kBuildingStyleExemplarGroup),
			cRZAutoRefCount<PersistResourceKeyFilterByTypeAndGroup>::kAddRef);
		cRZAutoRefCount<cIGZPersistResourceKeyList> keyList;

		rm->GetAvailableResourceList(keyList.AsPPObj(), filter);

		const uint32_t size = keyList->Size();

		if (size > 0)
		{
			entries.reserve(size);

			for (uint32_t i = 0; i < size; i++)
			{
				const cGZPersistResourceKey& key = keyList->GetKey(i);

				const uint32_t styleID = key.instance;

				if (iniFileStyles.contains(styleID))
				{
					// The styles defined in the INI file take precedence over the ones defined by an exemplar.
					// This is done to preserve the behavior of the INI file allowing for fully custom styles.
					continue;
				}
				else if (BuildingStyleUtil::IsReservedStyleID(styleID))
				{
					logger.WriteLineFormatted(
						LogLevel::Error,
						"Style exemplar 0x%08X, 0x%08X, 0x%08X: The style id cannot have a value of 0x%08X.",
						key.type,
						key.group,
						key.instance,
						styleID);

					continue;
				}

				cRZAutoRefCount<cISCResExemplar> exemplar;

				if (rm->GetPrivateResource(
					key,
					GZIID_cISCResExemplar,
					exemplar.AsPPVoid(),
					0,
					nullptr))
				{
					const cISCPropertyHolder* pPropertyHolder = exemplar->AsISCPropertyHolder();

					cRZAutoRefCount<cIGZString> name;

					if (LoadStringResourceFromProperty(
						pPropertyHolder,
						kBuildingStyleNamePropertyID,
						name))
					{
						StringResourceKey styleToolTipKey;

						if (!SCPropertyUtil::GetPropertyValue(
							pPropertyHolder,
							kBuildingStyleNameToolTipKeyPropertyID,
							styleToolTipKey))
						{
							styleToolTipKey.groupID = 0;
							styleToolTipKey.instanceID = 0;
						}

						const bool boldText = ReadBooleanPropertyWithDefault(
							pPropertyHolder,
							kBuildingStyleNameUseBoldTextPropertyID,
							false);

						entries.emplace_back(key.instance, *name, styleToolTipKey, boldText);
					}
					else
					{
						logger.WriteLineFormatted(
							LogLevel::Error,
							"Style exemplar 0x%08X, 0x%08X, 0x%08X: The style name was not present or failed to load.",
							key.type,
							key.group,
							key.instance);
					}
				}
			}
		}
	}

	return entries;
}