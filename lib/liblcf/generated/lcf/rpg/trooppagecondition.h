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

#ifndef LCF_RPG_TROOPPAGECONDITION_H
#define LCF_RPG_TROOPPAGECONDITION_H

// Headers
#include <array>
#include <stdint.h>
#include "lcf/context.h"
#include <ostream>
#include <type_traits>

/**
 * rpg::TroopPageCondition class.
 */
namespace lcf {
namespace rpg {
	class TroopPageCondition {
	public:
		struct Flags {
			union {
				struct {
					bool switch_a;
					bool switch_b;
					bool variable;
					bool turn;
					bool fatigue;
					bool enemy_hp;
					bool actor_hp;
					bool turn_enemy;
					bool turn_actor;
					bool command_actor;
				};
				std::array<bool, 10> flags;
			};
			Flags() noexcept: switch_a(false), switch_b(false), variable(false), turn(false), fatigue(false), enemy_hp(false), actor_hp(false), turn_enemy(false), turn_actor(false), command_actor(false)
			{}
		} flags;
		int32_t switch_a_id = 1;
		int32_t switch_b_id = 1;
		int32_t variable_id = 1;
		int32_t variable_value = 0;
		int32_t turn_a = 0;
		int32_t turn_b = 0;
		int32_t fatigue_min = 0;
		int32_t fatigue_max = 100;
		int32_t enemy_id = 0;
		int32_t enemy_hp_min = 0;
		int32_t enemy_hp_max = 100;
		int32_t actor_id = 1;
		int32_t actor_hp_min = 0;
		int32_t actor_hp_max = 100;
		int32_t turn_enemy_id = 0;
		int32_t turn_enemy_a = 0;
		int32_t turn_enemy_b = 0;
		int32_t turn_actor_id = 1;
		int32_t turn_actor_a = 0;
		int32_t turn_actor_b = 0;
		int32_t command_actor_id = 1;
		int32_t command_id = 1;
	};

	inline bool operator==(const TroopPageCondition::Flags& l, const TroopPageCondition::Flags& r) {
		return l.flags == r.flags;
	}

	inline bool operator!=(const TroopPageCondition::Flags& l, const TroopPageCondition::Flags& r) {
		return !(l == r);
	}

	std::ostream& operator<<(std::ostream& os, const TroopPageCondition::Flags& obj);

	inline bool operator==(const TroopPageCondition& l, const TroopPageCondition& r) {
		return l.flags == r.flags
		&& l.switch_a_id == r.switch_a_id
		&& l.switch_b_id == r.switch_b_id
		&& l.variable_id == r.variable_id
		&& l.variable_value == r.variable_value
		&& l.turn_a == r.turn_a
		&& l.turn_b == r.turn_b
		&& l.fatigue_min == r.fatigue_min
		&& l.fatigue_max == r.fatigue_max
		&& l.enemy_id == r.enemy_id
		&& l.enemy_hp_min == r.enemy_hp_min
		&& l.enemy_hp_max == r.enemy_hp_max
		&& l.actor_id == r.actor_id
		&& l.actor_hp_min == r.actor_hp_min
		&& l.actor_hp_max == r.actor_hp_max
		&& l.turn_enemy_id == r.turn_enemy_id
		&& l.turn_enemy_a == r.turn_enemy_a
		&& l.turn_enemy_b == r.turn_enemy_b
		&& l.turn_actor_id == r.turn_actor_id
		&& l.turn_actor_a == r.turn_actor_a
		&& l.turn_actor_b == r.turn_actor_b
		&& l.command_actor_id == r.command_actor_id
		&& l.command_id == r.command_id;
	}

	inline bool operator!=(const TroopPageCondition& l, const TroopPageCondition& r) {
		return !(l == r);
	}

	std::ostream& operator<<(std::ostream& os, const TroopPageCondition& obj);

	template <typename F, typename ParentCtx = Context<void,void>>
	void ForEachString(TroopPageCondition& obj, const F& f, const ParentCtx* parent_ctx = nullptr) {
		(void)obj;
		(void)f;
		(void)parent_ctx;
	}

} // namespace rpg
} // namespace lcf

#endif
