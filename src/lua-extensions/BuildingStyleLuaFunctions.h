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

#pragma once
#include "cISCLua.h"

namespace BuildingStyleLuaFunctions
{
	// Gets a value indicating if 'Build all styles at once' is enabled.
	int32_t is_build_all_styles_at_once(lua_State* pState);

	// Gets the number of years between styles changes when
	// 'Build all styles at once' is disabled.
	int32_t get_years_between_style_changes(lua_State* pState);

	// Gets a table containing the numeric values of the available styles.
	int32_t get_available_style_ids(lua_State* pState);

	// Gets a table containing the available styles as a name/id pair.
	// The name is the first item in the pair, and the style id is the second.
	// The name is UTF-8 encoded.
	int32_t get_available_style_name_id_pairs(lua_State* pState);

	// Gets a value indicating if the specified style id is available in
	// the building style UI.
	int32_t is_style_available(lua_State* pState);

	// Gets a table containing the numeric values of the active styles.
	int32_t get_active_style_ids(lua_State* pState);

	// Gets a table containing the active styles as a name/id pair.
	// The name is the first item in the pair, and the style id is the second.
	// The name is UTF-8 encoded.
	int32_t get_active_style_name_id_pairs(lua_State* pState);

	// Sets the active styles to the values specified in styleIDTable.
	// The table must contain only numbers.
	int32_t set_active_style_ids(lua_State* pState);

	// Gets the style name for the specified style id, the name is
	// UTF-8 encoded.
	int32_t get_style_name(lua_State* pState);

	// Gets a value indicating the active wall to wall mode in the building style UI.
	int32_t get_wall_to_wall_mode(lua_State* pState);

	// Gets a value indicating if the specified option is active in
	// the building style UI.
	int32_t is_ui_button_checked(lua_State* pState);
}