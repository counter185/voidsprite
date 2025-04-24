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
#include "lcf/lmu/reader.h"
#include "lcf/lmu/chunks.h"
#include "reader_struct_impl.h"

namespace lcf {

// Read EventPage.

template <>
char const* const Struct<rpg::EventPage>::name = "EventPage";
static TypedField<rpg::EventPage, rpg::EventPageCondition> static_condition(
	&rpg::EventPage::condition,
	LMU_Reader::ChunkEventPage::condition,
	"condition",
	1,
	0
);
static TypedField<rpg::EventPage, DBString> static_character_name(
	&rpg::EventPage::character_name,
	LMU_Reader::ChunkEventPage::character_name,
	"character_name",
	0,
	0
);
static TypedField<rpg::EventPage, int32_t> static_character_index(
	&rpg::EventPage::character_index,
	LMU_Reader::ChunkEventPage::character_index,
	"character_index",
	0,
	0
);
static TypedField<rpg::EventPage, int32_t> static_character_direction(
	&rpg::EventPage::character_direction,
	LMU_Reader::ChunkEventPage::character_direction,
	"character_direction",
	1,
	0
);
static TypedField<rpg::EventPage, int32_t> static_character_pattern(
	&rpg::EventPage::character_pattern,
	LMU_Reader::ChunkEventPage::character_pattern,
	"character_pattern",
	0,
	0
);
static TypedField<rpg::EventPage, bool> static_translucent(
	&rpg::EventPage::translucent,
	LMU_Reader::ChunkEventPage::translucent,
	"translucent",
	1,
	0
);
static TypedField<rpg::EventPage, int32_t> static_move_type(
	&rpg::EventPage::move_type,
	LMU_Reader::ChunkEventPage::move_type,
	"move_type",
	1,
	0
);
static TypedField<rpg::EventPage, int32_t> static_move_frequency(
	&rpg::EventPage::move_frequency,
	LMU_Reader::ChunkEventPage::move_frequency,
	"move_frequency",
	0,
	0
);
static TypedField<rpg::EventPage, int32_t> static_trigger(
	&rpg::EventPage::trigger,
	LMU_Reader::ChunkEventPage::trigger,
	"trigger",
	1,
	0
);
static TypedField<rpg::EventPage, int32_t> static_layer(
	&rpg::EventPage::layer,
	LMU_Reader::ChunkEventPage::layer,
	"layer",
	1,
	0
);
static TypedField<rpg::EventPage, bool> static_overlap_forbidden(
	&rpg::EventPage::overlap_forbidden,
	LMU_Reader::ChunkEventPage::overlap_forbidden,
	"overlap_forbidden",
	1,
	0
);
static TypedField<rpg::EventPage, int32_t> static_animation_type(
	&rpg::EventPage::animation_type,
	LMU_Reader::ChunkEventPage::animation_type,
	"animation_type",
	1,
	0
);
static TypedField<rpg::EventPage, int32_t> static_move_speed(
	&rpg::EventPage::move_speed,
	LMU_Reader::ChunkEventPage::move_speed,
	"move_speed",
	0,
	0
);
static TypedField<rpg::EventPage, rpg::MoveRoute> static_move_route(
	&rpg::EventPage::move_route,
	LMU_Reader::ChunkEventPage::move_route,
	"move_route",
	1,
	0
);
static SizeField<rpg::EventPage, std::vector<rpg::EventCommand>> static_size_event_commands(
	&rpg::EventPage::event_commands,
	LMU_Reader::ChunkEventPage::event_commands_size,
	1,
	0
);
static TypedField<rpg::EventPage, std::vector<rpg::EventCommand>> static_event_commands(
	&rpg::EventPage::event_commands,
	LMU_Reader::ChunkEventPage::event_commands,
	"event_commands",
	1,
	0
);


template <>
Field<rpg::EventPage> const* Struct<rpg::EventPage>::fields[] = {
	&static_condition,
	&static_character_name,
	&static_character_index,
	&static_character_direction,
	&static_character_pattern,
	&static_translucent,
	&static_move_type,
	&static_move_frequency,
	&static_trigger,
	&static_layer,
	&static_overlap_forbidden,
	&static_animation_type,
	&static_move_speed,
	&static_move_route,
	&static_size_event_commands,
	&static_event_commands,
	NULL
};

template class Struct<rpg::EventPage>;

} //namespace lcf
