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

#include "GZWinUtil.h"
#include "cIGZWin.h"
#include "cIGZWinBtn.h"
#include "cRZAutoRefCount.h"

static const uint32_t cGZWin_Flag_Enabled = 2;

bool GZWinUtil::GetButtonToggleState(cIGZWin* win, uint32_t buttonID)
{
	bool result = false;

	cRZAutoRefCount<cIGZWinBtn> button;

	if (win->GetChildAsRecursive(buttonID, GZIID_cIGZWinBtn, button.AsPPVoid()))
	{
		result = button->IsOn();
	}

	return result;
}

void GZWinUtil::SetButtonToggleState(cIGZWin* win, uint32_t buttonID, bool toggleState)
{
	cRZAutoRefCount<cIGZWinBtn> button;

	if (win->GetChildAsRecursive(buttonID, GZIID_cIGZWinBtn, button.AsPPVoid()))
	{
		if (toggleState)
		{
			button->ToggleOn();
		}
		else
		{
			button->ToggleOff();
		}
	}
}

bool GZWinUtil::GetChildWindowEnabled(cIGZWin* win, uint32_t childID)
{
	bool result = false;

	cIGZWin* child = win->GetChildWindowFromIDRecursive(childID);

	if (child)
	{
		result = child->GetFlag(cGZWin_Flag_Enabled);
	}

	return result;
}

void GZWinUtil::SetChildWindowEnabled(cIGZWin* win, uint32_t childID, bool enabled)
{
	cIGZWin* child = win->GetChildWindowFromIDRecursive(childID);

	if (child)
	{
		child->SetFlag(cGZWin_Flag_Enabled, enabled);
	}
}
