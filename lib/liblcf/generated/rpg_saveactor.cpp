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
#include "lcf/rpg/saveactor.h"

constexpr const char* lcf::rpg::SaveActor::kEmptyName;
namespace lcf {
namespace rpg {

std::ostream& operator<<(std::ostream& os, const SaveActor& obj) {
	os << "SaveActor{";
	os << "name="<< obj.name;
	os << ", title="<< obj.title;
	os << ", sprite_name="<< obj.sprite_name;
	os << ", sprite_id="<< obj.sprite_id;
	os << ", transparency="<< obj.transparency;
	os << ", face_name="<< obj.face_name;
	os << ", face_id="<< obj.face_id;
	os << ", level="<< obj.level;
	os << ", exp="<< obj.exp;
	os << ", hp_mod="<< obj.hp_mod;
	os << ", sp_mod="<< obj.sp_mod;
	os << ", attack_mod="<< obj.attack_mod;
	os << ", defense_mod="<< obj.defense_mod;
	os << ", spirit_mod="<< obj.spirit_mod;
	os << ", agility_mod="<< obj.agility_mod;
	os << ", skills=";
	for (size_t i = 0; i < obj.skills.size(); ++i) {
		os << (i == 0 ? "[" : ", ") << obj.skills[i];
	}
	os << "]";
	os << ", equipped=";
	for (size_t i = 0; i < obj.equipped.size(); ++i) {
		os << (i == 0 ? "[" : ", ") << obj.equipped[i];
	}
	os << "]";
	os << ", current_hp="<< obj.current_hp;
	os << ", current_sp="<< obj.current_sp;
	os << ", battle_commands=";
	for (size_t i = 0; i < obj.battle_commands.size(); ++i) {
		os << (i == 0 ? "[" : ", ") << obj.battle_commands[i];
	}
	os << "]";
	os << ", status=";
	for (size_t i = 0; i < obj.status.size(); ++i) {
		os << (i == 0 ? "[" : ", ") << obj.status[i];
	}
	os << "]";
	os << ", changed_battle_commands="<< obj.changed_battle_commands;
	os << ", class_id="<< obj.class_id;
	os << ", row="<< obj.row;
	os << ", two_weapon="<< obj.two_weapon;
	os << ", lock_equipment="<< obj.lock_equipment;
	os << ", auto_battle="<< obj.auto_battle;
	os << ", super_guard="<< obj.super_guard;
	os << ", battler_animation="<< obj.battler_animation;
	os << "}";
	return os;
}

} // namespace rpg
} // namespace lcf
