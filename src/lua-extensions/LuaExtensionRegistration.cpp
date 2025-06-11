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

#include "LuaExtensionRegistration.h"
#include "BuildingStyleLuaFunctions.h"
#include "BuildingStyleLuaFunctionTests.h"
#include "cISC4AdvisorSystem.h"
#include "DebugUtil.h"
#include "Logger.h"
#include "SCLuaUtil.h"
#include <array>
#include <string>
#include <utility>

namespace
{
	void RegisterLuaFunction(
		cISC4AdvisorSystem* pAdvisorSystem,
		const char* tableName,
		const std::string_view& functionName,
		lua_CFunction callback)
	{
		Logger& logger = Logger::GetInstance();

		SCLuaUtil::RegisterLuaFunctionStatus status = SCLuaUtil::RegisterLuaFunction(
			pAdvisorSystem,
			tableName,
			functionName,
			callback);

		if (status == SCLuaUtil::RegisterLuaFunctionStatus::Ok)
		{
			if (tableName)
			{
				logger.WriteLineFormatted(
					LogLevel::Info,
					"Registered the %s.%s function",
					tableName,
					std::string(functionName).c_str());
			}
			else
			{
				logger.WriteLineFormatted(
					LogLevel::Info,
					"Registered the %s function",
					std::string(functionName).c_str());
			}
		}
		else
		{
			if (status == SCLuaUtil::RegisterLuaFunctionStatus::InvalidParameter)
			{
				if (tableName)
				{
					logger.WriteLineFormatted(
						LogLevel::Info,
						"Failed to register the %s.%s function. "
						"One or more SCLuaUtil::RegisterLuaFunction parameters were invalid.",
						tableName,
						std::string(functionName).c_str());
				}
				else
				{
					logger.WriteLineFormatted(
						LogLevel::Info,
						"Failed to register the %s function. "
						"One or more SCLuaUtil::RegisterLuaFunction parameters were invalid.",
						std::string(functionName).c_str());
				}
			}
			else if (status == SCLuaUtil::RegisterLuaFunctionStatus::TableWrongType)
			{
				if (tableName)
				{
					logger.WriteLineFormatted(
						LogLevel::Info,
						"Failed to register the %s.%s function. The %s object is not a Lua table.",
						tableName,
						std::string(functionName).c_str(),
						tableName);
				}
				else
				{
					logger.WriteLineFormatted(
						LogLevel::Info,
						"Failed to register the %s function.",
						std::string(functionName).c_str());
				}
			}
			else
			{
				if (tableName)
				{
					logger.WriteLineFormatted(
						LogLevel::Info,
						"Failed to register the %s.%s function. "
						"Is SC4MoreBuildingStyles.dat in the plugins folder?",
						tableName,
						std::string(functionName).c_str());
				}
				else
				{
					logger.WriteLineFormatted(
						LogLevel::Info,
						"Failed to register the %s function.",
						std::string(functionName).c_str());
				}
			}
		}
	}

	void RegisterBuildingStyleLuaFunctions(cISC4AdvisorSystem* pAdvisorSystem)
	{
		constexpr std::array<std::pair<std::string_view, lua_CFunction>, 11> BuildingStyleLuaRegistrationArray =
		{
			std::pair("is_build_all_styles_at_once", BuildingStyleLuaFunctions::is_build_all_styles_at_once),
			std::pair("get_years_between_style_changes", BuildingStyleLuaFunctions::get_years_between_style_changes),
			std::pair("get_available_style_ids", BuildingStyleLuaFunctions::get_available_style_ids),
			std::pair("get_available_style_name_id_pairs", BuildingStyleLuaFunctions::get_available_style_name_id_pairs),
			std::pair("is_style_available", BuildingStyleLuaFunctions::is_style_available),
			std::pair("get_active_style_ids", BuildingStyleLuaFunctions::get_active_style_ids),
			std::pair("get_active_style_name_id_pairs", BuildingStyleLuaFunctions::get_active_style_name_id_pairs),
			std::pair("set_active_style_ids", BuildingStyleLuaFunctions::set_active_style_ids),
			std::pair("get_style_name", BuildingStyleLuaFunctions::get_style_name),
			std::pair("get_wall_to_wall_mode", BuildingStyleLuaFunctions::get_wall_to_wall_mode),
			std::pair("is_ui_button_checked", BuildingStyleLuaFunctions::is_ui_button_checked)
		};

		for (const auto& item : BuildingStyleLuaRegistrationArray)
		{
			RegisterLuaFunction(
				pAdvisorSystem,
				"building_style",
				item.first,
				item.second);
		}
	}
}

void LuaExtensionRegistration::Register(cISC4AdvisorSystem* pAdvisorSystem)
{
	RegisterBuildingStyleLuaFunctions(pAdvisorSystem);

#ifdef _DEBUG
	BuildingStyleLuaFunctionTests::Run();
#endif // _DEBUG
}
