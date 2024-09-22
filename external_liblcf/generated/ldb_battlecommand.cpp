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

// Read BattleCommand.

template <>
char const* const Struct<rpg::BattleCommand>::name = "BattleCommand";
static TypedField<rpg::BattleCommand, DBString> static_name(
	&rpg::BattleCommand::name,
	LDB_Reader::ChunkBattleCommand::name,
	"name",
	1,
	0
);
static TypedField<rpg::BattleCommand, int32_t> static_type(
	&rpg::BattleCommand::type,
	LDB_Reader::ChunkBattleCommand::type,
	"type",
	0,
	0
);


template <>
Field<rpg::BattleCommand> const* Struct<rpg::BattleCommand>::fields[] = {
	&static_name,
	&static_type,
	NULL
};

template class Struct<rpg::BattleCommand>;

} //namespace lcf
