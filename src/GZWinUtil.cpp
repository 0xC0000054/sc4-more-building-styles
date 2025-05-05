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
		result = GetWindowEnabled(child);
	}

	return result;
}

void GZWinUtil::SetChildWindowEnabled(cIGZWin* win, uint32_t childID, bool enabled)
{
	cIGZWin* child = win->GetChildWindowFromIDRecursive(childID);

	if (child)
	{
		SetWindowEnabled(child, enabled);
	}
}

bool GZWinUtil::GetWindowEnabled(cIGZWin* win)
{
	return win->GetFlag(cGZWin_Flag_Enabled);
}

void GZWinUtil::SetWindowEnabled(cIGZWin* win, bool enabled)
{
	win->SetFlag(cGZWin_Flag_Enabled, enabled);
}

bool GZWinUtil::GetWindowVisible(cIGZWin* win)
{
	return win->IsVisible();
}

void GZWinUtil::SetWindowVisible(cIGZWin* win, bool visible)
{
	if (visible)
	{
		win->ShowWindow();
	}
	else
	{
		win->HideWindow();
	}
}
