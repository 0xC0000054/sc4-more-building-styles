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
#include "cIGZFile.h"
#include "cRZAutoRefCount.h"
#include <iosfwd>
#include <limits>
#include <streambuf>
#include <vector>
#include <utility>

class GZFileStreamBufAdapter final : public std::streambuf
{
public:
	GZFileStreamBufAdapter(cRZAutoRefCount<cIGZFile>&& file);
	GZFileStreamBufAdapter(const GZFileStreamBufAdapter&) = delete;
	GZFileStreamBufAdapter(GZFileStreamBufAdapter&&) noexcept;

	GZFileStreamBufAdapter& operator=(const GZFileStreamBufAdapter&) = delete;
	GZFileStreamBufAdapter& operator=(GZFileStreamBufAdapter&&) noexcept;

private:
	std::streambuf::int_type underflow() override;

	cRZAutoRefCount<cIGZFile> file;
	std::vector<char> buffer;
};

