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

#ifndef LCF_RPG_STATE_H
#define LCF_RPG_STATE_H

// Headers
#include <stdint.h>
#include "lcf/dbbitarray.h"
#include "lcf/dbstring.h"
#include "lcf/enum_tags.h"
#include "lcf/context.h"
#include <ostream>
#include <type_traits>

/**
 * rpg::State class.
 */
namespace lcf {
namespace rpg {
	class State {
	public:
		// The ID of the special death state
		static constexpr int kDeathID = 1;

		enum Persistence {
			Persistence_ends = 0,
			Persistence_persists = 1
		};
		static constexpr auto kPersistenceTags = lcf::makeEnumTags<Persistence>(
			"ends",
			"persists"
		);
		enum Restriction {
			Restriction_normal = 0,
			Restriction_do_nothing = 1,
			Restriction_attack_enemy = 2,
			Restriction_attack_ally = 3
		};
		static constexpr auto kRestrictionTags = lcf::makeEnumTags<Restriction>(
			"normal",
			"do_nothing",
			"attack_enemy",
			"attack_ally"
		);
		enum AffectType {
			AffectType_half = 0,
			AffectType_double = 1,
			AffectType_nothing = 2
		};
		static constexpr auto kAffectTypeTags = lcf::makeEnumTags<AffectType>(
			"half",
			"double",
			"nothing"
		);
		enum ChangeType {
			ChangeType_lose = 0,
			ChangeType_gain = 1,
			ChangeType_nothing = 2
		};
		static constexpr auto kChangeTypeTags = lcf::makeEnumTags<ChangeType>(
			"lose",
			"gain",
			"nothing"
		);

		int ID = 0;
		DBString name;
		int32_t type = 0;
		int32_t color = 6;
		int32_t priority = 50;
		int32_t restriction = 0;
		int32_t a_rate = 100;
		int32_t b_rate = 80;
		int32_t c_rate = 60;
		int32_t d_rate = 30;
		int32_t e_rate = 0;
		int32_t hold_turn = 0;
		int32_t auto_release_prob = 0;
		int32_t release_by_damage = 0;
		int32_t affect_type = 0;
		bool affect_attack = false;
		bool affect_defense = false;
		bool affect_spirit = false;
		bool affect_agility = false;
		int32_t reduce_hit_ratio = 100;
		bool avoid_attacks = false;
		bool reflect_magic = false;
		bool cursed = false;
		int32_t battler_animation_id = 100;
		bool restrict_skill = false;
		int32_t restrict_skill_level = 0;
		bool restrict_magic = false;
		int32_t restrict_magic_level = 0;
		int32_t hp_change_type = 0;
		int32_t sp_change_type = 0;
		DBString message_actor;
		DBString message_enemy;
		DBString message_already;
		DBString message_affected;
		DBString message_recovery;
		int32_t hp_change_max = 0;
		int32_t hp_change_val = 0;
		int32_t hp_change_map_steps = 0;
		int32_t hp_change_map_val = 0;
		int32_t sp_change_max = 0;
		int32_t sp_change_val = 0;
		int32_t sp_change_map_steps = 0;
		int32_t sp_change_map_val = 0;
		DBBitArray easyrpg_immune_states;
	};
	inline std::ostream& operator<<(std::ostream& os, State::Persistence code) {
		os << static_cast<std::underlying_type_t<decltype(code)>>(code);
		return os;
	}
	inline std::ostream& operator<<(std::ostream& os, State::Restriction code) {
		os << static_cast<std::underlying_type_t<decltype(code)>>(code);
		return os;
	}
	inline std::ostream& operator<<(std::ostream& os, State::AffectType code) {
		os << static_cast<std::underlying_type_t<decltype(code)>>(code);
		return os;
	}
	inline std::ostream& operator<<(std::ostream& os, State::ChangeType code) {
		os << static_cast<std::underlying_type_t<decltype(code)>>(code);
		return os;
	}

	inline bool operator==(const State& l, const State& r) {
		return l.name == r.name
		&& l.type == r.type
		&& l.color == r.color
		&& l.priority == r.priority
		&& l.restriction == r.restriction
		&& l.a_rate == r.a_rate
		&& l.b_rate == r.b_rate
		&& l.c_rate == r.c_rate
		&& l.d_rate == r.d_rate
		&& l.e_rate == r.e_rate
		&& l.hold_turn == r.hold_turn
		&& l.auto_release_prob == r.auto_release_prob
		&& l.release_by_damage == r.release_by_damage
		&& l.affect_type == r.affect_type
		&& l.affect_attack == r.affect_attack
		&& l.affect_defense == r.affect_defense
		&& l.affect_spirit == r.affect_spirit
		&& l.affect_agility == r.affect_agility
		&& l.reduce_hit_ratio == r.reduce_hit_ratio
		&& l.avoid_attacks == r.avoid_attacks
		&& l.reflect_magic == r.reflect_magic
		&& l.cursed == r.cursed
		&& l.battler_animation_id == r.battler_animation_id
		&& l.restrict_skill == r.restrict_skill
		&& l.restrict_skill_level == r.restrict_skill_level
		&& l.restrict_magic == r.restrict_magic
		&& l.restrict_magic_level == r.restrict_magic_level
		&& l.hp_change_type == r.hp_change_type
		&& l.sp_change_type == r.sp_change_type
		&& l.message_actor == r.message_actor
		&& l.message_enemy == r.message_enemy
		&& l.message_already == r.message_already
		&& l.message_affected == r.message_affected
		&& l.message_recovery == r.message_recovery
		&& l.hp_change_max == r.hp_change_max
		&& l.hp_change_val == r.hp_change_val
		&& l.hp_change_map_steps == r.hp_change_map_steps
		&& l.hp_change_map_val == r.hp_change_map_val
		&& l.sp_change_max == r.sp_change_max
		&& l.sp_change_val == r.sp_change_val
		&& l.sp_change_map_steps == r.sp_change_map_steps
		&& l.sp_change_map_val == r.sp_change_map_val
		&& l.easyrpg_immune_states == r.easyrpg_immune_states;
	}

	inline bool operator!=(const State& l, const State& r) {
		return !(l == r);
	}

	std::ostream& operator<<(std::ostream& os, const State& obj);

	template <typename F, typename ParentCtx = Context<void,void>>
	void ForEachString(State& obj, const F& f, const ParentCtx* parent_ctx = nullptr) {
		const auto ctx1 = Context<State, ParentCtx>{ "name", -1, &obj, parent_ctx };
		f(obj.name, ctx1);
		const auto ctx30 = Context<State, ParentCtx>{ "message_actor", -1, &obj, parent_ctx };
		f(obj.message_actor, ctx30);
		const auto ctx31 = Context<State, ParentCtx>{ "message_enemy", -1, &obj, parent_ctx };
		f(obj.message_enemy, ctx31);
		const auto ctx32 = Context<State, ParentCtx>{ "message_already", -1, &obj, parent_ctx };
		f(obj.message_already, ctx32);
		const auto ctx33 = Context<State, ParentCtx>{ "message_affected", -1, &obj, parent_ctx };
		f(obj.message_affected, ctx33);
		const auto ctx34 = Context<State, ParentCtx>{ "message_recovery", -1, &obj, parent_ctx };
		f(obj.message_recovery, ctx34);
		(void)obj;
		(void)f;
		(void)parent_ctx;
	}

} // namespace rpg
} // namespace lcf

#endif
