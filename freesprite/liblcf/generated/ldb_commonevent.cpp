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
#include "lcf/ldb/reader.h"
#include "lcf/ldb/chunks.h"
#include "reader_struct_impl.h"

namespace lcf {

// Read CommonEvent.

template <>
char const* const Struct<rpg::CommonEvent>::name = "CommonEvent";
static TypedField<rpg::CommonEvent, DBString> static_name(
	&rpg::CommonEvent::name,
	LDB_Reader::ChunkCommonEvent::name,
	"name",
	0,
	0
);
static TypedField<rpg::CommonEvent, int32_t> static_trigger(
	&rpg::CommonEvent::trigger,
	LDB_Reader::ChunkCommonEvent::trigger,
	"trigger",
	0,
	0
);
static TypedField<rpg::CommonEvent, bool> static_switch_flag(
	&rpg::CommonEvent::switch_flag,
	LDB_Reader::ChunkCommonEvent::switch_flag,
	"switch_flag",
	0,
	0
);
static TypedField<rpg::CommonEvent, int32_t> static_switch_id(
	&rpg::CommonEvent::switch_id,
	LDB_Reader::ChunkCommonEvent::switch_id,
	"switch_id",
	0,
	0
);
static SizeField<rpg::CommonEvent, std::vector<rpg::EventCommand>> static_size_event_commands(
	&rpg::CommonEvent::event_commands,
	LDB_Reader::ChunkCommonEvent::event_commands_size,
	1,
	0
);
static TypedField<rpg::CommonEvent, std::vector<rpg::EventCommand>> static_event_commands(
	&rpg::CommonEvent::event_commands,
	LDB_Reader::ChunkCommonEvent::event_commands,
	"event_commands",
	1,
	0
);


template <>
Field<rpg::CommonEvent> const* Struct<rpg::CommonEvent>::fields[] = {
	&static_name,
	&static_trigger,
	&static_switch_flag,
	&static_switch_id,
	&static_size_event_commands,
	&static_event_commands,
	NULL
};

template class Struct<rpg::CommonEvent>;

} //namespace lcf
