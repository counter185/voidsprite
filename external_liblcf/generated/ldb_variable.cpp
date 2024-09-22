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

// Read Variable.

template <>
char const* const Struct<rpg::Variable>::name = "Variable";
static TypedField<rpg::Variable, DBString> static_name(
	&rpg::Variable::name,
	LDB_Reader::ChunkVariable::name,
	"name",
	0,
	0
);


template <>
Field<rpg::Variable> const* Struct<rpg::Variable>::fields[] = {
	&static_name,
	NULL
};

template class Struct<rpg::Variable>;

} //namespace lcf
