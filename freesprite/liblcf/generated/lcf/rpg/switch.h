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

#ifndef LCF_RPG_SWITCH_H
#define LCF_RPG_SWITCH_H

// Headers
#include "lcf/dbstring.h"
#include "lcf/context.h"
#include <ostream>
#include <type_traits>

/**
 * rpg::Switch class.
 */
namespace lcf {
namespace rpg {
	class Switch {
	public:
		int ID = 0;
		DBString name;
	};

	inline bool operator==(const Switch& l, const Switch& r) {
		return l.name == r.name;
	}

	inline bool operator!=(const Switch& l, const Switch& r) {
		return !(l == r);
	}

	std::ostream& operator<<(std::ostream& os, const Switch& obj);

	template <typename F, typename ParentCtx = Context<void,void>>
	void ForEachString(Switch& obj, const F& f, const ParentCtx* parent_ctx = nullptr) {
		const auto ctx1 = Context<Switch, ParentCtx>{ "name", -1, &obj, parent_ctx };
		f(obj.name, ctx1);
		(void)obj;
		(void)f;
		(void)parent_ctx;
	}

} // namespace rpg
} // namespace lcf

#endif
