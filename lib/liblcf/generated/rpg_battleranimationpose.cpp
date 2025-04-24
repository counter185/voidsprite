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
#include "lcf/rpg/battleranimationpose.h"

namespace lcf {
namespace rpg {

std::ostream& operator<<(std::ostream& os, const BattlerAnimationPose& obj) {
	os << "BattlerAnimationPose{";
	os << "name="<< obj.name;
	os << ", battler_name="<< obj.battler_name;
	os << ", battler_index="<< obj.battler_index;
	os << ", animation_type="<< obj.animation_type;
	os << ", battle_animation_id="<< obj.battle_animation_id;
	os << "}";
	return os;
}

} // namespace rpg
} // namespace lcf
