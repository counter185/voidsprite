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

/*
 * Headers
 */
#include "lcf/lmu/reader.h"
#include "lcf/lmu/chunks.h"
#include "reader_struct.h"

namespace lcf {

// Read EventPageCondition.

template <>
char const* const Flags<rpg::EventPageCondition::Flags>::name = "EventPageCondition_Flags";

template <>
decltype(Flags<rpg::EventPageCondition::Flags>::flag_names) Flags<rpg::EventPageCondition::Flags>::flag_names = {
	"switch_a",
	"switch_b",
	"variable",
	"item",
	"actor",
	"timer",
	"timer2"
};

template <>
decltype(Flags<rpg::EventPageCondition::Flags>::flags_is2k3) Flags<rpg::EventPageCondition::Flags>::flags_is2k3 = {
	0,
	0,
	0,
	0,
	0,
	0,
	1
};

} //namespace lcf
