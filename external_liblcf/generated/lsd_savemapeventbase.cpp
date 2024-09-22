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

// Read SaveMapEventBase.

template <>
char const* const Struct<rpg::SaveMapEventBase>::name = "SaveMapEventBase";
static TypedField<rpg::SaveMapEventBase, bool> static_active(
	&rpg::SaveMapEventBase::active,
	LSD_Reader::ChunkSaveMapEventBase::active,
	"active",
	0,
	0
);
static TypedField<rpg::SaveMapEventBase, int32_t> static_map_id(
	&rpg::SaveMapEventBase::map_id,
	LSD_Reader::ChunkSaveMapEventBase::map_id,
	"map_id",
	1,
	0
);
static TypedField<rpg::SaveMapEventBase, int32_t> static_position_x(
	&rpg::SaveMapEventBase::position_x,
	LSD_Reader::ChunkSaveMapEventBase::position_x,
	"position_x",
	1,
	0
);
static TypedField<rpg::SaveMapEventBase, int32_t> static_position_y(
	&rpg::SaveMapEventBase::position_y,
	LSD_Reader::ChunkSaveMapEventBase::position_y,
	"position_y",
	1,
	0
);
static TypedField<rpg::SaveMapEventBase, int32_t> static_direction(
	&rpg::SaveMapEventBase::direction,
	LSD_Reader::ChunkSaveMapEventBase::direction,
	"direction",
	1,
	0
);
static TypedField<rpg::SaveMapEventBase, int32_t> static_facing(
	&rpg::SaveMapEventBase::facing,
	LSD_Reader::ChunkSaveMapEventBase::facing,
	"facing",
	1,
	0
);
static TypedField<rpg::SaveMapEventBase, int32_t> static_anim_frame(
	&rpg::SaveMapEventBase::anim_frame,
	LSD_Reader::ChunkSaveMapEventBase::anim_frame,
	"anim_frame",
	0,
	0
);
static TypedField<rpg::SaveMapEventBase, int32_t> static_transparency(
	&rpg::SaveMapEventBase::transparency,
	LSD_Reader::ChunkSaveMapEventBase::transparency,
	"transparency",
	0,
	0
);
static TypedField<rpg::SaveMapEventBase, int32_t> static_remaining_step(
	&rpg::SaveMapEventBase::remaining_step,
	LSD_Reader::ChunkSaveMapEventBase::remaining_step,
	"remaining_step",
	0,
	0
);
static TypedField<rpg::SaveMapEventBase, int32_t> static_move_frequency(
	&rpg::SaveMapEventBase::move_frequency,
	LSD_Reader::ChunkSaveMapEventBase::move_frequency,
	"move_frequency",
	0,
	0
);
static TypedField<rpg::SaveMapEventBase, int32_t> static_layer(
	&rpg::SaveMapEventBase::layer,
	LSD_Reader::ChunkSaveMapEventBase::layer,
	"layer",
	1,
	0
);
static TypedField<rpg::SaveMapEventBase, bool> static_overlap_forbidden(
	&rpg::SaveMapEventBase::overlap_forbidden,
	LSD_Reader::ChunkSaveMapEventBase::overlap_forbidden,
	"overlap_forbidden",
	0,
	0
);
static TypedField<rpg::SaveMapEventBase, int32_t> static_animation_type(
	&rpg::SaveMapEventBase::animation_type,
	LSD_Reader::ChunkSaveMapEventBase::animation_type,
	"animation_type",
	1,
	0
);
static TypedField<rpg::SaveMapEventBase, bool> static_lock_facing(
	&rpg::SaveMapEventBase::lock_facing,
	LSD_Reader::ChunkSaveMapEventBase::lock_facing,
	"lock_facing",
	0,
	0
);
static TypedField<rpg::SaveMapEventBase, int32_t> static_move_speed(
	&rpg::SaveMapEventBase::move_speed,
	LSD_Reader::ChunkSaveMapEventBase::move_speed,
	"move_speed",
	1,
	0
);
static TypedField<rpg::SaveMapEventBase, rpg::MoveRoute> static_move_route(
	&rpg::SaveMapEventBase::move_route,
	LSD_Reader::ChunkSaveMapEventBase::move_route,
	"move_route",
	1,
	0
);
static TypedField<rpg::SaveMapEventBase, bool> static_move_route_overwrite(
	&rpg::SaveMapEventBase::move_route_overwrite,
	LSD_Reader::ChunkSaveMapEventBase::move_route_overwrite,
	"move_route_overwrite",
	0,
	0
);
static TypedField<rpg::SaveMapEventBase, int32_t> static_move_route_index(
	&rpg::SaveMapEventBase::move_route_index,
	LSD_Reader::ChunkSaveMapEventBase::move_route_index,
	"move_route_index",
	0,
	0
);
static TypedField<rpg::SaveMapEventBase, bool> static_move_route_finished(
	&rpg::SaveMapEventBase::move_route_finished,
	LSD_Reader::ChunkSaveMapEventBase::move_route_finished,
	"move_route_finished",
	0,
	0
);
static TypedField<rpg::SaveMapEventBase, bool> static_sprite_hidden(
	&rpg::SaveMapEventBase::sprite_hidden,
	LSD_Reader::ChunkSaveMapEventBase::sprite_hidden,
	"sprite_hidden",
	0,
	0
);
static TypedField<rpg::SaveMapEventBase, bool> static_move_route_through(
	&rpg::SaveMapEventBase::move_route_through,
	LSD_Reader::ChunkSaveMapEventBase::move_route_through,
	"move_route_through",
	0,
	0
);
static TypedField<rpg::SaveMapEventBase, int32_t> static_anim_paused(
	&rpg::SaveMapEventBase::anim_paused,
	LSD_Reader::ChunkSaveMapEventBase::anim_paused,
	"anim_paused",
	0,
	0
);
static TypedField<rpg::SaveMapEventBase, bool> static_through(
	&rpg::SaveMapEventBase::through,
	LSD_Reader::ChunkSaveMapEventBase::through,
	"through",
	0,
	0
);
static TypedField<rpg::SaveMapEventBase, int32_t> static_stop_count(
	&rpg::SaveMapEventBase::stop_count,
	LSD_Reader::ChunkSaveMapEventBase::stop_count,
	"stop_count",
	0,
	0
);
static TypedField<rpg::SaveMapEventBase, int32_t> static_anim_count(
	&rpg::SaveMapEventBase::anim_count,
	LSD_Reader::ChunkSaveMapEventBase::anim_count,
	"anim_count",
	0,
	0
);
static TypedField<rpg::SaveMapEventBase, int32_t> static_max_stop_count(
	&rpg::SaveMapEventBase::max_stop_count,
	LSD_Reader::ChunkSaveMapEventBase::max_stop_count,
	"max_stop_count",
	0,
	0
);
static TypedField<rpg::SaveMapEventBase, bool> static_jumping(
	&rpg::SaveMapEventBase::jumping,
	LSD_Reader::ChunkSaveMapEventBase::jumping,
	"jumping",
	0,
	0
);
static TypedField<rpg::SaveMapEventBase, int32_t> static_begin_jump_x(
	&rpg::SaveMapEventBase::begin_jump_x,
	LSD_Reader::ChunkSaveMapEventBase::begin_jump_x,
	"begin_jump_x",
	0,
	0
);
static TypedField<rpg::SaveMapEventBase, int32_t> static_begin_jump_y(
	&rpg::SaveMapEventBase::begin_jump_y,
	LSD_Reader::ChunkSaveMapEventBase::begin_jump_y,
	"begin_jump_y",
	0,
	0
);
static TypedField<rpg::SaveMapEventBase, bool> static_pause(
	&rpg::SaveMapEventBase::pause,
	LSD_Reader::ChunkSaveMapEventBase::pause,
	"pause",
	0,
	0
);
static TypedField<rpg::SaveMapEventBase, bool> static_flying(
	&rpg::SaveMapEventBase::flying,
	LSD_Reader::ChunkSaveMapEventBase::flying,
	"flying",
	0,
	0
);
static TypedField<rpg::SaveMapEventBase, std::string> static_sprite_name(
	&rpg::SaveMapEventBase::sprite_name,
	LSD_Reader::ChunkSaveMapEventBase::sprite_name,
	"sprite_name",
	0,
	0
);
static TypedField<rpg::SaveMapEventBase, int32_t> static_sprite_id(
	&rpg::SaveMapEventBase::sprite_id,
	LSD_Reader::ChunkSaveMapEventBase::sprite_id,
	"sprite_id",
	0,
	0
);
static TypedField<rpg::SaveMapEventBase, bool> static_processed(
	&rpg::SaveMapEventBase::processed,
	LSD_Reader::ChunkSaveMapEventBase::processed,
	"processed",
	0,
	0
);
static TypedField<rpg::SaveMapEventBase, int32_t> static_flash_red(
	&rpg::SaveMapEventBase::flash_red,
	LSD_Reader::ChunkSaveMapEventBase::flash_red,
	"flash_red",
	0,
	0
);
static TypedField<rpg::SaveMapEventBase, int32_t> static_flash_green(
	&rpg::SaveMapEventBase::flash_green,
	LSD_Reader::ChunkSaveMapEventBase::flash_green,
	"flash_green",
	0,
	0
);
static TypedField<rpg::SaveMapEventBase, int32_t> static_flash_blue(
	&rpg::SaveMapEventBase::flash_blue,
	LSD_Reader::ChunkSaveMapEventBase::flash_blue,
	"flash_blue",
	0,
	0
);
static TypedField<rpg::SaveMapEventBase, double> static_flash_current_level(
	&rpg::SaveMapEventBase::flash_current_level,
	LSD_Reader::ChunkSaveMapEventBase::flash_current_level,
	"flash_current_level",
	0,
	0
);
static TypedField<rpg::SaveMapEventBase, int32_t> static_flash_time_left(
	&rpg::SaveMapEventBase::flash_time_left,
	LSD_Reader::ChunkSaveMapEventBase::flash_time_left,
	"flash_time_left",
	0,
	0
);
static TypedField<rpg::SaveMapEventBase, int32_t> static_easyrpg_move_failure_count(
	&rpg::SaveMapEventBase::easyrpg_move_failure_count,
	LSD_Reader::ChunkSaveMapEventBase::easyrpg_move_failure_count,
	"easyrpg_move_failure_count",
	0,
	0
);


template <>
Field<rpg::SaveMapEventBase> const* Struct<rpg::SaveMapEventBase>::fields[] = {
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
	NULL
};

template class Struct<rpg::SaveMapEventBase>;

} //namespace lcf
