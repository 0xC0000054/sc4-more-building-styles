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

#include "FileSystem.h"

#include <Windows.h>
#include "wil/resource.h"
#include "wil/win32_helpers.h"

using namespace std::string_view_literals;

namespace
{
	std::filesystem::path GetModulePath()
	{
		wil::unique_cotaskmem_string modulePath = wil::GetModuleFileNameW(wil::GetModuleInstanceHandle());

		return std::filesystem::path(modulePath.get());
	}

	std::filesystem::path GetDllFolderPath()
	{
		static std::filesystem::path dllFolderPath = GetModulePath().parent_path();

		return dllFolderPath;
	}
}

std::filesystem::path FileSystem::GetBuildingStylesIniFilePath()
{
	std::filesystem::path path = GetDllFolderPath();
	path /= L"BuildingStyles.ini"sv;

	return path;
}

std::filesystem::path FileSystem::GetConfigFilePath()
{
	std::filesystem::path path = GetDllFolderPath();
	path /= L"SC4MoreBuildingStyles.ini"sv;

	return path;
}

std::filesystem::path FileSystem::GetLogFilePath()
{
	std::filesystem::path path = GetDllFolderPath();
	path /= L"SC4MoreBuildingStyles.log"sv;

	return path;
}
