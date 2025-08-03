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

#pragma once
#include "cIGZVariant.h"
#include "cISCProperty.h"
#include "cISCPropertyHolder.h"
#include "cRZAutoRefCount.h"
#include <span>
#include <type_traits>

class cISCPropertyHolder;

template<typename T>
class PropertyData
{
public:
	static_assert(
		std::is_same<T, bool>::value
		|| std::is_same<T, uint8_t>::value
		|| std::is_same<T, int8_t>::value
		|| std::is_same<T, uint16_t>::value
		|| std::is_same<T, int16_t>::value
		|| std::is_same<T, uint32_t>::value
		|| std::is_same<T, int32_t>::value
		|| std::is_same<T, uint64_t>::value
		|| std::is_same<T, int64_t>::value
		|| std::is_same<T, float>::value
		|| std::is_same<T, double>::value,
		"Unsupported integral type.");

	using container = std::span<T>;
	using iterator = container::iterator;

	PropertyData() : singleValue(0), span(), variant()
	{
	}

	PropertyData(cISCPropertyHolder* pPropertyHolder, uint32_t id)
		: singleValue(0), span(), variant()
	{
		load(pPropertyHolder, id);
	}

	PropertyData(const PropertyData& other) = delete;

	PropertyData(PropertyData&& other) noexcept
		: singleValue(0), span(), variant(std::move(other.variant))
	{
		if (variant)
		{
			T* pData = get_variant_data(variant);
			uint32_t repCount = variant->GetCount();

			if (repCount == 0)
			{
				// If the rep count is zero, the pointer's address is the value.
				singleValue = reinterpret_cast<T>(pData);
				span = std::span<T>(std::addressof(singleValue), 1);
			}
			else
			{
				span = std::span<T>(pData, repCount);
			}
		}
	}

	PropertyData& operator=(const PropertyData& other) = delete;

	PropertyData& operator=(PropertyData&& other) noexcept
	{
		variant = std::move(other.variant);

		if (variant)
		{
			T* pData = get_variant_data(variant);
			uint32_t repCount = variant->GetCount();

			if (repCount == 0)
			{
				// If the rep count is zero, the pointer's address is the value.
				singleValue = reinterpret_cast<T>(pData);
				span = std::span<T>(std::addressof(singleValue), 1);
			}
			else
			{
				span = std::span<T>(pData, repCount);
			}
		}
		else
		{
			singleValue = 0;
			span = std::span<T>();
		}

		return *this;
	}

	explicit operator bool() const noexcept
	{
		return !empty();
	}

	T operator[](size_t index) const
	{
		return span[index];
	}

	iterator begin() const
	{
		return span.begin();
	}

	iterator end() const
	{
		return span.end();
	}

	bool contains(T value) const
	{
		return std::find(span.begin(), span.end(), value) != span.end();
	}

	const T* data() const
	{
		return span.data();
	}

	bool empty() const
	{
		return span.empty();
	}

	bool load(cISCPropertyHolder* pPropertyHolder, uint32_t id)
	{
		cISCProperty* pProperty = pPropertyHolder->GetProperty(id);

		if (pProperty)
		{
			cIGZVariant* pVariant = pProperty->GetPropertyValue();

			if (pVariant)
			{
				T* pData = get_variant_data(pVariant);
				uint32_t repCount = pVariant->GetCount();

				if (repCount == 0)
				{
					// If the rep count is zero, the pointer's address is the value.
					singleValue = reinterpret_cast<T>(pData);
					span = std::span<T>(std::addressof(singleValue), 1);
				}
				else
				{
					span = std::span<T>(pData, repCount);
				}

				variant = pVariant;
			}
			else
			{
				singleValue = 0;
				span = std::span<T>();
				variant = nullptr;
			}
		}
		else
		{
			singleValue = 0;
			span = std::span<T>();
			variant = nullptr;
		}

		return !empty();
	}

	size_t size() const
	{
		return span.size();
	}
private:
	T* get_variant_data(cIGZVariant* pVariant)
	{
		if constexpr (std::is_same<T, bool>::value)
		{
			return pVariant->RefBool();
		}
		else if constexpr (std::is_same<T, uint8_t>::value)
		{
			return pVariant->RefUint8();
		}
		else if constexpr (std::is_same<T, int8_t>::value)
		{
			return pVariant->RefSint8();
		}
		else if constexpr (std::is_same<T, uint16_t>::value)
		{
			return pVariant->RefUint16();
		}
		else if constexpr (std::is_same<T, int16_t>::value)
		{
			return pVariant->RefSint16();
		}
		else if constexpr (std::is_same<T, uint32_t>::value)
		{
			return pVariant->RefUint32();
		}
		else if constexpr (std::is_same<T, int32_t>::value)
		{
			return pVariant->RefSint32();
		}
		else if constexpr (std::is_same<T, uint64_t>::value)
		{
			return pVariant->RefUint64();
		}
		else if constexpr (std::is_same<T, int64_t>::value)
		{
			return pVariant->RefSint64();
		}
		else if constexpr (std::is_same<T, float>::value)
		{
			return pVariant->RefFloat32();
		}
		else if constexpr (std::is_same<T, double>::value)
		{
			return pVariant->RefFloat64();
		}
		else
		{
			static_assert(false, "Unsupported integral type.");
			return nullptr;
		}
	}

	T singleValue;
	container span;
	cRZAutoRefCount<cIGZVariant> variant;
};
