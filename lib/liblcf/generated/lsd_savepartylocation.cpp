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
#include "lcf/lsd/reader.h"
#include "lcf/lsd/chunks.h"
#include "reader_struct_impl.h"

namespace lcf {

// Read SavePartyLocation.

template <>
char const* const Struct<rpg::SavePartyLocation>::name = "SavePartyLocation";
static TypedField<rpg::SavePartyLocation, bool> static_active(
	&rpg::SavePartyLocation::active,
	LSD_Reader::ChunkSavePartyLocation::active,
	"active",
	0,
	0
);
static TypedField<rpg::SavePartyLocation, int32_t> static_map_id(
	&rpg::SavePartyLocation::map_id,
	LSD_Reader::ChunkSavePartyLocation::map_id,
	"map_id",
	1,
	0
);
static TypedField<rpg::SavePartyLocation, int32_t> static_position_x(
	&rpg::SavePartyLocation::position_x,
	LSD_Reader::ChunkSavePartyLocation::position_x,
	"position_x",
	1,
	0
);
static TypedField<rpg::SavePartyLocation, int32_t> static_position_y(
	&rpg::SavePartyLocation::position_y,
	LSD_Reader::ChunkSavePartyLocation::position_y,
	"position_y",
	1,
	0
);
static TypedField<rpg::SavePartyLocation, int32_t> static_direction(
	&rpg::SavePartyLocation::direction,
	LSD_Reader::ChunkSavePartyLocation::direction,
	"direction",
	1,
	0
);
static TypedField<rpg::SavePartyLocation, int32_t> static_facing(
	&rpg::SavePartyLocation::facing,
	LSD_Reader::ChunkSavePartyLocation::facing,
	"facing",
	1,
	0
);
static TypedField<rpg::SavePartyLocation, int32_t> static_anim_frame(
	&rpg::SavePartyLocation::anim_frame,
	LSD_Reader::ChunkSavePartyLocation::anim_frame,
	"anim_frame",
	0,
	0
);
static TypedField<rpg::SavePartyLocation, int32_t> static_transparency(
	&rpg::SavePartyLocation::transparency,
	LSD_Reader::ChunkSavePartyLocation::transparency,
	"transparency",
	0,
	0
);
static TypedField<rpg::SavePartyLocation, int32_t> static_remaining_step(
	&rpg::SavePartyLocation::remaining_step,
	LSD_Reader::ChunkSavePartyLocation::remaining_step,
	"remaining_step",
	0,
	0
);
static TypedField<rpg::SavePartyLocation, int32_t> static_move_frequency(
	&rpg::SavePartyLocation::move_frequency,
	LSD_Reader::ChunkSavePartyLocation::move_frequency,
	"move_frequency",
	0,
	0
);
static TypedField<rpg::SavePartyLocation, int32_t> static_layer(
	&rpg::SavePartyLocation::layer,
	LSD_Reader::ChunkSavePartyLocation::layer,
	"layer",
	1,
	0
);
static TypedField<rpg::SavePartyLocation, bool> static_overlap_forbidden(
	&rpg::SavePartyLocation::overlap_forbidden,
	LSD_Reader::ChunkSavePartyLocation::overlap_forbidden,
	"overlap_forbidden",
	0,
	0
);
static TypedField<rpg::SavePartyLocation, int32_t> static_animation_type(
	&rpg::SavePartyLocation::animation_type,
	LSD_Reader::ChunkSavePartyLocation::animation_type,
	"animation_type",
	1,
	0
);
static TypedField<rpg::SavePartyLocation, bool> static_lock_facing(
	&rpg::SavePartyLocation::lock_facing,
	LSD_Reader::ChunkSavePartyLocation::lock_facing,
	"lock_facing",
	0,
	0
);
static TypedField<rpg::SavePartyLocation, int32_t> static_move_speed(
	&rpg::SavePartyLocation::move_speed,
	LSD_Reader::ChunkSavePartyLocation::move_speed,
	"move_speed",
	1,
	0
);
static TypedField<rpg::SavePartyLocation, rpg::MoveRoute> static_move_route(
	&rpg::SavePartyLocation::move_route,
	LSD_Reader::ChunkSavePartyLocation::move_route,
	"move_route",
	1,
	0
);
static TypedField<rpg::SavePartyLocation, bool> static_move_route_overwrite(
	&rpg::SavePartyLocation::move_route_overwrite,
	LSD_Reader::ChunkSavePartyLocation::move_route_overwrite,
	"move_route_overwrite",
	0,
	0
);
static TypedField<rpg::SavePartyLocation, int32_t> static_move_route_index(
	&rpg::SavePartyLocation::move_route_index,
	LSD_Reader::ChunkSavePartyLocation::move_route_index,
	"move_route_index",
	0,
	0
);
static TypedField<rpg::SavePartyLocation, bool> static_move_route_finished(
	&rpg::SavePartyLocation::move_route_finished,
	LSD_Reader::ChunkSavePartyLocation::move_route_finished,
	"move_route_finished",
	0,
	0
);
static TypedField<rpg::SavePartyLocation, bool> static_sprite_hidden(
	&rpg::SavePartyLocation::sprite_hidden,
	LSD_Reader::ChunkSavePartyLocation::sprite_hidden,
	"sprite_hidden",
	0,
	0
);
static TypedField<rpg::SavePartyLocation, bool> static_move_route_through(
	&rpg::SavePartyLocation::move_route_through,
	LSD_Reader::ChunkSavePartyLocation::move_route_through,
	"move_route_through",
	0,
	0
);
static TypedField<rpg::SavePartyLocation, int32_t> static_anim_paused(
	&rpg::SavePartyLocation::anim_paused,
	LSD_Reader::ChunkSavePartyLocation::anim_paused,
	"anim_paused",
	0,
	0
);
static TypedField<rpg::SavePartyLocation, bool> static_through(
	&rpg::SavePartyLocation::through,
	LSD_Reader::ChunkSavePartyLocation::through,
	"through",
	0,
	0
);
static TypedField<rpg::SavePartyLocation, int32_t> static_stop_count(
	&rpg::SavePartyLocation::stop_count,
	LSD_Reader::ChunkSavePartyLocation::stop_count,
	"stop_count",
	0,
	0
);
static TypedField<rpg::SavePartyLocation, int32_t> static_anim_count(
	&rpg::SavePartyLocation::anim_count,
	LSD_Reader::ChunkSavePartyLocation::anim_count,
	"anim_count",
	0,
	0
);
static TypedField<rpg::SavePartyLocation, int32_t> static_max_stop_count(
	&rpg::SavePartyLocation::max_stop_count,
	LSD_Reader::ChunkSavePartyLocation::max_stop_count,
	"max_stop_count",
	0,
	0
);
static TypedField<rpg::SavePartyLocation, bool> static_jumping(
	&rpg::SavePartyLocation::jumping,
	LSD_Reader::ChunkSavePartyLocation::jumping,
	"jumping",
	0,
	0
);
static TypedField<rpg::SavePartyLocation, int32_t> static_begin_jump_x(
	&rpg::SavePartyLocation::begin_jump_x,
	LSD_Reader::ChunkSavePartyLocation::begin_jump_x,
	"begin_jump_x",
	0,
	0
);
static TypedField<rpg::SavePartyLocation, int32_t> static_begin_jump_y(
	&rpg::SavePartyLocation::begin_jump_y,
	LSD_Reader::ChunkSavePartyLocation::begin_jump_y,
	"begin_jump_y",
	0,
	0
);
static TypedField<rpg::SavePartyLocation, bool> static_pause(
	&rpg::SavePartyLocation::pause,
	LSD_Reader::ChunkSavePartyLocation::pause,
	"pause",
	0,
	0
);
static TypedField<rpg::SavePartyLocation, bool> static_flying(
	&rpg::SavePartyLocation::flying,
	LSD_Reader::ChunkSavePartyLocation::flying,
	"flying",
	0,
	0
);
static TypedField<rpg::SavePartyLocation, std::string> static_sprite_name(
	&rpg::SavePartyLocation::sprite_name,
	LSD_Reader::ChunkSavePartyLocation::sprite_name,
	"sprite_name",
	0,
	0
);
static TypedField<rpg::SavePartyLocation, int32_t> static_sprite_id(
	&rpg::SavePartyLocation::sprite_id,
	LSD_Reader::ChunkSavePartyLocation::sprite_id,
	"sprite_id",
	0,
	0
);
static TypedField<rpg::SavePartyLocation, bool> static_processed(
	&rpg::SavePartyLocation::processed,
	LSD_Reader::ChunkSavePartyLocation::processed,
	"processed",
	0,
	0
);
static TypedField<rpg::SavePartyLocation, int32_t> static_flash_red(
	&rpg::SavePartyLocation::flash_red,
	LSD_Reader::ChunkSavePartyLocation::flash_red,
	"flash_red",
	0,
	0
);
static TypedField<rpg::SavePartyLocation, int32_t> static_flash_green(
	&rpg::SavePartyLocation::flash_green,
	LSD_Reader::ChunkSavePartyLocation::flash_green,
	"flash_green",
	0,
	0
);
static TypedField<rpg::SavePartyLocation, int32_t> static_flash_blue(
	&rpg::SavePartyLocation::flash_blue,
	LSD_Reader::ChunkSavePartyLocation::flash_blue,
	"flash_blue",
	0,
	0
);
static TypedField<rpg::SavePartyLocation, double> static_flash_current_level(
	&rpg::SavePartyLocation::flash_current_level,
	LSD_Reader::ChunkSavePartyLocation::flash_current_level,
	"flash_current_level",
	0,
	0
);
static TypedField<rpg::SavePartyLocation, int32_t> static_flash_time_left(
	&rpg::SavePartyLocation::flash_time_left,
	LSD_Reader::ChunkSavePartyLocation::flash_time_left,
	"flash_time_left",
	0,
	0
);
static TypedField<rpg::SavePartyLocation, int32_t> static_easyrpg_move_failure_count(
	&rpg::SavePartyLocation::easyrpg_move_failure_count,
	LSD_Reader::ChunkSavePartyLocation::easyrpg_move_failure_count,
	"easyrpg_move_failure_count",
	0,
	0
);
static TypedField<rpg::SavePartyLocation, bool> static_boarding(
	&rpg::SavePartyLocation::boarding,
	LSD_Reader::ChunkSavePartyLocation::boarding,
	"boarding",
	0,
	0
);
static TypedField<rpg::SavePartyLocation, bool> static_aboard(
	&rpg::SavePartyLocation::aboard,
	LSD_Reader::ChunkSavePartyLocation::aboard,
	"aboard",
	0,
	0
);
static TypedField<rpg::SavePartyLocation, int32_t> static_vehicle(
	&rpg::SavePartyLocation::vehicle,
	LSD_Reader::ChunkSavePartyLocation::vehicle,
	"vehicle",
	0,
	0
);
static TypedField<rpg::SavePartyLocation, bool> static_unboarding(
	&rpg::SavePartyLocation::unboarding,
	LSD_Reader::ChunkSavePartyLocation::unboarding,
	"unboarding",
	0,
	0
);
static TypedField<rpg::SavePartyLocation, int32_t> static_preboard_move_speed(
	&rpg::SavePartyLocation::preboard_move_speed,
	LSD_Reader::ChunkSavePartyLocation::preboard_move_speed,
	"preboard_move_speed",
	0,
	0
);
static TypedField<rpg::SavePartyLocation, bool> static_menu_calling(
	&rpg::SavePartyLocation::menu_calling,
	LSD_Reader::ChunkSavePartyLocation::menu_calling,
	"menu_calling",
	0,
	0
);
static TypedField<rpg::SavePartyLocation, int32_t> static_pan_state(
	&rpg::SavePartyLocation::pan_state,
	LSD_Reader::ChunkSavePartyLocation::pan_state,
	"pan_state",
	0,
	0
);
static TypedField<rpg::SavePartyLocation, int32_t> static_pan_current_x(
	&rpg::SavePartyLocation::pan_current_x,
	LSD_Reader::ChunkSavePartyLocation::pan_current_x,
	"pan_current_x",
	0,
	0
);
static TypedField<rpg::SavePartyLocation, int32_t> static_pan_current_y(
	&rpg::SavePartyLocation::pan_current_y,
	LSD_Reader::ChunkSavePartyLocation::pan_current_y,
	"pan_current_y",
	0,
	0
);
static TypedField<rpg::SavePartyLocation, int32_t> static_pan_finish_x(
	&rpg::SavePartyLocation::pan_finish_x,
	LSD_Reader::ChunkSavePartyLocation::pan_finish_x,
	"pan_finish_x",
	0,
	0
);
static TypedField<rpg::SavePartyLocation, int32_t> static_pan_finish_y(
	&rpg::SavePartyLocation::pan_finish_y,
	LSD_Reader::ChunkSavePartyLocation::pan_finish_y,
	"pan_finish_y",
	0,
	0
);
static TypedField<rpg::SavePartyLocation, int32_t> static_pan_speed(
	&rpg::SavePartyLocation::pan_speed,
	LSD_Reader::ChunkSavePartyLocation::pan_speed,
	"pan_speed",
	0,
	0
);
static TypedField<rpg::SavePartyLocation, int32_t> static_total_encounter_rate(
	&rpg::SavePartyLocation::total_encounter_rate,
	LSD_Reader::ChunkSavePartyLocation::total_encounter_rate,
	"total_encounter_rate",
	0,
	0
);
static TypedField<rpg::SavePartyLocation, bool> static_encounter_calling(
	&rpg::SavePartyLocation::encounter_calling,
	LSD_Reader::ChunkSavePartyLocation::encounter_calling,
	"encounter_calling",
	0,
	0
);
static TypedField<rpg::SavePartyLocation, int32_t> static_map_save_count(
	&rpg::SavePartyLocation::map_save_count,
	LSD_Reader::ChunkSavePartyLocation::map_save_count,
	"map_save_count",
	0,
	0
);
static TypedField<rpg::SavePartyLocation, int32_t> static_database_save_count(
	&rpg::SavePartyLocation::database_save_count,
	LSD_Reader::ChunkSavePartyLocation::database_save_count,
	"database_save_count",
	0,
	0
);
static TypedField<rpg::SavePartyLocation, double> static_maniac_horizontal_pan_speed(
	&rpg::SavePartyLocation::maniac_horizontal_pan_speed,
	LSD_Reader::ChunkSavePartyLocation::maniac_horizontal_pan_speed,
	"maniac_horizontal_pan_speed",
	0,
	0
);
static TypedField<rpg::SavePartyLocation, double> static_maniac_vertical_pan_speed(
	&rpg::SavePartyLocation::maniac_vertical_pan_speed,
	LSD_Reader::ChunkSavePartyLocation::maniac_vertical_pan_speed,
	"maniac_vertical_pan_speed",
	0,
	0
);


template <>
Field<rpg::SavePartyLocation> const* Struct<rpg::SavePartyLocation>::fields[] = {
	&static_active,
	&static_map_id,
	&static_position_x,
	&static_position_y,
	&static_direction,
	&static_facing,
	&static_anim_frame,
	&static_transparency,
	&static_remaining_step,
	&static_move_frequency,
	&static_layer,
	&static_overlap_forbidden,
	&static_animation_type,
	&static_lock_facing,
	&static_move_speed,
	&static_move_route,
	&static_move_route_overwrite,
	&static_move_route_index,
	&static_move_route_finished,
	&static_sprite_hidden,
	&static_move_route_through,
	&static_anim_paused,
	&static_through,
	&static_stop_count,
	&static_anim_count,
	&static_max_stop_count,
	&static_jumping,
	&static_begin_jump_x,
	&static_begin_jump_y,
	&static_pause,
	&static_flying,
	&static_sprite_name,
	&static_sprite_id,
	&static_processed,
	&static_flash_red,
	&static_flash_green,
	&static_flash_blue,
	&static_flash_current_level,
	&static_flash_time_left,
	&static_easyrpg_move_failure_count,
	&static_boarding,
	&static_aboard,
	&static_vehicle,
	&static_unboarding,
	&static_preboard_move_speed,
	&static_menu_calling,
	&static_pan_state,
	&static_pan_current_x,
	&static_pan_current_y,
	&static_pan_finish_x,
	&static_pan_finish_y,
	&static_pan_speed,
	&static_total_encounter_rate,
	&static_encounter_calling,
	&static_map_save_count,
	&static_database_save_count,
	&static_maniac_horizontal_pan_speed,
	&static_maniac_vertical_pan_speed,
	NULL
};

template class Struct<rpg::SavePartyLocation>;

} //namespace lcf
