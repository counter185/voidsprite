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

#include "lcf/rpg/eventcommand.h"
#include "lcf/rpg/movecommand.h"
#include "lcf/rpg/animationtiming.h"
#include "lcf/rpg/animation.h"
#include "lcf/rpg/attribute.h"
#include "lcf/rpg/battlecommand.h"
#include "lcf/rpg/battlecommands.h"
#include "lcf/rpg/battleranimation.h"
#include "lcf/rpg/battleranimationitemskill.h"
#include "lcf/rpg/battleranimationpose.h"
#include "lcf/rpg/chipset.h"
#include "lcf/rpg/commonevent.h"
#include "lcf/rpg/skill.h"
#include "lcf/rpg/item.h"
#include "lcf/rpg/enemyaction.h"
#include "lcf/rpg/terrain.h"
#include "lcf/rpg/state.h"
#include "lcf/rpg/system.h"
#include "lcf/rpg/eventpagecondition.h"
#include "lcf/rpg/eventpage.h"
#include "lcf/rpg/map.h"
#include "lcf/rpg/savesystem.h"
#include "lcf/rpg/savepicture.h"
#include "lcf/rpg/savepartylocation.h"
#include "lcf/rpg/savevehiclelocation.h"
#include "lcf/rpg/saveactor.h"
#include "lcf/rpg/mapinfo.h"
#include "lcf/rpg/treemap.h"

namespace lcf {
namespace rpg {
constexpr decltype(MoveCommand::kCodeTags) MoveCommand::kCodeTags;
constexpr decltype(AnimationTiming::kFlashScopeTags) AnimationTiming::kFlashScopeTags;
constexpr decltype(AnimationTiming::kScreenShakeTags) AnimationTiming::kScreenShakeTags;
constexpr decltype(Animation::kScopeTags) Animation::kScopeTags;
constexpr decltype(Animation::kPositionTags) Animation::kPositionTags;
constexpr decltype(Attribute::kTypeTags) Attribute::kTypeTags;
constexpr decltype(BattleCommand::kTypeTags) BattleCommand::kTypeTags;
constexpr decltype(BattleCommands::kPlacementTags) BattleCommands::kPlacementTags;
constexpr decltype(BattleCommands::kRowShownTags) BattleCommands::kRowShownTags;
constexpr decltype(BattleCommands::kBattleTypeTags) BattleCommands::kBattleTypeTags;
constexpr decltype(BattleCommands::kWindowSizeTags) BattleCommands::kWindowSizeTags;
constexpr decltype(BattleCommands::kTransparencyTags) BattleCommands::kTransparencyTags;
constexpr decltype(BattleCommands::kFacingTags) BattleCommands::kFacingTags;
constexpr decltype(BattlerAnimation::kPoseTags) BattlerAnimation::kPoseTags;
constexpr decltype(BattlerAnimationItemSkill::kSpeedTags) BattlerAnimationItemSkill::kSpeedTags;
constexpr decltype(BattlerAnimationItemSkill::kAnimTypeTags) BattlerAnimationItemSkill::kAnimTypeTags;
constexpr decltype(BattlerAnimationItemSkill::kMovementTags) BattlerAnimationItemSkill::kMovementTags;
constexpr decltype(BattlerAnimationItemSkill::kAfterimageTags) BattlerAnimationItemSkill::kAfterimageTags;
constexpr decltype(BattlerAnimationPose::kAnimTypeTags) BattlerAnimationPose::kAnimTypeTags;
constexpr decltype(Chipset::kAnimTypeTags) Chipset::kAnimTypeTags;
constexpr decltype(Skill::kTypeTags) Skill::kTypeTags;
constexpr decltype(Skill::kSpTypeTags) Skill::kSpTypeTags;
constexpr decltype(Skill::kScopeTags) Skill::kScopeTags;
constexpr decltype(Skill::kHpTypeTags) Skill::kHpTypeTags;
constexpr decltype(Item::kTypeTags) Item::kTypeTags;
constexpr decltype(Item::kTrajectoryTags) Item::kTrajectoryTags;
constexpr decltype(Item::kTargetTags) Item::kTargetTags;
constexpr decltype(EnemyAction::kKindTags) EnemyAction::kKindTags;
constexpr decltype(EnemyAction::kBasicTags) EnemyAction::kBasicTags;
constexpr decltype(EnemyAction::kConditionTypeTags) EnemyAction::kConditionTypeTags;
constexpr decltype(Terrain::kBushDepthTags) Terrain::kBushDepthTags;
constexpr decltype(Terrain::kBGAssociationTags) Terrain::kBGAssociationTags;
constexpr decltype(State::kPersistenceTags) State::kPersistenceTags;
constexpr decltype(State::kRestrictionTags) State::kRestrictionTags;
constexpr decltype(State::kAffectTypeTags) State::kAffectTypeTags;
constexpr decltype(State::kChangeTypeTags) State::kChangeTypeTags;
constexpr decltype(System::kFadeOutTags) System::kFadeOutTags;
constexpr decltype(System::kFadeInTags) System::kFadeInTags;
constexpr decltype(System::kStretchTags) System::kStretchTags;
constexpr decltype(System::kFontTags) System::kFontTags;
constexpr decltype(System::kBattleFormationTags) System::kBattleFormationTags;
constexpr decltype(System::kBattleConditionTags) System::kBattleConditionTags;
constexpr decltype(System::kEquipmentSettingTags) System::kEquipmentSettingTags;
constexpr decltype(EventPageCondition::kComparisonTags) EventPageCondition::kComparisonTags;
constexpr decltype(EventPage::kDirectionTags) EventPage::kDirectionTags;
constexpr decltype(EventPage::kFrameTags) EventPage::kFrameTags;
constexpr decltype(EventPage::kMoveTypeTags) EventPage::kMoveTypeTags;
constexpr decltype(EventPage::kTriggerTags) EventPage::kTriggerTags;
constexpr decltype(EventPage::kLayersTags) EventPage::kLayersTags;
constexpr decltype(EventPage::kAnimTypeTags) EventPage::kAnimTypeTags;
constexpr decltype(Map::kScrollTypeTags) Map::kScrollTypeTags;
constexpr decltype(Map::kGeneratorModeTags) Map::kGeneratorModeTags;
constexpr decltype(Map::kGeneratorTilesTags) Map::kGeneratorTilesTags;
constexpr decltype(SaveSystem::kSceneTags) SaveSystem::kSceneTags;
constexpr decltype(SaveSystem::kAtbModeTags) SaveSystem::kAtbModeTags;
constexpr decltype(SavePicture::kEffectTags) SavePicture::kEffectTags;
constexpr decltype(SavePicture::kMapLayerTags) SavePicture::kMapLayerTags;
constexpr decltype(SavePicture::kBattleLayerTags) SavePicture::kBattleLayerTags;
constexpr decltype(SavePicture::kEasyRpgFlipTags) SavePicture::kEasyRpgFlipTags;
constexpr decltype(SavePicture::kEasyRpgTypeTags) SavePicture::kEasyRpgTypeTags;
constexpr decltype(SavePartyLocation::kVehicleTypeTags) SavePartyLocation::kVehicleTypeTags;
constexpr decltype(SavePartyLocation::kPanStateTags) SavePartyLocation::kPanStateTags;
constexpr decltype(SaveVehicleLocation::kVehicleTypeTags) SaveVehicleLocation::kVehicleTypeTags;
constexpr decltype(SaveActor::kRowTypeTags) SaveActor::kRowTypeTags;
constexpr decltype(MapInfo::kMusicTypeTags) MapInfo::kMusicTypeTags;
constexpr decltype(MapInfo::kBGMTypeTags) MapInfo::kBGMTypeTags;
constexpr decltype(MapInfo::kTriStateTags) MapInfo::kTriStateTags;
constexpr decltype(TreeMap::kMapTypeTags) TreeMap::kMapTypeTags;

} // namespace rpg
} // namespace lcf
