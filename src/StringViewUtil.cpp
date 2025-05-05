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

#include "StringViewUtil.h"
#include "boost/algorithm/string.hpp"
#include <charconv>

bool StringViewUtil::EqualsIgnoreCase(const std::string_view& lhs, const std::string_view& rhs)
{
	return lhs.length() == rhs.length()
		&& boost::iequals(lhs, rhs);
}

bool StringViewUtil::StartsWithIgnoreCase(const std::string_view& lhs, const std::string_view& rhs)
{
	return lhs.length() >=  rhs.length()
		&& boost::istarts_with(lhs, rhs);
}

void StringViewUtil::Split(
	const std::string_view& input,
	std::string_view::value_type delim,
	std::vector<std::string_view>& results)
{
	// The following code is adapted from: https://stackoverflow.com/a/36301144

	const size_t inputLength = input.length();
	bool foundDoubleQuote = false;
	bool foundSingleQuote = false;
	size_t argumentLength = 0;

	for (size_t i = 0; i < inputLength; i++)
	{
		size_t start = i;
		if (input[i] == '\"')
		{
			foundDoubleQuote = true;
		}
		else if (input[i] == '\'')
		{
			foundSingleQuote = true;
		}

		if (foundDoubleQuote)
		{
			i++;
			start++;

			while (i < inputLength && input[i] != '\"')
			{
				i++;
			}

			if (i < inputLength)
			{
				foundDoubleQuote = false;
			}

			argumentLength = i - start;
			i++;
		}
		else if (foundSingleQuote)
		{
			i++;
			start++;

			while (i < inputLength && input[i] != '\'')
			{
				i++;
			}

			if (i < inputLength)
			{
				foundSingleQuote = false;
			}

			argumentLength = i - start;
			i++;
		}
		else
		{
			while (i < inputLength && input[i] != delim)
			{
				i++;
			}
			argumentLength = i - start;
		}

		if (argumentLength > 0)
		{
			results.push_back(input.substr(start, argumentLength));
		}
	}
}

std::string_view StringViewUtil::TrimWhiteSpace(const std::string_view& input)
{
	return boost::trim_copy(input);
}

bool StringViewUtil::TryParse(const std::string_view& input, uint32_t& outValue)
{
	const char* start = input.data();
	const char* end = start + input.size();
	int base = 10;

	if (StartsWithIgnoreCase(input, "0x"))
	{
		// std::from_chars can't parse hexadecimal numbers with the 0x prefix.
		start += 2;
		base = 16;
	}

	const std::from_chars_result result = std::from_chars(start, end, outValue, base);

	if (result.ec == std::errc{} && result.ptr == end)
	{
		return true;
	}

	outValue = 0;
	return false;
}
