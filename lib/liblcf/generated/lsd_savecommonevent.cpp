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

// Read SaveCommonEvent.

template <>
char const* const Struct<rpg::SaveCommonEvent>::name = "SaveCommonEvent";
static TypedField<rpg::SaveCommonEvent, rpg::SaveEventExecState> static_parallel_event_execstate(
	&rpg::SaveCommonEvent::parallel_event_execstate,
	LSD_Reader::ChunkSaveCommonEvent::parallel_event_execstate,
	"parallel_event_execstate",
	1,
	0
);


template <>
Field<rpg::SaveCommonEvent> const* Struct<rpg::SaveCommonEvent>::fields[] = {
	&static_parallel_event_execstate,
	NULL
};

template class Struct<rpg::SaveCommonEvent>;

} //namespace lcf
