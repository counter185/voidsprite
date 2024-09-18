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

// Read SaveInventory.

template <>
char const* const Struct<rpg::SaveInventory>::name = "SaveInventory";
static CountField<rpg::SaveInventory, std::vector<int16_t>> static_size_party(
	&rpg::SaveInventory::party,
	LSD_Reader::ChunkSaveInventory::party_size,
	1,
	0
);
static TypedField<rpg::SaveInventory, std::vector<int16_t>> static_party(
	&rpg::SaveInventory::party,
	LSD_Reader::ChunkSaveInventory::party,
	"party",
	1,
	0
);
static CountField<rpg::SaveInventory, std::vector<int16_t>> static_size_item_ids(
	&rpg::SaveInventory::item_ids,
	LSD_Reader::ChunkSaveInventory::item_ids_size,
	1,
	0
);
static TypedField<rpg::SaveInventory, std::vector<int16_t>> static_item_ids(
	&rpg::SaveInventory::item_ids,
	LSD_Reader::ChunkSaveInventory::item_ids,
	"item_ids",
	1,
	0
);
static TypedField<rpg::SaveInventory, std::vector<uint8_t>> static_item_counts(
	&rpg::SaveInventory::item_counts,
	LSD_Reader::ChunkSaveInventory::item_counts,
	"item_counts",
	1,
	0
);
static TypedField<rpg::SaveInventory, std::vector<uint8_t>> static_item_usage(
	&rpg::SaveInventory::item_usage,
	LSD_Reader::ChunkSaveInventory::item_usage,
	"item_usage",
	1,
	0
);
static TypedField<rpg::SaveInventory, int32_t> static_gold(
	&rpg::SaveInventory::gold,
	LSD_Reader::ChunkSaveInventory::gold,
	"gold",
	0,
	0
);
static TypedField<rpg::SaveInventory, int32_t> static_timer1_frames(
	&rpg::SaveInventory::timer1_frames,
	LSD_Reader::ChunkSaveInventory::timer1_frames,
	"timer1_frames",
	0,
	0
);
static TypedField<rpg::SaveInventory, bool> static_timer1_active(
	&rpg::SaveInventory::timer1_active,
	LSD_Reader::ChunkSaveInventory::timer1_active,
	"timer1_active",
	0,
	0
);
static TypedField<rpg::SaveInventory, bool> static_timer1_visible(
	&rpg::SaveInventory::timer1_visible,
	LSD_Reader::ChunkSaveInventory::timer1_visible,
	"timer1_visible",
	0,
	0
);
static TypedField<rpg::SaveInventory, bool> static_timer1_battle(
	&rpg::SaveInventory::timer1_battle,
	LSD_Reader::ChunkSaveInventory::timer1_battle,
	"timer1_battle",
	0,
	0
);
static TypedField<rpg::SaveInventory, int32_t> static_timer2_frames(
	&rpg::SaveInventory::timer2_frames,
	LSD_Reader::ChunkSaveInventory::timer2_frames,
	"timer2_frames",
	0,
	0
);
static TypedField<rpg::SaveInventory, bool> static_timer2_active(
	&rpg::SaveInventory::timer2_active,
	LSD_Reader::ChunkSaveInventory::timer2_active,
	"timer2_active",
	0,
	0
);
static TypedField<rpg::SaveInventory, bool> static_timer2_visible(
	&rpg::SaveInventory::timer2_visible,
	LSD_Reader::ChunkSaveInventory::timer2_visible,
	"timer2_visible",
	0,
	0
);
static TypedField<rpg::SaveInventory, bool> static_timer2_battle(
	&rpg::SaveInventory::timer2_battle,
	LSD_Reader::ChunkSaveInventory::timer2_battle,
	"timer2_battle",
	0,
	0
);
static TypedField<rpg::SaveInventory, int32_t> static_battles(
	&rpg::SaveInventory::battles,
	LSD_Reader::ChunkSaveInventory::battles,
	"battles",
	0,
	0
);
static TypedField<rpg::SaveInventory, int32_t> static_defeats(
	&rpg::SaveInventory::defeats,
	LSD_Reader::ChunkSaveInventory::defeats,
	"defeats",
	0,
	0
);
static TypedField<rpg::SaveInventory, int32_t> static_escapes(
	&rpg::SaveInventory::escapes,
	LSD_Reader::ChunkSaveInventory::escapes,
	"escapes",
	0,
	0
);
static TypedField<rpg::SaveInventory, int32_t> static_victories(
	&rpg::SaveInventory::victories,
	LSD_Reader::ChunkSaveInventory::victories,
	"victories",
	0,
	0
);
static TypedField<rpg::SaveInventory, int32_t> static_turns(
	&rpg::SaveInventory::turns,
	LSD_Reader::ChunkSaveInventory::turns,
	"turns",
	0,
	0
);
static TypedField<rpg::SaveInventory, int32_t> static_steps(
	&rpg::SaveInventory::steps,
	LSD_Reader::ChunkSaveInventory::steps,
	"steps",
	0,
	0
);


template <>
Field<rpg::SaveInventory> const* Struct<rpg::SaveInventory>::fields[] = {
	&static_size_party,
	&static_party,
	&static_size_item_ids,
	&static_item_ids,
	&static_item_counts,
	&static_item_usage,
	&static_gold,
	&static_timer1_frames,
	&static_timer1_active,
	&static_timer1_visible,
	&static_timer1_battle,
	&static_timer2_frames,
	&static_timer2_active,
	&static_timer2_visible,
	&static_timer2_battle,
	&static_battles,
	&static_defeats,
	&static_escapes,
	&static_victories,
	&static_turns,
	&static_steps,
	NULL
};

template class Struct<rpg::SaveInventory>;

} //namespace lcf
