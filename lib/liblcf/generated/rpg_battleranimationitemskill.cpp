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
#include "lcf/rpg/battleranimationitemskill.h"

namespace lcf {
namespace rpg {

std::ostream& operator<<(std::ostream& os, const BattlerAnimationItemSkill& obj) {
	os << "BattlerAnimationItemSkill{";
	os << "unknown02="<< obj.unknown02;
	os << ", type="<< obj.type;
	os << ", weapon_animation_id="<< obj.weapon_animation_id;
	os << ", movement="<< obj.movement;
	os << ", after_image="<< obj.after_image;
	os << ", attacks="<< obj.attacks;
	os << ", ranged="<< obj.ranged;
	os << ", ranged_animation_id="<< obj.ranged_animation_id;
	os << ", ranged_speed="<< obj.ranged_speed;
	os << ", battle_animation_id="<< obj.battle_animation_id;
	os << ", pose="<< obj.pose;
	os << "}";
	return os;
}

} // namespace rpg
} // namespace lcf
