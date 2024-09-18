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

#ifndef LCF_RPG_ATTRIBUTE_H
#define LCF_RPG_ATTRIBUTE_H

// Headers
#include <stdint.h>
#include "lcf/dbstring.h"
#include "lcf/enum_tags.h"
#include "lcf/context.h"
#include <ostream>
#include <type_traits>

/**
 * rpg::Attribute class.
 */
namespace lcf {
namespace rpg {
	class Attribute {
	public:
		enum Type {
			Type_physical = 0,
			Type_magical = 1
		};
		static constexpr auto kTypeTags = lcf::makeEnumTags<Type>(
			"physical",
			"magical"
		);

		int ID = 0;
		DBString name;
		int32_t type = 0;
		int32_t a_rate = 300;
		int32_t b_rate = 200;
		int32_t c_rate = 100;
		int32_t d_rate = 50;
		int32_t e_rate = 0;
	};
	inline std::ostream& operator<<(std::ostream& os, Attribute::Type code) {
		os << static_cast<std::underlying_type_t<decltype(code)>>(code);
		return os;
	}

	inline bool operator==(const Attribute& l, const Attribute& r) {
		return l.name == r.name
		&& l.type == r.type
		&& l.a_rate == r.a_rate
		&& l.b_rate == r.b_rate
		&& l.c_rate == r.c_rate
		&& l.d_rate == r.d_rate
		&& l.e_rate == r.e_rate;
	}

	inline bool operator!=(const Attribute& l, const Attribute& r) {
		return !(l == r);
	}

	std::ostream& operator<<(std::ostream& os, const Attribute& obj);

	template <typename F, typename ParentCtx = Context<void,void>>
	void ForEachString(Attribute& obj, const F& f, const ParentCtx* parent_ctx = nullptr) {
		const auto ctx1 = Context<Attribute, ParentCtx>{ "name", -1, &obj, parent_ctx };
		f(obj.name, ctx1);
		(void)obj;
		(void)f;
		(void)parent_ctx;
	}

} // namespace rpg
} // namespace lcf

#endif
