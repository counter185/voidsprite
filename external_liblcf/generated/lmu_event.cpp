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

// Read Event.

template <>
char const* const Struct<rpg::Event>::name = "Event";
static TypedField<rpg::Event, DBString> static_name(
	&rpg::Event::name,
	LMU_Reader::ChunkEvent::name,
	"name",
	0,
	0
);
static TypedField<rpg::Event, int32_t> static_x(
	&rpg::Event::x,
	LMU_Reader::ChunkEvent::x,
	"x",
	0,
	0
);
static TypedField<rpg::Event, int32_t> static_y(
	&rpg::Event::y,
	LMU_Reader::ChunkEvent::y,
	"y",
	0,
	0
);
static TypedField<rpg::Event, std::vector<rpg::EventPage>> static_pages(
	&rpg::Event::pages,
	LMU_Reader::ChunkEvent::pages,
	"pages",
	1,
	0
);


template <>
Field<rpg::Event> const* Struct<rpg::Event>::fields[] = {
	&static_name,
	&static_x,
	&static_y,
	&static_pages,
	NULL
};

template class Struct<rpg::Event>;

} //namespace lcf
