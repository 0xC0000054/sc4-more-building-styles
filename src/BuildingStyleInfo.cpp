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
#include "BuildingUtil.h"
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
#include "PropertyIDs.h"

namespace
{
	const std::string_view StyleNameListSeperator(", ");

	size_t GetStyleNamesFromVariant(
		const cIGZVariant& variant,
		const BuildingStyleCollection& availableBuildingStyles,
		cIGZString& destination)
	{
		size_t styleNameCount = 0;

		const uint32_t* pData = variant.RefUint32();
		const uint32_t repCount = variant.GetCount();

		if (repCount > 0)
		{
			const uint32_t* pDataEnd = pData + repCount;

			for (const auto& item : availableBuildingStyles)
			{
				if (std::find(pData, pDataEnd, item.styleID) != pDataEnd)
				{
					if (styleNameCount > 1)
					{
						destination.Append(StyleNameListSeperator.data(), StyleNameListSeperator.size());
					}

					destination.Append(item.styleName);
					styleNameCount++;
				}
			}
		}
		else
		{
			const uint32_t targetStyleID = reinterpret_cast<uint32_t>(pData);

			for (const auto& item : availableBuildingStyles)
			{
				if (item.styleID == targetStyleID)
				{
					destination.Append(item.styleName);
					styleNameCount = 1;
					break;
				}
			}
		}

		return styleNameCount;
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

	const cISC4BuildingOccupant::PurposeType purpose = BuildingUtil::GetPurposeType(pOccupant);

	if (BuildingUtil::PurposeTypeSupportsBuildingStyles(purpose))
	{
		const BuildingStyleCollection& availableBuildingStyles = buildingWinManager.GetAvailableBuildingStyles();

		if (availableBuildingStyles.size() > 0)
		{
			const cISCPropertyHolder* pPropertyHolder = pOccupant->AsPropertyHolder();
			size_t styleNameCount = 0;

			if (pPropertyHolder)
			{
				const cISCProperty* pProperty = pPropertyHolder->GetProperty(kBuildingStylesProperty);

				if (pProperty)
				{
					const cIGZVariant* pVariant = pProperty->GetPropertyValue();

					if (pVariant)
					{
						styleNameCount = GetStyleNamesFromVariant(
							*pVariant,
							availableBuildingStyles,
							destination);
					}
				}
				else
				{
					if (BuildingUtil::IsIndustrialBuilding(purpose))
					{
						// Industrial buildings without a BuildingStyles property
						// are compatible with all building styles.

						for (const auto& item : availableBuildingStyles)
						{
							if (styleNameCount > 1)
							{
								destination.Append(StyleNameListSeperator.data(), StyleNameListSeperator.size());
							}

							destination.Append(item.styleName);
							styleNameCount++;
						}
					}
					else
					{
						pProperty = pPropertyHolder->GetProperty(kOccupantGroupsProperty);

						if (pProperty)
						{
							const cIGZVariant* pVariant = pProperty->GetPropertyValue();

							if (pVariant)
							{
								styleNameCount = GetStyleNamesFromVariant(
									*pVariant,
									availableBuildingStyles,
									destination);
							}
						}
					}
				}
			}

			// Check that at least one style name has been written to the destination.
			if (destination.Strlen() > 0)
			{
				if (styleNameCount > 1)
				{
					// Remove the trailing separator from the last style in the list.
					destination.Resize(destination.Strlen() - StyleNameListSeperator.size());
				}
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
