/*
 * This file is part of sc4-more-building-styles, a DLL Plugin for
 * SimCity 4 that adds support for more building styles.
 *
 * Copyright (C) 2024, 2025, 2026 Nicholas Hayes
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

#include "BuildingDevelopmentSimulatorHooks.h"
#include "Logger.h"
#include "Patcher.h"
#include "SC4VersionDetection.h"
#include "wil/result.h"

namespace
{
	static uintptr_t DoMessage_FightThePowerCheatHook_JumpContinue = 0;

	void NAKED_FUN DoMessage_FightThePowerCheatHook()
	{
		__asm
		{
			// AL will be overwritten by the return value,
			// so we can use it as a scratch register.
			// This code inverts the Boolean value, making it
			// true/1 if the previous value was false/0 and vice versa.
			mov al, byte ptr[ebx + 0xd6]
			test al, al
			setz al
			mov byte ptr[ebx + 0xd6], al
			jmp DoMessage_FightThePowerCheatHook_JumpContinue
		}
	}

	static uintptr_t DoMessage_HowDryAmICheatHook_JumpContinue = 0;

	void NAKED_FUN DoMessage_HowDryAmICheatHook()
	{
		__asm
		{
			// AL will be overwritten by the return value,
			// so we can use it as a scratch register.
			// This code inverts the Boolean value, making it
			// true/1 if the previous value was false/0 and vice versa.
			mov al, byte ptr[ebx + 0xd5]
			test al, al
			setz al
			mov byte ptr[ebx + 0xd5], al
			jmp DoMessage_HowDryAmICheatHook_JumpContinue
		}
	}
}

void BuildingDevelopmentSimulatorHooks::Install()
{
	Logger& logger = Logger::GetInstance();

	uintptr_t DoMessage_FightThePowerCheatHook_Inject = 0;
	DoMessage_FightThePowerCheatHook_JumpContinue = 0;
	uintptr_t DoMessage_HowDryAmICheatHook_Inject = 0;
	DoMessage_HowDryAmICheatHook_JumpContinue = 0;

	const uint16_t gameVersion = SC4VersionDetection::GetGameVersion();
	bool setCallbacks = false;

	switch (gameVersion)
	{
	case 641:
		DoMessage_FightThePowerCheatHook_Inject = 0x69cd78;
		DoMessage_FightThePowerCheatHook_JumpContinue = 0x69cd7f;
		DoMessage_HowDryAmICheatHook_Inject = 0x69cd65;
		DoMessage_HowDryAmICheatHook_JumpContinue = 0x69cd6c;
		setCallbacks = true;
		break;
	}

	if (setCallbacks)
	{
		try
		{
			Patcher::InstallHook(DoMessage_FightThePowerCheatHook_Inject, &DoMessage_FightThePowerCheatHook);
			Patcher::InstallHook(DoMessage_HowDryAmICheatHook_Inject, &DoMessage_HowDryAmICheatHook);

			logger.WriteLine(LogLevel::Info, "Installed the FightThePower and HowDryAmI toggle patches.");
		}
		catch (const wil::ResultException& e)
		{
			logger.WriteLineFormatted(
				LogLevel::Error,
				"Failed to install the FightThePower and HowDryAmI toggle patches.\n%s",
				e.what());
		}
	}
	else
	{
		logger.WriteLineFormatted(
			LogLevel::Error,
			"Failed to install the FightThePower and HowDryAmI toggle patches, unsupported game version: %d",
			gameVersion);
	}
}

bool BuildingDevelopmentSimulatorHooks::IsSupportedGameVersion()
{
	return SC4VersionDetection::GetGameVersion() == 641;
}

void BuildingDevelopmentSimulatorHooks::ToggleDebugGrowthCheatState(cISC4BuildingDevelopmentSimulator* pBuildingDevelopmentSimulator)
{
	if (pBuildingDevelopmentSimulator)
	{
		intptr_t address = 0;

		switch (SC4VersionDetection::GetGameVersion())
		{
		case 641:
			address = reinterpret_cast<intptr_t>(pBuildingDevelopmentSimulator) + 0xe0;
			break;
		}

		if (address != 0)
		{
			bool* value = reinterpret_cast<bool*>(address);
			*value = !*value;
		}
	}
}

void BuildingDevelopmentSimulatorHooks::ToggleIgnoreCapsCheatState(cISC4BuildingDevelopmentSimulator* pBuildingDevelopmentSimulator)
{
	if (pBuildingDevelopmentSimulator)
	{
		intptr_t address = 0;

		switch (SC4VersionDetection::GetGameVersion())
		{
		case 641:
			address = reinterpret_cast<intptr_t>(pBuildingDevelopmentSimulator) + 0xdf;
			break;
		}

		if (address != 0)
		{
			bool* value = reinterpret_cast<bool*>(address);
			*value = !*value;
		}
	}
}
