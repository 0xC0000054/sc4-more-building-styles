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

static const uint32_t cGZWin_Flag_Enabled = 2;

namespace
{
	cIGZWinBtn* GetChildButtonRecursive(cIGZWin* win, uint32_t buttonID)
	{
		cIGZWinBtn* button = nullptr;

		cIGZWin* child = win->GetChildWindowFromIDRecursive(buttonID);

		if (child)
		{
			if (!child->QueryInterface(GZIID_cIGZWinBtn, reinterpret_cast<void**>(&button)))
			{
				button = nullptr;
			}
		}

		return button;
	}

}

bool GZWinUtil::GetButtonToggleState(cIGZWin* win, uint32_t buttonID)
{
	bool result = false;

	cIGZWinBtn* button = GetChildButtonRecursive(win, buttonID);

	if (button)
	{
		result = button->IsOn();

		button->Release();
	}

	return result;
}

void GZWinUtil::SetButtonToggleState(cIGZWin* win, uint32_t buttonID, bool toggleState)
{
	cIGZWinBtn* button = GetChildButtonRecursive(win, buttonID);

	if (button)
	{
		if (toggleState)
		{
			button->ToggleOn();
		}
		else
		{
			button->ToggleOff();
		}

		button->Release();
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
