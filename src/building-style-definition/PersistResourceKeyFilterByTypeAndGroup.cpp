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

#include "PersistResourceKeyFilterByTypeAndGroup.h"
#include "cGZPersistResourceKey.h"

PersistResourceKeyFilterByTypeAndGroup::PersistResourceKeyFilterByTypeAndGroup(uint32_t type, uint32_t group)
	: type(type), group(group)
{
}

bool PersistResourceKeyFilterByTypeAndGroup::QueryInterface(uint32_t riid, void** ppvObj)
{
	if (riid == GZIID_cIGZPersistResourceKeyFilter)
	{
		*ppvObj = static_cast<cIGZPersistResourceKeyFilter*>(this);
		AddRef();

		return true;
	}

	return cRZBaseUnknown::QueryInterface(riid, ppvObj);
}

uint32_t PersistResourceKeyFilterByTypeAndGroup::AddRef()
{
	return cRZBaseUnknown::AddRef();
}

uint32_t PersistResourceKeyFilterByTypeAndGroup::Release()
{
	return cRZBaseUnknown::Release();
}

bool PersistResourceKeyFilterByTypeAndGroup::IsKeyIncluded(cGZPersistResourceKey const& key)
{
	return key.type == type && key.group == group;
}
