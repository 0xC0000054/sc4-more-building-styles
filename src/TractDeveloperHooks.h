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

class cISC4TractDeveloper;
class Preferences;

namespace TractDeveloperHooks
{
	bool GetKickOutLowerWealthValue(const cISC4TractDeveloper* pTractDeveloper);
	void SetKickOutLowerWealthValue(cISC4TractDeveloper* pTractDeveloper, bool value);

	void Install(const Preferences& preferences);
}
