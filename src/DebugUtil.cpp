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

#include "DebugUtil.h"
#include "cIGZString.h"
#include "cIGZVariant.h"
#include "cISCProperty.h"
#include "cISCPropertyHolder.h"
#include "cISC4Occupant.h"
#include "cRZAutoRefCount.h"
#include "StringResourceKey.h"
#include "StringResourceManager.h"
#include <memory>
#include <Windows.h>

namespace
{
	bool GetOccupantNameKey(cISC4Occupant* pOccupant, StringResourceKey& key)
	{
		bool result = false;

		if (pOccupant)
		{
			cISCPropertyHolder* propertyHolder = pOccupant->AsPropertyHolder();

			constexpr uint32_t kUserVisibleName = 0x8A416A99;

			cISCProperty* userVisibleName = propertyHolder->GetProperty(kUserVisibleName);

			if (userVisibleName)
			{
				const cIGZVariant* propertyValue = userVisibleName->GetPropertyValue();

				if (propertyValue->GetType() == cIGZVariant::Type::Uint32Array
					&& propertyValue->GetCount() == 3)
				{
					const uint32_t* pTGI = propertyValue->RefUint32();

					uint32_t group = pTGI[1];
					uint32_t instance = pTGI[2];

					key.groupID = group;
					key.instanceID = instance;
					result = true;
				}
			}
		}

		return result;
	}
}

void DebugUtil::PrintLineToDebugOutput(const char* const line)
{
#ifdef _DEBUG
	OutputDebugStringA(line);
	OutputDebugStringA("\n");
#endif // _DEBUG
}

void DebugUtil::PrintLineToDebugOutputFormatted(const char* const format, ...)
{
#ifdef _DEBUG
	va_list args;
	va_start(args, format);

	va_list argsCopy;
	va_copy(argsCopy, args);

	int formattedStringLength = std::vsnprintf(nullptr, 0, format, argsCopy);

	va_end(argsCopy);

	if (formattedStringLength > 0)
	{
		size_t formattedStringLengthWithNull = static_cast<size_t>(formattedStringLength) + 1;

		constexpr size_t stackBufferSize = 1024;

		if (formattedStringLengthWithNull >= stackBufferSize)
		{
			std::unique_ptr<char[]> buffer = std::make_unique_for_overwrite<char[]>(formattedStringLengthWithNull);

			std::vsnprintf(buffer.get(), formattedStringLengthWithNull, format, args);

			PrintLineToDebugOutput(buffer.get());
		}
		else
		{
			char buffer[stackBufferSize]{};

			std::vsnprintf(buffer, stackBufferSize, format, args);

			PrintLineToDebugOutput(buffer);
		}
	}

	va_end(args);
#endif // _DEBUG
}

void DebugUtil::PrintOccupantNameToDebugOutput(cISC4Occupant* pOccupant)
{
#ifdef _DEBUG
	StringResourceKey occupantNameKey;

	if (GetOccupantNameKey(pOccupant, occupantNameKey))
	{
		cRZAutoRefCount<cIGZString> name;

		if (StringResourceManager::GetLocalizedString(occupantNameKey, name))
		{
			PrintLineToDebugOutput(name->ToChar());
		}
	}
#endif // _DEBUG
}
