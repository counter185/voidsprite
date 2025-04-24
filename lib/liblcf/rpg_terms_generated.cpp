/*
 * This file is part of liblcf. Copyright (c) liblcf authors.
 * https://github.com/EasyRPG/liblcf - https://easyrpg.org
 *
 * liblcf is Free/Libre Open Source Software, released under the MIT License.
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include "lcf/rpg/terms.h"

namespace lcf {

std::string rpg::Terms::TermOrDefault(const DBString& db_term, StringView default_term) {
	if (db_term == kDefaultTerm) {
		return ToString(default_term);
	}
	return ToString(db_term);
}

} // namespace lcf
