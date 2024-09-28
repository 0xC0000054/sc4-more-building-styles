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
#include "cIGZVariant.h"
#include "cISC4BuildingOccupant.h"
#include "cISC4Occupant.h"
#include "cISCProperty.h"
#include "cISCPropertyHolder.h"
#include "cRZAutoRefCount.h"
#include "cRZBaseString.h"
#include "GlobalPointers.h"
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
				case cISC4BuildingOccupant::PurposeType::Agriculture:
					result = spPreferences->AgriculturePurposeTypeSupportsBuildingStyles();
					break;
				case cISC4BuildingOccupant::PurposeType::Processing:
					result = spPreferences->ProcessingPurposeTypeSupportsBuildingStyles();
					break;
				case cISC4BuildingOccupant::PurposeType::Manufacturing:
					result = spPreferences->ManufacturingPurposeTypeSupportsBuildingStyles();
					break;
				case cISC4BuildingOccupant::PurposeType::HighTech:
					result = spPreferences->HighTechPurposeTypeSupportsBuildingStyles();
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
			const BuildingStyleCollection& availableBuildingStyles = buildingWinManager.GetAvailableBuildingStyles();

			for (const auto& item : availableBuildingStyles)
			{
				pStyles[styleCount] = item.styleID;
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
	const BuildingStyleCollection& availableBuildingStyles = buildingWinManager.GetAvailableBuildingStyles();

	for (const auto& item : availableBuildingStyles)
	{
		if (item.styleID == style)
		{
			name = item.styleName;
			return true;
		}
	}

	return false;
}

bool BuildingStyleInfo::GetBuildingStyleNames(cISC4Occupant* pOccupant, cIGZString& destination) const
{
	bool result = false;

	destination.Erase(0, destination.Strlen());

	if (OccupantSupportsBuildingStyles(pOccupant))
	{
		const BuildingStyleCollection& availableBuildingStyles = buildingWinManager.GetAvailableBuildingStyles();

		if (availableBuildingStyles.size() > 0)
		{
			const std::string_view separator(", ");

			const cISCPropertyHolder* pPropertyHolder = pOccupant->AsPropertyHolder();

			if (pPropertyHolder)
			{
				constexpr uint32_t kOccupantGroupsProperty = 0xAA1DD396;

				const cISCProperty* pOccupantGroupsProperty = pPropertyHolder->GetProperty(kOccupantGroupsProperty);

				if (pOccupantGroupsProperty)
				{
					const cIGZVariant* pVariant = pOccupantGroupsProperty->GetPropertyValue();

					if (pVariant)
					{
						const uint32_t* pDataStart = pVariant->RefUint32();
						const uint32_t* pDataEnd = pDataStart + pVariant->GetCount();

						for (const auto& item : availableBuildingStyles)
						{
							if (std::find(pDataStart, pDataEnd, item.styleID) != pDataEnd)
							{
								destination.Append(item.styleName);
								destination.Append(separator.data(), separator.size());
							}
						}
					}
				}
			}

			// Check that at least one style name has been written to the destination.
			if (destination.Strlen() > 0)
			{
				// Remove the trailing separator from the last style in the list.
				destination.Resize(destination.Strlen() - separator.size());
				result = true;
			}
		}
	}

	return result;
}

bool BuildingStyleInfo::IsBuildingStyleAvailable(uint32_t style) const
{
	const BuildingStyleCollection& availableBuildingStyles = buildingWinManager.GetAvailableBuildingStyles();

	for (const auto& item : availableBuildingStyles)
	{
		if (item.styleID == style)
		{
			return true;
		}
	}

	return false;
}
