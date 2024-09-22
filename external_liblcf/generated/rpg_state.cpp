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

// Headers
#include "lcf/rpg/state.h"

constexpr int lcf::rpg::State::kDeathID;
namespace lcf {
namespace rpg {

std::ostream& operator<<(std::ostream& os, const State& obj) {
	os << "State{";
	os << "name="<< obj.name;
	os << ", type="<< obj.type;
	os << ", color="<< obj.color;
	os << ", priority="<< obj.priority;
	os << ", restriction="<< obj.restriction;
	os << ", a_rate="<< obj.a_rate;
	os << ", b_rate="<< obj.b_rate;
	os << ", c_rate="<< obj.c_rate;
	os << ", d_rate="<< obj.d_rate;
	os << ", e_rate="<< obj.e_rate;
	os << ", hold_turn="<< obj.hold_turn;
	os << ", auto_release_prob="<< obj.auto_release_prob;
	os << ", release_by_damage="<< obj.release_by_damage;
	os << ", affect_type="<< obj.affect_type;
	os << ", affect_attack="<< obj.affect_attack;
	os << ", affect_defense="<< obj.affect_defense;
	os << ", affect_spirit="<< obj.affect_spirit;
	os << ", affect_agility="<< obj.affect_agility;
	os << ", reduce_hit_ratio="<< obj.reduce_hit_ratio;
	os << ", avoid_attacks="<< obj.avoid_attacks;
	os << ", reflect_magic="<< obj.reflect_magic;
	os << ", cursed="<< obj.cursed;
	os << ", battler_animation_id="<< obj.battler_animation_id;
	os << ", restrict_skill="<< obj.restrict_skill;
	os << ", restrict_skill_level="<< obj.restrict_skill_level;
	os << ", restrict_magic="<< obj.restrict_magic;
	os << ", restrict_magic_level="<< obj.restrict_magic_level;
	os << ", hp_change_type="<< obj.hp_change_type;
	os << ", sp_change_type="<< obj.sp_change_type;
	os << ", message_actor="<< obj.message_actor;
	os << ", message_enemy="<< obj.message_enemy;
	os << ", message_already="<< obj.message_already;
	os << ", message_affected="<< obj.message_affected;
	os << ", message_recovery="<< obj.message_recovery;
	os << ", hp_change_max="<< obj.hp_change_max;
	os << ", hp_change_val="<< obj.hp_change_val;
	os << ", hp_change_map_steps="<< obj.hp_change_map_steps;
	os << ", hp_change_map_val="<< obj.hp_change_map_val;
	os << ", sp_change_max="<< obj.sp_change_max;
	os << ", sp_change_val="<< obj.sp_change_val;
	os << ", sp_change_map_steps="<< obj.sp_change_map_steps;
	os << ", sp_change_map_val="<< obj.sp_change_map_val;
	os << ", easyrpg_immune_states=";
	for (size_t i = 0; i < obj.easyrpg_immune_states.size(); ++i) {
		os << (i == 0 ? "[" : ", ") << obj.easyrpg_immune_states[i];
	}
	os << "]";
	os << "}";
	return os;
}

} // namespace rpg
} // namespace lcf
