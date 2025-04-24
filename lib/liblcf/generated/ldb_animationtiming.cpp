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

// Read AnimationTiming.

template <>
char const* const Struct<rpg::AnimationTiming>::name = "AnimationTiming";
static TypedField<rpg::AnimationTiming, int32_t> static_frame(
	&rpg::AnimationTiming::frame,
	LDB_Reader::ChunkAnimationTiming::frame,
	"frame",
	0,
	0
);
static TypedField<rpg::AnimationTiming, rpg::Sound> static_se(
	&rpg::AnimationTiming::se,
	LDB_Reader::ChunkAnimationTiming::se,
	"se",
	1,
	0
);
static TypedField<rpg::AnimationTiming, int32_t> static_flash_scope(
	&rpg::AnimationTiming::flash_scope,
	LDB_Reader::ChunkAnimationTiming::flash_scope,
	"flash_scope",
	1,
	0
);
static TypedField<rpg::AnimationTiming, int32_t> static_flash_red(
	&rpg::AnimationTiming::flash_red,
	LDB_Reader::ChunkAnimationTiming::flash_red,
	"flash_red",
	0,
	0
);
static TypedField<rpg::AnimationTiming, int32_t> static_flash_green(
	&rpg::AnimationTiming::flash_green,
	LDB_Reader::ChunkAnimationTiming::flash_green,
	"flash_green",
	0,
	0
);
static TypedField<rpg::AnimationTiming, int32_t> static_flash_blue(
	&rpg::AnimationTiming::flash_blue,
	LDB_Reader::ChunkAnimationTiming::flash_blue,
	"flash_blue",
	0,
	0
);
static TypedField<rpg::AnimationTiming, int32_t> static_flash_power(
	&rpg::AnimationTiming::flash_power,
	LDB_Reader::ChunkAnimationTiming::flash_power,
	"flash_power",
	0,
	0
);
static TypedField<rpg::AnimationTiming, int32_t> static_screen_shake(
	&rpg::AnimationTiming::screen_shake,
	LDB_Reader::ChunkAnimationTiming::screen_shake,
	"screen_shake",
	0,
	0
);


template <>
Field<rpg::AnimationTiming> const* Struct<rpg::AnimationTiming>::fields[] = {
	&static_frame,
	&static_se,
	&static_flash_scope,
	&static_flash_red,
	&static_flash_green,
	&static_flash_blue,
	&static_flash_power,
	&static_screen_shake,
	NULL
};

template class Struct<rpg::AnimationTiming>;

} //namespace lcf
