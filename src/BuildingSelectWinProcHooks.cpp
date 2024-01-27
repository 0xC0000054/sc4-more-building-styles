////////////////////////////////////////////////////////////////////////////
//
// This file is part of sc4-more-building-styles, a DLL Plugin for
// SimCity 4 that adds support for more building styles.
//
// Copyright (c) 2024 Nicholas Hayes
//
// This file is licensed under terms of the MIT License.
// See LICENSE.txt for more information.
//
////////////////////////////////////////////////////////////////////////////

// Portions of this file have been adapted from:
/*
   Project: SC4Fix Patches for SimCity 4

   Copyright (c) 2015 Nelson Gomez (simmaster07)

   Licensed under the MIT License. A copy of the License is available in
   LICENSE or at:

	   http://opensource.org/licenses/MIT

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
   THE SOFTWARE.
*/

#include "BuildingSelectWinProcHooks.h"
#include "AvailableBuildingStyles.h"
#include "GlobalTractDeveloper.h"
#include "Logger.h"
#include "SC4VersionDetection.h"
#include "GZWinUtil.h"
#include "cIGZWin.h"
#include "cIGZWinProc.h"

#include <Windows.h>
#include "wil/resource.h"
#include "wil/win32_helpers.h"

namespace
{
	template <typename T>
	bool Contains(const eastl::vector<T>& vector, const T& value)
	{
		return std::find(vector.begin(), vector.end(), value) != vector.end();
	}

	bool HasMoreThanOneStyleChecked(cIGZWin* container, const std::vector<uint32_t>& styleButtonIDs)
	{
		int32_t numberOfButtonsChecked = 0;

		for (uint32_t id : styleButtonIDs)
		{
			if (GZWinUtil::GetButtonToggleState(container, id))
			{
				++numberOfButtonsChecked;

				if (numberOfButtonsChecked >= 2)
				{
					return true;
				}
			}
		}

		return false;
	}

	void InstallJump(uintptr_t address, uintptr_t destination)
	{
		DWORD oldProtect;
		THROW_IF_WIN32_BOOL_FALSE(VirtualProtect(reinterpret_cast<void*>(address), 5, PAGE_EXECUTE_READWRITE, &oldProtect));

		*((uint8_t*)address) = 0xE9;
		*((uintptr_t*)(address + 1)) = destination - address - 5;
	}

	void InstallCallHook(uintptr_t address, void (*pfnFunc)(void))
	{
		DWORD oldProtect;
		THROW_IF_WIN32_BOOL_FALSE(VirtualProtect(reinterpret_cast<void*>(address), 5, PAGE_EXECUTE_READWRITE, &oldProtect));

		*((uint8_t*)address) = 0xE8;
		*((uintptr_t*)(address + 1)) = reinterpret_cast<uintptr_t>(pfnFunc) - address - 5;
	}
}

#ifdef __clang__
#define NAKED_FUN __attribute__((naked))
#else
#define NAKED_FUN __declspec(naked)
#endif

static uintptr_t DoWinProcMessage_Hook_ContinueJump;
static uintptr_t DoWinProcMessage_Hook_NotFoundJump;

static void NAKED_FUN DoWinProcMessageHookFn(void)
{
	static uint32_t buttonID;

	_asm mov buttonID, eax
	_asm pushad

	if (AvailableBuildingStyles::GetInstance().ContainsBuildingStyle(buttonID))
	{
		_asm popad
		_asm push DoWinProcMessage_Hook_ContinueJump
		_asm ret
	}

	_asm popad
	_asm push DoWinProcMessage_Hook_NotFoundJump
	_asm ret
}

class cSC4BuildingSelectWinProc : public cIGZWinProc
{
public:

	virtual bool DoWinProcMessage(cIGZWin* pWin, cGZMessage& pMsg);

	virtual bool DoWinMsg(cIGZWin* pWin, uint32_t dwMessageID, uint32_t data1, uint32_t data2, uint32_t data3);

	// We add our new member functions below the existing class methods.

	void __thiscall EnableStyleButtons();

	void __thiscall SetActiveStyleButtons();

	// Class member variables.
	// The first 4 bytes are occupied by the class vtable pointer.
	uint32_t refCount;                 // 0x4
	uint32_t initialized;              // 0x8
	cIGZWin* window;                   // 0xc
	cIGZWin* styleListContainer;       // 0x10
	cIGZWin* styleChangeYearContainer; // 0x14
	uint32_t isExpanded;               // 0x18
};


void __thiscall cSC4BuildingSelectWinProc::EnableStyleButtons()
{
	const std::vector<uint32_t>& styleButtonIDs = AvailableBuildingStyles::GetInstance().GetBuildingStyles();

	if (HasMoreThanOneStyleChecked(this->styleListContainer, styleButtonIDs))
	{
		// Enable all of the radio buttons.
		for (uint32_t id : styleButtonIDs)
		{
			GZWinUtil::SetChildWindowEnabled(this->styleListContainer, id, true);
		}
	}
	else
	{
		// Disable the active radio button to prevent it from being deselected.
		// This ensures that there is always at least one radio button active.
		for (uint32_t id : styleButtonIDs)
		{
			if (GZWinUtil::GetButtonToggleState(this->styleListContainer, id))
			{
				GZWinUtil::SetChildWindowEnabled(this->styleListContainer, id, false);
				break;
			}
		}
	}
}

void __thiscall cSC4BuildingSelectWinProc::SetActiveStyleButtons()
{
	const std::vector<uint32_t>& allBuildingStyles = AvailableBuildingStyles::GetInstance().GetBuildingStyles();

	const eastl::vector<uint32_t>& activeBuildingStyles = spTractDeveloper->GetActiveStyles();

	for (uint32_t style : allBuildingStyles)
	{
		bool selected = Contains(activeBuildingStyles, style);

		GZWinUtil::SetButtonToggleState(this->styleListContainer, style, selected);
	}

	EnableStyleButtons();
}

static union EnableButtonsHookShim
{
	void (cSC4BuildingSelectWinProc::*pfnEnableButtons)(void) = &cSC4BuildingSelectWinProc::EnableStyleButtons;
	void (*pfnVoid)(void);
}enable_buttons_shim;

static_assert(sizeof(EnableButtonsHookShim::pfnEnableButtons) == sizeof(EnableButtonsHookShim::pfnVoid),
	"sizeof(EnableButtonsHookShim::pfnEnableButtonsHook) != sizeof(EnableButtonsHookShim::pfnVoid)"
	", is cSC4BuildingSelectWinProc using multiple inheritance?");

static union SetActiveStyleButtonsHookShim
{
	void (cSC4BuildingSelectWinProc::*pfnSetActiveStyleButtons)(void) = &cSC4BuildingSelectWinProc::SetActiveStyleButtons;
	void (*pfnVoid)(void);
}set_active_style_shim;

static_assert(sizeof(SetActiveStyleButtonsHookShim::pfnSetActiveStyleButtons) == sizeof(SetActiveStyleButtonsHookShim::pfnVoid),
	"sizeof(SetActiveStyleButtonsHookShim::pfnSetActiveStyleButtons) != sizeof(SetActiveStyleButtonsHookShim::pfnVoid)"
	", is cSC4BuildingSelectWinProc using multiple inheritance?");

void BuildingSelectWinProcHooks::Install()
{
	uintptr_t DoWinProcMessage_Hook_InjectPoint = 0;
	DoWinProcMessage_Hook_ContinueJump = 0;
	DoWinProcMessage_Hook_NotFoundJump = 0;
	uintptr_t DoWinProcMessage_EnableStyleButtons_Call_1 = 0;
	uintptr_t DoWinProcMessage_EnableStyleButtons_Call_2 = 0;

	uintptr_t ShowWindow_Jump_InjectPoint = 0; // Used to skip the existing style button loop.
	uintptr_t ShowWindow_Jump_Target = 0;
	uintptr_t ShowWindow_SetActiveStyleButtons_Call = 0;

	Logger& logger = Logger::GetInstance();

	const uint16_t gameVersion = SC4VersionDetection::GetInstance().GetGameVersion();
	bool setCallbacks = false;

	switch (gameVersion)
	{
	case 641:
		DoWinProcMessage_Hook_InjectPoint = 0x769979;
		DoWinProcMessage_Hook_ContinueJump = 0x76998f;
		DoWinProcMessage_Hook_NotFoundJump = 0x769b25;
		DoWinProcMessage_EnableStyleButtons_Call_1 = 0x7699a0;
		DoWinProcMessage_EnableStyleButtons_Call_2 = 0x7699b6;

		ShowWindow_Jump_InjectPoint = 0x769b77;
		ShowWindow_Jump_Target = 0x769bea;
		ShowWindow_SetActiveStyleButtons_Call = 0x769bec;
		setCallbacks = true;
		break;
	}

	if (setCallbacks)
	{
		try
		{
			InstallJump(DoWinProcMessage_Hook_InjectPoint, reinterpret_cast<uintptr_t>(&DoWinProcMessageHookFn));
			InstallCallHook(DoWinProcMessage_EnableStyleButtons_Call_1, enable_buttons_shim.pfnVoid);
			InstallCallHook(DoWinProcMessage_EnableStyleButtons_Call_2, enable_buttons_shim.pfnVoid);

			InstallJump(ShowWindow_Jump_InjectPoint, ShowWindow_Jump_Target);
			InstallCallHook(ShowWindow_SetActiveStyleButtons_Call, set_active_style_shim.pfnVoid);

			logger.WriteLine(LogLevel::Info, "Installed the building select window patch.");
		}
		catch (const wil::ResultException& e)
		{
			logger.WriteLineFormatted(
				LogLevel::Error,
				"Failed to install the building select window patch.\n%s",
				e.what());
		}
	}
	else
	{
		logger.WriteLineFormatted(
			LogLevel::Error,
			"Unsupported game version: %d",
			gameVersion);
	}
}
