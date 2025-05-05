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
#include "cIGZAllocatorService.h"
#include "GZServPtrs.h"
#include <cstdint>
#include <stdexcept>

template<typename T> class SC4VectorIterator
{
public:
	using iterator_category = std::forward_iterator_tag;
	using difference_type = std::ptrdiff_t;
	using value_type = T;
	using pointer = value_type*;
	using reference = value_type&;

	SC4VectorIterator(pointer pValue) : mpValue(pValue)
	{
	}

	~SC4VectorIterator()
	{
	}

	reference operator*() const
	{
		return *mpValue;
	}

	pointer operator->()
	{
		return mpValue;
	}

	// Prefix increment
	SC4VectorIterator& operator++()
	{
		mpValue = mpValue + 1;
		return *this;
	}

	// Postfix increment
	SC4VectorIterator operator++(int)
	{
		SC4VectorIterator tmp = *this;
		++(*this);
		return tmp;
	}

	static friend bool operator== (const SC4VectorIterator& a, const SC4VectorIterator& b)
	{
		return a.mpValue == b.mpValue;
	};
	static friend bool operator!= (const SC4VectorIterator& a, const SC4VectorIterator& b)
	{
		return a.mpValue != b.mpValue;
	};
private:
	pointer mpValue;
};

template<typename T> class SC4Vector
{
public:
	typedef SC4VectorIterator<T> iterator;
	typedef SC4VectorIterator<const T> const_iterator;

	iterator begin()
	{
		return iterator(mpStart);
	}

	iterator end()
	{
		return iterator(mpEnd);
	}

	const_iterator begin() const
	{
		return const_iterator(mpStart);
	}

	const_iterator end() const
	{
		return const_iterator(mpEnd);
	}

	const_iterator cbegin() const
	{
		return const_iterator(mpStart);
	}

	const_iterator cend() const
	{
		return const_iterator(mpEnd);
	}

	size_t capacity() const
	{
		return mpCapacity - mpStart;
	}

	void clear()
	{
		mpEnd = mpStart;
	}

	bool empty() const
	{
		return mpStart == mpEnd;
	}

	void push_back(const T& value)
	{
		push_back(&value, 1);
	}

	void push_back(const T* const values, size_t count)
	{
		if (count == 0)
		{
			return;
		}

		const size_t remainingCapacity = remaining_capacity();

		if (count > remainingCapacity)
		{
			reserve(capacity() + count);
		}

		memmove(mpEnd, values, count * sizeof(T));
		mpEnd = mpEnd + count;
	}

	void reserve(size_t newCapacity)
	{
		if (newCapacity > capacity())
		{
			T* newStart = allocate(newCapacity);
			T* newEnd = newStart;

			if (!empty())
			{
				const size_t existingSize = size();

				memmove(newStart, mpStart, existingSize * sizeof(T));
				deallocate(mpStart);
				newEnd = newStart + existingSize;
			}

			mpStart = newStart;
			mpEnd = newEnd;
			mpCapacity = mpStart + newCapacity;
		}
	}

	size_t size() const
	{
		return mpEnd - mpStart;
	}

	T operator[](size_t index) const
	{
		return mpStart[index];
	}

private:
	static T* allocate(size_t numElements)
	{
		cIGZAllocatorServicePtr pAllocator;

		T* ptr = static_cast<T*>(pAllocator->Allocate(numElements * sizeof(T)));

		if (!ptr)
		{
			throw std::bad_alloc();
		}

		return ptr;
	}

	static void deallocate(T* ptr)
	{
		cIGZAllocatorServicePtr pAllocator;

		pAllocator->Deallocate(ptr);
	}

	size_t remaining_capacity() const
	{
		return mpCapacity - mpEnd;
	}


	T* mpStart;
	T* mpEnd;
	T* mpCapacity;
};