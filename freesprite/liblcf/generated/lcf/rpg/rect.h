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

#ifndef LCF_RPG_RECT_H
#define LCF_RPG_RECT_H

// Headers
#include <stdint.h>
#include "lcf/context.h"
#include <ostream>
#include <type_traits>

/**
 * rpg::Rect class.
 */
namespace lcf {
namespace rpg {
	class Rect {
	public:
		uint32_t l = 0;
		uint32_t t = 0;
		uint32_t r = 0;
		uint32_t b = 0;
	};

	inline bool operator==(const Rect& l, const Rect& r) {
		return l.l == r.l
		&& l.t == r.t
		&& l.r == r.r
		&& l.b == r.b;
	}

	inline bool operator!=(const Rect& l, const Rect& r) {
		return !(l == r);
	}

	std::ostream& operator<<(std::ostream& os, const Rect& obj);

	template <typename F, typename ParentCtx = Context<void,void>>
	void ForEachString(Rect& obj, const F& f, const ParentCtx* parent_ctx = nullptr) {
		(void)obj;
		(void)f;
		(void)parent_ctx;
	}

} // namespace rpg
} // namespace lcf

#endif
