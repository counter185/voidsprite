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
#include "lcf/rpg/item.h"

constexpr const char* lcf::rpg::Item::kDefaultMessage;
namespace lcf {
namespace rpg {

std::ostream& operator<<(std::ostream& os, const Item& obj) {
	os << "Item{";
	os << "name="<< obj.name;
	os << ", description="<< obj.description;
	os << ", type="<< obj.type;
	os << ", price="<< obj.price;
	os << ", uses="<< obj.uses;
	os << ", atk_points1="<< obj.atk_points1;
	os << ", def_points1="<< obj.def_points1;
	os << ", spi_points1="<< obj.spi_points1;
	os << ", agi_points1="<< obj.agi_points1;
	os << ", two_handed="<< obj.two_handed;
	os << ", sp_cost="<< obj.sp_cost;
	os << ", hit="<< obj.hit;
	os << ", critical_hit="<< obj.critical_hit;
	os << ", animation_id="<< obj.animation_id;
	os << ", preemptive="<< obj.preemptive;
	os << ", dual_attack="<< obj.dual_attack;
	os << ", attack_all="<< obj.attack_all;
	os << ", ignore_evasion="<< obj.ignore_evasion;
	os << ", prevent_critical="<< obj.prevent_critical;
	os << ", raise_evasion="<< obj.raise_evasion;
	os << ", half_sp_cost="<< obj.half_sp_cost;
	os << ", no_terrain_damage="<< obj.no_terrain_damage;
	os << ", cursed="<< obj.cursed;
	os << ", entire_party="<< obj.entire_party;
	os << ", recover_hp_rate="<< obj.recover_hp_rate;
	os << ", recover_hp="<< obj.recover_hp;
	os << ", recover_sp_rate="<< obj.recover_sp_rate;
	os << ", recover_sp="<< obj.recover_sp;
	os << ", occasion_field1="<< obj.occasion_field1;
	os << ", ko_only="<< obj.ko_only;
	os << ", max_hp_points="<< obj.max_hp_points;
	os << ", max_sp_points="<< obj.max_sp_points;
	os << ", atk_points2="<< obj.atk_points2;
	os << ", def_points2="<< obj.def_points2;
	os << ", spi_points2="<< obj.spi_points2;
	os << ", agi_points2="<< obj.agi_points2;
	os << ", using_message="<< obj.using_message;
	os << ", skill_id="<< obj.skill_id;
	os << ", switch_id="<< obj.switch_id;
	os << ", occasion_field2="<< obj.occasion_field2;
	os << ", occasion_battle="<< obj.occasion_battle;
	os << ", actor_set=";
	for (size_t i = 0; i < obj.actor_set.size(); ++i) {
		os << (i == 0 ? "[" : ", ") << obj.actor_set[i];
	}
	os << "]";
	os << ", state_set=";
	for (size_t i = 0; i < obj.state_set.size(); ++i) {
		os << (i == 0 ? "[" : ", ") << obj.state_set[i];
	}
	os << "]";
	os << ", attribute_set=";
	for (size_t i = 0; i < obj.attribute_set.size(); ++i) {
		os << (i == 0 ? "[" : ", ") << obj.attribute_set[i];
	}
	os << "]";
	os << ", state_chance="<< obj.state_chance;
	os << ", reverse_state_effect="<< obj.reverse_state_effect;
	os << ", weapon_animation="<< obj.weapon_animation;
	os << ", animation_data=";
	for (size_t i = 0; i < obj.animation_data.size(); ++i) {
		os << (i == 0 ? "[" : ", ") << obj.animation_data[i];
	}
	os << "]";
	os << ", use_skill="<< obj.use_skill;
	os << ", class_set=";
	for (size_t i = 0; i < obj.class_set.size(); ++i) {
		os << (i == 0 ? "[" : ", ") << obj.class_set[i];
	}
	os << "]";
	os << ", ranged_trajectory="<< obj.ranged_trajectory;
	os << ", ranged_target="<< obj.ranged_target;
	os << ", easyrpg_using_message="<< obj.easyrpg_using_message;
	os << ", easyrpg_max_count="<< obj.easyrpg_max_count;
	os << "}";
	return os;
}

} // namespace rpg
} // namespace lcf
