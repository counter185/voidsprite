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

#ifndef LCF_RPG_ITEM_H
#define LCF_RPG_ITEM_H

// Headers
#include <stdint.h>
#include <vector>
#include "lcf/dbbitarray.h"
#include "lcf/dbstring.h"
#include "lcf/enum_tags.h"
#include "lcf/rpg/battleranimationitemskill.h"
#include "lcf/context.h"
#include <ostream>
#include <type_traits>

/**
 * rpg::Item class.
 */
namespace lcf {
namespace rpg {
	class Item {
	public:
		// Sentinel name used to denote that the default item start message should be used.
		static constexpr const char* kDefaultMessage = "default_message";

		enum Type {
			Type_normal = 0,
			Type_weapon = 1,
			Type_shield = 2,
			Type_armor = 3,
			Type_helmet = 4,
			Type_accessory = 5,
			Type_medicine = 6,
			Type_book = 7,
			Type_material = 8,
			Type_special = 9,
			Type_switch = 10
		};
		static constexpr auto kTypeTags = lcf::makeEnumTags<Type>(
			"normal",
			"weapon",
			"shield",
			"armor",
			"helmet",
			"accessory",
			"medicine",
			"book",
			"material",
			"special",
			"switch"
		);
		enum Trajectory {
			Trajectory_straight = 0,
			Trajectory_return = 1
		};
		static constexpr auto kTrajectoryTags = lcf::makeEnumTags<Trajectory>(
			"straight",
			"return"
		);
		enum Target {
			Target_single = 0,
			Target_center = 1,
			Target_simultaneous = 2,
			Target_sequential = 3
		};
		static constexpr auto kTargetTags = lcf::makeEnumTags<Target>(
			"single",
			"center",
			"simultaneous",
			"sequential"
		);

		int ID = 0;
		DBString name;
		DBString description;
		int32_t type = 0;
		int32_t price = 0;
		int32_t uses = 1;
		int32_t atk_points1 = 0;
		int32_t def_points1 = 0;
		int32_t spi_points1 = 0;
		int32_t agi_points1 = 0;
		bool two_handed = false;
		int32_t sp_cost = 0;
		int32_t hit = 90;
		int32_t critical_hit = 0;
		int32_t animation_id = 1;
		bool preemptive = false;
		bool dual_attack = false;
		bool attack_all = false;
		bool ignore_evasion = false;
		bool prevent_critical = false;
		bool raise_evasion = false;
		bool half_sp_cost = false;
		bool no_terrain_damage = false;
		bool cursed = false;
		bool entire_party = false;
		int32_t recover_hp_rate = 0;
		int32_t recover_hp = 0;
		int32_t recover_sp_rate = 0;
		int32_t recover_sp = 0;
		bool occasion_field1 = false;
		bool ko_only = false;
		int32_t max_hp_points = 0;
		int32_t max_sp_points = 0;
		int32_t atk_points2 = 0;
		int32_t def_points2 = 0;
		int32_t spi_points2 = 0;
		int32_t agi_points2 = 0;
		int32_t using_message = 0;
		int32_t skill_id = 1;
		int32_t switch_id = 1;
		bool occasion_field2 = true;
		bool occasion_battle = false;
		DBBitArray actor_set;
		DBBitArray state_set;
		DBBitArray attribute_set;
		int32_t state_chance = 0;
		bool reverse_state_effect = false;
		int32_t weapon_animation = -1;
		std::vector<BattlerAnimationItemSkill> animation_data;
		bool use_skill = false;
		DBBitArray class_set;
		int32_t ranged_trajectory = 0;
		int32_t ranged_target = 0;
		DBString easyrpg_using_message = DBString(kDefaultMessage);
		int32_t easyrpg_max_count = -1;
	};
	inline std::ostream& operator<<(std::ostream& os, Item::Type code) {
		os << static_cast<std::underlying_type_t<decltype(code)>>(code);
		return os;
	}
	inline std::ostream& operator<<(std::ostream& os, Item::Trajectory code) {
		os << static_cast<std::underlying_type_t<decltype(code)>>(code);
		return os;
	}
	inline std::ostream& operator<<(std::ostream& os, Item::Target code) {
		os << static_cast<std::underlying_type_t<decltype(code)>>(code);
		return os;
	}

	inline bool operator==(const Item& l, const Item& r) {
		return l.name == r.name
		&& l.description == r.description
		&& l.type == r.type
		&& l.price == r.price
		&& l.uses == r.uses
		&& l.atk_points1 == r.atk_points1
		&& l.def_points1 == r.def_points1
		&& l.spi_points1 == r.spi_points1
		&& l.agi_points1 == r.agi_points1
		&& l.two_handed == r.two_handed
		&& l.sp_cost == r.sp_cost
		&& l.hit == r.hit
		&& l.critical_hit == r.critical_hit
		&& l.animation_id == r.animation_id
		&& l.preemptive == r.preemptive
		&& l.dual_attack == r.dual_attack
		&& l.attack_all == r.attack_all
		&& l.ignore_evasion == r.ignore_evasion
		&& l.prevent_critical == r.prevent_critical
		&& l.raise_evasion == r.raise_evasion
		&& l.half_sp_cost == r.half_sp_cost
		&& l.no_terrain_damage == r.no_terrain_damage
		&& l.cursed == r.cursed
		&& l.entire_party == r.entire_party
		&& l.recover_hp_rate == r.recover_hp_rate
		&& l.recover_hp == r.recover_hp
		&& l.recover_sp_rate == r.recover_sp_rate
		&& l.recover_sp == r.recover_sp
		&& l.occasion_field1 == r.occasion_field1
		&& l.ko_only == r.ko_only
		&& l.max_hp_points == r.max_hp_points
		&& l.max_sp_points == r.max_sp_points
		&& l.atk_points2 == r.atk_points2
		&& l.def_points2 == r.def_points2
		&& l.spi_points2 == r.spi_points2
		&& l.agi_points2 == r.agi_points2
		&& l.using_message == r.using_message
		&& l.skill_id == r.skill_id
		&& l.switch_id == r.switch_id
		&& l.occasion_field2 == r.occasion_field2
		&& l.occasion_battle == r.occasion_battle
		&& l.actor_set == r.actor_set
		&& l.state_set == r.state_set
		&& l.attribute_set == r.attribute_set
		&& l.state_chance == r.state_chance
		&& l.reverse_state_effect == r.reverse_state_effect
		&& l.weapon_animation == r.weapon_animation
		&& l.animation_data == r.animation_data
		&& l.use_skill == r.use_skill
		&& l.class_set == r.class_set
		&& l.ranged_trajectory == r.ranged_trajectory
		&& l.ranged_target == r.ranged_target
		&& l.easyrpg_using_message == r.easyrpg_using_message
		&& l.easyrpg_max_count == r.easyrpg_max_count;
	}

	inline bool operator!=(const Item& l, const Item& r) {
		return !(l == r);
	}

	std::ostream& operator<<(std::ostream& os, const Item& obj);

	template <typename F, typename ParentCtx = Context<void,void>>
	void ForEachString(Item& obj, const F& f, const ParentCtx* parent_ctx = nullptr) {
		const auto ctx1 = Context<Item, ParentCtx>{ "name", -1, &obj, parent_ctx };
		f(obj.name, ctx1);
		const auto ctx2 = Context<Item, ParentCtx>{ "description", -1, &obj, parent_ctx };
		f(obj.description, ctx2);
		const auto ctx53 = Context<Item, ParentCtx>{ "easyrpg_using_message", -1, &obj, parent_ctx };
		f(obj.easyrpg_using_message, ctx53);
		(void)obj;
		(void)f;
		(void)parent_ctx;
	}

} // namespace rpg
} // namespace lcf

#endif
