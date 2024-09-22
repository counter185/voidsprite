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
#include "lcf/lmt/reader.h"
#include "lcf/lmt/chunks.h"
#include "reader_struct_impl.h"

namespace lcf {

// Read Encounter.

template <>
char const* const Struct<rpg::Encounter>::name = "Encounter";
static TypedField<rpg::Encounter, int32_t> static_troop_id(
	&rpg::Encounter::troop_id,
	LMT_Reader::ChunkEncounter::troop_id,
	"troop_id",
	0,
	0
);


template <>
Field<rpg::Encounter> const* Struct<rpg::Encounter>::fields[] = {
	&static_troop_id,
	NULL
};

template class Struct<rpg::Encounter>;

} //namespace lcf
