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

// Read Music.

template <>
char const* const Struct<rpg::Music>::name = "Music";
static TypedField<rpg::Music, std::string> static_name(
	&rpg::Music::name,
	LDB_Reader::ChunkMusic::name,
	"name",
	1,
	0
);
static TypedField<rpg::Music, int32_t> static_fadein(
	&rpg::Music::fadein,
	LDB_Reader::ChunkMusic::fadein,
	"fadein",
	0,
	0
);
static TypedField<rpg::Music, int32_t> static_volume(
	&rpg::Music::volume,
	LDB_Reader::ChunkMusic::volume,
	"volume",
	0,
	0
);
static TypedField<rpg::Music, int32_t> static_tempo(
	&rpg::Music::tempo,
	LDB_Reader::ChunkMusic::tempo,
	"tempo",
	0,
	0
);
static TypedField<rpg::Music, int32_t> static_balance(
	&rpg::Music::balance,
	LDB_Reader::ChunkMusic::balance,
	"balance",
	0,
	0
);


template <>
Field<rpg::Music> const* Struct<rpg::Music>::fields[] = {
	&static_name,
	&static_fadein,
	&static_volume,
	&static_tempo,
	&static_balance,
	NULL
};

template class Struct<rpg::Music>;

} //namespace lcf
