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

#include "BuildingStyleLuaFunctions.h"
#include "BuildingStyleButtons.h"
#include "cRZBaseString.h"
#include "GlobalPointers.h"
#include "SC4Vector.h"
#include "SCLuaUtil.h"
#include "SafeInt.hpp"
#include <algorithm>

namespace
{
	bool TryGetLuaValueAsUint32(cISCLua* pLua, int32_t index, uint32_t& result)
	{
		if (pLua->IsNumber(index))
		{
			const double luaNumber = pLua->ToNumber(index);

			if (SafeCast(luaNumber, result))
			{
				return true;
			}
		}

		return false;
	}

	std::vector<uint32_t> GetTableValuesAsUint32Vector(cISCLua* pLua, int32_t index)
	{
		std::vector<uint32_t> results;

		if (pLua->Type(index) == cIGZLua5Thread::LuaTypeTable)
		{
			pLua->PushNil(); // first item

			while (pLua->Next(index) != 0)
			{
				uint32_t value = 0;

				if (TryGetLuaValueAsUint32(pLua, -1, value))
				{
					results.push_back(value);
					pLua->Pop(1);
				}
				else
				{
					// The table entry is not an Uint32.
					results.clear();
					pLua->Pop(1);
					break;
				}
			}
		}

		return results;
	}

	enum UIButton : uint32_t
	{
		UIButtonNoKickOut = 0,
	};

	enum WallToWallMode : uint32_t
	{
		WallToWallModeMixed = 0,
		WallToWallModeOnly = 1,
		WallToWallModeBlock = 2,
	};
}

int32_t BuildingStyleLuaFunctions::is_build_all_styles_at_once(lua_State* pState)
{
	cRZAutoRefCount<cISCLua> lua = SCLuaUtil::GetISCLuaFromFunctionState(pState);

	lua->PushBoolean(spBuildingSelectWinManager->GetTractDeveloper()->IsUsingAllStylesAtOnce());
	return 1;
}

int32_t BuildingStyleLuaFunctions::get_years_between_style_changes(lua_State* pState)
{
	cRZAutoRefCount<cISCLua> lua = SCLuaUtil::GetISCLuaFromFunctionState(pState);

	const cISC4TractDeveloper* pTractDeveloper = spBuildingSelectWinManager->GetTractDeveloper();

	if (pTractDeveloper->IsUsingAllStylesAtOnce())
	{
		// We use nil to indicate that the value is not applicable to the user's current
		// game settings.
		// We could also return 0 in this case, but nil seems to be a better option.
		lua->PushNil();
	}
	else
	{
		lua->PushNumber(pTractDeveloper->GetYearsBetweenStyleChanges());
	}

	return 1;
}

int32_t BuildingStyleLuaFunctions::get_available_style_ids(lua_State* pState)
{
	cRZAutoRefCount<cISCLua> lua = SCLuaUtil::GetISCLuaFromFunctionState(pState);

	const auto& availableBuildingStyles = spBuildingSelectWinManager->GetAvailableBuildingStyles();

	// Push a new table onto the stack.
	lua->NewTable();

	int32_t luaTableIndex = 1; // Lua table indexing start at 1.

	for (const auto& entry : availableBuildingStyles)
	{
		lua->PushNumber(entry.styleID);
		lua->RawSetI(-2, luaTableIndex++);
	}

	return 1;
}

int32_t BuildingStyleLuaFunctions::get_available_style_name_id_pairs(lua_State* pState)
{
	cRZAutoRefCount<cISCLua> lua = SCLuaUtil::GetISCLuaFromFunctionState(pState);

	const auto& availableBuildingStyles = spBuildingSelectWinManager->GetAvailableBuildingStyles();

	// Push a new table onto the stack.
	lua->NewTable();

	for (const auto& entry : availableBuildingStyles)
	{
		if (entry.styleName.Strlen() > 0)
		{
			const cRZBaseString& name = entry.styleName;
			lua->PushLString(name.ToChar(), name.Strlen());
		}
		else
		{
			cRZBaseString name;
			name.Sprintf("0x%X", entry.styleID);

			lua->PushLString(name.ToChar(), name.Strlen());
		}

		lua->PushNumber(entry.styleID);
		lua->SetTable(-3);
	}

	return 1;
}

int32_t BuildingStyleLuaFunctions::is_style_available(lua_State* pState)
{
	bool result = false;

	cRZAutoRefCount<cISCLua> lua = SCLuaUtil::GetISCLuaFromFunctionState(pState);

	const int32_t parameterCount = lua->GetTop();

	if (parameterCount == 1)
	{
		uint32_t styleId = 0;

		if (TryGetLuaValueAsUint32(lua, -1, styleId))
		{
			result = spBuildingSelectWinManager->GetAvailableBuildingStyles().contains_style(styleId);
		}
	}

	lua->PushBoolean(result);
	return 1;
}

int32_t BuildingStyleLuaFunctions::get_active_style_ids(lua_State* pState)
{
	cRZAutoRefCount<cISCLua> lua = SCLuaUtil::GetISCLuaFromFunctionState(pState);

	const auto& activeStyles = spBuildingSelectWinManager->GetTractDeveloper()->GetActiveStyles();

	// Push a new table onto the stack.
	lua->NewTable();

	int32_t luaTableIndex = 1; // Lua table indexing start at 1.

	for (const auto& styleID : activeStyles)
	{
		lua->PushNumber(styleID);
		lua->RawSetI(-2, luaTableIndex++);
	}

	return 1;
}

int32_t BuildingStyleLuaFunctions::get_active_style_name_id_pairs(lua_State* pState)
{
	cRZAutoRefCount<cISCLua> lua = SCLuaUtil::GetISCLuaFromFunctionState(pState);

	const auto& availableBuildingStyles = spBuildingSelectWinManager->GetAvailableBuildingStyles();
	const auto& activeStyles = spBuildingSelectWinManager->GetTractDeveloper()->GetActiveStyles();

	// Push a new table onto the stack.
	lua->NewTable();

	for (const auto& styleID : activeStyles)
	{
		const auto styleEntry = availableBuildingStyles.find_style(styleID);

		if (styleEntry != availableBuildingStyles.end() && styleEntry->styleName.Strlen() > 0)
		{
			const cRZBaseString& name = styleEntry->styleName;
			lua->PushLString(name.ToChar(), name.Strlen());
		}
		else
		{
			cRZBaseString name;
			name.Sprintf("0x%X", styleID);

			lua->PushLString(name.ToChar(), name.Strlen());
		}

		lua->PushNumber(styleID);
		lua->SetTable(-3);
	}

	return 1;
}

int32_t BuildingStyleLuaFunctions::set_active_style_ids(lua_State* pState)
{
	bool result = false;

	cRZAutoRefCount<cISCLua> lua = SCLuaUtil::GetISCLuaFromFunctionState(pState);

	const int32_t parameterCount = lua->GetTop();

	if (parameterCount == 1)
	{
		std::vector<uint32_t> vector = GetTableValuesAsUint32Vector(lua, 1);

		if (!vector.empty())
		{
			// Sort the values and remove duplicates before handing the data to the game.
			std::sort(vector.begin(), vector.end());
			auto last = std::unique(vector.begin(), vector.end());
			if (last != vector.end())
			{
				vector.erase(last, vector.end());
			}

			SC4Vector<uint32_t> sc4Vector;
			sc4Vector.push_back(vector.data(), vector.size());

			spBuildingSelectWinManager->GetTractDeveloper()->SetActiveStyles(sc4Vector);
			result = true;
		}
	}

	lua->PushBoolean(result);
	return 1;
}

int32_t BuildingStyleLuaFunctions::get_style_name(lua_State* pState)
{
	cRZAutoRefCount<cISCLua> lua = SCLuaUtil::GetISCLuaFromFunctionState(pState);

	const int32_t parameterCount = lua->GetTop();

	if (parameterCount == 1)
	{
		uint32_t styleId = 0;

		if (TryGetLuaValueAsUint32(lua, -1, styleId))
		{
			const auto& availableStyles = spBuildingSelectWinManager->GetAvailableBuildingStyles();

			const auto entry = availableStyles.find_style(styleId);

			if (entry != availableStyles.end())
			{
				const cRZBaseString& name = entry->styleName;

				lua->PushLString(name.ToChar(), name.Strlen());
				return 1;
			}
		}
	}

	lua->PushNil();
	return 1;
}

int32_t BuildingStyleLuaFunctions::get_wall_to_wall_mode(lua_State* pState)
{
	cRZAutoRefCount<cISCLua> lua = SCLuaUtil::GetISCLuaFromFunctionState(pState);

	switch (spBuildingSelectWinManager->GetContext().GetWallToWallOption())
	{
	case IBuildingSelectWinContext::WallToWallOption::Only:
		lua->PushNumber(WallToWallModeOnly);
		break;
	case IBuildingSelectWinContext::WallToWallOption::Block:
		lua->PushNumber(WallToWallModeBlock);
		break;
	case IBuildingSelectWinContext::WallToWallOption::Mixed:
	default:
		lua->PushNumber(WallToWallModeMixed);
		break;
	}

	return 1;
}

int32_t BuildingStyleLuaFunctions::is_ui_button_checked(lua_State* pState)
{
	bool result = false;

	cRZAutoRefCount<cISCLua> lua = SCLuaUtil::GetISCLuaFromFunctionState(pState);

	const int32_t parameterCount = lua->GetTop();

	if (parameterCount == 1)
	{
		uint32_t buttonID = 0;

		if (TryGetLuaValueAsUint32(lua, -1, buttonID))
		{
			switch (buttonID)
			{
			case UIButtonNoKickOut:
				result = spBuildingSelectWinManager->GetContext().GetOptionalCheckBoxState(NoKickOutLowerWealthButtonID);
				break;
			}
		}
	}

	lua->PushBoolean(result);
	return 1;
}
