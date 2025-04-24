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

#ifndef LCF_RPG_CHIPSET_H
#define LCF_RPG_CHIPSET_H

// Headers
#include <stdint.h>
#include <vector>
#include "lcf/dbstring.h"
#include "lcf/enum_tags.h"
#include "lcf/context.h"
#include <ostream>
#include <type_traits>

/**
 * rpg::Chipset class.
 */
namespace lcf {
namespace rpg {
	class Chipset {
	public:
		enum AnimType {
			AnimType_reciprocating = 0,
			AnimType_cyclic = 1
		};
		static constexpr auto kAnimTypeTags = lcf::makeEnumTags<AnimType>(
			"reciprocating",
			"cyclic"
		);

		int ID = 0;
		DBString name;
		DBString chipset_name;
		std::vector<int16_t> terrain_data = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
		std::vector<uint8_t> passable_data_lower = {15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15};
		std::vector<uint8_t> passable_data_upper = {31, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15};
		int32_t animation_type = 0;
		int32_t animation_speed = 0;
	};
	inline std::ostream& operator<<(std::ostream& os, Chipset::AnimType code) {
		os << static_cast<std::underlying_type_t<decltype(code)>>(code);
		return os;
	}

	inline bool operator==(const Chipset& l, const Chipset& r) {
		return l.name == r.name
		&& l.chipset_name == r.chipset_name
		&& l.terrain_data == r.terrain_data
		&& l.passable_data_lower == r.passable_data_lower
		&& l.passable_data_upper == r.passable_data_upper
		&& l.animation_type == r.animation_type
		&& l.animation_speed == r.animation_speed;
	}

	inline bool operator!=(const Chipset& l, const Chipset& r) {
		return !(l == r);
	}

	std::ostream& operator<<(std::ostream& os, const Chipset& obj);

	template <typename F, typename ParentCtx = Context<void,void>>
	void ForEachString(Chipset& obj, const F& f, const ParentCtx* parent_ctx = nullptr) {
		const auto ctx1 = Context<Chipset, ParentCtx>{ "name", -1, &obj, parent_ctx };
		f(obj.name, ctx1);
		const auto ctx2 = Context<Chipset, ParentCtx>{ "chipset_name", -1, &obj, parent_ctx };
		f(obj.chipset_name, ctx2);
		(void)obj;
		(void)f;
		(void)parent_ctx;
	}

} // namespace rpg
} // namespace lcf

#endif
