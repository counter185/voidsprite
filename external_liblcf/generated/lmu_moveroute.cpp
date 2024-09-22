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

// Read MoveRoute.

template <>
char const* const Struct<rpg::MoveRoute>::name = "MoveRoute";
static SizeField<rpg::MoveRoute, std::vector<rpg::MoveCommand>> static_size_move_commands(
	&rpg::MoveRoute::move_commands,
	LMU_Reader::ChunkMoveRoute::move_commands_size,
	0,
	0
);
static TypedField<rpg::MoveRoute, std::vector<rpg::MoveCommand>> static_move_commands(
	&rpg::MoveRoute::move_commands,
	LMU_Reader::ChunkMoveRoute::move_commands,
	"move_commands",
	1,
	0
);
static TypedField<rpg::MoveRoute, bool> static_repeat(
	&rpg::MoveRoute::repeat,
	LMU_Reader::ChunkMoveRoute::repeat,
	"repeat",
	0,
	0
);
static TypedField<rpg::MoveRoute, bool> static_skippable(
	&rpg::MoveRoute::skippable,
	LMU_Reader::ChunkMoveRoute::skippable,
	"skippable",
	0,
	0
);


template <>
Field<rpg::MoveRoute> const* Struct<rpg::MoveRoute>::fields[] = {
	&static_size_move_commands,
	&static_move_commands,
	&static_repeat,
	&static_skippable,
	NULL
};

template class Struct<rpg::MoveRoute>;

} //namespace lcf
