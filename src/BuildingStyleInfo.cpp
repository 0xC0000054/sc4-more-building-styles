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

#include "BuildingStyleInfo.h"
#include "cIGZString.h"
#include "cISC4BuildingOccupant.h"
#include "cISC4Occupant.h"
#include "cRZAutoRefCount.h"
#include "cRZBaseString.h"
#include "IBuildingSelectWinManager.h"

namespace
{
	bool OccupantSupportsBuildingStyles(cISC4Occupant* pOccupant)
	{
		bool result = false;

		if (pOccupant)
		{
			cRZAutoRefCount<cISC4BuildingOccupant> pBuildingOccupant;

			if (pOccupant->QueryInterface(GZIID_cISC4BuildingOccupant, pBuildingOccupant.AsPPVoid()))
			{
				const cISC4BuildingOccupant::BuildingProfile& profile = pBuildingOccupant->GetBuildingProfile();

				// Only Residential Commercial Services and Commercial Office buildings support building styles.
				switch (profile.purpose)
				{
				case cISC4BuildingOccupant::PurposeType::Residence:
				case cISC4BuildingOccupant::PurposeType::Services:
				case cISC4BuildingOccupant::PurposeType::Office:
					result = true;
					break;
				}
			}
		}

		return result;
	}
}

BuildingStyleInfo::BuildingStyleInfo(const IBuildingSelectWinManager& buildingWinManager)
	: refCount(0),
	  buildingWinManager(buildingWinManager)
{
}

bool BuildingStyleInfo::QueryInterface(uint32_t riid, void** ppvObj)
{
	if (riid == GZIID_cIBuildingStyleInfo)
	{
		*ppvObj = static_cast<cIBuildingStyleInfo*>(this);
		AddRef();

		return true;
	}
	else if (riid == GZIID_cIGZUnknown)
	{
		*ppvObj = static_cast<cIGZUnknown*>(this);
		AddRef();

		return true;
	}

	return false;
}

uint32_t BuildingStyleInfo::AddRef()
{
	return ++refCount;
}

uint32_t BuildingStyleInfo::Release()
{
	if (refCount > 0)
	{
		--refCount;
	}

	return refCount;
}

uint32_t BuildingStyleInfo::GetAvailableBuildingStyleIds(uint32_t* pStyles, uint32_t size) const
{
	uint32_t styleCount = 0;

	if (pStyles)
	{
		if (size > 0)
		{
			const std::map<uint32_t, std::string>& availableBuildingStyles = buildingWinManager.GetAvailableBuildingStyles();

			for (const auto& item : availableBuildingStyles)
			{
				pStyles[styleCount] = item.first;
				styleCount++;

				if (styleCount >= size)
				{
					break;
				}
			}
		}
	}
	else
	{
		// If the array pointer is null and the size is 0, we return the total
		// number of available styles.
		// If the size is not zero that is an API usage error, and we return 0.
		if (size == 0)
		{
			styleCount = buildingWinManager.GetAvailableBuildingStyles().size();
		}
	}

	return styleCount;
}

bool BuildingStyleInfo::GetBuildingStyleName(uint32_t style, cIGZString& name) const
{
	bool result = false;

	const std::map<uint32_t, std::string>& availableBuildingStyles = buildingWinManager.GetAvailableBuildingStyles();

	const auto& item = availableBuildingStyles.find(style);

	if (item != availableBuildingStyles.end())
	{
		const std::string& styleName = item->second;

		name.FromChar(styleName.c_str(), styleName.size());
		result = true;
	}

	return result;
}

bool BuildingStyleInfo::GetBuildingStyleNames(cISC4Occupant* pOccupant, cIGZString& destination) const
{
	bool result = false;

	destination.Erase(0, destination.Strlen());

	if (OccupantSupportsBuildingStyles(pOccupant))
	{
		const std::map<uint32_t, std::string>& availableBuildingStyles = buildingWinManager.GetAvailableBuildingStyles();

		if (availableBuildingStyles.size() > 0)
		{
			const uint32_t lastItemIndex = availableBuildingStyles.size() - 1;
			const std::string_view separator(", ");
			size_t index = 0;

			for (const auto& item : availableBuildingStyles)
			{
				if (pOccupant->IsOccupantGroup(item.first))
				{
					const std::string& styleName = item.second;

					destination.Append(styleName.c_str(), styleName.size());

					if (index < lastItemIndex)
					{
						destination.Append(separator.data(), separator.size());
					}
				}
				index++;
			}

			// Check that at least one style name has been written to the destination.
			result = destination.Strlen() > 0;
		}
	}

	return result;
}

bool BuildingStyleInfo::IsBuildingStyleAvailable(uint32_t style) const
{
	return buildingWinManager.IsBuildingStyleAvailable(style);
}
