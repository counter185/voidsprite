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
#include "lcf/rpg/battlecommands.h"

namespace lcf {
namespace rpg {

std::ostream& operator<<(std::ostream& os, const BattleCommands& obj) {
	os << "BattleCommands{";
	os << "placement="<< obj.placement;
	os << ", death_handler_unused="<< obj.death_handler_unused;
	os << ", row="<< obj.row;
	os << ", battle_type="<< obj.battle_type;
	os << ", unused_display_normal_parameters="<< obj.unused_display_normal_parameters;
	os << ", commands=";
	for (size_t i = 0; i < obj.commands.size(); ++i) {
		os << (i == 0 ? "[" : ", ") << obj.commands[i];
	}
	os << "]";
	os << ", death_handler="<< obj.death_handler;
	os << ", death_event="<< obj.death_event;
	os << ", window_size="<< obj.window_size;
	os << ", transparency="<< obj.transparency;
	os << ", death_teleport="<< obj.death_teleport;
	os << ", death_teleport_id="<< obj.death_teleport_id;
	os << ", death_teleport_x="<< obj.death_teleport_x;
	os << ", death_teleport_y="<< obj.death_teleport_y;
	os << ", death_teleport_face="<< obj.death_teleport_face;
	os << ", easyrpg_default_atb_mode="<< obj.easyrpg_default_atb_mode;
	os << ", easyrpg_enable_battle_row_command="<< obj.easyrpg_enable_battle_row_command;
	os << ", easyrpg_sequential_order="<< obj.easyrpg_sequential_order;
	os << ", easyrpg_disable_row_feature="<< obj.easyrpg_disable_row_feature;
	os << ", easyrpg_fixed_actor_facing_direction="<< obj.easyrpg_fixed_actor_facing_direction;
	os << ", easyrpg_fixed_enemy_facing_direction="<< obj.easyrpg_fixed_enemy_facing_direction;
	os << "}";
	return os;
}

} // namespace rpg
} // namespace lcf
