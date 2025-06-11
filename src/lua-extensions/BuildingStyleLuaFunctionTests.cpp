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

#include "BuildingStyleLuaFunctionTests.h"
#include "BuildingStyleLuaFunctions.h"
#include "cISC4AdvisorSystem.h"
#include "cISC4App.h"
#include "cISC4City.h"
#include "cISC4TractDeveloper.h"
#include "cISCStringDetokenizer.h"
#include "DebugUtil.h"
#include "GZServPtrs.h"
#include "Logger.h"
#include "SC4Vector.h"
#include "SCLuaUtil.h"
#include <array>
#include <string>
#include <utility>

namespace
{
	void RunTestFunction(cIGZLua5Thread* pIGZLua5Thread, const char* functionName)
	{
		int32_t top = pIGZLua5Thread->GetTop();

		pIGZLua5Thread->GetGlobal(functionName);

		int32_t functionTop = pIGZLua5Thread->GetTop();

		if (functionTop != top)
		{
			if (pIGZLua5Thread->IsFunction(-1))
			{
				// The test functions take no parameters and return a string.

				int32_t status = pIGZLua5Thread->CallProtected(0, 1, 0, false);

				if (status == 0)
				{
					if (pIGZLua5Thread->IsString(-1))
					{
						const char* result = pIGZLua5Thread->ToString(-1);

						DebugUtil::PrintLineToDebugOutputFormatted(
							"%s() returned: %s",
							functionName,
							result ? result : "<null>");
					}
					else
					{
						DebugUtil::PrintLineToDebugOutputFormatted(
							"%s() did not return a string.",
							functionName);
					}
				}
				else
				{
					const char* errorString = pIGZLua5Thread->ToString(-1);

					DebugUtil::PrintLineToDebugOutputFormatted(
						"Error status code %d returned when calling %s(), error text: %s.",
						status,
						functionName,
						errorString ? errorString : "");

					// Pop the error string off the stack.
					pIGZLua5Thread->Pop(1);
				}
			}
			else
			{
				DebugUtil::PrintLineToDebugOutputFormatted(
					"%s is not a function, actual type: %s.",
					functionName,
					pIGZLua5Thread->TypeName(-1));
			}
		}
		else
		{
			DebugUtil::PrintLineToDebugOutputFormatted(
				"%s() does not exist.",
				functionName);
		}

		pIGZLua5Thread->SetTop(top);
	}

	void RunLuaScriptTests(cISC4AdvisorSystem* pAdvisorSystem, cIGZLua5Thread* pIGZLua5Thread)
	{
		constexpr std::array<const char*, 13> BuildingStyleLuaTestFunctions =
		{
			"null45_building_style_test_is_build_all_styles_at_once",
			"null45_building_style_test_get_years_between_style_changes",
			"null45_building_style_test_get_available_style_ids",
			"null45_building_style_test_get_available_style_name_id_pairs",
			"null45_building_style_test_is_style_available",
			"null45_building_style_test_get_active_style_ids",
			"null45_building_style_test_get_active_style_name_id_pairs",
			"null45_building_style_test_set_active_style_ids_1",
			"null45_building_style_test_set_active_style_ids_2",
			"null45_building_style_test_set_active_style_ids_3",
			"null45_building_style_test_get_style_name",
			"null45_building_style_test_get_wall_to_wall_mode",
			"null45_building_style_test_is_ui_button_checked",
		};

		for (const auto& item : BuildingStyleLuaTestFunctions)
		{
			RunTestFunction(pIGZLua5Thread, item);
		}
	}
}

void BuildingStyleLuaFunctionTests::Run()
{
	cISC4AppPtr sc4App;

	cISC4City* pCity = sc4App->GetCity();

	if (pCity)
	{
		cISC4AdvisorSystem* pAdvisorSystem = pCity->GetAdvisorSystem();

		cIGZLua5Thread* pIGZLua5Thread = pAdvisorSystem->GetScriptingContext()->AsIGZLua5()->AsIGZLua5Thread();
		cISC4TractDeveloper* pTractDeveloper = pCity->GetTractDeveloper();

		// We preserve the existing city styles and restore them after running the tests.
		// This ensures that the building_style.set_active_style_ids tests don't modify
		// the loaded city data.

		SC4Vector<uint32_t> cityStyles = pTractDeveloper->GetActiveStyles();

		RunLuaScriptTests(pAdvisorSystem, pIGZLua5Thread);

		pTractDeveloper->SetActiveStyles(cityStyles);
	}
}
