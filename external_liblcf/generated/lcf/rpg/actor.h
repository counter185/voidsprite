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

#ifndef LCF_RPG_ACTOR_H
#define LCF_RPG_ACTOR_H

// Headers
#include <stdint.h>
#include <vector>
#include "lcf/dbbitarray.h"
#include "lcf/dbstring.h"
#include "lcf/rpg/equipment.h"
#include "lcf/rpg/learning.h"
#include "lcf/rpg/parameters.h"
#include "lcf/context.h"
#include <ostream>
#include <type_traits>

/**
 * rpg::Actor class.
 */
namespace lcf {
namespace rpg {
	class Actor {
	public:
		void Setup(bool is2k3);
		int ID = 0;
		DBString name;
		DBString title;
		DBString character_name;
		int32_t character_index = 0;
		bool transparent = false;
		int32_t initial_level = 1;
		int32_t final_level = -1;
		bool critical_hit = true;
		int32_t critical_hit_chance = 30;
		DBString face_name;
		int32_t face_index = 0;
		bool two_weapon = false;
		bool lock_equipment = false;
		bool auto_battle = false;
		bool super_guard = false;
		Parameters parameters;
		int32_t exp_base = -1;
		int32_t exp_inflation = -1;
		int32_t exp_correction = 0;
		Equipment initial_equipment;
		int32_t unarmed_animation = 1;
		int32_t class_id = 0;
		int32_t battle_x = 220;
		int32_t battle_y = 120;
		int32_t battler_animation = 1;
		std::vector<Learning> skills;
		bool rename_skill = false;
		DBString skill_name;
		std::vector<uint8_t> state_ranks;
		std::vector<uint8_t> attribute_ranks;
		std::vector<int32_t> battle_commands;
		int32_t easyrpg_actorai = -1;
		bool easyrpg_prevent_critical = false;
		bool easyrpg_raise_evasion = false;
		bool easyrpg_immune_to_attribute_downshifts = false;
		bool easyrpg_ignore_evasion = false;
		int32_t easyrpg_unarmed_hit = -1;
		DBBitArray easyrpg_unarmed_state_set;
		int32_t easyrpg_unarmed_state_chance = 0;
		DBBitArray easyrpg_unarmed_attribute_set;
		bool easyrpg_dual_attack = false;
		bool easyrpg_attack_all = false;
	};

	inline bool operator==(const Actor& l, const Actor& r) {
		return l.name == r.name
		&& l.title == r.title
		&& l.character_name == r.character_name
		&& l.character_index == r.character_index
		&& l.transparent == r.transparent
		&& l.initial_level == r.initial_level
		&& l.final_level == r.final_level
		&& l.critical_hit == r.critical_hit
		&& l.critical_hit_chance == r.critical_hit_chance
		&& l.face_name == r.face_name
		&& l.face_index == r.face_index
		&& l.two_weapon == r.two_weapon
		&& l.lock_equipment == r.lock_equipment
		&& l.auto_battle == r.auto_battle
		&& l.super_guard == r.super_guard
		&& l.parameters == r.parameters
		&& l.exp_base == r.exp_base
		&& l.exp_inflation == r.exp_inflation
		&& l.exp_correction == r.exp_correction
		&& l.initial_equipment == r.initial_equipment
		&& l.unarmed_animation == r.unarmed_animation
		&& l.class_id == r.class_id
		&& l.battle_x == r.battle_x
		&& l.battle_y == r.battle_y
		&& l.battler_animation == r.battler_animation
		&& l.skills == r.skills
		&& l.rename_skill == r.rename_skill
		&& l.skill_name == r.skill_name
		&& l.state_ranks == r.state_ranks
		&& l.attribute_ranks == r.attribute_ranks
		&& l.battle_commands == r.battle_commands
		&& l.easyrpg_actorai == r.easyrpg_actorai
		&& l.easyrpg_prevent_critical == r.easyrpg_prevent_critical
		&& l.easyrpg_raise_evasion == r.easyrpg_raise_evasion
		&& l.easyrpg_immune_to_attribute_downshifts == r.easyrpg_immune_to_attribute_downshifts
		&& l.easyrpg_ignore_evasion == r.easyrpg_ignore_evasion
		&& l.easyrpg_unarmed_hit == r.easyrpg_unarmed_hit
		&& l.easyrpg_unarmed_state_set == r.easyrpg_unarmed_state_set
		&& l.easyrpg_unarmed_state_chance == r.easyrpg_unarmed_state_chance
		&& l.easyrpg_unarmed_attribute_set == r.easyrpg_unarmed_attribute_set
		&& l.easyrpg_dual_attack == r.easyrpg_dual_attack
		&& l.easyrpg_attack_all == r.easyrpg_attack_all;
	}

	inline bool operator!=(const Actor& l, const Actor& r) {
		return !(l == r);
	}

	std::ostream& operator<<(std::ostream& os, const Actor& obj);

	template <typename F, typename ParentCtx = Context<void,void>>
	void ForEachString(Actor& obj, const F& f, const ParentCtx* parent_ctx = nullptr) {
		const auto ctx1 = Context<Actor, ParentCtx>{ "name", -1, &obj, parent_ctx };
		f(obj.name, ctx1);
		const auto ctx2 = Context<Actor, ParentCtx>{ "title", -1, &obj, parent_ctx };
		f(obj.title, ctx2);
		const auto ctx3 = Context<Actor, ParentCtx>{ "character_name", -1, &obj, parent_ctx };
		f(obj.character_name, ctx3);
		const auto ctx10 = Context<Actor, ParentCtx>{ "face_name", -1, &obj, parent_ctx };
		f(obj.face_name, ctx10);
		const auto ctx16 = Context<Actor, ParentCtx>{ "parameters", -1, &obj, parent_ctx };
		ForEachString(obj.parameters, f, &ctx16);
		const auto ctx20 = Context<Actor, ParentCtx>{ "initial_equipment", -1, &obj, parent_ctx };
		ForEachString(obj.initial_equipment, f, &ctx20);
		for (int i = 0; i < static_cast<int>(obj.skills.size()); ++i) {
			const auto ctx26 = Context<Actor, ParentCtx>{ "skills", i, &obj, parent_ctx };
			ForEachString(obj.skills[i], f, &ctx26);
		}
		const auto ctx28 = Context<Actor, ParentCtx>{ "skill_name", -1, &obj, parent_ctx };
		f(obj.skill_name, ctx28);
		(void)obj;
		(void)f;
		(void)parent_ctx;
	}

} // namespace rpg
} // namespace lcf

#endif
