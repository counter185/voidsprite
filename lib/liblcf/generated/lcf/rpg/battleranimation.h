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

#ifndef LCF_RPG_BATTLERANIMATION_H
#define LCF_RPG_BATTLERANIMATION_H

// Headers
#include <vector>
#include "lcf/dbstring.h"
#include "lcf/enum_tags.h"
#include "lcf/rpg/battleranimationpose.h"
#include "lcf/rpg/battleranimationweapon.h"
#include "lcf/context.h"
#include <ostream>
#include <type_traits>

/**
 * rpg::BattlerAnimation class.
 */
namespace lcf {
namespace rpg {
	class BattlerAnimation {
	public:
		enum Speed {
			Speed_slow = 20,
			Speed_medium = 14,
			Speed_fast = 8
		};
		static constexpr auto kSpeedTags = lcf::EnumTags{
			Speed_slow, "slow",
			Speed_medium, "medium",
			Speed_fast, "fast"
		};
		enum Pose {
			Pose_Idle = 0,
			Pose_AttackRight = 1,
			Pose_AttackLeft = 2,
			Pose_Skill = 3,
			Pose_Dead = 4,
			Pose_Damage = 5,
			Pose_Dazed = 6,
			Pose_Defend = 7,
			Pose_WalkLeft = 8,
			Pose_WalkRight = 9,
			Pose_Victory = 10,
			Pose_Item = 11
		};
		static constexpr auto kPoseTags = lcf::makeEnumTags<Pose>(
			"Idle",
			"AttackRight",
			"AttackLeft",
			"Skill",
			"Dead",
			"Damage",
			"Dazed",
			"Defend",
			"WalkLeft",
			"WalkRight",
			"Victory",
			"Item"
		);

		int ID = 0;
		DBString name;
		int32_t speed = 20;
		std::vector<BattlerAnimationPose> poses;
		std::vector<BattlerAnimationWeapon> weapons;
	};
	inline std::ostream& operator<<(std::ostream& os, BattlerAnimation::Speed code) {
		os << static_cast<std::underlying_type_t<decltype(code)>>(code);
		return os;
	}
	inline std::ostream& operator<<(std::ostream& os, BattlerAnimation::Pose code) {
		os << static_cast<std::underlying_type_t<decltype(code)>>(code);
		return os;
	}

	inline bool operator==(const BattlerAnimation& l, const BattlerAnimation& r) {
		return l.name == r.name
		&& l.speed == r.speed
		&& l.poses == r.poses
		&& l.weapons == r.weapons;
	}

	inline bool operator!=(const BattlerAnimation& l, const BattlerAnimation& r) {
		return !(l == r);
	}

	std::ostream& operator<<(std::ostream& os, const BattlerAnimation& obj);

	template <typename F, typename ParentCtx = Context<void,void>>
	void ForEachString(BattlerAnimation& obj, const F& f, const ParentCtx* parent_ctx = nullptr) {
		const auto ctx1 = Context<BattlerAnimation, ParentCtx>{ "name", -1, &obj, parent_ctx };
		f(obj.name, ctx1);
		for (int i = 0; i < static_cast<int>(obj.poses.size()); ++i) {
			const auto ctx3 = Context<BattlerAnimation, ParentCtx>{ "poses", i, &obj, parent_ctx };
			ForEachString(obj.poses[i], f, &ctx3);
		}
		for (int i = 0; i < static_cast<int>(obj.weapons.size()); ++i) {
			const auto ctx4 = Context<BattlerAnimation, ParentCtx>{ "weapons", i, &obj, parent_ctx };
			ForEachString(obj.weapons[i], f, &ctx4);
		}
		(void)obj;
		(void)f;
		(void)parent_ctx;
	}

} // namespace rpg
} // namespace lcf

#endif
