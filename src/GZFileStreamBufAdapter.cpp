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

#include "GZFileStreamBufAdapter.h"

constexpr size_t MaxBufferSize = 4096;
constexpr size_t PutBack = 1;

namespace
{
	size_t GetBufferLength(cIGZFile* pFile)
	{
		size_t bufferLength = 0;

		if (pFile)
		{
			bufferLength = PutBack + std::min(static_cast<size_t>(pFile->Length()), MaxBufferSize);
		}

		return bufferLength;
	}
}


GZFileStreamBufAdapter::GZFileStreamBufAdapter(cRZAutoRefCount<cIGZFile>&& pIGZFile)
	: file(std::move(pIGZFile)),
	  buffer(GetBufferLength(file))
{
	char* end = buffer.data() + buffer.size();

	setg(end, end, end);
}

GZFileStreamBufAdapter::GZFileStreamBufAdapter(GZFileStreamBufAdapter&& other) noexcept
	: file(std::move(other.file)),
	  buffer(std::move(other.buffer))
{
	char* end = buffer.data() + buffer.size();

	setg(end, end, end);
}

GZFileStreamBufAdapter& GZFileStreamBufAdapter::operator=(GZFileStreamBufAdapter&& other) noexcept
{
	file = std::move(other.file);
	buffer = std::move(other.buffer);

	char* end = buffer.data() + buffer.size();

	setg(end, end, end);

	return *this;
}

std::streambuf::int_type GZFileStreamBufAdapter::underflow()
{
	// This code is adapted from Example 1 at:
	// https://web.archive.org/web/20150711115652/http://www.mr-edd.co.uk/blog/beginners_guide_streambuf

	if (gptr() < egptr()) // buffer not exhausted
	{
		return traits_type::to_int_type(*gptr());
	}

	if (!file || !file->IsOpen())
	{
		return traits_type::eof();
	}

	char* base = buffer.data();
	char* start = base;

	if (eback() == base) // true when this isn't the first fill
	{
		// Make arrangements for putback characters
		std::memmove(base, egptr() - PutBack, PutBack);
		start += PutBack;
	}

	// Start is now the start of the buffer, proper.
	// Read from the file in to the provided buffer
	uint32_t byteCount = std::min(
		buffer.size() - (start - base),
		file->Length() - file->Position());

	if (!file->ReadWithCount(start, byteCount) || byteCount == 0)
	{
		return traits_type::eof();
	}

	// Set buffer pointers
	setg(base, start, start + byteCount);

	return traits_type::to_int_type(*gptr());
}
