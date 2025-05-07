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
#include "MaxisBuildingStyleUIControlIDs.h"
#include "ReservedStyleIDs.h"

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
