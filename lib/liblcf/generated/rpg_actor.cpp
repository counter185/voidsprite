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
#include "lcf/rpg/actor.h"

namespace lcf {
namespace rpg {

std::ostream& operator<<(std::ostream& os, const Actor& obj) {
	os << "Actor{";
	os << "name="<< obj.name;
	os << ", title="<< obj.title;
	os << ", character_name="<< obj.character_name;
	os << ", character_index="<< obj.character_index;
	os << ", transparent="<< obj.transparent;
	os << ", initial_level="<< obj.initial_level;
	os << ", final_level="<< obj.final_level;
	os << ", critical_hit="<< obj.critical_hit;
	os << ", critical_hit_chance="<< obj.critical_hit_chance;
	os << ", face_name="<< obj.face_name;
	os << ", face_index="<< obj.face_index;
	os << ", two_weapon="<< obj.two_weapon;
	os << ", lock_equipment="<< obj.lock_equipment;
	os << ", auto_battle="<< obj.auto_battle;
	os << ", super_guard="<< obj.super_guard;
	os << ", parameters="<< obj.parameters;
	os << ", exp_base="<< obj.exp_base;
	os << ", exp_inflation="<< obj.exp_inflation;
	os << ", exp_correction="<< obj.exp_correction;
	os << ", initial_equipment="<< obj.initial_equipment;
	os << ", unarmed_animation="<< obj.unarmed_animation;
	os << ", class_id="<< obj.class_id;
	os << ", battle_x="<< obj.battle_x;
	os << ", battle_y="<< obj.battle_y;
	os << ", battler_animation="<< obj.battler_animation;
	os << ", skills=";
	for (size_t i = 0; i < obj.skills.size(); ++i) {
		os << (i == 0 ? "[" : ", ") << obj.skills[i];
	}
	os << "]";
	os << ", rename_skill="<< obj.rename_skill;
	os << ", skill_name="<< obj.skill_name;
	os << ", state_ranks=";
	for (size_t i = 0; i < obj.state_ranks.size(); ++i) {
		os << (i == 0 ? "[" : ", ") << obj.state_ranks[i];
	}
	os << "]";
	os << ", attribute_ranks=";
	for (size_t i = 0; i < obj.attribute_ranks.size(); ++i) {
		os << (i == 0 ? "[" : ", ") << obj.attribute_ranks[i];
	}
	os << "]";
	os << ", battle_commands=";
	for (size_t i = 0; i < obj.battle_commands.size(); ++i) {
		os << (i == 0 ? "[" : ", ") << obj.battle_commands[i];
	}
	os << "]";
	os << ", easyrpg_actorai="<< obj.easyrpg_actorai;
	os << ", easyrpg_prevent_critical="<< obj.easyrpg_prevent_critical;
	os << ", easyrpg_raise_evasion="<< obj.easyrpg_raise_evasion;
	os << ", easyrpg_immune_to_attribute_downshifts="<< obj.easyrpg_immune_to_attribute_downshifts;
	os << ", easyrpg_ignore_evasion="<< obj.easyrpg_ignore_evasion;
	os << ", easyrpg_unarmed_hit="<< obj.easyrpg_unarmed_hit;
	os << ", easyrpg_unarmed_state_set=";
	for (size_t i = 0; i < obj.easyrpg_unarmed_state_set.size(); ++i) {
		os << (i == 0 ? "[" : ", ") << obj.easyrpg_unarmed_state_set[i];
	}
	os << "]";
	os << ", easyrpg_unarmed_state_chance="<< obj.easyrpg_unarmed_state_chance;
	os << ", easyrpg_unarmed_attribute_set=";
	for (size_t i = 0; i < obj.easyrpg_unarmed_attribute_set.size(); ++i) {
		os << (i == 0 ? "[" : ", ") << obj.easyrpg_unarmed_attribute_set[i];
	}
	os << "]";
	os << ", easyrpg_dual_attack="<< obj.easyrpg_dual_attack;
	os << ", easyrpg_attack_all="<< obj.easyrpg_attack_all;
	os << "}";
	return os;
}

} // namespace rpg
} // namespace lcf
