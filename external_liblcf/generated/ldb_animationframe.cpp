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

// Read AnimationFrame.

template <>
char const* const Struct<rpg::AnimationFrame>::name = "AnimationFrame";
static TypedField<rpg::AnimationFrame, std::vector<rpg::AnimationCellData>> static_cells(
	&rpg::AnimationFrame::cells,
	LDB_Reader::ChunkAnimationFrame::cells,
	"cells",
	1,
	0
);


template <>
Field<rpg::AnimationFrame> const* Struct<rpg::AnimationFrame>::fields[] = {
	&static_cells,
	NULL
};

template class Struct<rpg::AnimationFrame>;

} //namespace lcf
