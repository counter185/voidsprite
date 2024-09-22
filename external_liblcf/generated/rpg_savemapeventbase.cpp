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
#include "lcf/rpg/savemapeventbase.h"

namespace lcf {
namespace rpg {

std::ostream& operator<<(std::ostream& os, const SaveMapEventBase& obj) {
	os << "SaveMapEventBase{";
	os << "active="<< obj.active;
	os << ", map_id="<< obj.map_id;
	os << ", position_x="<< obj.position_x;
	os << ", position_y="<< obj.position_y;
	os << ", direction="<< obj.direction;
	os << ", facing="<< obj.facing;
	os << ", anim_frame="<< obj.anim_frame;
	os << ", transparency="<< obj.transparency;
	os << ", remaining_step="<< obj.remaining_step;
	os << ", move_frequency="<< obj.move_frequency;
	os << ", layer="<< obj.layer;
	os << ", overlap_forbidden="<< obj.overlap_forbidden;
	os << ", animation_type="<< obj.animation_type;
	os << ", lock_facing="<< obj.lock_facing;
	os << ", move_speed="<< obj.move_speed;
	os << ", move_route="<< obj.move_route;
	os << ", move_route_overwrite="<< obj.move_route_overwrite;
	os << ", move_route_index="<< obj.move_route_index;
	os << ", move_route_finished="<< obj.move_route_finished;
	os << ", sprite_hidden="<< obj.sprite_hidden;
	os << ", move_route_through="<< obj.move_route_through;
	os << ", anim_paused="<< obj.anim_paused;
	os << ", through="<< obj.through;
	os << ", stop_count="<< obj.stop_count;
	os << ", anim_count="<< obj.anim_count;
	os << ", max_stop_count="<< obj.max_stop_count;
	os << ", jumping="<< obj.jumping;
	os << ", begin_jump_x="<< obj.begin_jump_x;
	os << ", begin_jump_y="<< obj.begin_jump_y;
	os << ", pause="<< obj.pause;
	os << ", flying="<< obj.flying;
	os << ", sprite_name="<< obj.sprite_name;
	os << ", sprite_id="<< obj.sprite_id;
	os << ", processed="<< obj.processed;
	os << ", flash_red="<< obj.flash_red;
	os << ", flash_green="<< obj.flash_green;
	os << ", flash_blue="<< obj.flash_blue;
	os << ", flash_current_level="<< obj.flash_current_level;
	os << ", flash_time_left="<< obj.flash_time_left;
	os << ", easyrpg_move_failure_count="<< obj.easyrpg_move_failure_count;
	os << "}";
	return os;
}

} // namespace rpg
} // namespace lcf
