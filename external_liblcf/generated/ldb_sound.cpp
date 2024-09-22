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

// Read Sound.

template <>
char const* const Struct<rpg::Sound>::name = "Sound";
static TypedField<rpg::Sound, std::string> static_name(
	&rpg::Sound::name,
	LDB_Reader::ChunkSound::name,
	"name",
	1,
	0
);
static TypedField<rpg::Sound, int32_t> static_volume(
	&rpg::Sound::volume,
	LDB_Reader::ChunkSound::volume,
	"volume",
	0,
	0
);
static TypedField<rpg::Sound, int32_t> static_tempo(
	&rpg::Sound::tempo,
	LDB_Reader::ChunkSound::tempo,
	"tempo",
	0,
	0
);
static TypedField<rpg::Sound, int32_t> static_balance(
	&rpg::Sound::balance,
	LDB_Reader::ChunkSound::balance,
	"balance",
	0,
	0
);


template <>
Field<rpg::Sound> const* Struct<rpg::Sound>::fields[] = {
	&static_name,
	&static_volume,
	&static_tempo,
	&static_balance,
	NULL
};

template class Struct<rpg::Sound>;

} //namespace lcf
