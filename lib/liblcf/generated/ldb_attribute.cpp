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

// Read Attribute.

template <>
char const* const Struct<rpg::Attribute>::name = "Attribute";
static TypedField<rpg::Attribute, DBString> static_name(
	&rpg::Attribute::name,
	LDB_Reader::ChunkAttribute::name,
	"name",
	0,
	0
);
static TypedField<rpg::Attribute, int32_t> static_type(
	&rpg::Attribute::type,
	LDB_Reader::ChunkAttribute::type,
	"type",
	1,
	0
);
static TypedField<rpg::Attribute, int32_t> static_a_rate(
	&rpg::Attribute::a_rate,
	LDB_Reader::ChunkAttribute::a_rate,
	"a_rate",
	0,
	0
);
static TypedField<rpg::Attribute, int32_t> static_b_rate(
	&rpg::Attribute::b_rate,
	LDB_Reader::ChunkAttribute::b_rate,
	"b_rate",
	0,
	0
);
static TypedField<rpg::Attribute, int32_t> static_c_rate(
	&rpg::Attribute::c_rate,
	LDB_Reader::ChunkAttribute::c_rate,
	"c_rate",
	0,
	0
);
static TypedField<rpg::Attribute, int32_t> static_d_rate(
	&rpg::Attribute::d_rate,
	LDB_Reader::ChunkAttribute::d_rate,
	"d_rate",
	0,
	0
);
static TypedField<rpg::Attribute, int32_t> static_e_rate(
	&rpg::Attribute::e_rate,
	LDB_Reader::ChunkAttribute::e_rate,
	"e_rate",
	0,
	0
);


template <>
Field<rpg::Attribute> const* Struct<rpg::Attribute>::fields[] = {
	&static_name,
	&static_type,
	&static_a_rate,
	&static_b_rate,
	&static_c_rate,
	&static_d_rate,
	&static_e_rate,
	NULL
};

template class Struct<rpg::Attribute>;

} //namespace lcf
