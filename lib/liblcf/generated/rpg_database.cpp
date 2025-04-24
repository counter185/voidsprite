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
#include "lcf/rpg/database.h"

namespace lcf {
namespace rpg {

std::ostream& operator<<(std::ostream& os, const Database& obj) {
	os << "Database{";
	os << "actors=";
	for (size_t i = 0; i < obj.actors.size(); ++i) {
		os << (i == 0 ? "[" : ", ") << obj.actors[i];
	}
	os << "]";
	os << ", skills=";
	for (size_t i = 0; i < obj.skills.size(); ++i) {
		os << (i == 0 ? "[" : ", ") << obj.skills[i];
	}
	os << "]";
	os << ", items=";
	for (size_t i = 0; i < obj.items.size(); ++i) {
		os << (i == 0 ? "[" : ", ") << obj.items[i];
	}
	os << "]";
	os << ", enemies=";
	for (size_t i = 0; i < obj.enemies.size(); ++i) {
		os << (i == 0 ? "[" : ", ") << obj.enemies[i];
	}
	os << "]";
	os << ", troops=";
	for (size_t i = 0; i < obj.troops.size(); ++i) {
		os << (i == 0 ? "[" : ", ") << obj.troops[i];
	}
	os << "]";
	os << ", terrains=";
	for (size_t i = 0; i < obj.terrains.size(); ++i) {
		os << (i == 0 ? "[" : ", ") << obj.terrains[i];
	}
	os << "]";
	os << ", attributes=";
	for (size_t i = 0; i < obj.attributes.size(); ++i) {
		os << (i == 0 ? "[" : ", ") << obj.attributes[i];
	}
	os << "]";
	os << ", states=";
	for (size_t i = 0; i < obj.states.size(); ++i) {
		os << (i == 0 ? "[" : ", ") << obj.states[i];
	}
	os << "]";
	os << ", animations=";
	for (size_t i = 0; i < obj.animations.size(); ++i) {
		os << (i == 0 ? "[" : ", ") << obj.animations[i];
	}
	os << "]";
	os << ", chipsets=";
	for (size_t i = 0; i < obj.chipsets.size(); ++i) {
		os << (i == 0 ? "[" : ", ") << obj.chipsets[i];
	}
	os << "]";
	os << ", terms="<< obj.terms;
	os << ", system="<< obj.system;
	os << ", switches=";
	for (size_t i = 0; i < obj.switches.size(); ++i) {
		os << (i == 0 ? "[" : ", ") << obj.switches[i];
	}
	os << "]";
	os << ", variables=";
	for (size_t i = 0; i < obj.variables.size(); ++i) {
		os << (i == 0 ? "[" : ", ") << obj.variables[i];
	}
	os << "]";
	os << ", commonevents=";
	for (size_t i = 0; i < obj.commonevents.size(); ++i) {
		os << (i == 0 ? "[" : ", ") << obj.commonevents[i];
	}
	os << "]";
	os << ", version="<< obj.version;
	os << ", battlecommands="<< obj.battlecommands;
	os << ", classes=";
	for (size_t i = 0; i < obj.classes.size(); ++i) {
		os << (i == 0 ? "[" : ", ") << obj.classes[i];
	}
	os << "]";
	os << ", battleranimations=";
	for (size_t i = 0; i < obj.battleranimations.size(); ++i) {
		os << (i == 0 ? "[" : ", ") << obj.battleranimations[i];
	}
	os << "]";
	os << "}";
	return os;
}

} // namespace rpg
} // namespace lcf
