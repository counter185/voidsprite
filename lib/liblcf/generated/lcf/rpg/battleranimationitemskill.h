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

#ifndef LCF_RPG_BATTLERANIMATIONITEMSKILL_H
#define LCF_RPG_BATTLERANIMATIONITEMSKILL_H

// Headers
#include <stdint.h>
#include "lcf/enum_tags.h"
#include "lcf/context.h"
#include <ostream>
#include <type_traits>

/**
 * rpg::BattlerAnimationItemSkill class.
 */
namespace lcf {
namespace rpg {
	class BattlerAnimationItemSkill {
	public:
		enum Speed {
			Speed_fast = 0,
			Speed_medium = 1,
			Speed_slow = 2
		};
		static constexpr auto kSpeedTags = lcf::makeEnumTags<Speed>(
			"fast",
			"medium",
			"slow"
		);
		enum AnimType {
			AnimType_weapon = 0,
			AnimType_battle = 1
		};
		static constexpr auto kAnimTypeTags = lcf::makeEnumTags<AnimType>(
			"weapon",
			"battle"
		);
		enum Movement {
			Movement_none = 0,
			Movement_step = 1,
			Movement_jump = 2,
			Movement_move = 3
		};
		static constexpr auto kMovementTags = lcf::makeEnumTags<Movement>(
			"none",
			"step",
			"jump",
			"move"
		);
		enum Afterimage {
			Afterimage_none = 0,
			Afterimage_add = 1
		};
		static constexpr auto kAfterimageTags = lcf::makeEnumTags<Afterimage>(
			"none",
			"add"
		);

		int ID = 0;
		int32_t unknown02 = 0;
		int32_t type = 0;
		int32_t weapon_animation_id = 0;
		int32_t movement = 0;
		int32_t after_image = 0;
		int32_t attacks = 0;
		bool ranged = false;
		int32_t ranged_animation_id = 0;
		int32_t ranged_speed = 0;
		int32_t battle_animation_id = 1;
		int32_t pose = 3;
	};
	inline std::ostream& operator<<(std::ostream& os, BattlerAnimationItemSkill::Speed code) {
		os << static_cast<std::underlying_type_t<decltype(code)>>(code);
		return os;
	}
	inline std::ostream& operator<<(std::ostream& os, BattlerAnimationItemSkill::AnimType code) {
		os << static_cast<std::underlying_type_t<decltype(code)>>(code);
		return os;
	}
	inline std::ostream& operator<<(std::ostream& os, BattlerAnimationItemSkill::Movement code) {
		os << static_cast<std::underlying_type_t<decltype(code)>>(code);
		return os;
	}
	inline std::ostream& operator<<(std::ostream& os, BattlerAnimationItemSkill::Afterimage code) {
		os << static_cast<std::underlying_type_t<decltype(code)>>(code);
		return os;
	}

	inline bool operator==(const BattlerAnimationItemSkill& l, const BattlerAnimationItemSkill& r) {
		return l.unknown02 == r.unknown02
		&& l.type == r.type
		&& l.weapon_animation_id == r.weapon_animation_id
		&& l.movement == r.movement
		&& l.after_image == r.after_image
		&& l.attacks == r.attacks
		&& l.ranged == r.ranged
		&& l.ranged_animation_id == r.ranged_animation_id
		&& l.ranged_speed == r.ranged_speed
		&& l.battle_animation_id == r.battle_animation_id
		&& l.pose == r.pose;
	}

	inline bool operator!=(const BattlerAnimationItemSkill& l, const BattlerAnimationItemSkill& r) {
		return !(l == r);
	}

	std::ostream& operator<<(std::ostream& os, const BattlerAnimationItemSkill& obj);

	template <typename F, typename ParentCtx = Context<void,void>>
	void ForEachString(BattlerAnimationItemSkill& obj, const F& f, const ParentCtx* parent_ctx = nullptr) {
		(void)obj;
		(void)f;
		(void)parent_ctx;
	}

} // namespace rpg
} // namespace lcf

#endif
