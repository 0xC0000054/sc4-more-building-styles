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
#include "BuildingStyleButtons.h"
#include "BuildingStyleMessages.h"
#include "GlobalPointers.h"
#include "Logger.h"
#include "Patcher.h"
#include "SC4VersionDetection.h"
#include "GZWinUtil.h"
#include "cIGZWin.h"
#include "cIGZWinProc.h"
#include "cIGZMessageServer2.h"
#include "cRZMessage2Standard.h"
#include "GZServPtrs.h"

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

	bool HasMoreThanOneStyleChecked(cIGZWin* container, const std::map<uint32_t, std::string>& styleButtons)
	{
		int32_t numberOfButtonsChecked = 0;

		for (const std::pair<uint32_t, std::string>& item : styleButtons)
		{
			if (GZWinUtil::GetButtonToggleState(container, item.first))
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
}

static uintptr_t DoWinProcMessage_Hook_Button_StyleControlRadio_CycleEveryNYears_Continue_Jump;
static uintptr_t DoWinProcMessage_Hook_Button_StyleControlRadio_UseAllStylesAtOnce_Continue_Jump;
static uintptr_t DoWinProcMessage_Hook_Button_StylePanelTitleBar_ToggleVisibility_Continue_Jump;
static uintptr_t DoWinProcMessage_Hook_Button_ShowBuildingStyleControl_Continue_Jump;
static uintptr_t DoWinProcMessage_Hook_Button_HideBuildingStyleControl_Continue_Jump;
static uintptr_t DoWinProcMessage_Hook_StyleValidJump;
static uintptr_t DoWinProcMessage_Hook_ExitMethodJump;

constexpr uint32_t StyleControlRadio_CycleEveryNYears = 0xcbc61559;
constexpr uint32_t StyleControlRadio_UseAllStylesAtOnce = 0xebc61560;
constexpr uint32_t StylePanel_Collapsed_TitleBar = 0xbc61548;
constexpr uint32_t StylePanel_Expanded_TitleBar = 0x2bc619f3;
constexpr uint32_t StylePanel_Collapsed_ToggleButton = 0xcbc61567;
constexpr uint32_t StylePanel_Expanded_ToggleButton = 0xebc619fd;

class cSC4BuildingSelectWinProc : public cIGZWinProc
{
public:

	virtual bool DoWinProcMessage(cIGZWin* pWin, cGZMessage& pMsg);

	virtual bool DoWinMsg(cIGZWin* pWin, uint32_t dwMessageID, uint32_t data1, uint32_t data2, uint32_t data3);

	// We add our new member functions below the existing class methods.

	void __thiscall EnableStyleButtons();

	void __thiscall SetActiveStyleButtons();

	void __thiscall AddActiveStyle(uint32_t style);

	void __thiscall RemoveActiveStyle(uint32_t style);

	// Class member variables.
	// The first 4 bytes are occupied by the class vtable pointer.
	uint32_t refCount;                 // 0x4
	uint32_t initialized;              // 0x8
	cIGZWin* window;                   // 0xc
	cIGZWin* styleListContainer;       // 0x10
	cIGZWin* styleChangeYearContainer; // 0x14
	uint32_t isCollapsed;              // 0x18
};

static void UpdateOptionalCheckBoxState(const cSC4BuildingSelectWinProc* pThis, uint32_t buttonID)
{
	spBuildingSelectWinManager->UpdateOptionalCheckBoxState(pThis->styleListContainer, buttonID);
}

static bool IsBuildingStyleAvailable(uint32_t style)
{
	return spBuildingSelectWinManager->IsBuildingStyleAvailable(style);
}

static void NAKED_FUN DoWinProcMessageHookFn(void)
{
	__asm
	{
		// esi is the this pointer
		// eax is the button id
		push eax // store
		push ecx // store
		push edx // store
		cmp eax, StyleControlRadio_CycleEveryNYears
		jz cycleStyleEveryNYears
		cmp eax, StyleControlRadio_UseAllStylesAtOnce
		jz useAllStylesAtOnce
		cmp eax, StylePanel_Collapsed_TitleBar
		jz stylePanelTitleBar_ToggleVisibility
		cmp eax, StylePanel_Expanded_TitleBar
		jz stylePanelTitleBar_ToggleVisibility
		cmp eax, StylePanel_Collapsed_ToggleButton
		jz stylePanelCollapsedToggle
		cmp eax, StylePanel_Expanded_ToggleButton
		jz stylePanelExpandedToggle
		cmp eax, AutoHistoricalButtonID
		jz updateOptionalCheckBoxState
		// Any button id below this line should be a style id
		push eax
		call IsBuildingStyleAvailable // (cdecl)
		add esp, 4
		test al, al
		jz exit_method
		pop edx // restore
		pop ecx // restore
		pop eax // restore
		push DoWinProcMessage_Hook_StyleValidJump
		ret

		cycleStyleEveryNYears:
		pop edx // restore
		pop ecx // restore
		pop eax // restore
		push DoWinProcMessage_Hook_Button_StyleControlRadio_CycleEveryNYears_Continue_Jump
		ret

		useAllStylesAtOnce:
		pop edx // restore
		pop ecx // restore
		pop eax // restore
		push DoWinProcMessage_Hook_Button_StyleControlRadio_UseAllStylesAtOnce_Continue_Jump
		ret

		stylePanelTitleBar_ToggleVisibility:
		pop edx // restore
		pop ecx // restore
		pop eax // restore
		push DoWinProcMessage_Hook_Button_StylePanelTitleBar_ToggleVisibility_Continue_Jump
		ret

		stylePanelCollapsedToggle:
		pop edx // restore
		pop ecx // restore
		pop eax // restore
		push DoWinProcMessage_Hook_Button_ShowBuildingStyleControl_Continue_Jump
		ret

		stylePanelExpandedToggle:
		pop edx // restore
		pop ecx // restore
		pop eax // restore
		push DoWinProcMessage_Hook_Button_HideBuildingStyleControl_Continue_Jump
		ret

		updateOptionalCheckBoxState:
		push eax // button id
		push esi // this pointer
		call UpdateOptionalCheckBoxState // (cdecl)
		add esp, 8

		exit_method:
		pop edx // restore
		pop ecx // restore
		pop eax // restore
		push DoWinProcMessage_Hook_ExitMethodJump
		ret
	}
}

void __thiscall cSC4BuildingSelectWinProc::EnableStyleButtons()
{
	const std::map<uint32_t, std::string>& styleButtons = spBuildingSelectWinManager->GetAvailableBuildingStyles();

	if (HasMoreThanOneStyleChecked(this->styleListContainer, styleButtons))
	{
		// Enable all of the radio buttons.
		for (const std::pair<uint32_t, std::string>& item : styleButtons)
		{
			GZWinUtil::SetChildWindowEnabled(this->styleListContainer, item.first, true);
		}
	}
	else
	{
		// Disable the active radio button to prevent it from being deselected.
		// This ensures that there is always at least one radio button active.
		for (const std::pair<uint32_t, std::string>& item : styleButtons)
		{
			uint32_t id = item.first;
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
	const std::map<uint32_t, std::string>& allBuildingStyles = spBuildingSelectWinManager->GetAvailableBuildingStyles();

	const eastl::vector<uint32_t>& activeBuildingStyles = spBuildingSelectWinManager->GetTractDeveloper()->GetActiveStyles();

	for (const std::pair<uint32_t, std::string>& item : allBuildingStyles)
	{
		uint32_t style = item.first;
		bool selected = Contains(activeBuildingStyles, style);

		GZWinUtil::SetButtonToggleState(this->styleListContainer, style, selected);
	}

	if (spBuildingSelectWinManager->UIHasOptionalCheckBox(AutoHistoricalButtonID))
	{
		bool selected = spBuildingSelectWinManager->GetOptionalCheckBoxState(AutoHistoricalButtonID);

		GZWinUtil::SetButtonToggleState(this->styleListContainer, AutoHistoricalButtonID, selected);
	}

	EnableStyleButtons();
}

void __thiscall cSC4BuildingSelectWinProc::AddActiveStyle(uint32_t style)
{
	cISC4TractDeveloper* const pTractDeveloper = spBuildingSelectWinManager->GetTractDeveloper();

	// This is a copy of the existing active style list, not a reference to it.
	// We will call SetActiveStyles to update SC4's copy after we modify it.
	eastl::vector<uint32_t> activeStyles = pTractDeveloper->GetActiveStyles();

	if (!Contains(activeStyles, style))
	{
		activeStyles.push_back(style);
		pTractDeveloper->SetActiveStyles(activeStyles);

		spBuildingSelectWinManager->SendActiveBuildingStyleCheckboxChangedMessage(true, style);
	}
}

void __thiscall cSC4BuildingSelectWinProc::RemoveActiveStyle(uint32_t style)
{

	cISC4TractDeveloper* const pTractDeveloper = spBuildingSelectWinManager->GetTractDeveloper();

	// This is a copy of the existing active style list, not a reference to it.
	// We will call SetActiveStyles to update SC4's copy after we modify it.
	eastl::vector<uint32_t> activeStyles = pTractDeveloper->GetActiveStyles();
	bool itemRemoved = false;

	for (auto it = activeStyles.begin(); it != activeStyles.end(); it++)
	{
		if (*it == style)
		{
			activeStyles.erase(it);
			itemRemoved = true;
			break;
		}
	}

	if (itemRemoved)
	{
		pTractDeveloper->SetActiveStyles(activeStyles);
		spBuildingSelectWinManager->SendActiveBuildingStyleCheckboxChangedMessage(false, style);
	}
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

static union AddActiveStyleHookShim
{
	void (cSC4BuildingSelectWinProc::*pfnAddActiveStyle)(uint32_t) = &cSC4BuildingSelectWinProc::AddActiveStyle;
	void (*pfnVoid)(void);
}add_active_style_shim;

static_assert(sizeof(AddActiveStyleHookShim::pfnAddActiveStyle) == sizeof(AddActiveStyleHookShim::pfnVoid),
	"sizeof(AddActiveStyleHookShim::pfnAddActiveStyle) != sizeof(AddActiveStyleHookShim::pfnVoid)"
	", is cSC4BuildingSelectWinProc using multiple inheritance?");

static union RemoveActiveStyleHookShim
{
	void (cSC4BuildingSelectWinProc::*pfnRemoveActiveStyle)(uint32_t) = &cSC4BuildingSelectWinProc::RemoveActiveStyle;
	void (*pfnVoid)(void);
}remove_active_style_shim;

static_assert(sizeof(RemoveActiveStyleHookShim::pfnRemoveActiveStyle) == sizeof(RemoveActiveStyleHookShim::pfnVoid),
	"sizeof(RemoveActiveStyleHookShim::pfnRemoveActiveStyle) != sizeof(RemoveActiveStyleHookShim::pfnVoid)"
	", is cSC4BuildingSelectWinProc using multiple inheritance?");

void BuildingSelectWinProcHooks::Install()
{
	uintptr_t DoWinProcMessage_Hook_InjectPoint = 0;
	DoWinProcMessage_Hook_Button_StyleControlRadio_CycleEveryNYears_Continue_Jump = 0;
	DoWinProcMessage_Hook_Button_StyleControlRadio_UseAllStylesAtOnce_Continue_Jump = 0;
	DoWinProcMessage_Hook_Button_StylePanelTitleBar_ToggleVisibility_Continue_Jump = 0;
	DoWinProcMessage_Hook_Button_ShowBuildingStyleControl_Continue_Jump = 0;
	DoWinProcMessage_Hook_Button_HideBuildingStyleControl_Continue_Jump = 0;
	DoWinProcMessage_Hook_StyleValidJump = 0;
	DoWinProcMessage_Hook_ExitMethodJump = 0;
	uintptr_t DoWinProcMessage_EnableStyleButtons_Call_1 = 0;
	uintptr_t DoWinProcMessage_EnableStyleButtons_Call_2 = 0;
	uintptr_t DoWinProcMessage_AddActiveStyle_Call = 0;
	uintptr_t DoWinProcMessage_RemoveActiveStyle_Call = 0;

	uintptr_t ShowWindow_Jump_InjectPoint = 0; // Used to skip the existing style button loop.
	uintptr_t ShowWindow_Jump_Target = 0;
	uintptr_t ShowWindow_SetActiveStyleButtons_Call = 0;

	Logger& logger = Logger::GetInstance();

	const uint16_t gameVersion = SC4VersionDetection::GetInstance().GetGameVersion();
	bool setCallbacks = false;

	switch (gameVersion)
	{
	case 641:
		DoWinProcMessage_Hook_InjectPoint = 0x769968;
		DoWinProcMessage_Hook_Button_StyleControlRadio_CycleEveryNYears_Continue_Jump = 0x7699ea;
		DoWinProcMessage_Hook_Button_StyleControlRadio_UseAllStylesAtOnce_Continue_Jump = 0x769a5b;
		DoWinProcMessage_Hook_Button_StylePanelTitleBar_ToggleVisibility_Continue_Jump = 0x7699d0;
		DoWinProcMessage_Hook_Button_ShowBuildingStyleControl_Continue_Jump = 0x769a8f;
		DoWinProcMessage_Hook_Button_HideBuildingStyleControl_Continue_Jump = 0x769a37;
		DoWinProcMessage_Hook_StyleValidJump = 0x76998f;
		DoWinProcMessage_Hook_ExitMethodJump = 0x769b25;
		DoWinProcMessage_EnableStyleButtons_Call_1 = 0x7699a0;
		DoWinProcMessage_EnableStyleButtons_Call_2 = 0x7699b6;
		DoWinProcMessage_AddActiveStyle_Call = 0x769999;
		DoWinProcMessage_RemoveActiveStyle_Call = 0x7699af;

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
			Patcher::InstallJump(DoWinProcMessage_Hook_InjectPoint, reinterpret_cast<uintptr_t>(&DoWinProcMessageHookFn));
			Patcher::InstallCallHook(DoWinProcMessage_EnableStyleButtons_Call_1, enable_buttons_shim.pfnVoid);
			Patcher::InstallCallHook(DoWinProcMessage_EnableStyleButtons_Call_2, enable_buttons_shim.pfnVoid);
			Patcher::InstallCallHook(DoWinProcMessage_AddActiveStyle_Call, add_active_style_shim.pfnVoid);
			Patcher::InstallCallHook(DoWinProcMessage_RemoveActiveStyle_Call, remove_active_style_shim.pfnVoid);

			Patcher::InstallJump(ShowWindow_Jump_InjectPoint, ShowWindow_Jump_Target);
			Patcher::InstallCallHook(ShowWindow_SetActiveStyleButtons_Call, set_active_style_shim.pfnVoid);

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
