/*
 * This file is part of liblcf. Copyright (c) liblcf authors.
 * https://github.com/EasyRPG/liblcf - https://easyrpg.org
 *
 * liblcf is Free/Libre Open Source Software, released under the MIT License.
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef LCF_OUTPUT_H
#define LCF_OUTPUT_H

#include "string_view.h"
#include "enum_tags.h"

namespace lcf {
namespace LogHandler {

enum class Level {
	Debug,
	Warning,
	Error,
	Highest
};

static constexpr auto kLevelTags = lcf::makeEnumTags<Level>(
	"Debug",
	"Warning",
	"Error",
	"Highest"
);

using UserData = void*;
using LogHandlerFn = void (*)(Level level, StringView message, UserData userdata);

/**
 * Sets the output handler for all lcf logging.
 * The default handler prints to standard error.
 *
 * @param fn New output handler. nullptr for default handler.
 * @param userdata Passed to the log handler function. Is not touched by liblcf.
 */
void SetHandler(LogHandlerFn fn, UserData userdata = nullptr);

/**
 * Only report issues that have at least this log level.
 * Use Highest to disable logging.
 * Default: Debug
 *
 * @param new_level New log level
 */
void SetLevel(Level new_level);

} // namespace LogHandler
} // namespace lcf

#endif
