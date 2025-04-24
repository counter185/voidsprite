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

// Read Switch.

template <>
char const* const Struct<rpg::Switch>::name = "Switch";
static TypedField<rpg::Switch, DBString> static_name(
	&rpg::Switch::name,
	LDB_Reader::ChunkSwitch::name,
	"name",
	0,
	0
);


template <>
Field<rpg::Switch> const* Struct<rpg::Switch>::fields[] = {
	&static_name,
	NULL
};

template class Struct<rpg::Switch>;

} //namespace lcf
