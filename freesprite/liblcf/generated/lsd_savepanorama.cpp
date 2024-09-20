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
#include "lcf/lsd/reader.h"
#include "lcf/lsd/chunks.h"
#include "reader_struct_impl.h"

namespace lcf {

// Read SavePanorama.

template <>
char const* const Struct<rpg::SavePanorama>::name = "SavePanorama";
static TypedField<rpg::SavePanorama, int32_t> static_pan_x(
	&rpg::SavePanorama::pan_x,
	LSD_Reader::ChunkSavePanorama::pan_x,
	"pan_x",
	0,
	0
);
static TypedField<rpg::SavePanorama, int32_t> static_pan_y(
	&rpg::SavePanorama::pan_y,
	LSD_Reader::ChunkSavePanorama::pan_y,
	"pan_y",
	0,
	0
);


template <>
Field<rpg::SavePanorama> const* Struct<rpg::SavePanorama>::fields[] = {
	&static_pan_x,
	&static_pan_y,
	NULL
};

template class Struct<rpg::SavePanorama>;

} //namespace lcf
