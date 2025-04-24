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

// Read SaveEventExecState.

template <>
char const* const Struct<rpg::SaveEventExecState>::name = "SaveEventExecState";
static TypedField<rpg::SaveEventExecState, std::vector<rpg::SaveEventExecFrame>> static_stack(
	&rpg::SaveEventExecState::stack,
	LSD_Reader::ChunkSaveEventExecState::stack,
	"stack",
	1,
	0
);
static TypedField<rpg::SaveEventExecState, bool> static_show_message(
	&rpg::SaveEventExecState::show_message,
	LSD_Reader::ChunkSaveEventExecState::show_message,
	"show_message",
	0,
	0
);
static TypedField<rpg::SaveEventExecState, bool> static_abort_on_escape(
	&rpg::SaveEventExecState::abort_on_escape,
	LSD_Reader::ChunkSaveEventExecState::abort_on_escape,
	"abort_on_escape",
	0,
	0
);
static TypedField<rpg::SaveEventExecState, bool> static_wait_movement(
	&rpg::SaveEventExecState::wait_movement,
	LSD_Reader::ChunkSaveEventExecState::wait_movement,
	"wait_movement",
	0,
	0
);
static TypedField<rpg::SaveEventExecState, bool> static_keyinput_wait(
	&rpg::SaveEventExecState::keyinput_wait,
	LSD_Reader::ChunkSaveEventExecState::keyinput_wait,
	"keyinput_wait",
	0,
	0
);
static TypedField<rpg::SaveEventExecState, uint8_t> static_keyinput_variable(
	&rpg::SaveEventExecState::keyinput_variable,
	LSD_Reader::ChunkSaveEventExecState::keyinput_variable,
	"keyinput_variable",
	0,
	0
);
static TypedField<rpg::SaveEventExecState, bool> static_keyinput_all_directions(
	&rpg::SaveEventExecState::keyinput_all_directions,
	LSD_Reader::ChunkSaveEventExecState::keyinput_all_directions,
	"keyinput_all_directions",
	0,
	0
);
static TypedField<rpg::SaveEventExecState, int32_t> static_keyinput_decision(
	&rpg::SaveEventExecState::keyinput_decision,
	LSD_Reader::ChunkSaveEventExecState::keyinput_decision,
	"keyinput_decision",
	0,
	0
);
static TypedField<rpg::SaveEventExecState, int32_t> static_keyinput_cancel(
	&rpg::SaveEventExecState::keyinput_cancel,
	LSD_Reader::ChunkSaveEventExecState::keyinput_cancel,
	"keyinput_cancel",
	0,
	0
);
static TypedField<rpg::SaveEventExecState, int32_t> static_keyinput_2kshift_2k3numbers(
	&rpg::SaveEventExecState::keyinput_2kshift_2k3numbers,
	LSD_Reader::ChunkSaveEventExecState::keyinput_2kshift_2k3numbers,
	"keyinput_2kshift_2k3numbers",
	0,
	0
);
static TypedField<rpg::SaveEventExecState, int32_t> static_keyinput_2kdown_2k3operators(
	&rpg::SaveEventExecState::keyinput_2kdown_2k3operators,
	LSD_Reader::ChunkSaveEventExecState::keyinput_2kdown_2k3operators,
	"keyinput_2kdown_2k3operators",
	0,
	0
);
static TypedField<rpg::SaveEventExecState, int32_t> static_keyinput_2kleft_2k3shift(
	&rpg::SaveEventExecState::keyinput_2kleft_2k3shift,
	LSD_Reader::ChunkSaveEventExecState::keyinput_2kleft_2k3shift,
	"keyinput_2kleft_2k3shift",
	0,
	0
);
static TypedField<rpg::SaveEventExecState, int32_t> static_keyinput_2kright(
	&rpg::SaveEventExecState::keyinput_2kright,
	LSD_Reader::ChunkSaveEventExecState::keyinput_2kright,
	"keyinput_2kright",
	0,
	0
);
static TypedField<rpg::SaveEventExecState, int32_t> static_keyinput_2kup(
	&rpg::SaveEventExecState::keyinput_2kup,
	LSD_Reader::ChunkSaveEventExecState::keyinput_2kup,
	"keyinput_2kup",
	0,
	0
);
static TypedField<rpg::SaveEventExecState, int32_t> static_wait_time(
	&rpg::SaveEventExecState::wait_time,
	LSD_Reader::ChunkSaveEventExecState::wait_time,
	"wait_time",
	0,
	0
);
static TypedField<rpg::SaveEventExecState, int32_t> static_keyinput_time_variable(
	&rpg::SaveEventExecState::keyinput_time_variable,
	LSD_Reader::ChunkSaveEventExecState::keyinput_time_variable,
	"keyinput_time_variable",
	0,
	0
);
static TypedField<rpg::SaveEventExecState, int32_t> static_keyinput_2k3down(
	&rpg::SaveEventExecState::keyinput_2k3down,
	LSD_Reader::ChunkSaveEventExecState::keyinput_2k3down,
	"keyinput_2k3down",
	0,
	0
);
static TypedField<rpg::SaveEventExecState, int32_t> static_keyinput_2k3left(
	&rpg::SaveEventExecState::keyinput_2k3left,
	LSD_Reader::ChunkSaveEventExecState::keyinput_2k3left,
	"keyinput_2k3left",
	0,
	0
);
static TypedField<rpg::SaveEventExecState, int32_t> static_keyinput_2k3right(
	&rpg::SaveEventExecState::keyinput_2k3right,
	LSD_Reader::ChunkSaveEventExecState::keyinput_2k3right,
	"keyinput_2k3right",
	0,
	0
);
static TypedField<rpg::SaveEventExecState, int32_t> static_keyinput_2k3up(
	&rpg::SaveEventExecState::keyinput_2k3up,
	LSD_Reader::ChunkSaveEventExecState::keyinput_2k3up,
	"keyinput_2k3up",
	0,
	0
);
static TypedField<rpg::SaveEventExecState, bool> static_keyinput_timed(
	&rpg::SaveEventExecState::keyinput_timed,
	LSD_Reader::ChunkSaveEventExecState::keyinput_timed,
	"keyinput_timed",
	0,
	0
);
static TypedField<rpg::SaveEventExecState, bool> static_wait_key_enter(
	&rpg::SaveEventExecState::wait_key_enter,
	LSD_Reader::ChunkSaveEventExecState::wait_key_enter,
	"wait_key_enter",
	0,
	0
);
static TypedField<rpg::SaveEventExecState, bool> static_easyrpg_active(
	&rpg::SaveEventExecState::easyrpg_active,
	LSD_Reader::ChunkSaveEventExecState::easyrpg_active,
	"easyrpg_active",
	0,
	0
);
static TypedField<rpg::SaveEventExecState, DBString> static_easyrpg_string(
	&rpg::SaveEventExecState::easyrpg_string,
	LSD_Reader::ChunkSaveEventExecState::easyrpg_string,
	"easyrpg_string",
	0,
	0
);
static TypedField<rpg::SaveEventExecState, std::vector<int32_t>> static_easyrpg_parameters(
	&rpg::SaveEventExecState::easyrpg_parameters,
	LSD_Reader::ChunkSaveEventExecState::easyrpg_parameters,
	"easyrpg_parameters",
	0,
	0
);


template <>
Field<rpg::SaveEventExecState> const* Struct<rpg::SaveEventExecState>::fields[] = {
	&static_stack,
	&static_show_message,
	&static_abort_on_escape,
	&static_wait_movement,
	&static_keyinput_wait,
	&static_keyinput_variable,
	&static_keyinput_all_directions,
	&static_keyinput_decision,
	&static_keyinput_cancel,
	&static_keyinput_2kshift_2k3numbers,
	&static_keyinput_2kdown_2k3operators,
	&static_keyinput_2kleft_2k3shift,
	&static_keyinput_2kright,
	&static_keyinput_2kup,
	&static_wait_time,
	&static_keyinput_time_variable,
	&static_keyinput_2k3down,
	&static_keyinput_2k3left,
	&static_keyinput_2k3right,
	&static_keyinput_2k3up,
	&static_keyinput_timed,
	&static_wait_key_enter,
	&static_easyrpg_active,
	&static_easyrpg_string,
	&static_easyrpg_parameters,
	NULL
};

template class Struct<rpg::SaveEventExecState>;

} //namespace lcf
