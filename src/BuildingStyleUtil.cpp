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
#include <algorithm>

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
		|| MaxisUIControlIDs.count(style) != 0
		|| OptionalButtonIDs.count(style) != 0;
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
			output = std::move(temp);
			result = true;
		}
	}

	return result;
}
