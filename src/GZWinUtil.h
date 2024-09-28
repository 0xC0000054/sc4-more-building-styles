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

#pragma once
#include <cstdint>

class cIGZWin;

namespace GZWinUtil
{
	bool GetButtonToggleState(cIGZWin* win, uint32_t buttonID);
	void SetButtonToggleState(cIGZWin* win, uint32_t buttonID, bool toggleState);

	bool GetChildWindowEnabled(cIGZWin* win, uint32_t childID);
	void SetChildWindowEnabled(cIGZWin* win, uint32_t childID, bool enabled);
	bool GetWindowEnabled(cIGZWin* win);
	void SetWindowEnabled(cIGZWin* win, bool enabled);

	bool GetWindowVisible(cIGZWin* win);
	void SetWindowVisible(cIGZWin* win, bool visible);
}