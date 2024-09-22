/* !!!! GENERATED FILE - DO NOT EDIT !!!!
 * --------------------------------------
 *
 * This file is part of liblcf. Copyright (c) liblcf authors.
 * https://github.com/EasyRPG/liblcf - https://easyrpg.org
 *
 * liblcf is Free/Libre Open Source Software, released under the MIT License.
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef LCF_RPG_SAVETARGET_H
#define LCF_RPG_SAVETARGET_H

// Headers
#include <stdint.h>
#include "lcf/context.h"
#include <ostream>
#include <type_traits>

/**
 * rpg::SaveTarget class.
 */
namespace lcf {
namespace rpg {
	class SaveTarget {
	public:
		int ID = 0;
		int32_t map_id = 0;
		int32_t map_x = 0;
		int32_t map_y = 0;
		bool switch_on = false;
		int32_t switch_id = 0;
	};

	inline bool operator==(const SaveTarget& l, const SaveTarget& r) {
		return l.map_id == r.map_id
		&& l.map_x == r.map_x
		&& l.map_y == r.map_y
		&& l.switch_on == r.switch_on
		&& l.switch_id == r.switch_id;
	}

	inline bool operator!=(const SaveTarget& l, const SaveTarget& r) {
		return !(l == r);
	}

	std::ostream& operator<<(std::ostream& os, const SaveTarget& obj);

	template <typename F, typename ParentCtx = Context<void,void>>
	void ForEachString(SaveTarget& obj, const F& f, const ParentCtx* parent_ctx = nullptr) {
		(void)obj;
		(void)f;
		(void)parent_ctx;
	}

} // namespace rpg
} // namespace lcf

#endif
