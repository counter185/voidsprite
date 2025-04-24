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

// Read SavePicture.

template <>
char const* const Struct<rpg::SavePicture>::name = "SavePicture";
static TypedField<rpg::SavePicture, std::string> static_name(
	&rpg::SavePicture::name,
	LSD_Reader::ChunkSavePicture::name,
	"name",
	0,
	0
);
static TypedField<rpg::SavePicture, double> static_start_x(
	&rpg::SavePicture::start_x,
	LSD_Reader::ChunkSavePicture::start_x,
	"start_x",
	0,
	0
);
static TypedField<rpg::SavePicture, double> static_start_y(
	&rpg::SavePicture::start_y,
	LSD_Reader::ChunkSavePicture::start_y,
	"start_y",
	0,
	0
);
static TypedField<rpg::SavePicture, double> static_current_x(
	&rpg::SavePicture::current_x,
	LSD_Reader::ChunkSavePicture::current_x,
	"current_x",
	0,
	0
);
static TypedField<rpg::SavePicture, double> static_current_y(
	&rpg::SavePicture::current_y,
	LSD_Reader::ChunkSavePicture::current_y,
	"current_y",
	0,
	0
);
static TypedField<rpg::SavePicture, bool> static_fixed_to_map(
	&rpg::SavePicture::fixed_to_map,
	LSD_Reader::ChunkSavePicture::fixed_to_map,
	"fixed_to_map",
	0,
	0
);
static TypedField<rpg::SavePicture, double> static_current_magnify(
	&rpg::SavePicture::current_magnify,
	LSD_Reader::ChunkSavePicture::current_magnify,
	"current_magnify",
	0,
	0
);
static TypedField<rpg::SavePicture, double> static_current_top_trans(
	&rpg::SavePicture::current_top_trans,
	LSD_Reader::ChunkSavePicture::current_top_trans,
	"current_top_trans",
	0,
	0
);
static TypedField<rpg::SavePicture, bool> static_use_transparent_color(
	&rpg::SavePicture::use_transparent_color,
	LSD_Reader::ChunkSavePicture::use_transparent_color,
	"use_transparent_color",
	0,
	0
);
static TypedField<rpg::SavePicture, double> static_current_red(
	&rpg::SavePicture::current_red,
	LSD_Reader::ChunkSavePicture::current_red,
	"current_red",
	0,
	0
);
static TypedField<rpg::SavePicture, double> static_current_green(
	&rpg::SavePicture::current_green,
	LSD_Reader::ChunkSavePicture::current_green,
	"current_green",
	0,
	0
);
static TypedField<rpg::SavePicture, double> static_current_blue(
	&rpg::SavePicture::current_blue,
	LSD_Reader::ChunkSavePicture::current_blue,
	"current_blue",
	0,
	0
);
static TypedField<rpg::SavePicture, double> static_current_sat(
	&rpg::SavePicture::current_sat,
	LSD_Reader::ChunkSavePicture::current_sat,
	"current_sat",
	0,
	0
);
static TypedField<rpg::SavePicture, int32_t> static_effect_mode(
	&rpg::SavePicture::effect_mode,
	LSD_Reader::ChunkSavePicture::effect_mode,
	"effect_mode",
	0,
	0
);
static TypedField<rpg::SavePicture, double> static_current_effect_power(
	&rpg::SavePicture::current_effect_power,
	LSD_Reader::ChunkSavePicture::current_effect_power,
	"current_effect_power",
	0,
	0
);
static TypedField<rpg::SavePicture, double> static_current_bot_trans(
	&rpg::SavePicture::current_bot_trans,
	LSD_Reader::ChunkSavePicture::current_bot_trans,
	"current_bot_trans",
	0,
	0
);
static TypedField<rpg::SavePicture, int32_t> static_spritesheet_cols(
	&rpg::SavePicture::spritesheet_cols,
	LSD_Reader::ChunkSavePicture::spritesheet_cols,
	"spritesheet_cols",
	0,
	1
);
static TypedField<rpg::SavePicture, int32_t> static_spritesheet_rows(
	&rpg::SavePicture::spritesheet_rows,
	LSD_Reader::ChunkSavePicture::spritesheet_rows,
	"spritesheet_rows",
	0,
	1
);
static TypedField<rpg::SavePicture, int32_t> static_spritesheet_frame(
	&rpg::SavePicture::spritesheet_frame,
	LSD_Reader::ChunkSavePicture::spritesheet_frame,
	"spritesheet_frame",
	0,
	1
);
static TypedField<rpg::SavePicture, int32_t> static_spritesheet_speed(
	&rpg::SavePicture::spritesheet_speed,
	LSD_Reader::ChunkSavePicture::spritesheet_speed,
	"spritesheet_speed",
	0,
	1
);
static TypedField<rpg::SavePicture, int32_t> static_frames(
	&rpg::SavePicture::frames,
	LSD_Reader::ChunkSavePicture::frames,
	"frames",
	0,
	1
);
static TypedField<rpg::SavePicture, bool> static_spritesheet_play_once(
	&rpg::SavePicture::spritesheet_play_once,
	LSD_Reader::ChunkSavePicture::spritesheet_play_once,
	"spritesheet_play_once",
	0,
	1
);
static TypedField<rpg::SavePicture, int32_t> static_map_layer(
	&rpg::SavePicture::map_layer,
	LSD_Reader::ChunkSavePicture::map_layer,
	"map_layer",
	0,
	0
);
static TypedField<rpg::SavePicture, int32_t> static_battle_layer(
	&rpg::SavePicture::battle_layer,
	LSD_Reader::ChunkSavePicture::battle_layer,
	"battle_layer",
	0,
	0
);
static TypedField<rpg::SavePicture, rpg::SavePicture::Flags> static_flags(
	&rpg::SavePicture::flags,
	LSD_Reader::ChunkSavePicture::flags,
	"flags",
	0,
	0
);
static TypedField<rpg::SavePicture, double> static_finish_x(
	&rpg::SavePicture::finish_x,
	LSD_Reader::ChunkSavePicture::finish_x,
	"finish_x",
	0,
	0
);
static TypedField<rpg::SavePicture, double> static_finish_y(
	&rpg::SavePicture::finish_y,
	LSD_Reader::ChunkSavePicture::finish_y,
	"finish_y",
	0,
	0
);
static TypedField<rpg::SavePicture, int32_t> static_finish_magnify(
	&rpg::SavePicture::finish_magnify,
	LSD_Reader::ChunkSavePicture::finish_magnify,
	"finish_magnify",
	0,
	0
);
static TypedField<rpg::SavePicture, int32_t> static_finish_top_trans(
	&rpg::SavePicture::finish_top_trans,
	LSD_Reader::ChunkSavePicture::finish_top_trans,
	"finish_top_trans",
	0,
	0
);
static TypedField<rpg::SavePicture, int32_t> static_finish_bot_trans(
	&rpg::SavePicture::finish_bot_trans,
	LSD_Reader::ChunkSavePicture::finish_bot_trans,
	"finish_bot_trans",
	0,
	0
);
static TypedField<rpg::SavePicture, int32_t> static_finish_red(
	&rpg::SavePicture::finish_red,
	LSD_Reader::ChunkSavePicture::finish_red,
	"finish_red",
	0,
	0
);
static TypedField<rpg::SavePicture, int32_t> static_finish_green(
	&rpg::SavePicture::finish_green,
	LSD_Reader::ChunkSavePicture::finish_green,
	"finish_green",
	0,
	0
);
static TypedField<rpg::SavePicture, int32_t> static_finish_blue(
	&rpg::SavePicture::finish_blue,
	LSD_Reader::ChunkSavePicture::finish_blue,
	"finish_blue",
	0,
	0
);
static TypedField<rpg::SavePicture, int32_t> static_finish_sat(
	&rpg::SavePicture::finish_sat,
	LSD_Reader::ChunkSavePicture::finish_sat,
	"finish_sat",
	0,
	0
);
static TypedField<rpg::SavePicture, int32_t> static_finish_effect_power(
	&rpg::SavePicture::finish_effect_power,
	LSD_Reader::ChunkSavePicture::finish_effect_power,
	"finish_effect_power",
	0,
	0
);
static TypedField<rpg::SavePicture, int32_t> static_time_left(
	&rpg::SavePicture::time_left,
	LSD_Reader::ChunkSavePicture::time_left,
	"time_left",
	0,
	0
);
static TypedField<rpg::SavePicture, double> static_current_rotation(
	&rpg::SavePicture::current_rotation,
	LSD_Reader::ChunkSavePicture::current_rotation,
	"current_rotation",
	0,
	0
);
static TypedField<rpg::SavePicture, int32_t> static_current_waver(
	&rpg::SavePicture::current_waver,
	LSD_Reader::ChunkSavePicture::current_waver,
	"current_waver",
	0,
	0
);
static TypedField<rpg::SavePicture, int32_t> static_easyrpg_flip(
	&rpg::SavePicture::easyrpg_flip,
	LSD_Reader::ChunkSavePicture::easyrpg_flip,
	"easyrpg_flip",
	0,
	1
);
static TypedField<rpg::SavePicture, int32_t> static_easyrpg_blend_mode(
	&rpg::SavePicture::easyrpg_blend_mode,
	LSD_Reader::ChunkSavePicture::easyrpg_blend_mode,
	"easyrpg_blend_mode",
	0,
	1
);
static TypedField<rpg::SavePicture, int32_t> static_easyrpg_type(
	&rpg::SavePicture::easyrpg_type,
	LSD_Reader::ChunkSavePicture::easyrpg_type,
	"easyrpg_type",
	0,
	1
);
static TypedField<rpg::SavePicture, double> static_maniac_current_magnify_height(
	&rpg::SavePicture::maniac_current_magnify_height,
	LSD_Reader::ChunkSavePicture::maniac_current_magnify_height,
	"maniac_current_magnify_height",
	0,
	1
);
static TypedField<rpg::SavePicture, int32_t> static_maniac_finish_magnify_height(
	&rpg::SavePicture::maniac_finish_magnify_height,
	LSD_Reader::ChunkSavePicture::maniac_finish_magnify_height,
	"maniac_finish_magnify_height",
	0,
	1
);


template <>
Field<rpg::SavePicture> const* Struct<rpg::SavePicture>::fields[] = {
	&static_name,
	&static_start_x,
	&static_start_y,
	&static_current_x,
	&static_current_y,
	&static_fixed_to_map,
	&static_current_magnify,
	&static_current_top_trans,
	&static_use_transparent_color,
	&static_current_red,
	&static_current_green,
	&static_current_blue,
	&static_current_sat,
	&static_effect_mode,
	&static_current_effect_power,
	&static_current_bot_trans,
	&static_spritesheet_cols,
	&static_spritesheet_rows,
	&static_spritesheet_frame,
	&static_spritesheet_speed,
	&static_frames,
	&static_spritesheet_play_once,
	&static_map_layer,
	&static_battle_layer,
	&static_flags,
	&static_finish_x,
	&static_finish_y,
	&static_finish_magnify,
	&static_finish_top_trans,
	&static_finish_bot_trans,
	&static_finish_red,
	&static_finish_green,
	&static_finish_blue,
	&static_finish_sat,
	&static_finish_effect_power,
	&static_time_left,
	&static_current_rotation,
	&static_current_waver,
	&static_easyrpg_flip,
	&static_easyrpg_blend_mode,
	&static_easyrpg_type,
	&static_maniac_current_magnify_height,
	&static_maniac_finish_magnify_height,
	NULL
};

template class Struct<rpg::SavePicture>;

} //namespace lcf
