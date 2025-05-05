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

#include "Patcher.h"

#include <Windows.h>
#include "wil/resource.h"
#include "wil/win32_helpers.h"

void Patcher::InstallJump(uintptr_t address, uintptr_t destination)
{
	DWORD oldProtect;
	THROW_IF_WIN32_BOOL_FALSE(VirtualProtect(reinterpret_cast<void*>(address), 5, PAGE_EXECUTE_READWRITE, &oldProtect));

	*((uint8_t*)address) = 0xE9;
	*((uintptr_t*)(address + 1)) = destination - address - 5;
}

void Patcher::InstallCallHook(uintptr_t address, void(*pfnFunc)(void))
{
	InstallCallHook(address, reinterpret_cast<uintptr_t>(pfnFunc));
}

void Patcher::InstallCallHook(uintptr_t address, uintptr_t pfnFunc)
{
	DWORD oldProtect;
	THROW_IF_WIN32_BOOL_FALSE(VirtualProtect(reinterpret_cast<void*>(address), 5, PAGE_EXECUTE_READWRITE, &oldProtect));

	*((uint8_t*)address) = 0xE8;
	*((uintptr_t*)(address + 1)) = pfnFunc - address - 5;
}
