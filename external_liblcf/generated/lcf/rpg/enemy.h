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

#ifndef LCF_RPG_ENEMY_H
#define LCF_RPG_ENEMY_H

// Headers
#include <stdint.h>
#include <vector>
#include "lcf/dbbitarray.h"
#include "lcf/dbstring.h"
#include "lcf/rpg/enemyaction.h"
#include "lcf/context.h"
#include <ostream>
#include <type_traits>

/**
 * rpg::Enemy class.
 */
namespace lcf {
namespace rpg {
	class Enemy {
	public:
		int ID = 0;
		DBString name;
		DBString battler_name;
		int32_t battler_hue = 0;
		int32_t max_hp = 10;
		int32_t max_sp = 10;
		int32_t attack = 10;
		int32_t defense = 10;
		int32_t spirit = 10;
		int32_t agility = 10;
		bool transparent = false;
		int32_t exp = 0;
		int32_t gold = 0;
		int32_t drop_id = 0;
		int32_t drop_prob = 100;
		bool critical_hit = false;
		int32_t critical_hit_chance = 30;
		bool miss = false;
		bool levitate = false;
		std::vector<uint8_t> state_ranks;
		std::vector<uint8_t> attribute_ranks;
		std::vector<EnemyAction> actions;
		int32_t maniac_unarmed_animation = 1;
		int32_t easyrpg_enemyai = -1;
		bool easyrpg_prevent_critical = false;
		bool easyrpg_raise_evasion = false;
		bool easyrpg_immune_to_attribute_downshifts = false;
		bool easyrpg_ignore_evasion = false;
		int32_t easyrpg_hit = -1;
		DBBitArray easyrpg_state_set;
		int32_t easyrpg_state_chance = 0;
		DBBitArray easyrpg_attribute_set;
		bool easyrpg_super_guard = false;
		bool easyrpg_attack_all = false;
	};

	inline bool operator==(const Enemy& l, const Enemy& r) {
		return l.name == r.name
		&& l.battler_name == r.battler_name
		&& l.battler_hue == r.battler_hue
		&& l.max_hp == r.max_hp
		&& l.max_sp == r.max_sp
		&& l.attack == r.attack
		&& l.defense == r.defense
		&& l.spirit == r.spirit
		&& l.agility == r.agility
		&& l.transparent == r.transparent
		&& l.exp == r.exp
		&& l.gold == r.gold
		&& l.drop_id == r.drop_id
		&& l.drop_prob == r.drop_prob
		&& l.critical_hit == r.critical_hit
		&& l.critical_hit_chance == r.critical_hit_chance
		&& l.miss == r.miss
		&& l.levitate == r.levitate
		&& l.state_ranks == r.state_ranks
		&& l.attribute_ranks == r.attribute_ranks
		&& l.actions == r.actions
		&& l.maniac_unarmed_animation == r.maniac_unarmed_animation
		&& l.easyrpg_enemyai == r.easyrpg_enemyai
		&& l.easyrpg_prevent_critical == r.easyrpg_prevent_critical
		&& l.easyrpg_raise_evasion == r.easyrpg_raise_evasion
		&& l.easyrpg_immune_to_attribute_downshifts == r.easyrpg_immune_to_attribute_downshifts
		&& l.easyrpg_ignore_evasion == r.easyrpg_ignore_evasion
		&& l.easyrpg_hit == r.easyrpg_hit
		&& l.easyrpg_state_set == r.easyrpg_state_set
		&& l.easyrpg_state_chance == r.easyrpg_state_chance
		&& l.easyrpg_attribute_set == r.easyrpg_attribute_set
		&& l.easyrpg_super_guard == r.easyrpg_super_guard
		&& l.easyrpg_attack_all == r.easyrpg_attack_all;
	}

	inline bool operator!=(const Enemy& l, const Enemy& r) {
		return !(l == r);
	}

	std::ostream& operator<<(std::ostream& os, const Enemy& obj);

	template <typename F, typename ParentCtx = Context<void,void>>
	void ForEachString(Enemy& obj, const F& f, const ParentCtx* parent_ctx = nullptr) {
		const auto ctx1 = Context<Enemy, ParentCtx>{ "name", -1, &obj, parent_ctx };
		f(obj.name, ctx1);
		const auto ctx2 = Context<Enemy, ParentCtx>{ "battler_name", -1, &obj, parent_ctx };
		f(obj.battler_name, ctx2);
		for (int i = 0; i < static_cast<int>(obj.actions.size()); ++i) {
			const auto ctx21 = Context<Enemy, ParentCtx>{ "actions", i, &obj, parent_ctx };
			ForEachString(obj.actions[i], f, &ctx21);
		}
		(void)obj;
		(void)f;
		(void)parent_ctx;
	}

} // namespace rpg
} // namespace lcf

#endif
