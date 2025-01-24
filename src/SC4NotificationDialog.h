////////////////////////////////////////////////////////////////////////////
//
// This file is part of sc4-more-building-styles, a DLL Plugin for
// SimCity 4 that adds support for more building styles.
//
// Copyright (c) 2024, 2025 Nicholas Hayes
//
// This file is licensed under terms of the MIT License.
// See LICENSE.txt for more information.
//
////////////////////////////////////////////////////////////////////////////

#pragma once
#include "StringResourceKey.h"

class cIGZString;

// SC4's standard notification dialog with an OK button.
// TGI 0, 0x96A006B0, 0xCA8CBF0F
namespace SC4NotificationDialog
{
	void ShowDialog(cIGZString const& message, cIGZString const& caption);
	void ShowDialog(StringResourceKey const& messageKey, StringResourceKey const& captionKey);
};

