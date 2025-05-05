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
#include <filesystem>
#include <fstream>

enum class LogLevel : int32_t
{
	Info = 0,
	Error = 1,
	Debug = 2,
	Trace = 3
};

class Logger
{
public:

	static Logger& GetInstance();

	void Init(std::filesystem::path logFilePath, LogLevel logLevel);

	bool IsEnabled(LogLevel option) const;

	void WriteLogFileHeader(const char* const message);

	void WriteLine(LogLevel level, const char* const message);

	void WriteLineFormatted(LogLevel level, const char* const format, ...);

private:

	Logger();
	~Logger();

	void WriteLineCore(const char* const message);

	bool initialized;
	LogLevel logLevel;
	std::ofstream logFile;
};

