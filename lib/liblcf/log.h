/*
 * This file is part of liblcf. Copyright (c) liblcf authors.
 * https://github.com/EasyRPG/liblcf - https://easyrpg.org
 *
 * liblcf is Free/Libre Open Source Software, released under the MIT License.
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef LCF_LOG_H
#define LCF_LOG_H

#include "lcf/log_handler.h"

#ifdef __GNUG__
	#define LIKE_PRINTF __attribute__((format(printf, 1, 2)))
#else
	#define LIKE_PRINTF
#endif

namespace lcf {
namespace Log {

void Debug(const char* fmt, ...) LIKE_PRINTF;
void Warning(const char* fmt, ...) LIKE_PRINTF;
void Error(const char* fmt, ...) LIKE_PRINTF;

} // namespace Log
} // namespace lcf

#undef LIKE_PRINTF

#endif
