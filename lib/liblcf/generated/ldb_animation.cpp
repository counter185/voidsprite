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

// Read Animation.

template <>
char const* const Struct<rpg::Animation>::name = "Animation";
static TypedField<rpg::Animation, DBString> static_name(
	&rpg::Animation::name,
	LDB_Reader::ChunkAnimation::name,
	"name",
	0,
	0
);
static TypedField<rpg::Animation, DBString> static_animation_name(
	&rpg::Animation::animation_name,
	LDB_Reader::ChunkAnimation::animation_name,
	"animation_name",
	0,
	0
);
static TypedField<rpg::Animation, bool> static_large(
	&rpg::Animation::large,
	LDB_Reader::ChunkAnimation::large,
	"large",
	0,
	0
);
static TypedField<rpg::Animation, std::vector<rpg::AnimationTiming>> static_timings(
	&rpg::Animation::timings,
	LDB_Reader::ChunkAnimation::timings,
	"timings",
	1,
	0
);
static TypedField<rpg::Animation, int32_t> static_scope(
	&rpg::Animation::scope,
	LDB_Reader::ChunkAnimation::scope,
	"scope",
	1,
	0
);
static TypedField<rpg::Animation, int32_t> static_position(
	&rpg::Animation::position,
	LDB_Reader::ChunkAnimation::position,
	"position",
	1,
	0
);
static TypedField<rpg::Animation, std::vector<rpg::AnimationFrame>> static_frames(
	&rpg::Animation::frames,
	LDB_Reader::ChunkAnimation::frames,
	"frames",
	1,
	0
);


template <>
Field<rpg::Animation> const* Struct<rpg::Animation>::fields[] = {
	&static_name,
	&static_animation_name,
	&static_large,
	&static_timings,
	&static_scope,
	&static_position,
	&static_frames,
	NULL
};

template class Struct<rpg::Animation>;

} //namespace lcf
