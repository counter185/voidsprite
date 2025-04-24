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

#ifndef LCF_RPG_BATTLERANIMATIONPOSE_H
#define LCF_RPG_BATTLERANIMATIONPOSE_H

// Headers
#include <stdint.h>
#include "lcf/dbstring.h"
#include "lcf/enum_tags.h"
#include "lcf/context.h"
#include <ostream>
#include <type_traits>

/**
 * rpg::BattlerAnimationPose class.
 */
namespace lcf {
namespace rpg {
	class BattlerAnimationPose {
	public:
		enum AnimType {
			AnimType_character = 0,
			AnimType_battle = 1
		};
		static constexpr auto kAnimTypeTags = lcf::makeEnumTags<AnimType>(
			"character",
			"battle"
		);

		int ID = 0;
		DBString name;
		DBString battler_name;
		int32_t battler_index = 0;
		int32_t animation_type = 0;
		int32_t battle_animation_id = 1;
	};
	inline std::ostream& operator<<(std::ostream& os, BattlerAnimationPose::AnimType code) {
		os << static_cast<std::underlying_type_t<decltype(code)>>(code);
		return os;
	}

	inline bool operator==(const BattlerAnimationPose& l, const BattlerAnimationPose& r) {
		return l.name == r.name
		&& l.battler_name == r.battler_name
		&& l.battler_index == r.battler_index
		&& l.animation_type == r.animation_type
		&& l.battle_animation_id == r.battle_animation_id;
	}

	inline bool operator!=(const BattlerAnimationPose& l, const BattlerAnimationPose& r) {
		return !(l == r);
	}

	std::ostream& operator<<(std::ostream& os, const BattlerAnimationPose& obj);

	template <typename F, typename ParentCtx = Context<void,void>>
	void ForEachString(BattlerAnimationPose& obj, const F& f, const ParentCtx* parent_ctx = nullptr) {
		const auto ctx1 = Context<BattlerAnimationPose, ParentCtx>{ "name", -1, &obj, parent_ctx };
		f(obj.name, ctx1);
		const auto ctx2 = Context<BattlerAnimationPose, ParentCtx>{ "battler_name", -1, &obj, parent_ctx };
		f(obj.battler_name, ctx2);
		(void)obj;
		(void)f;
		(void)parent_ctx;
	}

} // namespace rpg
} // namespace lcf

#endif
