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
#include "lcf/rpg/enemy.h"

namespace lcf {
namespace rpg {

std::ostream& operator<<(std::ostream& os, const Enemy& obj) {
	os << "Enemy{";
	os << "name="<< obj.name;
	os << ", battler_name="<< obj.battler_name;
	os << ", battler_hue="<< obj.battler_hue;
	os << ", max_hp="<< obj.max_hp;
	os << ", max_sp="<< obj.max_sp;
	os << ", attack="<< obj.attack;
	os << ", defense="<< obj.defense;
	os << ", spirit="<< obj.spirit;
	os << ", agility="<< obj.agility;
	os << ", transparent="<< obj.transparent;
	os << ", exp="<< obj.exp;
	os << ", gold="<< obj.gold;
	os << ", drop_id="<< obj.drop_id;
	os << ", drop_prob="<< obj.drop_prob;
	os << ", critical_hit="<< obj.critical_hit;
	os << ", critical_hit_chance="<< obj.critical_hit_chance;
	os << ", miss="<< obj.miss;
	os << ", levitate="<< obj.levitate;
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
	os << ", actions=";
	for (size_t i = 0; i < obj.actions.size(); ++i) {
		os << (i == 0 ? "[" : ", ") << obj.actions[i];
	}
	os << "]";
	os << ", maniac_unarmed_animation="<< obj.maniac_unarmed_animation;
	os << ", easyrpg_enemyai="<< obj.easyrpg_enemyai;
	os << ", easyrpg_prevent_critical="<< obj.easyrpg_prevent_critical;
	os << ", easyrpg_raise_evasion="<< obj.easyrpg_raise_evasion;
	os << ", easyrpg_immune_to_attribute_downshifts="<< obj.easyrpg_immune_to_attribute_downshifts;
	os << ", easyrpg_ignore_evasion="<< obj.easyrpg_ignore_evasion;
	os << ", easyrpg_hit="<< obj.easyrpg_hit;
	os << ", easyrpg_state_set=";
	for (size_t i = 0; i < obj.easyrpg_state_set.size(); ++i) {
		os << (i == 0 ? "[" : ", ") << obj.easyrpg_state_set[i];
	}
	os << "]";
	os << ", easyrpg_state_chance="<< obj.easyrpg_state_chance;
	os << ", easyrpg_attribute_set=";
	for (size_t i = 0; i < obj.easyrpg_attribute_set.size(); ++i) {
		os << (i == 0 ? "[" : ", ") << obj.easyrpg_attribute_set[i];
	}
	os << "]";
	os << ", easyrpg_super_guard="<< obj.easyrpg_super_guard;
	os << ", easyrpg_attack_all="<< obj.easyrpg_attack_all;
	os << "}";
	return os;
}

} // namespace rpg
} // namespace lcf
