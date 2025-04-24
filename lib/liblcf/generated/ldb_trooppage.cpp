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

// Read TroopPage.

template <>
char const* const Struct<rpg::TroopPage>::name = "TroopPage";
static TypedField<rpg::TroopPage, rpg::TroopPageCondition> static_condition(
	&rpg::TroopPage::condition,
	LDB_Reader::ChunkTroopPage::condition,
	"condition",
	1,
	0
);
static SizeField<rpg::TroopPage, std::vector<rpg::EventCommand>> static_size_event_commands(
	&rpg::TroopPage::event_commands,
	LDB_Reader::ChunkTroopPage::event_commands_size,
	1,
	0
);
static TypedField<rpg::TroopPage, std::vector<rpg::EventCommand>> static_event_commands(
	&rpg::TroopPage::event_commands,
	LDB_Reader::ChunkTroopPage::event_commands,
	"event_commands",
	1,
	0
);


template <>
Field<rpg::TroopPage> const* Struct<rpg::TroopPage>::fields[] = {
	&static_condition,
	&static_size_event_commands,
	&static_event_commands,
	NULL
};

template class Struct<rpg::TroopPage>;

} //namespace lcf
