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
#include "lcf/rpg/saveinventory.h"

namespace lcf {
namespace rpg {

std::ostream& operator<<(std::ostream& os, const SaveInventory& obj) {
	os << "SaveInventory{";
	os << "party=";
	for (size_t i = 0; i < obj.party.size(); ++i) {
		os << (i == 0 ? "[" : ", ") << obj.party[i];
	}
	os << "]";
	os << ", item_ids=";
	for (size_t i = 0; i < obj.item_ids.size(); ++i) {
		os << (i == 0 ? "[" : ", ") << obj.item_ids[i];
	}
	os << "]";
	os << ", item_counts=";
	for (size_t i = 0; i < obj.item_counts.size(); ++i) {
		os << (i == 0 ? "[" : ", ") << obj.item_counts[i];
	}
	os << "]";
	os << ", item_usage=";
	for (size_t i = 0; i < obj.item_usage.size(); ++i) {
		os << (i == 0 ? "[" : ", ") << obj.item_usage[i];
	}
	os << "]";
	os << ", gold="<< obj.gold;
	os << ", timer1_frames="<< obj.timer1_frames;
	os << ", timer1_active="<< obj.timer1_active;
	os << ", timer1_visible="<< obj.timer1_visible;
	os << ", timer1_battle="<< obj.timer1_battle;
	os << ", timer2_frames="<< obj.timer2_frames;
	os << ", timer2_active="<< obj.timer2_active;
	os << ", timer2_visible="<< obj.timer2_visible;
	os << ", timer2_battle="<< obj.timer2_battle;
	os << ", battles="<< obj.battles;
	os << ", defeats="<< obj.defeats;
	os << ", escapes="<< obj.escapes;
	os << ", victories="<< obj.victories;
	os << ", turns="<< obj.turns;
	os << ", steps="<< obj.steps;
	os << "}";
	return os;
}

} // namespace rpg
} // namespace lcf
