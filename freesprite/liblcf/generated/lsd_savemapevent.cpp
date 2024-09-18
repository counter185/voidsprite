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

// Read SaveMapEvent.

template <>
char const* const Struct<rpg::SaveMapEvent>::name = "SaveMapEvent";
static TypedField<rpg::SaveMapEvent, bool> static_active(
	&rpg::SaveMapEvent::active,
	LSD_Reader::ChunkSaveMapEvent::active,
	"active",
	0,
	0
);
static TypedField<rpg::SaveMapEvent, int32_t> static_map_id(
	&rpg::SaveMapEvent::map_id,
	LSD_Reader::ChunkSaveMapEvent::map_id,
	"map_id",
	1,
	0
);
static TypedField<rpg::SaveMapEvent, int32_t> static_position_x(
	&rpg::SaveMapEvent::position_x,
	LSD_Reader::ChunkSaveMapEvent::position_x,
	"position_x",
	1,
	0
);
static TypedField<rpg::SaveMapEvent, int32_t> static_position_y(
	&rpg::SaveMapEvent::position_y,
	LSD_Reader::ChunkSaveMapEvent::position_y,
	"position_y",
	1,
	0
);
static TypedField<rpg::SaveMapEvent, int32_t> static_direction(
	&rpg::SaveMapEvent::direction,
	LSD_Reader::ChunkSaveMapEvent::direction,
	"direction",
	1,
	0
);
static TypedField<rpg::SaveMapEvent, int32_t> static_facing(
	&rpg::SaveMapEvent::facing,
	LSD_Reader::ChunkSaveMapEvent::facing,
	"facing",
	1,
	0
);
static TypedField<rpg::SaveMapEvent, int32_t> static_anim_frame(
	&rpg::SaveMapEvent::anim_frame,
	LSD_Reader::ChunkSaveMapEvent::anim_frame,
	"anim_frame",
	0,
	0
);
static TypedField<rpg::SaveMapEvent, int32_t> static_transparency(
	&rpg::SaveMapEvent::transparency,
	LSD_Reader::ChunkSaveMapEvent::transparency,
	"transparency",
	0,
	0
);
static TypedField<rpg::SaveMapEvent, int32_t> static_remaining_step(
	&rpg::SaveMapEvent::remaining_step,
	LSD_Reader::ChunkSaveMapEvent::remaining_step,
	"remaining_step",
	0,
	0
);
static TypedField<rpg::SaveMapEvent, int32_t> static_move_frequency(
	&rpg::SaveMapEvent::move_frequency,
	LSD_Reader::ChunkSaveMapEvent::move_frequency,
	"move_frequency",
	0,
	0
);
static TypedField<rpg::SaveMapEvent, int32_t> static_layer(
	&rpg::SaveMapEvent::layer,
	LSD_Reader::ChunkSaveMapEvent::layer,
	"layer",
	1,
	0
);
static TypedField<rpg::SaveMapEvent, bool> static_overlap_forbidden(
	&rpg::SaveMapEvent::overlap_forbidden,
	LSD_Reader::ChunkSaveMapEvent::overlap_forbidden,
	"overlap_forbidden",
	0,
	0
);
static TypedField<rpg::SaveMapEvent, int32_t> static_animation_type(
	&rpg::SaveMapEvent::animation_type,
	LSD_Reader::ChunkSaveMapEvent::animation_type,
	"animation_type",
	1,
	0
);
static TypedField<rpg::SaveMapEvent, bool> static_lock_facing(
	&rpg::SaveMapEvent::lock_facing,
	LSD_Reader::ChunkSaveMapEvent::lock_facing,
	"lock_facing",
	0,
	0
);
static TypedField<rpg::SaveMapEvent, int32_t> static_move_speed(
	&rpg::SaveMapEvent::move_speed,
	LSD_Reader::ChunkSaveMapEvent::move_speed,
	"move_speed",
	1,
	0
);
static TypedField<rpg::SaveMapEvent, rpg::MoveRoute> static_move_route(
	&rpg::SaveMapEvent::move_route,
	LSD_Reader::ChunkSaveMapEvent::move_route,
	"move_route",
	1,
	0
);
static TypedField<rpg::SaveMapEvent, bool> static_move_route_overwrite(
	&rpg::SaveMapEvent::move_route_overwrite,
	LSD_Reader::ChunkSaveMapEvent::move_route_overwrite,
	"move_route_overwrite",
	0,
	0
);
static TypedField<rpg::SaveMapEvent, int32_t> static_move_route_index(
	&rpg::SaveMapEvent::move_route_index,
	LSD_Reader::ChunkSaveMapEvent::move_route_index,
	"move_route_index",
	0,
	0
);
static TypedField<rpg::SaveMapEvent, bool> static_move_route_finished(
	&rpg::SaveMapEvent::move_route_finished,
	LSD_Reader::ChunkSaveMapEvent::move_route_finished,
	"move_route_finished",
	0,
	0
);
static TypedField<rpg::SaveMapEvent, bool> static_sprite_hidden(
	&rpg::SaveMapEvent::sprite_hidden,
	LSD_Reader::ChunkSaveMapEvent::sprite_hidden,
	"sprite_hidden",
	0,
	0
);
static TypedField<rpg::SaveMapEvent, bool> static_move_route_through(
	&rpg::SaveMapEvent::move_route_through,
	LSD_Reader::ChunkSaveMapEvent::move_route_through,
	"move_route_through",
	0,
	0
);
static TypedField<rpg::SaveMapEvent, int32_t> static_anim_paused(
	&rpg::SaveMapEvent::anim_paused,
	LSD_Reader::ChunkSaveMapEvent::anim_paused,
	"anim_paused",
	0,
	0
);
static TypedField<rpg::SaveMapEvent, bool> static_through(
	&rpg::SaveMapEvent::through,
	LSD_Reader::ChunkSaveMapEvent::through,
	"through",
	0,
	0
);
static TypedField<rpg::SaveMapEvent, int32_t> static_stop_count(
	&rpg::SaveMapEvent::stop_count,
	LSD_Reader::ChunkSaveMapEvent::stop_count,
	"stop_count",
	0,
	0
);
static TypedField<rpg::SaveMapEvent, int32_t> static_anim_count(
	&rpg::SaveMapEvent::anim_count,
	LSD_Reader::ChunkSaveMapEvent::anim_count,
	"anim_count",
	0,
	0
);
static TypedField<rpg::SaveMapEvent, int32_t> static_max_stop_count(
	&rpg::SaveMapEvent::max_stop_count,
	LSD_Reader::ChunkSaveMapEvent::max_stop_count,
	"max_stop_count",
	0,
	0
);
static TypedField<rpg::SaveMapEvent, bool> static_jumping(
	&rpg::SaveMapEvent::jumping,
	LSD_Reader::ChunkSaveMapEvent::jumping,
	"jumping",
	0,
	0
);
static TypedField<rpg::SaveMapEvent, int32_t> static_begin_jump_x(
	&rpg::SaveMapEvent::begin_jump_x,
	LSD_Reader::ChunkSaveMapEvent::begin_jump_x,
	"begin_jump_x",
	0,
	0
);
static TypedField<rpg::SaveMapEvent, int32_t> static_begin_jump_y(
	&rpg::SaveMapEvent::begin_jump_y,
	LSD_Reader::ChunkSaveMapEvent::begin_jump_y,
	"begin_jump_y",
	0,
	0
);
static TypedField<rpg::SaveMapEvent, bool> static_pause(
	&rpg::SaveMapEvent::pause,
	LSD_Reader::ChunkSaveMapEvent::pause,
	"pause",
	0,
	0
);
static TypedField<rpg::SaveMapEvent, bool> static_flying(
	&rpg::SaveMapEvent::flying,
	LSD_Reader::ChunkSaveMapEvent::flying,
	"flying",
	0,
	0
);
static TypedField<rpg::SaveMapEvent, std::string> static_sprite_name(
	&rpg::SaveMapEvent::sprite_name,
	LSD_Reader::ChunkSaveMapEvent::sprite_name,
	"sprite_name",
	0,
	0
);
static TypedField<rpg::SaveMapEvent, int32_t> static_sprite_id(
	&rpg::SaveMapEvent::sprite_id,
	LSD_Reader::ChunkSaveMapEvent::sprite_id,
	"sprite_id",
	0,
	0
);
static TypedField<rpg::SaveMapEvent, bool> static_processed(
	&rpg::SaveMapEvent::processed,
	LSD_Reader::ChunkSaveMapEvent::processed,
	"processed",
	0,
	0
);
static TypedField<rpg::SaveMapEvent, int32_t> static_flash_red(
	&rpg::SaveMapEvent::flash_red,
	LSD_Reader::ChunkSaveMapEvent::flash_red,
	"flash_red",
	0,
	0
);
static TypedField<rpg::SaveMapEvent, int32_t> static_flash_green(
	&rpg::SaveMapEvent::flash_green,
	LSD_Reader::ChunkSaveMapEvent::flash_green,
	"flash_green",
	0,
	0
);
static TypedField<rpg::SaveMapEvent, int32_t> static_flash_blue(
	&rpg::SaveMapEvent::flash_blue,
	LSD_Reader::ChunkSaveMapEvent::flash_blue,
	"flash_blue",
	0,
	0
);
static TypedField<rpg::SaveMapEvent, double> static_flash_current_level(
	&rpg::SaveMapEvent::flash_current_level,
	LSD_Reader::ChunkSaveMapEvent::flash_current_level,
	"flash_current_level",
	0,
	0
);
static TypedField<rpg::SaveMapEvent, int32_t> static_flash_time_left(
	&rpg::SaveMapEvent::flash_time_left,
	LSD_Reader::ChunkSaveMapEvent::flash_time_left,
	"flash_time_left",
	0,
	0
);
static TypedField<rpg::SaveMapEvent, int32_t> static_easyrpg_move_failure_count(
	&rpg::SaveMapEvent::easyrpg_move_failure_count,
	LSD_Reader::ChunkSaveMapEvent::easyrpg_move_failure_count,
	"easyrpg_move_failure_count",
	0,
	0
);
static TypedField<rpg::SaveMapEvent, bool> static_waiting_execution(
	&rpg::SaveMapEvent::waiting_execution,
	LSD_Reader::ChunkSaveMapEvent::waiting_execution,
	"waiting_execution",
	0,
	0
);
static TypedField<rpg::SaveMapEvent, int32_t> static_original_move_route_index(
	&rpg::SaveMapEvent::original_move_route_index,
	LSD_Reader::ChunkSaveMapEvent::original_move_route_index,
	"original_move_route_index",
	0,
	0
);
static TypedField<rpg::SaveMapEvent, bool> static_triggered_by_decision_key(
	&rpg::SaveMapEvent::triggered_by_decision_key,
	LSD_Reader::ChunkSaveMapEvent::triggered_by_decision_key,
	"triggered_by_decision_key",
	0,
	0
);
static TypedField<rpg::SaveMapEvent, rpg::SaveEventExecState> static_parallel_event_execstate(
	&rpg::SaveMapEvent::parallel_event_execstate,
	LSD_Reader::ChunkSaveMapEvent::parallel_event_execstate,
	"parallel_event_execstate",
	1,
	0
);


template <>
Field<rpg::SaveMapEvent> const* Struct<rpg::SaveMapEvent>::fields[] = {
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
	&static_waiting_execution,
	&static_original_move_route_index,
	&static_triggered_by_decision_key,
	&static_parallel_event_execstate,
	NULL
};

template class Struct<rpg::SaveMapEvent>;

} //namespace lcf
