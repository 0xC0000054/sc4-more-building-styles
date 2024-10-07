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

	template<typename EnumType>
	void ReadEnum(cISC4DBSegmentIStream* stream, EnumType& value)
	{
		static_assert(std::is_enum_v<EnumType>, "T must be an enum.");

		using T = std::underlying_type_t<EnumType>;

		T integerValue = 0;

		if constexpr (std::is_same_v<T, uint8_t>)
		{
			stream->GetUint8(integerValue);
		}
		else if constexpr (std::is_same_v<T, int8_t>)
		{
			stream->GetSint8(integerValue);
		}
		else if constexpr (std::is_same_v<T, uint16_t>)
		{
			stream->GetUint16(integerValue);
		}
		else if constexpr (std::is_same_v<T, int16_t>)
		{
			stream->GetSint16(integerValue);
		}
		else if constexpr (std::is_same_v<T, uint32_t>)
		{
			stream->GetUint32(integerValue);
		}
		else if constexpr (std::is_same_v<T, int32_t>)
		{
			stream->GetSint32(integerValue);
		}
		else if constexpr (std::is_same_v<T, uint64_t>)
		{
			stream->GetUint64(integerValue);
		}
		else if constexpr (std::is_same_v<T, int64_t>)
		{
			stream->GetSint64(integerValue);
		}
		else
		{
			static_assert(false, "Unsupported enum underlying type.");
		}

		value = static_cast<EnumType>(integerValue);
	}

	template<typename EnumType>
	void WriteEnum(cISC4DBSegmentOStream* stream, EnumType value)
	{
		static_assert(std::is_enum_v<EnumType>, "EnumType must be an enum.");

		using T = std::underlying_type_t<EnumType>;

		if constexpr (std::is_same_v<T, uint8_t>)
		{
			stream->SetUint8(static_cast<uint8_t>(value));
		}
		else if constexpr (std::is_same_v<T, int8_t>)
		{
			stream->SetSint8(static_cast<int8_t>(value));
		}
		else if constexpr (std::is_same_v<T, uint16_t>)
		{
			stream->SetUint16(static_cast<uint16_t>(value));
		}
		else if constexpr (std::is_same_v<T, int16_t>)
		{
			stream->SetSint16(static_cast<int16_t>(value));
		}
		else if constexpr (std::is_same_v<T, uint32_t>)
		{
			stream->SetUint32(static_cast<uint32_t>(value));
		}
		else if constexpr (std::is_same_v<T, int32_t>)
		{
			stream->SetSint32(static_cast<int32_t>(value));
		}
		else if constexpr (std::is_same_v<T, uint64_t>)
		{
			stream->SetUint64(static_cast<uint64_t>(value));
		}
		else if constexpr (std::is_same_v<T, int64_t>)
		{
			stream->SetSint64(static_cast<int64_t>(value));
		}
		else
		{
			static_assert(false, "Unsupported enum underlying type.");
		}
	}
}

BuildingSelectWinContext::BuildingSelectWinContext()
	: automaticallyMarkBuildingsAsHistorical(false),
	  automaticallyGrowifyPloppedBuildings(false),
	  wallToWallOption(WallToWallOption::Mixed),
	  keepLotZoneSizes(false)
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
					if (version == 4)
					{
						ReadBoolean(pSC4IStream, automaticallyMarkBuildingsAsHistorical);
						ReadBoolean(pSC4IStream, automaticallyGrowifyPloppedBuildings);
						ReadEnum(pSC4IStream, wallToWallOption);
						ReadBoolean(pSC4IStream, keepLotZoneSizes);
					}
					else if (version == 3)
					{
						ReadBoolean(pSC4IStream, automaticallyMarkBuildingsAsHistorical);
						ReadBoolean(pSC4IStream, automaticallyGrowifyPloppedBuildings);
						ReadEnum(pSC4IStream, wallToWallOption);
					}
					else if (version == 2)
					{
						ReadBoolean(pSC4IStream, automaticallyMarkBuildingsAsHistorical);
						ReadBoolean(pSC4IStream, automaticallyGrowifyPloppedBuildings);
					}
					else if (version == 1)
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
				pSC4OStream->SetUint32(4); // version
				WriteBoolean(pSC4OStream, automaticallyMarkBuildingsAsHistorical);
				WriteBoolean(pSC4OStream, automaticallyGrowifyPloppedBuildings);
				WriteEnum(pSC4OStream, wallToWallOption);
				WriteBoolean(pSC4OStream, keepLotZoneSizes);
			}
		}
	}
}

bool BuildingSelectWinContext::AutomaticallyMarkBuildingsAsHistorical() const
{
	return automaticallyMarkBuildingsAsHistorical;
}

bool BuildingSelectWinContext::AutomaticallyGrowifyPloppedBuildings() const
{
	return automaticallyGrowifyPloppedBuildings;
}

bool BuildingSelectWinContext::GetOptionalCheckBoxState(uint32_t buttonID) const
{
	bool result = false;

	switch (buttonID)
	{
	case AutoHistoricalButtonID:
		result = automaticallyMarkBuildingsAsHistorical;
		break;
	case AutoGrowifyButtonID:
		result = automaticallyGrowifyPloppedBuildings;
		break;
	case KeepLotZoneSizesButtonID:
		result = keepLotZoneSizes;
		break;
	}

	return result;
}

void BuildingSelectWinContext::UpdateOptionalCheckBoxState(cIGZWin* pWin, uint32_t buttonID)
{
	switch (buttonID)
	{
	case AutoHistoricalButtonID:
		automaticallyMarkBuildingsAsHistorical = GZWinUtil::GetButtonToggleState(pWin, AutoHistoricalButtonID);
		break;
	case AutoGrowifyButtonID:
		automaticallyGrowifyPloppedBuildings = GZWinUtil::GetButtonToggleState(pWin, AutoGrowifyButtonID);
		break;
	case KeepLotZoneSizesButtonID:
		keepLotZoneSizes = GZWinUtil::GetButtonToggleState(pWin, KeepLotZoneSizesButtonID);
		break;
	}
}

IBuildingSelectWinContext::WallToWallOption BuildingSelectWinContext::GetWallToWallOption() const
{
	return wallToWallOption;
}

void BuildingSelectWinContext::SetWallToWallOption(WallToWallOption value)
{
	wallToWallOption = value;
}

bool BuildingSelectWinContext::KeepLotZoneSizes() const
{
	return keepLotZoneSizes;
}
