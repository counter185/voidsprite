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
#include "lcf/rpg/eventpage.h"

namespace lcf {
namespace rpg {

std::ostream& operator<<(std::ostream& os, const EventPage& obj) {
	os << "EventPage{";
	os << "condition="<< obj.condition;
	os << ", character_name="<< obj.character_name;
	os << ", character_index="<< obj.character_index;
	os << ", character_direction="<< obj.character_direction;
	os << ", character_pattern="<< obj.character_pattern;
	os << ", translucent="<< obj.translucent;
	os << ", move_type="<< obj.move_type;
	os << ", move_frequency="<< obj.move_frequency;
	os << ", trigger="<< obj.trigger;
	os << ", layer="<< obj.layer;
	os << ", overlap_forbidden="<< obj.overlap_forbidden;
	os << ", animation_type="<< obj.animation_type;
	os << ", move_speed="<< obj.move_speed;
	os << ", move_route="<< obj.move_route;
	os << ", event_commands=";
	for (size_t i = 0; i < obj.event_commands.size(); ++i) {
		os << (i == 0 ? "[" : ", ") << obj.event_commands[i];
	}
	os << "]";
	os << "}";
	return os;
}

} // namespace rpg
} // namespace lcf
