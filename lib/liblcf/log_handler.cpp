/*
 * This file is part of liblcf. Copyright (c) liblcf authors.
 * https://github.com/EasyRPG/liblcf - https://easyrpg.org
 *
 * liblcf is Free/Libre Open Source Software, released under the MIT License.
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include "lcf/log_handler.h"
#include <cassert>
#include <cstdarg>
#include <cstdio>
#include <iostream>

namespace lcf {
namespace LogHandler {
namespace {
	void DefaultHandler(LogHandler::Level level, StringView message, UserData) {
		switch (level) {
			case Level::Debug:
				std::cerr << "Debug: ";
				break;
			case Level::Warning:
				std::cerr << "Warning: ";
				break;
			case Level::Error:
				std::cerr << "Error: ";
				break;
			default:
				assert(false && "Invalid Log Level");
		}
		std::cerr << message << "\n";
	}

	Level level = Level::Debug;
	LogHandlerFn output_fn = DefaultHandler;
	UserData output_userdata = nullptr;
}

void SetHandler(LogHandlerFn fn, UserData userdata) {
	if (!fn) {
		output_fn = DefaultHandler;
		output_userdata = nullptr;
	} else {
		output_fn = fn;
		output_userdata = userdata;
	}
}

void SetLevel(Level new_level) {
	level = new_level;
}

} // namespace Output

namespace Log {
namespace {
	std::string format_string(char const* fmt, va_list args) {
		char buf[4096];
		int const result = vsnprintf(buf, sizeof(buf), fmt, args);
		if (result < 0) {
			return {};
		}

		return {buf, static_cast<unsigned int>(result) < sizeof(buf) ? result : sizeof(buf)};
	}
}

void Debug(const char* fmt, ...) {
	if (static_cast<int>(LogHandler::Level::Debug) >= static_cast<int>(LogHandler::level)) {
		va_list args;
		va_start(args, fmt);
		auto msg = format_string(fmt, args);
		LogHandler::output_fn(LogHandler::Level::Debug, msg, LogHandler::output_userdata);
		va_end(args);
	}
}

void Warning(const char* fmt, ...) {
	if (static_cast<int>(LogHandler::Level::Warning) >= static_cast<int>(LogHandler::level)) {
		va_list args;
		va_start(args, fmt);
		auto msg = format_string(fmt, args);
		LogHandler::output_fn(LogHandler::Level::Warning, msg, LogHandler::output_userdata);
		va_end(args);
	}
}

void Error(const char* fmt, ...) {
	if (static_cast<int>(LogHandler::Level::Error) >= static_cast<int>(LogHandler::level)) {
		va_list args;
		va_start(args, fmt);
		auto msg = format_string(fmt, args);
		LogHandler::output_fn(LogHandler::Level::Error, msg, LogHandler::output_userdata);
		va_end(args);
	}
}

} // namespace Log
} // namespace lcf
