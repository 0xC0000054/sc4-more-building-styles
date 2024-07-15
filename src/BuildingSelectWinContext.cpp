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

#include "BuildingSelectWinContext.h"
#include "BuildingStyleButtons.h"
#include "cGZPersistResourceKey.h"
#include "cIGZPersistDBSegment.h"
#include "cISC4DBSegment.h"
#include "cISC4DBSegmentIStream.h"
#include "cISC4DBSegmentOStream.h"
#include "cRZAutoRefCount.h"
#include "GZWinUtil.h"

static constexpr uint32_t BuildingSelectWinContextTypeID = 0xB9757739;
static constexpr uint32_t BuildingSelectWinContextGroupID = 0x1FD8DC83;
static constexpr uint32_t BuildingSelectWinContextInstanceID = 0x00000001;

namespace
{
	void ReadBoolean(cISC4DBSegmentIStream* stream, bool& value)
	{
		uint8_t temp = 0;
		// We use GetVoid because GetUint8 always returns false.
		if (stream->GetVoid(&temp, 1))
		{
			value = temp != 0;
		}
	}

	void WriteBoolean(cISC4DBSegmentOStream* stream, bool value)
	{
		uint8_t valueAsUint8 = static_cast<uint8_t>(value);

		stream->SetVoid(&valueAsUint8, 1);
	}
}

BuildingSelectWinContext::BuildingSelectWinContext()
	: automaticallyMarkBuildingsAsHistorical(false)
{
}

void BuildingSelectWinContext::LoadFromDBSegment(cIGZPersistDBSegment* pSegment)
{
	if (pSegment)
	{
		cRZAutoRefCount<cISC4DBSegment> pSC4DBSegment;

		if (pSegment->QueryInterface(GZIID_cISC4DBSegment, pSC4DBSegment.AsPPVoid()))
		{
			cGZPersistResourceKey key(
				BuildingSelectWinContextTypeID,
				BuildingSelectWinContextGroupID,
				BuildingSelectWinContextInstanceID);

			cRZAutoRefCount<cISC4DBSegmentIStream> pSC4IStream;

			if (pSC4DBSegment->OpenIStream(key, pSC4IStream.AsPPObj()))
			{
				uint32_t version = 0;

				if (pSC4IStream->GetUint32(version))
				{
					if (version == 1)
					{
						ReadBoolean(pSC4IStream, automaticallyMarkBuildingsAsHistorical);
					}
				}
			}
		}
	}
}

void BuildingSelectWinContext::SaveToDBSegment(cIGZPersistDBSegment* pSegment) const
{
	if (pSegment)
	{
		cRZAutoRefCount<cISC4DBSegment> pSC4DBSegment;

		if (pSegment->QueryInterface(GZIID_cISC4DBSegment, pSC4DBSegment.AsPPVoid()))
		{
			cGZPersistResourceKey key(
				BuildingSelectWinContextTypeID,
				BuildingSelectWinContextGroupID,
				BuildingSelectWinContextInstanceID);

			cRZAutoRefCount<cISC4DBSegmentOStream> pSC4OStream;

			if (pSC4DBSegment->OpenOStream(key, pSC4OStream.AsPPObj(), true))
			{
				pSC4OStream->SetUint32(1); // version
				WriteBoolean(pSC4OStream, automaticallyMarkBuildingsAsHistorical);
			}
		}
	}
}

bool BuildingSelectWinContext::AutomaticallyMarkBuildingsAsHistorical() const
{
	return automaticallyMarkBuildingsAsHistorical;
}

bool BuildingSelectWinContext::GetOptionalCheckBoxState(uint32_t buttonID) const
{
	bool result = false;

	if (buttonID == AutoHistoricalButtonID)
	{
		result = automaticallyMarkBuildingsAsHistorical;
	}

	return result;
}

void BuildingSelectWinContext::UpdateOptionalCheckBoxState(cIGZWin* pWin, uint32_t buttonID)
{
	if (buttonID == AutoHistoricalButtonID)
	{
		automaticallyMarkBuildingsAsHistorical = GZWinUtil::GetButtonToggleState(pWin, AutoHistoricalButtonID);
	}
}
