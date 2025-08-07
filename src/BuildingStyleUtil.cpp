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

#include "BuildingStyleUtil.h"
#include "BuildingStyleButtons.h"
#include "IUnavailableUIBuildingStyles.h"
#include "MaxisBuildingStyleUIControlIDs.h"
#include "PropertyIDs.h"
#include "ReservedStyleIDs.h"
#include "frozen/unordered_set.h"
#include <algorithm>


namespace
{
	static constexpr frozen::unordered_set<uint32_t, 24> PIMXBuildingTemplateIDs =
	{
		0x2c8fbb95, // (R $) Low Wealth
		0xf6a23754, // (R $) Low Wealth W2W
		0x6c8fbba5, // (R $$) Medium Wealth
		0xce253a47, // (R $$) Medium Wealth W2W
		0x0c8fbbae, // (R $$$) High Wealth
		0x4ec7d949, // (R $$$) High Wealth W2W
		0x8c8fbbcc, // (CS $) Low Wealth
		0xbcd2c7e1, // (CS $) Low Wealth W2W
		0x0c8fbbdc, // (CS $$) Medium Wealth
		0x539f3c1d, // (CS $$) Medium Wealth W2W
		0xac8fbbeb, // (CS $$$) High Wealth
		0x3b3f3641, // (CS $$$) High Wealth W2W
		0x6c8fbbf5, // (CO $$) Medium Wealth
		0x452925b6, // (CO $$) Medium Wealth W2W
		0xcc8fbc01, // (CO $$$) High Wealth
		0x8840bc41, // (CO $$$) High Wealth W2W
		0x2caa4d2a, // (I-a) Agricultural Industry
		0xc035b844, // (I-a) Agricultural Industry W2W
		0x2c8fbc17, // (I-d) Dirty Industry
		0x4b2d4f0b, // (I-d) Dirty Industry W2W
		0x6c7e983b, // (I-m) Manufacturing Industry
		0xee28b711, // (I-m) Manufacturing Industry W2W
		0x6c8fbddc, // (I-ht) High-Tech Industry
		0x3b238bc0, // (I-ht) High-Tech Industry W2W
	};

	bool IsPIMXTemplateBuildingExemplar(cISCPropertyHolder* pPropertyHolder)
	{
		const uint32_t kExemplarCategoryPropertyID = 0x2C8F8746;

		bool result = false;

		// The PIMX building templates set the ExemplarCategory property to
		// the template id, which we use to detect the exemplars it modified.

		const PropertyData<uint32_t> exemplarCategory(pPropertyHolder, kExemplarCategoryPropertyID);

		if (exemplarCategory)
		{
			if (exemplarCategory.size() == 1)
			{
				const uint32_t category = exemplarCategory[0];

				result = PIMXBuildingTemplateIDs.contains(category);
			}
		}

		return result;
	}

	bool IsPIMXStyle2004Placeholder(
		cISCPropertyHolder* pPropertyHolder,
		const PropertyData<uint32_t>& buildingStyles)
	{
		bool result = false;

		// Some versions of PIMX accidentally used the community style id 0x2004
		// as a placeholder in the Building Styles property.

		if (buildingStyles.size() == 1 && buildingStyles[0] == 0x2004)
		{
			// Detect PIMX and fall back to the Maxis styles if the
			// Building Styles PIMX Template Marker is not present.
			if (IsPIMXTemplateBuildingExemplar(pPropertyHolder))
			{
				result = !pPropertyHolder->HasProperty(kBuildingStylesPIMXTemplateMarker);
			}
		}

		return result;
	}
}

bool BuildingStyleUtil::IsMaxisStyleID(uint32_t style)
{
	// We check the high value first as an optimization, custom building styles
	// should be 0x2004 or higher.
	return style <= 0x2003 && style >= 0x2000;
}

bool BuildingStyleUtil::IsReservedStyleID(uint32_t style)
{
	// The following values are invalid for use as a style id:
	// 1. All values in the DLL's special button range.
	// 2. The PIM-X Building Style property template placeholder.
	// 3. The Maxis Building Style UI control ids.
	// 4. The DLL's optional Building Style UI control ids.

	return style <= StyleControlReservedButtonRangeEnd
		|| style == PIMXPlaceholderStyleID
		|| MaxisUIControlIDs.contains(style)
		|| OptionalButtonIDs.contains(style);
}

bool BuildingStyleUtil::IsStyleIDReservedOrNotInUI(uint32_t style)
{
	return IsReservedStyleID(style) || spUnavailableUIBuildingStyles->Contains(style);
}

bool BuildingStyleUtil::TryReadBuildingStylesProperty(
	cISCPropertyHolder* pPropertyHolder,
	PropertyData<uint32_t>& output)
{
	bool result = false;
	PropertyData<uint32_t> temp(pPropertyHolder, kBuildingStylesProperty);

	if (temp)
	{
		// A Building Styles property set to the PIM-X placeholder style is currently
		// used on over 100 released buildings.
		// This style acts as a blocker when the DLL is installed with one of these
		// updated buildings.
		// Additionally, there are a number of other values that can't be used as a
		// style id such as the control ids in the Building Style Control UI.
		//
		// We also handle the case where the Building Styles property doesn't contain
		// any styles that are present as check boxes in the Building Style Control UI,
		// which would act as a blocker for those buildings.
		//
		// Buildings that have only the reserved or unavailable style ids in their
		// Building Styles property will be made to use the legacy Maxis styles
		// in the Occupant Groups property.

		if (std::find_if_not(
			temp.begin(),
			temp.end(),
			IsStyleIDReservedOrNotInUI) != temp.end())
		{
			// Some versions of PIMX accidentally used the community style id 0x2004
			// as a placeholder in the Building Styles property, fall back to the
			// Maxis styles in that case.
			if (!IsPIMXStyle2004Placeholder(pPropertyHolder, temp))
			{
				output = std::move(temp);
				result = true;
			}
		}
	}

	return result;
}
