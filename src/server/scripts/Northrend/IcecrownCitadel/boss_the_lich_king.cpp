/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "ObjectMgr.h"
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "SpellScript.h"
#include "SpellAuraEffects.h"
#include "Spell.h"
#include "Vehicle.h"
#include "Cell.h"
#include "CellImpl.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "icecrown_citadel.h"

enum Texts
{
    // The Lich King
    SAY_LK_INTRO_1                  = 0,
    SAY_LK_INTRO_2                  = 1,
    SAY_LK_INTRO_3                  = 2,
    SAY_LK_REMORSELESS_WINTER       = 4,
    SAY_LK_QUAKE                    = 5,
    SAY_LK_SUMMON_VALKYR            = 6,
    SAY_LK_HARVEST_SOUL             = 7,
    SAY_LK_FROSTMOURNE_ESCAPE       = 8,    // not said on heroic
    SAY_LK_FROSTMOURNE_KILL         = 9,    // not said on heroic
    SAY_LK_KILL                     = 10,
    SAY_LK_BERSERK                  = 11,
    EMOTE_DEFILE_WARNING            = 12,
    EMOTE_NECROTIC_PLAGUE_WARNING   = 13,
    SAY_LK_OUTRO_1                  = 14,
    SAY_LK_OUTRO_2                  = 15,
    SAY_LK_OUTRO_3                  = 16,
    SAY_LK_OUTRO_4                  = 17,
    SAY_LK_OUTRO_5                  = 18,
    SAY_LK_OUTRO_6                  = 19,
    SAY_LK_OUTRO_7                  = 20,
    SAY_LK_OUTRO_8                  = 21,

    // Highlord Tirion Fordring
    SAY_TIRION_INTRO_1              = 0,
    SAY_TIRION_INTRO_2              = 1,
    SAY_TIRION_OUTRO_1              = 2,
    SAY_TIRION_OUTRO_2              = 3,

    // Terenas Menethil (outro)
    SAY_TERENAS_OUTRO_1             = 0,
    SAY_TERENAS_OUTRO_2             = 1,

    // Terenas Menethil (Frostmourne)
    SAY_TERENAS_INTRO_1             = 0,
    SAY_TERENAS_INTRO_2             = 1,
    SAY_TERENAS_INTRO_3             = 2,
};

enum Spells
{
    // The Lich King
    SPELL_PLAGUE_AVOIDANCE              = 72846,    // raging spirits also get it
    SPELL_EMOTE_SIT_NO_SHEATH           = 73220,
    SPELL_BOSS_HITTIN_YA                = 73878,
    SPELL_EMOTE_SHOUT_NO_SHEATH         = 73213,
    SPELL_ICE_LOCK                      = 71614,

    // Phase 1
    SPELL_SUMMON_SHAMBLING_HORROR       = 70372,
    SPELL_RISEN_WITCH_DOCTOR_SPAWN      = 69639,
    SPELL_SUMMON_DRUDGE_GHOULS          = 70358,
    SPELL_INFEST                        = 70541,
    SPELL_NECROTIC_PLAGUE               = 70337,
    SPELL_NECROTIC_PLAGUE_JUMP          = 70338,
    SPELL_PLAGUE_SIPHON                 = 74074,
    SPELL_SHADOW_TRAP                   = 73539,
    SPELL_SHADOW_TRAP_AURA              = 73525,
    SPELL_SHADOW_TRAP_KNOCKBACK         = 73529,

    // Phase Transition
    SPELL_REMORSELESS_WINTER_1          = 68981,
    SPELL_REMORSELESS_WINTER_2          = 72259,
    SPELL_PAIN_AND_SUFFERING            = 72133,
    SPELL_SUMMON_ICE_SPHERE             = 69104,
    SPELL_ICE_SPHERE                    = 69090,
    SPELL_ICE_BURST_TARGET_SEARCH       = 69109,
    SPELL_ICE_PULSE                     = 69091,
    SPELL_ICE_BURST                     = 69108,
    SPELL_RAGING_SPIRIT                 = 69200,
    SPELL_RAGING_SPIRIT_VISUAL          = 69197,
    SPELL_RAGING_SPIRIT_VISUAL_CLONE    = 69198,
    SPELL_SOUL_SHRIEK                   = 69242,
    SPELL_QUAKE                         = 72262,

    // Phase 2
    SPELL_DEFILE                        = 72762,
    SPELL_DEFILE_AURA                   = 72743,
    SPELL_DEFILE_GROW                   = 72756,
    SPELL_SUMMON_VALKYR                 = 69037,
    SPELL_SUMMON_VALKYR_PERIODIC        = 74361,
    SPELL_HARVEST_SOUL_VALKYR           = 68985,    // Val'kyr Shadowguard vehicle aura
    SPELL_SOUL_REAPER                   = 69409,
    SPELL_SOUL_REAPER_BUFF              = 69410,
    SPELL_WINGS_OF_THE_DAMNED           = 74352,
    SPELL_VALKYR_TARGET_SEARCH          = 69030,
    SPELL_CHARGE                        = 74399,    // cast on selected target
    SPELL_VALKYR_CARRY                  = 74445,    // removes unselectable flag
    SPELL_LIFE_SIPHON                   = 73488,
    SPELL_LIFE_SIPHON_HEAL              = 73489,
    SPELL_EJECT_ALL_PASSENGERS          = 68576,

    // Phase 3
    SPELL_VILE_SPIRITS                  = 70498,
    SPELL_VILE_SPIRIT_MOVE_SEARCH       = 70501,
    SPELL_VILE_SPIRIT_DAMAGE_SEARCH     = 70502,
    SPELL_SPIRIT_BURST                  = 70503,
    SPELL_HARVEST_SOUL                  = 68980,
    SPELL_HARVEST_SOULS                 = 73654,    // Heroic version, weird because it has all 4 difficulties just like above spell
    SPELL_HARVEST_SOUL_VEHICLE          = 68984,
    SPELL_HARVEST_SOUL_VISUAL           = 71372,
    SPELL_HARVEST_SOUL_TELEPORT         = 72546,
    SPELL_HARVEST_SOULS_TELEPORT        = 73655,
    SPELL_HARVEST_SOUL_TELEPORT_BACK    = 72597,
    SPELL_IN_FROSTMOURNE_ROOM           = 74276,
    SPELL_KILL_FROSTMOURNE_PLAYERS      = 75127,
    SPELL_HARVESTED_SOUL                = 72679,
    SPELL_TRIGGER_VILE_SPIRIT_HEROIC    = 73582,

    // Frostmourne
    SPELL_LIGHTS_FAVOR                  = 69382,
    SPELL_RESTORE_SOUL                  = 72595,
    SPELL_RESTORE_SOULS                 = 73650,    // Heroic
    SPELL_DARK_HUNGER                   = 69383,    // Passive proc healing
    SPELL_DESTROY_SOUL                  = 74086,    // Used when Terenas Menethil dies
    SPELL_SOUL_RIP                      = 69397,    // Deals increasing damage
    SPELL_SOUL_RIP_DAMAGE               = 69398,
    SPELL_TERENAS_LOSES_INSIDE          = 72572,
    SPELL_SUMMON_SPIRIT_BOMB_1          = 73581,    // (Heroic)
    SPELL_SUMMON_SPIRIT_BOMB_2          = 74299,    // (Heroic)
    SPELL_EXPLOSION                     = 73576,    // Spirit Bomb (Heroic)

    // Outro
    SPELL_FURY_OF_FROSTMOURNE           = 72350,
    SPELL_FURY_OF_FROSTMOURNE_NO_REZ    = 72351,
    SPELL_EMOTE_QUESTION_NO_SHEATH      = 73330,
    SPELL_RAISE_DEAD                    = 71769,
    SPELL_LIGHTS_BLESSING               = 71797,
    SPELL_JUMP                          = 71809,
    SPELL_JUMP_TRIGGERED                = 71811,
    SPELL_JUMP_2                        = 72431,
    SPELL_SUMMON_BROKEN_FROSTMOURNE     = 74081,    // visual
    SPELL_SUMMON_BROKEN_FROSTMOURNE_2   = 72406,    // animation
    SPELL_SUMMON_BROKEN_FROSTMOURNE_3   = 73017,    // real summon
    SPELL_BROKEN_FROSTMOURNE            = 72398,
    SPELL_BROKEN_FROSTMOURNE_KNOCK      = 72405,
    SPELL_SOUL_BARRAGE                  = 72305,
    SPELL_SUMMON_TERENAS                = 72420,
    SPELL_MASS_RESURRECTION             = 72429,
    SPELL_MASS_RESURRECTION_REAL        = 72423,
    SPELL_PLAY_MOVIE                    = 73159,

    // Shambling Horror
    SPELL_SHOCKWAVE                     = 72149,
    SPELL_ENRAGE                        = 72143,
    SPELL_FRENZY                        = 28747,
};

#define NECROTIC_PLAGUE_LK   RAID_MODE<uint32>(70337, 73912, 73913, 73914)
#define NECROTIC_PLAGUE_PLR  RAID_MODE<uint32>(70338, 73785, 73786, 73787)
#define REMORSELESS_WINTER_1 RAID_MODE<uint32>(68981, 74270, 74271, 74272)
#define REMORSELESS_WINTER_2 RAID_MODE<uint32>(72259, 74273, 74274, 74275)
#define SUMMON_VALKYR        RAID_MODE<uint32>(69037, 74361, 69037, 74361)
#define HARVEST_SOUL         RAID_MODE<uint32>(68980, 74325, 74296, 74297)

enum Events
{
    // The Lich King
    // intro events
    EVENT_INTRO_MOVE_1              = 1,
    EVENT_INTRO_MOVE_2              = 2,
    EVENT_INTRO_MOVE_3              = 3,
    EVENT_INTRO_TALK_1              = 4,
    EVENT_EMOTE_CAST_SHOUT          = 5,
    EVENT_INTRO_EMOTE_1             = 6,
    EVENT_INTRO_CHARGE              = 7,
    EVENT_INTRO_CAST_FREEZE         = 8,
    EVENT_FINISH_INTRO              = 9,

    // combat events
    EVENT_SUMMON_SHAMBLING_HORROR   = 10,
    EVENT_SUMMON_DRUDGE_GHOUL       = 11,
    EVENT_INFEST                    = 12,
    EVENT_NECROTIC_PLAGUE           = 13,
    EVENT_SHADOW_TRAP               = 14,   // heroic only
    EVENT_SOUL_REAPER               = 15,
    EVENT_DEFILE                    = 16,
    EVENT_HARVEST_SOUL              = 17,   // normal mode only
    EVENT_PAIN_AND_SUFFERING        = 18,
    EVENT_SUMMON_ICE_SPHERE         = 19,
    EVENT_SUMMON_RAGING_SPIRIT      = 20,
    EVENT_QUAKE                     = 21,
    EVENT_SUMMON_VALKYR             = 22,
    EVENT_GRAB_PLAYER               = 23,
    EVENT_MOVE_TO_DROP_POS          = 24,
    EVENT_LIFE_SIPHON               = 25,   // heroic only
    EVENT_START_ATTACK              = 26,
    EVENT_QUAKE_2                   = 27,
    EVENT_VILE_SPIRITS              = 28,
    EVENT_HARVEST_SOULS             = 29,   // heroic only
    EVENT_WICKED_SPIRITS            = 30,
    EVENT_SOUL_RIP                  = 31,
    EVENT_DESTROY_SOUL              = 32,
    EVENT_FROSTMOURNE_TALK_1        = 33,
    EVENT_FROSTMOURNE_TALK_2        = 34,
    EVENT_FROSTMOURNE_TALK_3        = 35,
    EVENT_TELEPORT_BACK             = 36,
    EVENT_FROSTMOURNE_HEROIC        = 37,
    EVENT_OUTRO_TALK_1              = 38,
    EVENT_OUTRO_TALK_2              = 39,
    EVENT_OUTRO_EMOTE_TALK          = 40,
    EVENT_OUTRO_TALK_3              = 41,
    EVENT_OUTRO_MOVE_CENTER         = 42,
    EVENT_OUTRO_TALK_4              = 43,
    EVENT_OUTRO_RAISE_DEAD          = 44,
    EVENT_OUTRO_TALK_5              = 45,
    EVENT_OUTRO_BLESS               = 46,
    EVENT_OUTRO_REMOVE_ICE          = 47,
    EVENT_OUTRO_MOVE_1              = 48,
    EVENT_OUTRO_JUMP                = 49,
    EVENT_OUTRO_TALK_6              = 50,
    EVENT_OUTRO_KNOCK_BACK          = 51,
    EVENT_OUTRO_SOUL_BARRAGE        = 52,
    EVENT_OUTRO_SUMMON_TERENAS      = 53,
    EVENT_OUTRO_TERENAS_TALK_1      = 54,
    EVENT_OUTRO_TERENAS_TALK_2      = 55,
    EVENT_OUTRO_TALK_7              = 56,
    EVENT_OUTRO_TALK_8              = 57,
    EVENT_BERSERK                   = 58,

    // Shambling Horror
    EVENT_SHOCKWAVE                 = 59,
    EVENT_ENRAGE                    = 60,

    // Raging Spirit
    EVENT_SOUL_SHRIEK               = 61,

    // Strangulate Vehicle (Harvest Soul)
    EVENT_TELEPORT                  = 62,
    EVENT_MOVE_TO_LICH_KING         = 63,
    EVENT_DESPAWN_SELF              = 64,

    // Vile Spirits
    EVENT_VILE_SPIRIT_START_ATTACK  = 65,
};

enum EventGroups
{
    EVENT_GROUP_BERSERK         = 1,
    EVENT_GROUP_VILE_SPIRITS    = 2,
};

enum Phases
{
    PHASE_INTRO                 = 1,
    PHASE_ONE                   = 2,
    PHASE_TWO                   = 3,
    PHASE_THREE                 = 4,
    PHASE_TRANSITION            = 5,
    PHASE_FROSTMOURNE           = 6,    // only set on heroic mode when all players are sent into frostmourne
    PHASE_OUTRO                 = 7,

    PHASE_MASK_INTRO            = 1 << PHASE_INTRO,
    PHASE_MASK_ONE              = 1 << PHASE_ONE,
    PHASE_MASK_TWO              = 1 << PHASE_TWO,
    PHASE_MASK_THREE            = 1 << PHASE_THREE,
    PHASE_MASK_TRANSITION       = 1 << PHASE_TRANSITION,
    PHASE_MASK_NO_CAST_CHECK    = (1 << PHASE_TRANSITION) | (1 << PHASE_FROSTMOURNE) | (1 << PHASE_OUTRO),
    PHASE_MASK_FROSTMOURNE      = 1 << PHASE_FROSTMOURNE,
    PHASE_MASK_OUTRO            = 1 << PHASE_OUTRO,
    PHASE_MASK_NO_VICTIM        = (1 << PHASE_INTRO) | (1 << PHASE_OUTRO) | (1 << PHASE_FROSTMOURNE),
};

#define PHASE_TWO_THREE  (events.GetPhaseMask() & PHASE_MASK_TWO ? PHASE_TWO : PHASE_THREE)

Position const CenterPosition     = {503.6282f, -2124.655f, 840.8569f, 0.0f};
Position const TirionIntro        = {489.2970f, -2124.840f, 840.8569f, 0.0f};
Position const TirionCharge       = {482.9019f, -2124.479f, 840.8570f, 0.0f};
Position const LichKingIntro[3]   =
{
    {432.0851f, -2123.673f, 864.6582f, 0.0f},
    {457.8351f, -2123.423f, 841.1582f, 0.0f},
    {465.0730f, -2123.470f, 840.8569f, 0.0f},
};
Position const OutroPosition1     = {493.6286f, -2124.569f, 840.8569f, 0.0f};
Position const OutroFlying        = {508.9897f, -2124.561f, 845.3565f, 0.0f};
Position const TerenasSpawn       = {495.5542f, -2517.012f, 1050.000f, 4.6993f};
Position const TerenasSpawnHeroic = {495.7080f, -2523.760f, 1050.000f, 0.0f};
Position const SpiritWardenSpawn  = {495.3406f, -2529.983f, 1050.000f, 1.5592f};

enum MovePoints
{
    POINT_CENTER_1          = 1,
    POINT_CENTER_2          = 2,
    POINT_TIRION_INTRO      = 3,
    POINT_LK_INTRO_1        = 4,
    POINT_LK_INTRO_2        = 5,
    POINT_LK_INTRO_3        = 6,
    POINT_TIRION_CHARGE     = 7,
    POINT_DROP_PLAYER       = 8,
    POINT_LK_OUTRO_1        = 9,
    POINT_TIRION_OUTRO_1    = 10,
    POINT_OUTRO_JUMP        = 11,
    POINT_LK_OUTRO_2        = 12,
    POINT_GROUND            = 13,
    POINT_CHARGE            = 1003, // globally used number for charge spell effects
};

enum EncounterActions
{
    ACTION_START_ENCOUNTER      = 0,
    ACTION_CONTINUE_INTRO       = 1,
    ACTION_START_ATTACK         = 2,
    ACTION_OUTRO                = 3,
    ACTION_PLAY_MUSIC           = 4,
    ACTION_BREAK_FROSTMOURNE    = 5,
    ACTION_SUMMON_TERENAS       = 6,
    ACTION_FINISH_OUTRO         = 7,
    ACTION_TELEPORT_BACK        = 8,
    ACTION_START_ATTACK_1       = 9,
    ACTION_START_ATTACK_2       = 10,
};

enum MiscData
{
    LIGHT_SNOWSTORM             = 2490,
    LIGHT_SOULSTORM             = 2508,

    MUSIC_FROZEN_THRONE         = 17457,
    MUSIC_SPECIAL               = 17458,    // Summon Shambling Horror, Remorseless Winter, Quake, Summon Val'kyr Periodic, Harvest Soul, Vile Spirits
    MUSIC_FURY_OF_FROSTMOURNE   = 17459,
    MUSIC_FINAL                 = 17460,    // Raise Dead, Light's Blessing

    SOUND_PAIN                  = 17360,    // separate sound, not attached to any text

    EQUIP_ASHBRINGER_GLOWING    = 50442,
    EQUIP_BROKEN_FROSTMOURNE    = 50840,

    MOVIE_FALL_OF_THE_LICH_KING = 16,
};

#define DATA_PLAGUE_STACK 70337
#define DATA_VILE 45814622

class NecroticPlagueTargetCheck : public std::unary_function<Unit*, bool>
{
    public:
        NecroticPlagueTargetCheck(Unit const* obj, uint32 notAura1 = 0, uint32 notAura2 = 0)
            : _sourceObj(obj), _notAura1(notAura1), _notAura2(notAura2)
        {
        }

        bool operator()(Unit* unit) const
        {
            if (!unit || unit == _sourceObj || !unit->isTargetableForAttack() || unit->isTotem() || unit->HasAura(SPELL_PLAGUE_AVOIDANCE))
                return false;
            if ((_notAura1 && unit->HasAura(_notAura1)) || (_notAura2 && unit->HasAura(_notAura2)))
                return false;
            return true;
        }

    private:
        Unit const* _sourceObj;
        uint32 _notAura1;
        uint32 _notAura2;
};

class HeightDifferenceCheck
{
    public:
        HeightDifferenceCheck(GameObject* go, float diff, bool reverse)
            : _baseObject(go), _difference(diff), _reverse(reverse)
        {
        }

        bool operator()(Unit* unit) const
        {
            return (unit->GetPositionZ() - _baseObject->GetPositionZ() > _difference) != _reverse;
        }

    private:
        GameObject* _baseObject;
        float _difference;
        bool _reverse;
};

class FrozenThroneResetWorker
{
    public:
        FrozenThroneResetWorker() { }

        bool operator()(GameObject* go)
        {
            switch (go->GetEntry())
            {
                case GO_ARTHAS_PLATFORM:
                    go->SetDestructibleState(GO_DESTRUCTIBLE_REBUILDING);
                    break;
                case GO_DOODAD_ICECROWN_THRONEFROSTYWIND01:
                    go->SetGoState(GO_STATE_ACTIVE);
                    break;
                case GO_DOODAD_ICECROWN_SNOWEDGEWARNING01:
                case GO_DOODAD_ICECROWN_THRONEFROSTYEDGE01:
                    go->SetGoState(GO_STATE_READY);
                    break;
                case GO_DOODAD_ICESHARD_STANDING02:
                case GO_DOODAD_ICESHARD_STANDING01:
                case GO_DOODAD_ICESHARD_STANDING03:
                case GO_DOODAD_ICESHARD_STANDING04:
                    go->ResetDoorOrButton();
                    break;
                default:
                    break;
            }

            return false;
        }
};

class StartMovementEvent : public BasicEvent
{
    public:
        StartMovementEvent(Creature* summoner, Creature* owner)
            : _summoner(summoner), _owner(owner)
        {
        }

        bool Execute(uint64 /*time*/, uint32 /*diff*/)
        {
            _owner->SetReactState(REACT_AGGRESSIVE);
            if (Unit* target = _summoner->AI()->SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankTargetSelector(_summoner)))
                _owner->AI()->AttackStart(target);
            return true;
        }

    private:
        Creature* _summoner;
        Creature* _owner;
};

class IceSphereMovementEvent : public BasicEvent
{
    public:
        explicit IceSphereMovementEvent(Creature* owner, Unit* target)
            : _owner(owner), _target(target)
        {
        }

        bool Execute(uint64 /*time*/, uint32 /*diff*/)
        {
            if (!_owner)
                return true;

            if (_target && _target->isAlive() && _owner->IsValidAttackTarget(_target) && _target->GetPositionZ() > 830.0f && _target->GetInstanceId() == _owner->GetInstanceId())
            {
                _owner->GetMotionMaster()->MovePoint(1, _target->GetPositionX(), _target->GetPositionY(), _target->GetPositionZ());
                _owner->m_Events.AddEvent(new IceSphereMovementEvent(_owner, _target), _owner->m_Events.CalculateTime(500));
                return true;
            }
            else if (Unit* newTarget = _owner->AI()->SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true))
            {
                if (newTarget->isAlive() && _owner->IsValidAttackTarget(newTarget) && newTarget->GetPositionZ() > 830.0f && newTarget->GetInstanceId() == _owner->GetInstanceId())
                {
                    _owner->CastSpell(newTarget, SPELL_ICE_PULSE, false);
                    _owner->ClearUnitState(UNIT_STATE_CASTING);
                    _owner->m_Events.AddEvent(new IceSphereMovementEvent(_owner, newTarget), _owner->m_Events.CalculateTime(500));
                    return true;
                }
            }

            _owner->DespawnOrUnsummon();
            return true;
        }

    private:
        Creature* _owner;
        Unit* _target;
};

class HeightFilterValkyrTargetSelection
{
    public:
        HeightFilterValkyrTargetSelection() { }

        bool operator()(Unit* unit) const
        {
            return unit->GetPositionZ() < 830.0f;
        }
};

class boss_the_lich_king : public CreatureScript
{
    public:
        boss_the_lich_king() : CreatureScript("boss_the_lich_king") { }

        struct boss_the_lich_kingAI : public BossAI
        {
            boss_the_lich_kingAI(Creature* creature) : BossAI(creature, DATA_THE_LICH_KING)
            {
            }

            void Reset()
            {
                _Reset();
                me->SetReactState(REACT_PASSIVE);
                events.SetPhase(PHASE_INTRO);
                _necroticPlagueStack = 0;
                _vileSpiritExplosions = 0;
                SetEquipmentSlots(true);
            }

            void JustDied(Unit* /*killer*/)
            {
                _JustDied();
                DoCastAOE(SPELL_PLAY_MOVIE, false);
                me->RemoveUnitMovementFlag(MOVEMENTFLAG_LEVITATING);
                me->RemoveByteFlag(UNIT_FIELD_BYTES_1, 3, 0x03);
                me->GetMotionMaster()->MoveFall();
            }

            void EnterCombat(Unit* target)
            {
                if (!instance->CheckRequiredBosses(DATA_THE_LICH_KING, target->ToPlayer()))
                {
                    EnterEvadeMode();
                    instance->DoCastSpellOnPlayers(LIGHT_S_HAMMER_TELEPORT);
                    return;
                }

                me->setActive(true);
                DoZoneInCombat();

                events.SetPhase(PHASE_ONE);
                events.ScheduleEvent(EVENT_SUMMON_SHAMBLING_HORROR, 20000, 0, PHASE_ONE);
                events.ScheduleEvent(EVENT_SUMMON_DRUDGE_GHOUL, 10000, 0, PHASE_ONE);
                events.ScheduleEvent(EVENT_INFEST, 5000, 0, PHASE_ONE);
                events.ScheduleEvent(EVENT_NECROTIC_PLAGUE, urand(30000, 33000), 0, PHASE_ONE);
                events.ScheduleEvent(EVENT_BERSERK, 900000, EVENT_GROUP_BERSERK);
                if (IsHeroic())
                    events.ScheduleEvent(EVENT_SHADOW_TRAP, 15500, 0, PHASE_ONE);
            }

            void JustReachedHome()
            {
                _JustReachedHome();
                instance->SetBossState(DATA_THE_LICH_KING, NOT_STARTED);

                // Reset The Frozen Throne gameobjects
                FrozenThroneResetWorker reset;
                Trinity::GameObjectWorker<FrozenThroneResetWorker> worker(me, reset);
                me->VisitNearbyGridObject(333.0f, worker);

                // Reset any light override
                SendLightOverride(0, 5000);
            }

            bool CanAIAttack(Unit const* target) const
            {
                // The Lich King must not select targets in frostmourne room if he killed everyone outside and no falling targets
                return !target->HasAura(SPELL_IN_FROSTMOURNE_ROOM) && target->GetPositionZ() > 830.0f;
            }

            void EnterEvadeMode()
            {
                instance->SetBossState(DATA_THE_LICH_KING, FAIL);
                BossAI::EnterEvadeMode();
                if (Creature* tirion = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_HIGHLORD_TIRION_FORDRING)))
                    tirion->AI()->EnterEvadeMode();
                DoCastAOE(SPELL_KILL_FROSTMOURNE_PLAYERS);
                summons.DoAction(NPC_STRANGULATE_VEHICLE, ACTION_TELEPORT_BACK);
            }

            void KilledUnit(Unit* victim)
            {
                if (victim->GetTypeId() == TYPEID_PLAYER && !me->IsInEvadeMode() && !(events.GetPhaseMask() & PHASE_MASK_OUTRO))
                    Talk(SAY_LK_KILL);
            }

            void DoAction(int32 const action)
            {
                switch (action)
                {
                    case ACTION_START_ENCOUNTER:
                        instance->SetBossState(DATA_THE_LICH_KING, IN_PROGRESS);
                        Talk(SAY_LK_INTRO_1);
                        SendMusicToPlayers(MUSIC_FROZEN_THRONE);
                        // schedule talks
                        me->SetStandState(UNIT_STAND_STATE_STAND);
                        events.ScheduleEvent(EVENT_INTRO_MOVE_1, 4000);
                        break;
                    case ACTION_START_ATTACK:
                        events.ScheduleEvent(EVENT_START_ATTACK, 5000);
                        break;
                    case ACTION_PLAY_MUSIC:
                        SendMusicToPlayers(MUSIC_FINAL);
                        break;
                    case ACTION_RESTORE_LIGHT:
                        SendLightOverride(0, 5000);
                        break;
                    case ACTION_BREAK_FROSTMOURNE:
                        me->CastSpell((Unit*)NULL, SPELL_SUMMON_BROKEN_FROSTMOURNE, TRIGGERED_IGNORE_CAST_IN_PROGRESS);
                        me->CastSpell((Unit*)NULL, SPELL_SUMMON_BROKEN_FROSTMOURNE_2, TRIGGERED_IGNORE_CAST_IN_PROGRESS);
                        SetEquipmentSlots(false, EQUIP_BROKEN_FROSTMOURNE);
                        events.ScheduleEvent(EVENT_OUTRO_TALK_6, 2500, 0, PHASE_OUTRO);
                        break;
                    case ACTION_FINISH_OUTRO:
                        events.ScheduleEvent(EVENT_OUTRO_TALK_7, 7000, 0, PHASE_OUTRO);
                        events.ScheduleEvent(EVENT_OUTRO_TALK_8, 17000, 0, PHASE_OUTRO);
                        break;
                    case ACTION_TELEPORT_BACK:
                        summons.DoAction(NPC_STRANGULATE_VEHICLE, ACTION_TELEPORT_BACK);
                        if (!IsHeroic())
                            Talk(SAY_LK_FROSTMOURNE_ESCAPE);
                        else
                            DoCastAOE(SPELL_TRIGGER_VILE_SPIRIT_HEROIC);
                        break;
                    default:
                        break;
                }
            }

            uint32 GetData(uint32 type)
            {
                switch (type)
                {
                    case DATA_PLAGUE_STACK:
                        return _necroticPlagueStack;
                    case DATA_VILE:
                        return _vileSpiritExplosions;
                    default:
                        break;
                }

                return 0;
            }

            void SetData(uint32 type, uint32 value)
            {
                switch (type)
                {
                    case DATA_PLAGUE_STACK:
                        _necroticPlagueStack = std::max(value, _necroticPlagueStack);
                        break;
                    case DATA_VILE:
                        _vileSpiritExplosions += value;
                        break;
                    default:
                        break;
                }
            }

            void DamageTaken(Unit* /*attacker*/, uint32& /*damage*/)
            {
                if (events.GetPhaseMask() & PHASE_MASK_ONE && !HealthAbovePct(70))
                {
                    events.SetPhase(PHASE_TRANSITION);
                    me->GetMotionMaster()->MovePoint(POINT_CENTER_1, CenterPosition);
                    return;
                }

                if (events.GetPhaseMask() & PHASE_MASK_TWO && !HealthAbovePct(40))
                {
                    events.SetPhase(PHASE_TRANSITION);
                    me->GetMotionMaster()->MovePoint(POINT_CENTER_2, CenterPosition);
                    return;
                }

                if (events.GetPhaseMask() & PHASE_MASK_THREE && !HealthAbovePct(10))
                {
                    me->SetReactState(REACT_PASSIVE);
                    me->AttackStop();
                    events.Reset();
                    events.SetPhase(PHASE_OUTRO);
                    summons.DespawnAll();
                    SendMusicToPlayers(MUSIC_FURY_OF_FROSTMOURNE);
                    DoCastAOE(SPELL_FURY_OF_FROSTMOURNE);
                    me->AddUnitMovementFlag(MOVEMENTFLAG_WALKING);
                    events.ScheduleEvent(EVENT_OUTRO_TALK_1, 2600, 0, PHASE_OUTRO);
                    events.ScheduleEvent(EVENT_OUTRO_EMOTE_TALK, 6600, 0, PHASE_OUTRO);
                    events.ScheduleEvent(EVENT_OUTRO_EMOTE_TALK, 17600, 0, PHASE_OUTRO);
                    events.ScheduleEvent(EVENT_OUTRO_EMOTE_TALK, 27600, 0, PHASE_OUTRO);
                    events.ScheduleEvent(EVENT_OUTRO_TALK_2, 34600, 0, PHASE_OUTRO);
                    events.ScheduleEvent(EVENT_OUTRO_TALK_3, 43600, 0, PHASE_OUTRO);
                    events.ScheduleEvent(EVENT_EMOTE_CAST_SHOUT, 54600, 0, PHASE_OUTRO);
                    events.ScheduleEvent(EVENT_OUTRO_EMOTE_TALK, 58600, 0, PHASE_OUTRO);
                    events.ScheduleEvent(EVENT_OUTRO_MOVE_CENTER, 69600, 0, PHASE_OUTRO);
                    // stop here. rest will get scheduled from MovementInform
                    return;
                }
            }

            void JustSummoned(Creature* summon)
            {
                switch (summon->GetEntry())
                {
                    case NPC_SHAMBLING_HORROR:
                    case NPC_DRUDGE_GHOUL:
                        summon->CastSpell(summon, SPELL_RISEN_WITCH_DOCTOR_SPAWN, true);
                        summon->SetReactState(REACT_PASSIVE);
                        summon->HandleEmoteCommand(EMOTE_ONESHOT_EMERGE);
                        summon->m_Events.AddEvent(new StartMovementEvent(me, summon), summon->m_Events.CalculateTime(5000));
                        break;
                    case NPC_ICE_SPHERE:
                    {
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true))
                        {
                            summon->SetReactState(REACT_PASSIVE);
                            summon->CastSpell(summon, SPELL_ICE_SPHERE, false);
                            summon->CastSpell(summon, SPELL_ICE_BURST_TARGET_SEARCH, false);
                            summon->CastSpell(target, SPELL_ICE_PULSE, false);
                            summon->ClearUnitState(UNIT_STATE_CASTING);
                            summon->m_Events.AddEvent(new IceSphereMovementEvent(summon, target), summon->m_Events.CalculateTime(500));
                        }
                        else
                            summon->DespawnOrUnsummon();
                        break;
                    }
                    case NPC_DEFILE:
                        summon->SetReactState(REACT_PASSIVE);
                        summon->CastSpell(summon, SPELL_DEFILE_AURA, false);
                        break;
                    case NPC_FROSTMOURNE_TRIGGER:
                    {
                        summons.Summon(summon);
                        summon->CastSpell((Unit*)NULL, SPELL_BROKEN_FROSTMOURNE, true);

                        SendLightOverride(LIGHT_SOULSTORM, 10000);
                        SendWeather(WEATHER_STATE_BLACKSNOW);

                        events.ScheduleEvent(EVENT_OUTRO_SOUL_BARRAGE, 5000, 0, PHASE_OUTRO);
                        return;
                    }
                    case NPC_VILE_SPIRIT:
                    {
                        summons.Summon(summon);
                        if (events.GetPhaseMask() & PHASE_MASK_FROSTMOURNE)
                        {
                            TeleportSpirit(summon);
                            return;
                        }

                        summon->SetReactState(REACT_PASSIVE);
                        summon->SetSpeed(MOVE_FLIGHT, 0.5f);
                        summon->GetMotionMaster()->MoveRandom(10.0f);
                        summon->AI()->DoAction(ACTION_START_ATTACK_1);
                        return;
                    }
                    case NPC_STRANGULATE_VEHICLE:
                        summons.Summon(summon);
                        return;
                    default:
                        break;
                }

                BossAI::JustSummoned(summon);
            }

            void SummonedCreatureDies(Creature* summon, Unit* /*killer*/)
            {
                switch (summon->GetEntry())
                {
                    case NPC_SHAMBLING_HORROR:
                    case NPC_DRUDGE_GHOUL:
                    case NPC_ICE_SPHERE:
                    case NPC_VALKYR_SHADOWGUARD:
                    case NPC_RAGING_SPIRIT:
                    case NPC_VILE_SPIRIT:
                    case NPC_WICKED_SPIRIT:
                        summon->ToTempSummon()->SetTempSummonType(TEMPSUMMON_CORPSE_DESPAWN);
                        break;
                    default:
                        break;
                }
            }

            void SpellHit(Unit* /*caster*/, SpellInfo const* spell)
            {
                if (spell->Id == SPELL_HARVESTED_SOUL && me->isInCombat() && !IsHeroic())
                    Talk(SAY_LK_FROSTMOURNE_KILL);
            }

            void SpellHitTarget(Unit* /*target*/, SpellInfo const* spell)
            {
                if (spell->Id == REMORSELESS_WINTER_1 || spell->Id == REMORSELESS_WINTER_2)
                {
                    SendLightOverride(LIGHT_SNOWSTORM, 5000);
                    SendWeather(WEATHER_STATE_LIGHT_SNOW);
                }
            }

            void MovementInform(uint32 type, uint32 pointId)
            {
                if (type != POINT_MOTION_TYPE)
                    return;

                switch (pointId)
                {
                    case POINT_LK_INTRO_1:
                        // schedule for next update cycle, current update must finalize movement
                        events.ScheduleEvent(EVENT_INTRO_MOVE_2, 1, 0, PHASE_INTRO);
                        break;
                    case POINT_LK_INTRO_2:
                        events.ScheduleEvent(EVENT_INTRO_MOVE_3, 1, 0, PHASE_INTRO);
                        break;
                    case POINT_LK_INTRO_3:
                        if (Creature* tirion = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_HIGHLORD_TIRION_FORDRING)))
                            tirion->AI()->DoAction(ACTION_CONTINUE_INTRO);
                        events.ScheduleEvent(EVENT_INTRO_TALK_1, 9000, 0, PHASE_INTRO);
                        break;
                    case POINT_CENTER_1:
                        me->SetFacingTo(0.0f);
                        Talk(SAY_LK_REMORSELESS_WINTER);
                        SendMusicToPlayers(MUSIC_SPECIAL);
                        me->InterruptNonMeleeSpells(true); // Otherwise remorseless winter might fail
                        me->SetReactState(REACT_PASSIVE);
                        me->AttackStop();
                        DoCast(me, SPELL_REMORSELESS_WINTER_1);
                        events.DelayEvents(62500, EVENT_GROUP_BERSERK); // delay berserk timer, its not ticking during phase transitions
                        events.ScheduleEvent(EVENT_QUAKE, 62500, 0, PHASE_TRANSITION);
                        events.ScheduleEvent(EVENT_PAIN_AND_SUFFERING, 4000, 0, PHASE_TRANSITION);
                        events.ScheduleEvent(EVENT_SUMMON_ICE_SPHERE, 8000, 0, PHASE_TRANSITION);
                        events.ScheduleEvent(EVENT_SUMMON_RAGING_SPIRIT, 3000, 0, PHASE_TRANSITION);
                        events.ScheduleEvent(EVENT_SUMMON_VALKYR, 78000, 0, PHASE_TWO);
                        events.ScheduleEvent(EVENT_INFEST, 70000, 0, PHASE_TWO);
                        events.ScheduleEvent(EVENT_DEFILE, 97000, 0, PHASE_TWO);
                        events.ScheduleEvent(EVENT_SOUL_REAPER, 94000, 0, PHASE_TWO);
                        break;
                    case POINT_CENTER_2:
                        me->SetFacingTo(0.0f);
                        Talk(SAY_LK_REMORSELESS_WINTER);
                        SendMusicToPlayers(MUSIC_SPECIAL);
                        me->InterruptNonMeleeSpells(true); // Otherwise remorseless winter might fail
                        me->SetReactState(REACT_PASSIVE);
                        me->AttackStop();
                        DoCast(me, SPELL_REMORSELESS_WINTER_2);
                        summons.DespawnEntry(NPC_VALKYR_SHADOWGUARD);
                        events.DelayEvents(62500, EVENT_GROUP_BERSERK); // delay berserk timer, its not ticking during phase transitions
                        events.ScheduleEvent(EVENT_QUAKE_2, 62500, 0, PHASE_TRANSITION);
                        events.ScheduleEvent(EVENT_PAIN_AND_SUFFERING, 6000, 0, PHASE_TRANSITION);
                        events.ScheduleEvent(EVENT_SUMMON_ICE_SPHERE, 8000, 0, PHASE_TRANSITION);
                        events.ScheduleEvent(EVENT_SUMMON_RAGING_SPIRIT, 5000, 0, PHASE_TRANSITION);
                        events.ScheduleEvent(EVENT_DEFILE, 95500, 0, PHASE_THREE);
                        events.ScheduleEvent(EVENT_SOUL_REAPER, 99500, 0, PHASE_THREE);
                        events.ScheduleEvent(EVENT_VILE_SPIRITS, 79500, EVENT_GROUP_VILE_SPIRITS, PHASE_THREE);
                        events.ScheduleEvent(IsHeroic() ? EVENT_HARVEST_SOULS : EVENT_HARVEST_SOUL, 73500, 0, PHASE_THREE);
                        break;
                    case POINT_LK_OUTRO_1:
                        events.ScheduleEvent(EVENT_OUTRO_TALK_4, 1, 0, PHASE_OUTRO);
                        events.ScheduleEvent(EVENT_OUTRO_RAISE_DEAD, 1000, 0, PHASE_OUTRO);
                        events.ScheduleEvent(EVENT_OUTRO_TALK_5, 29000, 0, PHASE_OUTRO);
                        break;
                    case POINT_LK_OUTRO_2:
                        if (Creature* tirion = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_HIGHLORD_TIRION_FORDRING)))
                            tirion->AI()->Talk(SAY_TIRION_OUTRO_2);
                        if (Creature* frostmourne = me->FindNearestCreature(NPC_FROSTMOURNE_TRIGGER, 50.0f))
                            frostmourne->AI()->DoAction(ACTION_SUMMON_TERENAS);
                        break;
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const diff)
            {
                // check phase first to prevent updating victim and entering evade mode when not wanted
                if (!(events.GetPhaseMask() & PHASE_MASK_NO_VICTIM))
                    if (!UpdateVictim())
                        return;

                events.Update(diff);

                // during Remorseless Winter phases The Lich King is channeling a spell, but we must continue casting other spells
                if (me->HasUnitState(UNIT_STATE_CASTING) && !(events.GetPhaseMask() & PHASE_MASK_NO_CAST_CHECK))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_INTRO_MOVE_1:
                            me->SetSheath(SHEATH_STATE_MELEE);
                            me->RemoveAurasDueToSpell(SPELL_EMOTE_SIT_NO_SHEATH);
                            me->AddUnitMovementFlag(MOVEMENTFLAG_WALKING);
                            me->GetMotionMaster()->MovePoint(POINT_LK_INTRO_1, LichKingIntro[0]);
                            break;
                        case EVENT_INTRO_MOVE_2:
                            me->GetMotionMaster()->MovePoint(POINT_LK_INTRO_2, LichKingIntro[1]);
                            break;
                        case EVENT_INTRO_MOVE_3:
                            me->GetMotionMaster()->MovePoint(POINT_LK_INTRO_3, LichKingIntro[2]);
                            break;
                        case EVENT_INTRO_TALK_1:
                            Talk(SAY_LK_INTRO_2);
                            // for some reason blizz sends 2 emotes in row here so (we handle one in Talk)
                            me->HandleEmoteCommand(EMOTE_ONESHOT_TALK_NO_SHEATHE);
                            events.ScheduleEvent(EVENT_EMOTE_CAST_SHOUT, 7000, 0, PHASE_INTRO);
                            events.ScheduleEvent(EVENT_INTRO_EMOTE_1, 13000, 0, PHASE_INTRO);
                            events.ScheduleEvent(EVENT_EMOTE_CAST_SHOUT, 18000, 0, PHASE_INTRO);
                            events.ScheduleEvent(EVENT_INTRO_CAST_FREEZE, 31000, 0, PHASE_INTRO);
                            break;
                        case EVENT_EMOTE_CAST_SHOUT:
                            DoCast(me, SPELL_EMOTE_SHOUT_NO_SHEATH, false);
                            break;
                        case EVENT_INTRO_EMOTE_1:
                            me->HandleEmoteCommand(EMOTE_ONESHOT_POINT_NO_SHEATHE);
                            break;
                        case EVENT_INTRO_CAST_FREEZE:
                            Talk(SAY_LK_INTRO_3);
                            DoCastAOE(SPELL_ICE_LOCK, false);
                            events.ScheduleEvent(EVENT_FINISH_INTRO, 1000, 0, PHASE_INTRO);
                            break;
                        case EVENT_FINISH_INTRO:
                            me->RemoveUnitMovementFlag(MOVEMENTFLAG_WALKING);
                            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
                            me->SetReactState(REACT_AGGRESSIVE);
                            events.SetPhase(PHASE_ONE);
                            break;
                        case EVENT_SUMMON_SHAMBLING_HORROR:
                            DoCast(me, SPELL_SUMMON_SHAMBLING_HORROR);
                            SendMusicToPlayers(MUSIC_SPECIAL);
                            events.ScheduleEvent(EVENT_SUMMON_SHAMBLING_HORROR, 60000, 0, PHASE_ONE);
                            break;
                        case EVENT_SUMMON_DRUDGE_GHOUL:
                            DoCast(me, SPELL_SUMMON_DRUDGE_GHOULS);
                            events.ScheduleEvent(EVENT_SUMMON_DRUDGE_GHOUL, 30000, 0, PHASE_ONE);
                            break;
                        case EVENT_INFEST:
                            DoCast(me, SPELL_INFEST);
                            events.ScheduleEvent(EVENT_INFEST, urand(21000, 24000), 0, (events.GetPhaseMask() & PHASE_MASK_ONE) ? PHASE_ONE : PHASE_TWO);
                            break;
                        case EVENT_NECROTIC_PLAGUE:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1, NecroticPlagueTargetCheck(me, NECROTIC_PLAGUE_LK, NECROTIC_PLAGUE_PLR)))
                            {
                                Talk(EMOTE_NECROTIC_PLAGUE_WARNING, target->GetGUID());
                                DoCast(target, SPELL_NECROTIC_PLAGUE);
                            }
                            events.ScheduleEvent(EVENT_NECROTIC_PLAGUE, urand(30000, 33000), 0, PHASE_ONE);
                            break;
                        case EVENT_SHADOW_TRAP:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankTargetSelector(me)))
                                DoCast(target, SPELL_SHADOW_TRAP);
                            events.ScheduleEvent(EVENT_SHADOW_TRAP, 15500, 0, PHASE_ONE);
                            break;
                        case EVENT_SOUL_REAPER:
                            DoCastVictim(SPELL_SOUL_REAPER);
                            events.ScheduleEvent(EVENT_SOUL_REAPER, urand(33000, 35000), 0, PHASE_TWO_THREE);
                            break;
                        case EVENT_DEFILE:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1, 0.0f, true, -SPELL_HARVEST_SOUL_VALKYR))
                            {
                                Talk(EMOTE_DEFILE_WARNING);
                                DoCast(target, SPELL_DEFILE);
                            }
                            events.ScheduleEvent(EVENT_DEFILE, urand(32000, 35000), 0, PHASE_TWO_THREE);
                            break;
                        case EVENT_HARVEST_SOUL:
                            Talk(SAY_LK_HARVEST_SOUL);
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, SpellTargetSelector(me, SPELL_HARVEST_SOUL)))
                                DoCast(target, SPELL_HARVEST_SOUL);
                            events.ScheduleEvent(EVENT_HARVEST_SOUL, 75000, 0, PHASE_THREE);
                            break;
                        case EVENT_PAIN_AND_SUFFERING:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true))
                                me->CastSpell(target, SPELL_PAIN_AND_SUFFERING, TRIGGERED_NONE);
                            events.ScheduleEvent(EVENT_PAIN_AND_SUFFERING, urand(1500, 4000), 0, PHASE_TRANSITION);
                            break;
                        case EVENT_SUMMON_ICE_SPHERE:
                            DoCastAOE(SPELL_SUMMON_ICE_SPHERE);
                            events.ScheduleEvent(EVENT_SUMMON_ICE_SPHERE, urand(7500, 8500), 0, PHASE_TRANSITION);
                            break;
                        case EVENT_SUMMON_RAGING_SPIRIT:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true))
                                me->CastSpell(target, SPELL_RAGING_SPIRIT, TRIGGERED_NONE);
                            events.ScheduleEvent(EVENT_SUMMON_RAGING_SPIRIT, urand(22000, 23000), 0, PHASE_TRANSITION);
                            break;
                        case EVENT_QUAKE:
                            events.SetPhase(PHASE_TWO);
                            me->ClearUnitState(UNIT_STATE_CASTING);  // clear state to ensure check in DoCastAOE passes
                            DoCastAOE(SPELL_QUAKE);
                            SendMusicToPlayers(MUSIC_SPECIAL);
                            Talk(SAY_LK_QUAKE);
                            break;
                        case EVENT_QUAKE_2:
                            events.SetPhase(PHASE_THREE);
                            me->ClearUnitState(UNIT_STATE_CASTING);  // clear state to ensure check in DoCastAOE passes
                            DoCastAOE(SPELL_QUAKE);
                            SendMusicToPlayers(MUSIC_SPECIAL);
                            Talk(SAY_LK_QUAKE);
                            break;
                        case EVENT_SUMMON_VALKYR:
                            SendMusicToPlayers(MUSIC_SPECIAL);
                            Talk(SAY_LK_SUMMON_VALKYR);
                            DoCastAOE(SUMMON_VALKYR);
                            events.ScheduleEvent(EVENT_SUMMON_VALKYR, urand(45000, 50000), 0, PHASE_TWO);
                            break;
                        case EVENT_START_ATTACK:
                            me->SetReactState(REACT_AGGRESSIVE);
                            if (events.GetPhaseMask() & PHASE_MASK_FROSTMOURNE)
                                events.SetPhase(PHASE_THREE);
                            break;
                        case EVENT_VILE_SPIRITS:
                            SendMusicToPlayers(MUSIC_SPECIAL);
                            DoCastAOE(SPELL_VILE_SPIRITS);
                            events.ScheduleEvent(EVENT_VILE_SPIRITS, urand(35000, 40000), EVENT_GROUP_VILE_SPIRITS, PHASE_THREE);
                            break;
                        case EVENT_WICKED_SPIRITS:
                            DoCastAOE(SPELL_VILE_SPIRITS);
                            events.ScheduleEvent(EVENT_WICKED_SPIRITS, urand(35000, 40000), 0, PHASE_FROSTMOURNE);
                            break;
                        case EVENT_HARVEST_SOULS:
                            Talk(SAY_LK_HARVEST_SOUL);
                            DoCastAOE(SPELL_HARVEST_SOULS);
                            events.ScheduleEvent(EVENT_HARVEST_SOULS, urand(100000, 110000), 0, PHASE_THREE);
                            events.SetPhase(PHASE_FROSTMOURNE); // will stop running UpdateVictim (no evading)
                            me->SetReactState(REACT_PASSIVE);
                            me->AttackStop();
                            events.ScheduleEvent(EVENT_WICKED_SPIRITS, events.GetNextEventTime(EVENT_VILE_SPIRITS) - events.GetTimer(), 0, PHASE_FROSTMOURNE);
                            events.DelayEvents(50000, EVENT_GROUP_VILE_SPIRITS);
                            events.RescheduleEvent(EVENT_DEFILE, 50000, 0, PHASE_THREE);
                            events.RescheduleEvent(EVENT_SOUL_REAPER, urand(57000, 62000), 0, PHASE_THREE);
                            events.ScheduleEvent(EVENT_START_ATTACK, 49000);
                            events.ScheduleEvent(EVENT_FROSTMOURNE_HEROIC, 6500);
                            break;
                        case EVENT_FROSTMOURNE_HEROIC:
                            if (TempSummon* terenas = me->GetMap()->SummonCreature(NPC_TERENAS_MENETHIL_FROSTMOURNE_H, TerenasSpawnHeroic, NULL, 50000))
                            {
                                terenas->AI()->DoAction(ACTION_FROSTMOURNE_INTRO);
                                std::list<Creature*> triggers;
                                GetCreatureListWithEntryInGrid(triggers, terenas, NPC_WORLD_TRIGGER_INFINITE_AOI, 100.0f);
                                if (!triggers.empty())
                                {
                                    triggers.sort(Trinity::ObjectDistanceOrderPred(terenas, true));
                                    Unit* spawner = triggers.front();
                                    spawner->CastSpell(spawner, SPELL_SUMMON_SPIRIT_BOMB_1, true);  // summons bombs randomly
                                    spawner->CastSpell(spawner, SPELL_SUMMON_SPIRIT_BOMB_2, true);  // summons bombs on players
                                }

                                for (SummonList::iterator i = summons.begin(); i != summons.end(); ++i)
                                {
                                    Creature* summon = ObjectAccessor::GetCreature(*me, *i);
                                    if (summon && summon->GetEntry() == NPC_VILE_SPIRIT)
                                        TeleportSpirit(summon);
                                }
                            }
                            break;
                        case EVENT_OUTRO_TALK_1:
                            Talk(SAY_LK_OUTRO_1);
                            DoCastAOE(SPELL_FURY_OF_FROSTMOURNE_NO_REZ, true);
                            break;
                        case EVENT_OUTRO_TALK_2:
                            Talk(SAY_LK_OUTRO_2);
                            DoCastAOE(SPELL_EMOTE_QUESTION_NO_SHEATH);
                            break;
                        case EVENT_OUTRO_EMOTE_TALK:
                            me->HandleEmoteCommand(EMOTE_ONESHOT_TALK_NO_SHEATHE);
                            break;
                        case EVENT_OUTRO_TALK_3:
                            if (Creature* tirion = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_HIGHLORD_TIRION_FORDRING)))
                                me->SetFacingToObject(tirion);
                            Talk(SAY_LK_OUTRO_3);
                            break;
                        case EVENT_OUTRO_MOVE_CENTER:
                            me->GetMotionMaster()->MovePoint(POINT_LK_OUTRO_1, CenterPosition);
                            break;
                        case EVENT_OUTRO_TALK_4:
                            me->SetFacingTo(0.01745329f);
                            Talk(SAY_LK_OUTRO_4);
                            break;
                        case EVENT_OUTRO_RAISE_DEAD:
                            DoCastAOE(SPELL_RAISE_DEAD);
                            me->ClearUnitState(UNIT_STATE_CASTING);
                            SendMusicToPlayers(MUSIC_FINAL);
                            break;
                        case EVENT_OUTRO_TALK_5:
                            Talk(SAY_LK_OUTRO_5);
                            if (Creature* tirion = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_HIGHLORD_TIRION_FORDRING)))
                                tirion->AI()->DoAction(ACTION_OUTRO);
                            break;
                        case EVENT_OUTRO_TALK_6:
                            Talk(SAY_LK_OUTRO_6);
                            if (Creature* tirion = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_HIGHLORD_TIRION_FORDRING)))
                                tirion->SetFacingToObject(me);
                            me->CastSpell((Unit*)NULL, SPELL_SUMMON_BROKEN_FROSTMOURNE_3, TRIGGERED_IGNORE_CAST_IN_PROGRESS);
                            SetEquipmentSlots(false, EQUIP_UNEQUIP);
                            break;
                        case EVENT_OUTRO_SOUL_BARRAGE:
                            me->CastSpell((Unit*)NULL, SPELL_SOUL_BARRAGE, TRIGGERED_IGNORE_CAST_IN_PROGRESS);
                            sCreatureTextMgr->SendSound(me, SOUND_PAIN, CHAT_MSG_MONSTER_YELL, 0, TEXT_RANGE_NORMAL, TEAM_OTHER, false);
                            // set flight
                            me->AddUnitMovementFlag(MOVEMENTFLAG_LEVITATING);
                            me->SetByteFlag(UNIT_FIELD_BYTES_1, 3, 0x03);
                            me->GetMotionMaster()->MovePoint(POINT_LK_OUTRO_2, OutroFlying);
                            break;
                        case EVENT_OUTRO_TALK_7:
                            Talk(SAY_LK_OUTRO_7);
                            break;
                        case EVENT_OUTRO_TALK_8:
                            Talk(SAY_LK_OUTRO_8);
                            break;
                        case EVENT_BERSERK:
                            Talk(SAY_LK_BERSERK);
                            DoCast(me, SPELL_BERSERK2);
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

        private:

            void TeleportSpirit(Creature* summon)
            {
                float dist = me->GetObjectSize() + (15.0f - me->GetObjectSize()) * float(rand_norm());
                float angle = float(rand_norm()) * float(2.0f * M_PI);
                Position dest = TerenasSpawnHeroic;
                me->MovePosition(dest, dist, angle);
                dest.m_positionZ += 15.0f;
                summon->UpdateEntry(NPC_WICKED_SPIRIT);
                summon->SetReactState(REACT_PASSIVE);
                summon->NearTeleportTo(dest.GetPositionX(), dest.GetPositionY(), dest.GetPositionZ(), dest.GetOrientation());
                summon->SetSpeed(MOVE_FLIGHT, 0.5f);
                summon->AI()->DoAction(ACTION_START_ATTACK_2);
            }

            void SendMusicToPlayers(uint32 musicId) const
            {
                WorldPacket data(SMSG_PLAY_MUSIC, 4);
                data << uint32(musicId);
                SendPacketToPlayers(&data);
            }

            void SendLightOverride(uint32 overrideId, uint32 fadeInTime) const
            {
                WorldPacket data(SMSG_OVERRIDE_LIGHT, 12);
                data << uint32(2488);       // Light.dbc entry (map default)
                data << uint32(overrideId); // Light.dbc entry (override)
                data << uint32(fadeInTime);
                SendPacketToPlayers(&data);
            }

            void SendWeather(WeatherState weather) const
            {
                WorldPacket data(SMSG_WEATHER, 9);
                data << uint32(weather);
                data << float(0.5f);
                data << uint8(0);
                SendPacketToPlayers(&data);
            }

            // Send packet to all players in The Frozen Throne
            void SendPacketToPlayers(WorldPacket const* data) const
            {
                Map::PlayerList const& players = me->GetMap()->GetPlayers();
                if (!players.isEmpty())
                    for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
                        if (Player* player = itr->getSource())
                            if (player->GetAreaId() == AREA_THE_FROZEN_THRONE)
                                player->GetSession()->SendPacket(data);
            }

            uint32 _necroticPlagueStack;
            uint32 _vileSpiritExplosions;
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return GetIcecrownCitadelAI<boss_the_lich_kingAI>(creature);
        }
};

class npc_tirion_fordring_tft : public CreatureScript
{
    public:
        npc_tirion_fordring_tft() : CreatureScript("npc_tirion_fordring_tft") { }

        struct npc_tirion_fordringAI : public ScriptedAI
        {
            npc_tirion_fordringAI(Creature* creature) : ScriptedAI(creature),
                _instance(creature->GetInstanceScript())
            {
            }

            void Reset()
            {
                _events.Reset();
                if (_instance->GetBossState(DATA_THE_LICH_KING) == DONE)
                    me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            }

            void MovementInform(uint32 type, uint32 id)
            {
                if (type != POINT_MOTION_TYPE)
                    return;

                switch (id)
                {
                    case POINT_TIRION_INTRO:
                        me->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_READY2H);
                        if (Creature* theLichKing = ObjectAccessor::GetCreature(*me, _instance->GetData64(DATA_THE_LICH_KING)))
                            theLichKing->AI()->DoAction(ACTION_START_ENCOUNTER);
                        break;
                    case POINT_TIRION_OUTRO_1:
                        _events.ScheduleEvent(EVENT_OUTRO_JUMP, 1, 0, PHASE_OUTRO);
                        break;
                }
            }

            void DoAction(int32 const action)
            {
                switch (action)
                {
                    case ACTION_CONTINUE_INTRO:
                        Talk(SAY_TIRION_INTRO_1);
                        _events.ScheduleEvent(EVENT_INTRO_TALK_1, 34000, 0, PHASE_INTRO);
                        break;
                    case ACTION_OUTRO:
                        _events.SetPhase(PHASE_OUTRO);
                        _events.ScheduleEvent(EVENT_OUTRO_TALK_1, 7000, 0, PHASE_OUTRO);
                        _events.ScheduleEvent(EVENT_OUTRO_BLESS, 18000, 0, PHASE_OUTRO);
                        _events.ScheduleEvent(EVENT_OUTRO_REMOVE_ICE, 23000, 0, PHASE_OUTRO);
                        _events.ScheduleEvent(EVENT_OUTRO_MOVE_1, 25000, 0, PHASE_OUTRO);
                        break;
                }
            }

            void SpellHit(Unit* /*caster*/, SpellInfo const* spell)
            {
                if (spell->Id == SPELL_ICE_LOCK)
                    me->SetFacingTo(3.085098f);
                else if (spell->Id == SPELL_BROKEN_FROSTMOURNE_KNOCK)
                    SetEquipmentSlots(true);    // remove glow on ashbringer
            }

            void sGossipSelect(Player* /*player*/, uint32 sender, uint32 action)
            {
                if (me->GetCreatureInfo()->GossipMenuId == sender && !action)
                {
                    _events.SetPhase(PHASE_INTRO);
                    me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                    me->AddUnitMovementFlag(MOVEMENTFLAG_WALKING);
                    me->GetMotionMaster()->MovePoint(POINT_TIRION_INTRO, TirionIntro);
                }
            }

            void JustReachedHome()
            {
                me->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_ONESHOT_NONE);

                if (_instance->GetBossState(DATA_THE_LICH_KING) == DONE)
                    return;

                me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim() && !(_events.GetPhaseMask() & (PHASE_MASK_INTRO | PHASE_MASK_OUTRO)))
                    return;

                _events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = _events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_INTRO_TALK_1:
                            Talk(SAY_TIRION_INTRO_2);
                            _events.ScheduleEvent(EVENT_INTRO_EMOTE_1, 2000, 0, PHASE_INTRO);
                            _events.ScheduleEvent(EVENT_INTRO_CHARGE, 5000, 0, PHASE_INTRO);
                            break;
                        case EVENT_INTRO_EMOTE_1:
                            me->HandleEmoteCommand(EMOTE_ONESHOT_POINT_NO_SHEATHE);
                            break;
                        case EVENT_INTRO_CHARGE:
                            me->RemoveUnitMovementFlag(MOVEMENTFLAG_WALKING);
                            me->GetMotionMaster()->MovePoint(POINT_TIRION_CHARGE, TirionCharge);
                            break;
                        case EVENT_OUTRO_TALK_1:
                            Talk(SAY_TIRION_OUTRO_1);
                            break;
                        case EVENT_OUTRO_BLESS:
                            DoCast(me, SPELL_LIGHTS_BLESSING);
                            break;
                        case EVENT_OUTRO_REMOVE_ICE:
                            me->RemoveAurasDueToSpell(SPELL_ICE_LOCK);
                            SetEquipmentSlots(false, EQUIP_ASHBRINGER_GLOWING);
                            if (Creature* lichKing = ObjectAccessor::GetCreature(*me, _instance->GetData64(DATA_THE_LICH_KING)))
                            {
                                me->SetFacingToObject(lichKing);
                                lichKing->AI()->DoAction(ACTION_PLAY_MUSIC);
                            }
                            break;
                        case EVENT_OUTRO_MOVE_1:
                            me->GetMotionMaster()->MovePoint(POINT_TIRION_OUTRO_1, OutroPosition1);
                            break;
                        case EVENT_OUTRO_JUMP:
                            DoCastAOE(SPELL_JUMP);
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

        private:
            EventMap _events;
            InstanceScript* _instance;
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return GetIcecrownCitadelAI<npc_tirion_fordringAI>(creature);
        }
};

class npc_shambling_horror_icc : public CreatureScript
{
    public:
        npc_shambling_horror_icc() :  CreatureScript("npc_shambling_horror_icc") { }

        struct npc_shambling_horror_iccAI : public ScriptedAI
        {
            npc_shambling_horror_iccAI(Creature* creature) : ScriptedAI(creature)
            {
                _frenzied = false;
            }

            void Reset()
            {
                _events.Reset();
                _events.ScheduleEvent(EVENT_SHOCKWAVE, urand(20000, 25000));
                _events.ScheduleEvent(EVENT_ENRAGE, urand(11000, 14000));
            }

            bool CanAIAttack(Unit const* target) const
            {
                // Must not select falling targets
                return target->GetPositionZ() > 830.0f;
            }

            void DamageTaken(Unit* /*attacker*/, uint32& damage)
            {
                if (!_frenzied && IsHeroic() && me->HealthBelowPctDamaged(20, damage))
                {
                    _frenzied = true;
                    DoCast(me, SPELL_FRENZY, true);
                }
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
                    return;

                _events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = _events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_SHOCKWAVE:
                            DoCast(me, SPELL_SHOCKWAVE);
                            _events.ScheduleEvent(EVENT_SHOCKWAVE, urand(20000, 25000));
                            break;
                        case EVENT_ENRAGE:
                            DoCast(me, SPELL_ENRAGE);
                            _events.ScheduleEvent(EVENT_ENRAGE, urand(20000, 25000));
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

        private:
            EventMap _events;
            bool _frenzied;
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return GetIcecrownCitadelAI<npc_shambling_horror_iccAI>(creature);
        }
};

class npc_raging_spirit : public CreatureScript
{
    public:
        npc_raging_spirit() : CreatureScript("npc_raging_spirit") { }

        struct npc_raging_spiritAI : public ScriptedAI
        {
            npc_raging_spiritAI(Creature* creature) : ScriptedAI(creature),
                _instance(creature->GetInstanceScript())
            {
            }

            void Reset()
            {
                _events.Reset();
                _events.ScheduleEvent(EVENT_SOUL_SHRIEK, urand(12000, 15000));
                DoCast(me, SPELL_PLAGUE_AVOIDANCE, true);
                DoCast(me, SPELL_RAGING_SPIRIT_VISUAL, true);
                if (TempSummon* summon = me->ToTempSummon())
                    if (Unit* summoner = summon->GetSummoner())
                        summoner->CastSpell(me, SPELL_RAGING_SPIRIT_VISUAL_CLONE, true);
                DoCast(me, SPELL_BOSS_HITTIN_YA, true);
            }

            bool CanAIAttack(Unit const* target) const
            {
                // Must not select falling targets
                return target->GetPositionZ() > 830.0f;
            }

            void IsSummonedBy(Unit* /*summoner*/)
            {
                // player is the spellcaster so register summon manually
                if (Creature* lichKing = ObjectAccessor::GetCreature(*me, _instance->GetData64(DATA_THE_LICH_KING)))
                    lichKing->AI()->JustSummoned(me);
            }

            void JustDied(Unit* /*killer*/)
            {
                if (Creature* lichKing = ObjectAccessor::GetCreature(*me, _instance->GetData64(DATA_THE_LICH_KING)))
                    lichKing->AI()->SummonedCreatureDespawn(me);
                if (TempSummon* summon = me->ToTempSummon())
                    summon->SetTempSummonType(TEMPSUMMON_CORPSE_DESPAWN);
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
                    return;

                _events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = _events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_SOUL_SHRIEK:
                            DoCastAOE(SPELL_SOUL_SHRIEK);
                            _events.ScheduleEvent(EVENT_SOUL_SHRIEK, urand(12000, 15000));
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

        private:
            EventMap _events;
            InstanceScript* _instance;
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return GetIcecrownCitadelAI<npc_raging_spiritAI>(creature);
        }
};

class npc_valkyr_shadowguard : public CreatureScript
{
    public:
        npc_valkyr_shadowguard() : CreatureScript("npc_valkyr_shadowguard") { }

        struct npc_valkyr_shadowguardAI : public ScriptedAI
        {
            npc_valkyr_shadowguardAI(Creature* creature) : ScriptedAI(creature),
                _grabbedPlayer(0), _instance(creature->GetInstanceScript())
            {
            }

            void Reset()
            {
                _events.Reset();
                me->SetReactState(REACT_PASSIVE);
                DoCast(me, SPELL_WINGS_OF_THE_DAMNED, false);
                me->SetSpeed(MOVE_FLIGHT, 0.642857f, true);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK_DEST, true);
                _movementWasStopped = true;
            }

            void IsSummonedBy(Unit* /*summoner*/)
            {
                _events.Reset();
                _events.ScheduleEvent(EVENT_GRAB_PLAYER, 2500);
            }

            void DamageTaken(Unit* /*attacker*/, uint32& damage)
            {
                if (!IsHeroic())
                    return;

                if (!me->HasAuraType(SPELL_AURA_CONTROL_VEHICLE))
                    return;

                if (me->HealthBelowPctDamaged(50, damage))
                {
                    _events.Reset();
                    DoCastAOE(SPELL_EJECT_ALL_PASSENGERS);
                    me->GetMotionMaster()->MoveTargetedHome();
                    me->ClearUnitState(UNIT_STATE_EVADE);
                }
            }

            void JustReachedHome()
            {
                // schedule siphon life event (heroic only)
                DoZoneInCombat();
                _events.Reset();
                _events.ScheduleEvent(EVENT_LIFE_SIPHON, 2000);
            }

            void AttackStart(Unit* /*target*/)
            {
            }

            void MovementInform(uint32 type, uint32 id)
            {
                if (type != POINT_MOTION_TYPE)
                    return;

                switch (id)
                {
                    case POINT_DROP_PLAYER:
                        // On stun point motion is finalized, if position is really reached, distance is null
                        if (!me->GetDistance(_dropPoint))
                        {
                            DoCastAOE(SPELL_EJECT_ALL_PASSENGERS);
                            me->DespawnOrUnsummon(1000);
                        }
                        else
                            _movementWasStopped = true;
                        break;
                    case POINT_CHARGE:
                        if (Player* target = ObjectAccessor::GetPlayer(*me, _grabbedPlayer))
                        {
                            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                            if (GameObject* platform = ObjectAccessor::GetGameObject(*me, _instance->GetData64(DATA_ARTHAS_PLATFORM)))
                            {
                                std::list<Creature*> triggers;
                                GetCreatureListWithEntryInGrid(triggers, me, NPC_WORLD_TRIGGER, 150.0f);
                                triggers.remove_if(HeightDifferenceCheck(platform, 5.0f, true));
                                if (triggers.empty())
                                    return;

                                triggers.sort(Trinity::ObjectDistanceOrderPred(me));
                                DoCast(target, SPELL_VALKYR_CARRY);
                                _dropPoint.Relocate(triggers.front());
                                _events.ScheduleEvent(EVENT_MOVE_TO_DROP_POS, 1500);
                            }
                        }
                        else
                            me->DespawnOrUnsummon();
                        break;
                    default:
                        break;
                }
            }

            void SetGUID(uint64 guid, int32 /* = 0*/)
            {
                _grabbedPlayer = guid;
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
                    return;

                _events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = _events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_GRAB_PLAYER:
                            if (!_grabbedPlayer)
                            {
                                DoCastAOE(SPELL_VALKYR_TARGET_SEARCH);
                                _events.ScheduleEvent(EVENT_GRAB_PLAYER, 2000);
                            }
                            break;
                        case EVENT_MOVE_TO_DROP_POS:
                            if (!me->HasAuraType(SPELL_AURA_MOD_STUN) && _movementWasStopped)
                            {
                                _movementWasStopped = false;
                                me->GetMotionMaster()->MovePoint(POINT_DROP_PLAYER, _dropPoint);
                            }

                            _events.ScheduleEvent(EVENT_MOVE_TO_DROP_POS, 500);
                            break;
                        case EVENT_LIFE_SIPHON:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1))
                                DoCast(target, SPELL_LIFE_SIPHON);
                            _events.ScheduleEvent(EVENT_LIFE_SIPHON, 2500);
                            break;
                        default:
                            break;
                    }
                }

                // no melee attacks
            }

        private:
            EventMap _events;
            Position _dropPoint;
            uint64 _grabbedPlayer;
            InstanceScript* _instance;
            bool _movementWasStopped;
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return GetIcecrownCitadelAI<npc_valkyr_shadowguardAI>(creature);
        }
};

class npc_strangulate_vehicle : public CreatureScript
{
    public:
        npc_strangulate_vehicle() : CreatureScript("npc_strangulate_vehicle") { }

        struct npc_strangulate_vehicleAI : public ScriptedAI
        {
            npc_strangulate_vehicleAI(Creature* creature) : ScriptedAI(creature),
                _instance(creature->GetInstanceScript())
            {
            }

            void IsSummonedBy(Unit* summoner)
            {
                me->SetFacingToObject(summoner);
                DoCast(summoner, SPELL_HARVEST_SOUL_VEHICLE);
                _events.Reset();
                _events.ScheduleEvent(EVENT_MOVE_TO_LICH_KING, 2000);
                _events.ScheduleEvent(EVENT_TELEPORT, 6000);

                // this will let us easily access all creatures of this entry on heroic mode when its time to teleport back
                if (Creature* lichKing = ObjectAccessor::GetCreature(*me, _instance->GetData64(DATA_THE_LICH_KING)))
                    lichKing->AI()->JustSummoned(me);
            }

            void DoAction(int32 const action)
            {
                if (action != ACTION_TELEPORT_BACK)
                    return;

                // Teleport to fixed location, otherwise we might land under the map
                if (TempSummon* summ = me->ToTempSummon())
                    if (Unit* summoner = summ->GetSummoner())
                        summoner->NearTeleportTo(OutroPosition1.GetPositionX(), OutroPosition1.GetPositionY(), OutroPosition1.GetPositionZ(), OutroPosition1.GetOrientation());

                if (Creature* lichKing = ObjectAccessor::GetCreature(*me, _instance->GetData64(DATA_THE_LICH_KING)))
                    lichKing->AI()->SummonedCreatureDespawn(me);

                me->DespawnOrUnsummon(2000);
            }

            void UpdateAI(uint32 const diff)
            {
                UpdateVictim();

                _events.Update(diff);

                while (uint32 eventId = _events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_TELEPORT:
                            me->GetMotionMaster()->Clear(false);
                            me->GetMotionMaster()->MoveIdle();
                            if (TempSummon* summ = me->ToTempSummon())
                            {
                                if (Unit* summoner = summ->GetSummoner())
                                {
                                    summoner->CastSpell((Unit*)NULL, SPELL_HARVEST_SOUL_VISUAL, true);
                                    summoner->ExitVehicle(summoner);
                                    if (!IsHeroic())
                                        summoner->CastSpell(summoner, SPELL_HARVEST_SOUL_TELEPORT, true);
                                    else
                                    {
                                        summoner->CastSpell(summoner, SPELL_HARVEST_SOULS_TELEPORT, true);
                                        summoner->RemoveAurasDueToSpell(HARVEST_SOUL, 0, 0, AURA_REMOVE_BY_EXPIRE);
                                    }
                                }
                            }

                            _events.ScheduleEvent(EVENT_DESPAWN_SELF, 65000);
                            break;
                        case EVENT_MOVE_TO_LICH_KING:
                            if (Creature* lichKing = ObjectAccessor::GetCreature(*me, _instance->GetData64(DATA_THE_LICH_KING)))
                            {
                                if (me->GetExactDist(lichKing) > 10.0f)
                                {
                                    Position pos;
                                    lichKing->GetNearPosition(pos, float(rand_norm()) * 5.0f  + 7.5f, lichKing->GetAngle(me));
                                    me->GetMotionMaster()->MovePoint(0, pos);
                                }
                            }
                            break;
                        case EVENT_DESPAWN_SELF:
                            if (Creature* lichKing = ObjectAccessor::GetCreature(*me, _instance->GetData64(DATA_THE_LICH_KING)))
                                lichKing->AI()->SummonedCreatureDespawn(me);
                            me->DespawnOrUnsummon(1);
                            break;
                        default:
                            break;
                    }
                }
            }

        private:
            EventMap _events;
            InstanceScript* _instance;
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return GetIcecrownCitadelAI<npc_strangulate_vehicleAI>(creature);
        }
};

class npc_terenas_menethil : public CreatureScript
{
    public:
        npc_terenas_menethil() : CreatureScript("npc_terenas_menethil") { }

        struct npc_terenas_menethilAI : public ScriptedAI
        {
            npc_terenas_menethilAI(Creature* creature) : ScriptedAI(creature),
                _instance(creature->GetInstanceScript())
            {
            }

            bool CanAIAttack(Unit const* target) const
            {
                return target->GetEntry() != NPC_THE_LICH_KING;
            }

            void DoAction(int32 const action)
            {
                switch (action)
                {
                    case ACTION_FROSTMOURNE_INTRO:
                        me->setActive(true);
                        if (!IsHeroic())
                            me->SetHealth(me->GetMaxHealth() / 2);
                        DoCast(me, SPELL_LIGHTS_FAVOR);
                        _events.Reset();
                        _events.ScheduleEvent(EVENT_FROSTMOURNE_TALK_1, 2000, PHASE_FROSTMOURNE);
                        _events.ScheduleEvent(EVENT_FROSTMOURNE_TALK_2, 11000, PHASE_FROSTMOURNE);
                        if (!IsHeroic())
                        {
                            _events.ScheduleEvent(EVENT_DESTROY_SOUL, 60000, PHASE_FROSTMOURNE);
                            _events.ScheduleEvent(EVENT_FROSTMOURNE_TALK_3, 25000);
                        }
                        break;
                    case ACTION_TELEPORT_BACK:
                        me->CastSpell((Unit*)NULL, SPELL_RESTORE_SOUL, TRIGGERED_NONE);
                        me->DespawnOrUnsummon(3000);
                        break;
                    default:
                        break;
                }
            }

            void EnterEvadeMode()
            {
                // no running back home
                if (!me->isAlive())
                    return;

                me->DeleteThreatList();
                me->CombatStop(false);
            }

            void DamageTaken(Unit* /*attacker*/, uint32& damage)
            {
                if (damage >= me->GetHealth())
                {
                    damage = me->GetHealth() - 1;
                    if (!me->HasAura(SPELL_TERENAS_LOSES_INSIDE) && !IsHeroic())
                    {
                        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                        DoCast(SPELL_TERENAS_LOSES_INSIDE);
                        _events.ScheduleEvent(EVENT_TELEPORT_BACK, 1000);
                        if (Creature* warden = me->FindNearestCreature(NPC_SPIRIT_WARDEN, 20.0f))
                        {
                            warden->CastSpell((Unit*)NULL, SPELL_DESTROY_SOUL, TRIGGERED_NONE);
                            warden->DespawnOrUnsummon(2000);
                        }

                        me->DespawnOrUnsummon(2000);
                    }
                }
            }

            void IsSummonedBy(Unit* /*summoner*/)
            {
                _events.Reset();
                _events.SetPhase(PHASE_OUTRO);
                if (Creature* lichKing = ObjectAccessor::GetCreature(*me, _instance->GetData64(DATA_THE_LICH_KING)))
                    me->SetFacingToObject(lichKing);

                _events.ScheduleEvent(EVENT_OUTRO_TERENAS_TALK_1, 2000, 0, PHASE_OUTRO);
                _events.ScheduleEvent(EVENT_OUTRO_TERENAS_TALK_2, 14000, 0, PHASE_OUTRO);
            }

            void UpdateAI(uint32 const diff)
            {
                UpdateVictim();

                _events.Update(diff);

                while (uint32 eventId = _events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_FROSTMOURNE_TALK_1:
                            Talk(SAY_TERENAS_INTRO_1);
                            if (IsHeroic())
                                DoCastAOE(SPELL_RESTORE_SOULS);
                            break;
                        case EVENT_FROSTMOURNE_TALK_2:
                            Talk(SAY_TERENAS_INTRO_2);
                            break;
                        case EVENT_FROSTMOURNE_TALK_3:
                            Talk(SAY_TERENAS_INTRO_3);
                            break;
                        case EVENT_OUTRO_TERENAS_TALK_1:
                            Talk(SAY_TERENAS_OUTRO_1);
                            break;
                        case EVENT_OUTRO_TERENAS_TALK_2:
                            Talk(SAY_TERENAS_OUTRO_2);
                            DoCastAOE(SPELL_MASS_RESURRECTION);
                            if (Creature* lichKing = ObjectAccessor::GetCreature(*me, _instance->GetData64(DATA_THE_LICH_KING)))
                            {
                                lichKing->AI()->DoAction(ACTION_FINISH_OUTRO);
                                lichKing->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC);
                                if (Creature* tirion = ObjectAccessor::GetCreature(*me, _instance->GetData64(DATA_HIGHLORD_TIRION_FORDRING)))
                                    tirion->AI()->AttackStart(lichKing);
                            }
                            break;
                        case EVENT_DESTROY_SOUL:
                            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                            if (Creature* warden = me->FindNearestCreature(NPC_SPIRIT_WARDEN, 20.0f))
                                warden->CastSpell((Unit*)NULL, SPELL_DESTROY_SOUL, TRIGGERED_NONE);

                            DoCast(SPELL_TERENAS_LOSES_INSIDE);
                            _events.ScheduleEvent(EVENT_TELEPORT_BACK, 1000);
                            break;
                        case EVENT_TELEPORT_BACK:
                            if (Creature* lichKing = ObjectAccessor::GetCreature(*me, _instance->GetData64(DATA_THE_LICH_KING)))
                                lichKing->AI()->DoAction(ACTION_TELEPORT_BACK);
                            break;
                        default:
                            break;
                    }
                }

                // fighting Spirit Warden
                if (me->isInCombat())
                    DoMeleeAttackIfReady();
            }

        private:
            EventMap _events;
            InstanceScript* _instance;
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return GetIcecrownCitadelAI<npc_terenas_menethilAI>(creature);
        }
};

class npc_spirit_warden : public CreatureScript
{
    public:
        npc_spirit_warden() : CreatureScript("npc_spirit_warden") { }

        struct npc_spirit_wardenAI : public ScriptedAI
        {
            npc_spirit_wardenAI(Creature* creature) : ScriptedAI(creature),
                _instance(creature->GetInstanceScript())
            {
            }

            void Reset()
            {
                _events.Reset();
                _events.ScheduleEvent(EVENT_SOUL_RIP, urand(12000, 15000));
                DoCast(SPELL_DARK_HUNGER);
            }

            void JustDied(Unit* /*killer*/)
            {
                if (Creature* terenas = ObjectAccessor::GetCreature(*me, _instance->GetData64(DATA_TERENAS_MENETHIL)))
                    terenas->AI()->DoAction(ACTION_TELEPORT_BACK);
            }

            void KilledUnit(Unit* victim)
            {
                if (victim->GetTypeId() == TYPEID_PLAYER)
                    victim->RemoveAurasDueToSpell(IsHeroic() ? SPELL_HARVEST_SOULS_TELEPORT : SPELL_HARVEST_SOUL_TELEPORT);
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
                    return;

                _events.Update(diff);

                while (uint32 eventId = _events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_SOUL_RIP:
                            DoCastVictim(SPELL_SOUL_RIP);
                            _events.ScheduleEvent(EVENT_SOUL_RIP, urand(23000, 27000));
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

        private:
            EventMap _events;
            InstanceScript* _instance;
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return GetIcecrownCitadelAI<npc_spirit_wardenAI>(creature);
        }
};

class npc_spirit_bomb : public CreatureScript
{
    public:
        npc_spirit_bomb() : CreatureScript("npc_spirit_bomb") { }

        struct npc_spirit_bombAI : public CreatureAI
        {
            npc_spirit_bombAI(Creature* creature) : CreatureAI(creature)
            {
            }

            void IsSummonedBy(Unit* /*summoner*/)
            {
                float destX, destY, destZ;
                me->GetPosition(destX, destY);
                destZ = 1055.0f;    // approximation, gets more precise later
                me->UpdateGroundPositionZ(destX, destY, destZ);
                me->GetMotionMaster()->MovePoint(POINT_GROUND, destX, destY, destZ);
            }

            void MovementInform(uint32 type, uint32 point)
            {
                if (type != POINT_MOTION_TYPE || point != POINT_GROUND)
                    return;

                me->RemoveAllAuras();
                DoCastAOE(SPELL_EXPLOSION);
                me->DespawnOrUnsummon(1000);
            }

            void AttackStart(Unit* /*victim*/)
            {
            }

            void UpdateAI(uint32 const /*diff*/)
            {
                UpdateVictim();
                // no melee attacks
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return GetIcecrownCitadelAI<npc_spirit_bombAI>(creature);
        }
};

class npc_broken_frostmourne : public CreatureScript
{
    public:
        npc_broken_frostmourne() : CreatureScript("npc_broken_frostmourne") { }

        struct npc_broken_frostmourneAI : public CreatureAI
        {
            npc_broken_frostmourneAI(Creature* creature) : CreatureAI(creature)
            {
            }

            void Reset()
            {
                _events.Reset();
            }

            void IsSummonedBy(Unit* /*summoner*/)
            {
                _events.SetPhase(PHASE_OUTRO);
                _events.ScheduleEvent(EVENT_OUTRO_KNOCK_BACK, 3000, 0, PHASE_OUTRO);
            }

            void DoAction(int32 const action)
            {
                if (action == ACTION_SUMMON_TERENAS)
                    _events.ScheduleEvent(EVENT_OUTRO_SUMMON_TERENAS, 6000, 0, PHASE_OUTRO);
            }

            void UpdateAI(uint32 const diff)
            {
                UpdateVictim();

                _events.Update(diff);

                while (uint32 eventId = _events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_OUTRO_KNOCK_BACK:
                            DoCastAOE(SPELL_BROKEN_FROSTMOURNE_KNOCK);
                            break;
                        case EVENT_OUTRO_SUMMON_TERENAS:
                            DoCastAOE(SPELL_SUMMON_TERENAS);
                            break;
                        default:
                            break;
                    }
                }

                // no melee attacks
            }

        private:
            EventMap _events;
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return GetIcecrownCitadelAI<npc_broken_frostmourneAI>(creature);
        }
};

class npc_vile_spirit : public CreatureScript
{
    public:
        npc_vile_spirit() : CreatureScript("npc_vile_spirit") { }

        struct npc_vile_spiritAI : public ScriptedAI
        {
            npc_vile_spiritAI(Creature* creature) : ScriptedAI(creature)
            {
            }

            void Reset()
            {
                _attackStarted = false;
                _events.Reset();
            }

            bool CanAIAttack(Unit const* target) const
            {
                // Must only select non falling players, who are not in frostmourne zone in non heroic, or who are in frostmourne zone in heroic
                return target->GetTypeId() == TYPEID_PLAYER && target->GetPositionZ() > 830.0f && ((!IsHeroic() && !target->HasAura(SPELL_IN_FROSTMOURNE_ROOM)) || (IsHeroic() && target->HasAura(SPELL_IN_FROSTMOURNE_ROOM)));
            }

            void DoAction(int32 const action)
            {
                switch (action)
                {
                    case ACTION_START_ATTACK_1:
                        if (!_attackStarted)
                            _events.RescheduleEvent(EVENT_VILE_SPIRIT_START_ATTACK, 15000);
                        break;
                    case ACTION_START_ATTACK_2:
                        if (!_attackStarted)
                            _events.RescheduleEvent(EVENT_VILE_SPIRIT_START_ATTACK, 1000);
                        break;
                }
            }

            void UpdateAI(uint32 const diff)
            {
                if (_attackStarted)
                    UpdateVictim();

                _events.Update(diff);

                while (uint32 eventId = _events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_VILE_SPIRIT_START_ATTACK:
                            _attackStarted = true;

                            // We need this for threat management later
                            if (TempSummon* summon = me->ToTempSummon())
                                if (Unit* summoner = summon->GetSummoner())
                                    if (Creature* summonerCreature = summoner->ToCreature())
                                        if (summonerCreature->isInCombat() && summonerCreature->AI())
                                            summonerCreature->AI()->DoZoneInCombat(me);

                            me->SetReactState(REACT_AGGRESSIVE);
                            me->CastSpell(me, SPELL_VILE_SPIRIT_MOVE_SEARCH, true);
                            break;
                    }
                }

                // no melee attacks
            }

        private:
            bool _attackStarted;
            EventMap _events;
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return GetIcecrownCitadelAI<npc_vile_spiritAI>(creature);
        }
};

class spell_the_lich_king_infest : public SpellScriptLoader
{
    public:
        spell_the_lich_king_infest() :  SpellScriptLoader("spell_the_lich_king_infest") { }

        class spell_the_lich_king_infest_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_the_lich_king_infest_AuraScript);

            void OnPeriodic(AuraEffect const* /*aurEff*/)
            {
                if (GetUnitOwner()->HealthAbovePct(90))
                {
                    PreventDefaultAction();
                    Remove(AURA_REMOVE_BY_ENEMY_SPELL);
                }
            }

            void OnUpdate(AuraEffect* aurEff)
            {
                // multiply, starting from 2nd tick
                if (aurEff->GetTickNumber() == 1)
                    return;

                aurEff->SetAmount(int32(aurEff->GetAmount() * 1.15f));
            }

            void Register()
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_the_lich_king_infest_AuraScript::OnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
                OnEffectUpdatePeriodic += AuraEffectUpdatePeriodicFn(spell_the_lich_king_infest_AuraScript::OnUpdate, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_the_lich_king_infest_AuraScript();
        }
};

class spell_the_lich_king_necrotic_plague : public SpellScriptLoader
{
    public:
        spell_the_lich_king_necrotic_plague() :  SpellScriptLoader("spell_the_lich_king_necrotic_plague") { }

        class spell_the_lich_king_necrotic_plague_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_the_lich_king_necrotic_plague_AuraScript);

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_NECROTIC_PLAGUE_JUMP))
                    return false;
                return true;
            }

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                switch (GetTargetApplication()->GetRemoveMode())
                {
                    case AURA_REMOVE_BY_ENEMY_SPELL:
                    case AURA_REMOVE_BY_EXPIRE:
                    case AURA_REMOVE_BY_DEATH:
                        break;
                    default:
                        return;
                }

                CustomSpellValues values;
                //values.AddSpellMod(SPELLVALUE_AURA_STACK, 2);
                values.AddSpellMod(SPELLVALUE_MAX_TARGETS, 1);
                GetTarget()->CastCustomSpell(SPELL_NECROTIC_PLAGUE_JUMP, values, NULL, true, NULL, NULL, GetCasterGUID());
                if (Unit* caster = GetCaster())
                    caster->CastSpell(caster, SPELL_PLAGUE_SIPHON, true);
            }

            void Register()
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_the_lich_king_necrotic_plague_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_the_lich_king_necrotic_plague_AuraScript();
        }
};

class spell_the_lich_king_necrotic_plague_jump : public SpellScriptLoader
{
    public:
        spell_the_lich_king_necrotic_plague_jump() :  SpellScriptLoader("spell_the_lich_king_necrotic_plague_jump") { }

        class spell_the_lich_king_necrotic_plague_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_the_lich_king_necrotic_plague_SpellScript);

            void AddMissingStack()
            {
                // Add stack if we were not removed by dispel
                if (GetHitAura() && GetSpellValue()->EffectBasePoints[EFFECT_1] != AURA_REMOVE_BY_ENEMY_SPELL)
                    GetHitAura()->ModStackAmount(1);

                // Add stack if target has LK spell on it
                if (GetHitAura() && GetHitUnit())
                {
                    if (GetHitUnit()->HasAura(70337) || GetHitUnit()->HasAura(73912) || GetHitUnit()->HasAura(73913) || GetHitUnit()->HasAura(73914))
                    {
                        GetHitUnit()->RemoveAurasDueToSpell(70337);
                        GetHitUnit()->RemoveAurasDueToSpell(73912);
                        GetHitUnit()->RemoveAurasDueToSpell(73913);
                        GetHitUnit()->RemoveAurasDueToSpell(73914);

                        GetHitAura()->ModStackAmount(1);
                    }
                }
            }

            void Register()
            {
                OnHit += SpellHitFn(spell_the_lich_king_necrotic_plague_SpellScript::AddMissingStack);
            }
        };

        class spell_the_lich_king_necrotic_plague_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_the_lich_king_necrotic_plague_AuraScript);

            bool Load()
            {
                _lastAmount = 0;
                return true;
            }

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* caster = GetCaster())
                    caster->GetAI()->SetData(DATA_PLAGUE_STACK, GetStackAmount());
            }

            void OnRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
            {
                _lastAmount = aurEff->GetAmount();
                switch (GetTargetApplication()->GetRemoveMode())
                {
                    case AURA_REMOVE_BY_EXPIRE:
                    case AURA_REMOVE_BY_DEATH:
                        break;
                    default:
                        return;
                }

                CustomSpellValues values;
                values.AddSpellMod(SPELLVALUE_AURA_STACK, GetStackAmount());
                GetTarget()->CastCustomSpell(SPELL_NECROTIC_PLAGUE_JUMP, values, NULL, true, NULL, NULL, GetCasterGUID());
                if (Unit* caster = GetCaster())
                    caster->CastSpell(caster, SPELL_PLAGUE_SIPHON, true);
            }

            void OnDispel(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
            {
                _lastAmount = aurEff->GetAmount();
            }

            void AfterDispel(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
            {
                // this means the stack increased so don't process as if dispelled
                if (aurEff->GetAmount() > _lastAmount)
                    return;

                CustomSpellValues values;
                values.AddSpellMod(SPELLVALUE_AURA_STACK, GetStackAmount());
                values.AddSpellMod(SPELLVALUE_BASE_POINT1, AURA_REMOVE_BY_ENEMY_SPELL); // add as marker (spell has no effect 1)
                GetTarget()->CastCustomSpell(SPELL_NECROTIC_PLAGUE_JUMP, values, NULL, true, NULL, NULL, GetCasterGUID());
                if (Unit* caster = GetCaster())
                    caster->CastSpell(caster, SPELL_PLAGUE_SIPHON, true);

                Remove(AURA_REMOVE_BY_ENEMY_SPELL);
            }

            void Register()
            {
                OnEffectApply += AuraEffectApplyFn(spell_the_lich_king_necrotic_plague_AuraScript::OnApply, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
                AfterEffectRemove += AuraEffectRemoveFn(spell_the_lich_king_necrotic_plague_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_the_lich_king_necrotic_plague_AuraScript::OnDispel, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAPPLY);
                AfterEffectApply += AuraEffectRemoveFn(spell_the_lich_king_necrotic_plague_AuraScript::AfterDispel, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAPPLY);
            }

            int32 _lastAmount;
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_the_lich_king_necrotic_plague_SpellScript();
        }

        AuraScript* GetAuraScript() const
        {
            return new spell_the_lich_king_necrotic_plague_AuraScript();
        }
};

class spell_the_lich_king_shadow_trap_visual : public SpellScriptLoader
{
    public:
        spell_the_lich_king_shadow_trap_visual() : SpellScriptLoader("spell_the_lich_king_shadow_trap_visual") { }

        class spell_the_lich_king_shadow_trap_visual_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_the_lich_king_shadow_trap_visual_AuraScript);

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (GetTargetApplication()->GetRemoveMode() == AURA_REMOVE_BY_EXPIRE)
                    GetTarget()->CastSpell(GetTarget(), SPELL_SHADOW_TRAP_AURA, TRIGGERED_NONE);
            }

            void Register()
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_the_lich_king_shadow_trap_visual_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_the_lich_king_shadow_trap_visual_AuraScript();
        }
};

class spell_the_lich_king_shadow_trap_periodic : public SpellScriptLoader
{
    public:
        spell_the_lich_king_shadow_trap_periodic() : SpellScriptLoader("spell_the_lich_king_shadow_trap_periodic") { }

        class spell_the_lich_king_shadow_trap_periodic_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_the_lich_king_shadow_trap_periodic_SpellScript);

            void CheckTargetCount(std::list<Unit*>& targets)
            {
                if (targets.empty())
                    return;

                GetCaster()->CastSpell((Unit*)NULL, SPELL_SHADOW_TRAP_KNOCKBACK, true);
            }

            void Register()
            {
                OnUnitTargetSelect += SpellUnitTargetFn(spell_the_lich_king_shadow_trap_periodic_SpellScript::CheckTargetCount, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_the_lich_king_shadow_trap_periodic_SpellScript();
        }
};

class spell_the_lich_king_quake : public SpellScriptLoader
{
    public:
        spell_the_lich_king_quake() : SpellScriptLoader("spell_the_lich_king_quake") { }

        class spell_the_lich_king_quake_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_the_lich_king_quake_SpellScript);

            bool Load()
            {
                return GetCaster()->GetInstanceScript() != NULL;
            }

            void FilterTargets(std::list<Unit*>& unitList)
            {
                if (GameObject* platform = ObjectAccessor::GetGameObject(*GetCaster(), GetCaster()->GetInstanceScript()->GetData64(DATA_ARTHAS_PLATFORM)))
                    unitList.remove_if(HeightDifferenceCheck(platform, 5.0f, false));
            }

            void HandleSendEvent(SpellEffIndex /*effIndex*/)
            {
                if (GetCaster()->IsAIEnabled)
                    GetCaster()->GetAI()->DoAction(ACTION_START_ATTACK);
            }

            void Register()
            {
                OnUnitTargetSelect += SpellUnitTargetFn(spell_the_lich_king_quake_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
                OnEffectHit += SpellEffectFn(spell_the_lich_king_quake_SpellScript::HandleSendEvent, EFFECT_1, SPELL_EFFECT_SEND_EVENT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_the_lich_king_quake_SpellScript();
        }
};

class spell_the_lich_king_ice_burst_target_search : public SpellScriptLoader
{
    public:
        spell_the_lich_king_ice_burst_target_search() : SpellScriptLoader("spell_the_lich_king_ice_burst_target_search") { }

        class spell_the_lich_king_ice_burst_target_search_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_the_lich_king_ice_burst_target_search_SpellScript);

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_ICE_BURST))
                    return false;
                return true;
            }

            void CheckTargetCount(std::list<Unit*>& unitList)
            {
                if (unitList.empty())
                    return;

                // if there is at least one affected target cast the explosion
                GetCaster()->CastSpell(GetCaster(), SPELL_ICE_BURST, true);
                if (GetCaster()->GetTypeId() == TYPEID_UNIT)
                    GetCaster()->ToCreature()->DespawnOrUnsummon(500);
            }

            void Register()
            {
                OnUnitTargetSelect += SpellUnitTargetFn(spell_the_lich_king_ice_burst_target_search_SpellScript::CheckTargetCount, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_the_lich_king_ice_burst_target_search_SpellScript();
        }
};

class spell_the_lich_king_raging_spirit : public SpellScriptLoader
{
    public:
        spell_the_lich_king_raging_spirit() : SpellScriptLoader("spell_the_lich_king_raging_spirit") { }

        class spell_the_lich_king_raging_spirit_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_the_lich_king_raging_spirit_SpellScript);

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_LIFE_SIPHON_HEAL))
                    return false;
                return true;
            }

            void HandleScript(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);
                GetHitUnit()->CastSpell(GetHitUnit(), uint32(GetEffectValue()), true);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_the_lich_king_raging_spirit_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_the_lich_king_raging_spirit_SpellScript();
        }
};

class ExactDistanceCheck
{
    public:
        ExactDistanceCheck(Unit* source, float dist) : _source(source), _dist(dist) {}

        bool operator()(Unit* unit)
        {
            return _source->GetExactDist2d(unit) > _dist;
        }

    private:
        Unit* _source;
        float _dist;
};

class spell_the_lich_king_defile : public SpellScriptLoader
{
    public:
        spell_the_lich_king_defile() : SpellScriptLoader("spell_the_lich_king_defile") { }

        class spell_the_lich_king_defile_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_the_lich_king_defile_SpellScript);

            void CorrectRange(std::list<Unit*>& targets)
            {
                targets.remove_if(ExactDistanceCheck(GetCaster(), 10.0f * GetCaster()->GetFloatValue(OBJECT_FIELD_SCALE_X)));
            }

            void ChangeDamageAndGrow()
            {
                SetHitDamage(int32(GetHitDamage() * GetCaster()->GetFloatValue(OBJECT_FIELD_SCALE_X)));
                // HACK: target player should cast this spell on defile
                // however with current aura handling auras cast by different units
                // cannot stack on the same aura object increasing the stack count
                GetCaster()->CastSpell(GetCaster(), SPELL_DEFILE_GROW, true);
            }

            void Register()
            {
                OnUnitTargetSelect += SpellUnitTargetFn(spell_the_lich_king_defile_SpellScript::CorrectRange, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnUnitTargetSelect += SpellUnitTargetFn(spell_the_lich_king_defile_SpellScript::CorrectRange, EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY);
                OnHit += SpellHitFn(spell_the_lich_king_defile_SpellScript::ChangeDamageAndGrow);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_the_lich_king_defile_SpellScript();
        }
};

class spell_the_lich_king_summon_into_air : public SpellScriptLoader
{
    public:
        spell_the_lich_king_summon_into_air() : SpellScriptLoader("spell_the_lich_king_summon_into_air") { }

        class spell_the_lich_king_summon_into_air_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_the_lich_king_summon_into_air_SpellScript);

            void ModDestHeight(SpellEffIndex effIndex)
            {
                static Position const offset = {0.0f, 0.0f, 15.0f, 0.0f};
                WorldLocation* dest = const_cast<WorldLocation*>(GetTargetDest());
                dest->RelocateOffset(offset);
                // spirit bombs get higher
                if (GetSpellInfo()->Effects[effIndex].MiscValue == NPC_SPIRIT_BOMB)
                    dest->RelocateOffset(offset);
            }

            void Register()
            {
                OnEffectLaunch += SpellEffectFn(spell_the_lich_king_summon_into_air_SpellScript::ModDestHeight, EFFECT_0, SPELL_EFFECT_SUMMON);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_the_lich_king_summon_into_air_SpellScript();
        }
};

class spell_the_lich_king_soul_reaper : public SpellScriptLoader
{
    public:
        spell_the_lich_king_soul_reaper() :  SpellScriptLoader("spell_the_lich_king_soul_reaper") { }

        class spell_the_lich_king_soul_reaper_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_the_lich_king_soul_reaper_AuraScript);

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_SOUL_REAPER_BUFF))
                    return false;
                return true;
            }

            void OnPeriodic(AuraEffect const* /*aurEff*/)
            {
                if (Unit* caster = GetCaster())
                    GetTarget()->CastSpell(caster, SPELL_SOUL_REAPER_BUFF, true);
            }

            void Register()
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_the_lich_king_soul_reaper_AuraScript::OnPeriodic, EFFECT_1, SPELL_AURA_PERIODIC_DAMAGE);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_the_lich_king_soul_reaper_AuraScript();
        }
};

class spell_the_lich_king_valkyr_target_search : public SpellScriptLoader
{
    public:
        spell_the_lich_king_valkyr_target_search() : SpellScriptLoader("spell_the_lich_king_valkyr_target_search") { }

        class spell_the_lich_king_valkyr_target_search_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_the_lich_king_valkyr_target_search_SpellScript);

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_ICE_BURST))
                    return false;
                return true;
            }

            bool Load()
            {
                _target = NULL;
                return true;
            }

            void SelectTarget(std::list<Unit*>& unitList)
            {
                if (unitList.empty())
                    return;

                unitList.remove_if(HeightFilterValkyrTargetSelection());
                unitList.remove_if(Trinity::UnitAuraCheck(true, GetSpellInfo()->Id));
                if (unitList.empty())
                    return;

                _target = SelectRandomContainerElement(unitList);
                unitList.clear();
                unitList.push_back(_target);
                GetCaster()->GetAI()->SetGUID(_target->GetGUID());
            }

            void ReplaceTarget(std::list<Unit*>& unitList)
            {
                unitList.clear();
                if (_target)
                    unitList.push_back(_target);
            }

            void HandleScript(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);
                if (GetCaster() && GetCaster()->GetMotionMaster() && GetHitUnit())
                    GetCaster()->GetMotionMaster()->MoveCharge(GetHitUnit()->GetPositionX(), GetHitUnit()->GetPositionY(), GetHitUnit()->GetPositionZ());
            }

            void Register()
            {
                OnUnitTargetSelect += SpellUnitTargetFn(spell_the_lich_king_valkyr_target_search_SpellScript::SelectTarget, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnUnitTargetSelect += SpellUnitTargetFn(spell_the_lich_king_valkyr_target_search_SpellScript::ReplaceTarget, EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY);
                OnEffectHitTarget += SpellEffectFn(spell_the_lich_king_valkyr_target_search_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }

            Unit* _target;
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_the_lich_king_valkyr_target_search_SpellScript();
        }
};

class spell_the_lich_king_eject_all_passengers : public SpellScriptLoader
{
    public:
        spell_the_lich_king_eject_all_passengers() : SpellScriptLoader("spell_the_lich_king_eject_all_passengers") { }

        class spell_the_lich_king_eject_all_passengers_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_the_lich_king_eject_all_passengers_SpellScript);

            bool Load()
            {
                return GetCaster()->IsVehicle();
            }

            void HandleDummy(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);
                GetCaster()->GetVehicleKit()->RemoveAllPassengers();
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_the_lich_king_eject_all_passengers_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_the_lich_king_eject_all_passengers_SpellScript();
        }
};

class spell_the_lich_king_cast_back_to_caster : public SpellScriptLoader
{
    public:
        spell_the_lich_king_cast_back_to_caster() :  SpellScriptLoader("spell_the_lich_king_cast_back_to_caster") { }

        class spell_the_lich_king_cast_back_to_caster_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_the_lich_king_cast_back_to_caster_SpellScript);

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                GetHitUnit()->CastSpell(GetCaster(), uint32(GetEffectValue()), true);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_the_lich_king_cast_back_to_caster_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_the_lich_king_cast_back_to_caster_SpellScript();
        }
};

class spell_the_lich_king_life_siphon : public SpellScriptLoader
{
    public:
        spell_the_lich_king_life_siphon() : SpellScriptLoader("spell_the_lich_king_life_siphon") { }

        class spell_the_lich_king_life_siphon_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_the_lich_king_life_siphon_SpellScript);

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_LIFE_SIPHON_HEAL))
                    return false;
                return true;
            }

            void TriggerHeal()
            {
                GetHitUnit()->CastCustomSpell(SPELL_LIFE_SIPHON_HEAL, SPELLVALUE_BASE_POINT0, GetHitDamage() * 10, GetCaster(), true);
            }

            void Register()
            {
                AfterHit += SpellHitFn(spell_the_lich_king_life_siphon_SpellScript::TriggerHeal);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_the_lich_king_life_siphon_SpellScript();
        }
};

class spell_the_lich_king_vile_spirits : public SpellScriptLoader
{
    public:
        spell_the_lich_king_vile_spirits() : SpellScriptLoader("spell_the_lich_king_vile_spirits") { }

        class spell_the_lich_king_vile_spirits_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_the_lich_king_vile_spirits_AuraScript);

            bool Load()
            {
                _is25Man = GetUnitOwner()->GetMap()->Is25ManRaid();
                return true;
            }

            void OnPeriodic(AuraEffect const* aurEff)
            {
                if (_is25Man || ((aurEff->GetTickNumber() - 1) % 5))
                    GetTarget()->CastSpell((Unit*)NULL, GetSpellInfo()->Effects[aurEff->GetEffIndex()].TriggerSpell, true, NULL, aurEff, GetCasterGUID());
            }

            void Register()
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_the_lich_king_vile_spirits_AuraScript::OnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }

            bool _is25Man;
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_the_lich_king_vile_spirits_AuraScript();
        }
};

class spell_the_lich_king_vile_spirits_visual : public SpellScriptLoader
{
    public:
        spell_the_lich_king_vile_spirits_visual() : SpellScriptLoader("spell_the_lich_king_vile_spirits_visual") { }

        class spell_the_lich_king_vile_spirits_visual_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_the_lich_king_vile_spirits_visual_SpellScript);

            void ModDestHeight(SpellEffIndex /*effIndex*/)
            {
                Position offset = {0.0f, 0.0f, 15.0f, 0.0f};
                const_cast<WorldLocation*>(GetTargetDest())->RelocateOffset(offset);
            }

            void Register()
            {
                OnEffectLaunch += SpellEffectFn(spell_the_lich_king_vile_spirits_visual_SpellScript::ModDestHeight, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_the_lich_king_vile_spirits_visual_SpellScript();
        }
};

class spell_the_lich_king_vile_spirit_move_target_search : public SpellScriptLoader
{
    public:
        spell_the_lich_king_vile_spirit_move_target_search() : SpellScriptLoader("spell_the_lich_king_vile_spirit_move_target_search") { }

        class spell_the_lich_king_vile_spirit_move_target_search_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_the_lich_king_vile_spirit_move_target_search_SpellScript);

            bool Load()
            {
                _target = NULL;
                return GetCaster()->GetTypeId() == TYPEID_UNIT;
            }

            void SelectTarget(std::list<Unit*>& targets)
            {
                // Only move towards players in range of 200 yards
                targets.remove_if(Trinity::ObjectTypeIdCheck(TYPEID_PLAYER, false));
                targets.remove_if(ExactDistanceCheck(GetCaster(), 200.0f));

                if (targets.empty())
                    return;

                _target = SelectRandomContainerElement(targets);
            }

            void HandleScript(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);
                // for this spell, all units are in target map, however it should select one to attack
                if (GetHitUnit() != _target)
                    return;

                GetCaster()->ToCreature()->AI()->AttackStart(GetHitUnit());
                GetCaster()->AddThreat(GetHitUnit(), 100000.0f);
            }

            void Register()
            {
                OnUnitTargetSelect += SpellUnitTargetFn(spell_the_lich_king_vile_spirit_move_target_search_SpellScript::SelectTarget, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnEffectHitTarget += SpellEffectFn(spell_the_lich_king_vile_spirit_move_target_search_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }

            Unit* _target;
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_the_lich_king_vile_spirit_move_target_search_SpellScript();
        }
};

class spell_the_lich_king_vile_spirit_damage_target_search : public SpellScriptLoader
{
    public:
        spell_the_lich_king_vile_spirit_damage_target_search() : SpellScriptLoader("spell_the_lich_king_vile_spirit_damage_target_search") { }

        class spell_the_lich_king_vile_spirit_damage_target_search_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_the_lich_king_vile_spirit_damage_target_search_SpellScript);

            bool Load()
            {
                return GetCaster()->GetTypeId() == TYPEID_UNIT;
            }

            void CheckTargetCount(std::list<Unit*>& targets)
            {
                if (targets.empty())
                    return;

                // this spell has SPELL_AURA_BLOCK_SPELL_FAMILY so every next cast of this
                // searcher spell will be blocked
                if (TempSummon* summon = GetCaster()->ToTempSummon())
                    if (Unit* summoner = summon->GetSummoner())
                        summoner->GetAI()->SetData(DATA_VILE, 1);
                GetCaster()->CastSpell((Unit*)NULL, SPELL_SPIRIT_BURST, true);
                GetCaster()->ToCreature()->DespawnOrUnsummon(3000);
                GetCaster()->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            }

            void Register()
            {
                OnUnitTargetSelect += SpellUnitTargetFn(spell_the_lich_king_vile_spirit_damage_target_search_SpellScript::CheckTargetCount, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            }

            Unit* _target;
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_the_lich_king_vile_spirit_damage_target_search_SpellScript();
        }
};

class spell_the_lich_king_harvest_soul : public SpellScriptLoader
{
    public:
        spell_the_lich_king_harvest_soul() : SpellScriptLoader("spell_the_lich_king_harvest_soul") { }

        class spell_the_lich_king_harvest_soul_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_the_lich_king_harvest_soul_AuraScript);

            bool Load()
            {
                return GetOwner()->GetInstanceScript() != NULL;
            }

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                // m_originalCaster to allow stacking from different casters, meh
                if (GetTargetApplication()->GetRemoveMode() == AURA_REMOVE_BY_DEATH)
                    GetTarget()->CastSpell((Unit*)NULL, SPELL_HARVESTED_SOUL, true, NULL, NULL, GetTarget()->GetInstanceScript()->GetData64(DATA_THE_LICH_KING));
            }

            void Register()
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_the_lich_king_harvest_soul_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_the_lich_king_harvest_soul_AuraScript();
        }
};

class spell_the_lich_king_lights_favor : public SpellScriptLoader
{
    public:
        spell_the_lich_king_lights_favor() : SpellScriptLoader("spell_the_lich_king_lights_favor") { }

        class spell_the_lich_king_lights_favor_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_the_lich_king_lights_favor_AuraScript);

            void OnPeriodic(AuraEffect const* /*aurEff*/)
            {
                if (Unit* caster = GetCaster())
                    if (AuraEffect* effect = GetAura()->GetEffect(EFFECT_1))
                        effect->RecalculateAmount(caster);
            }

            void CalculateBonus(AuraEffect const* /*aurEff*/, int32& amount, bool& canBeRecalculated)
            {
                canBeRecalculated = true;
                amount = 0;
                if (Unit* caster = GetCaster())
                    amount = int32(caster->GetHealthPct());
            }

            void Register()
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_the_lich_king_lights_favor_AuraScript::OnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_HEAL);
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_the_lich_king_lights_favor_AuraScript::CalculateBonus, EFFECT_1, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_the_lich_king_lights_favor_AuraScript();
        }
};

class spell_the_lich_king_soul_rip : public SpellScriptLoader
{
    public:
        spell_the_lich_king_soul_rip() : SpellScriptLoader("spell_the_lich_king_soul_rip") { }

        class spell_the_lich_king_soul_rip_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_the_lich_king_soul_rip_AuraScript);

            void OnPeriodic(AuraEffect const* aurEff)
            {
                PreventDefaultAction();
                // shouldn't be needed, this is channeled
                if (Unit* caster = GetCaster())
                    caster->CastCustomSpell(SPELL_SOUL_RIP_DAMAGE, SPELLVALUE_BASE_POINT0, 5000 * aurEff->GetTickNumber(), GetTarget(), true, NULL, aurEff, GetCasterGUID());
            }

            void Register()
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_the_lich_king_soul_rip_AuraScript::OnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_the_lich_king_soul_rip_AuraScript();
        }
};

class spell_the_lich_king_restore_soul : public SpellScriptLoader
{
    public:
        spell_the_lich_king_restore_soul() : SpellScriptLoader("spell_the_lich_king_restore_soul") { }

        class spell_the_lich_king_restore_soul_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_the_lich_king_restore_soul_SpellScript);

            bool Load()
            {
                _instance = GetCaster()->GetInstanceScript();
                return _instance != NULL;
            }

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                if (Creature* lichKing = ObjectAccessor::GetCreature(*GetCaster(), _instance->GetData64(DATA_THE_LICH_KING)))
                    lichKing->AI()->DoAction(ACTION_TELEPORT_BACK);
                if (Creature* spawner = GetCaster()->FindNearestCreature(NPC_WORLD_TRIGGER_INFINITE_AOI, 50.0f))
                    spawner->RemoveAllAuras();
            }

            void RemoveAura()
            {
                if (Unit* target = GetHitUnit())
                    target->RemoveAurasDueToSpell(target->GetMap()->IsHeroic() ? SPELL_HARVEST_SOULS_TELEPORT : SPELL_HARVEST_SOUL_TELEPORT);
            }

            void Register()
            {
                OnEffectHit += SpellEffectFn(spell_the_lich_king_restore_soul_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
                BeforeHit += SpellHitFn(spell_the_lich_king_restore_soul_SpellScript::RemoveAura);
            }

            InstanceScript* _instance;
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_the_lich_king_restore_soul_SpellScript();
        }
};

class spell_the_lich_king_in_frostmourne_room : public SpellScriptLoader
{
    public:
        spell_the_lich_king_in_frostmourne_room() : SpellScriptLoader("spell_the_lich_king_in_frostmourne_room") { }

        class spell_the_lich_king_in_frostmourne_room_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_the_lich_king_in_frostmourne_room_AuraScript);

            bool Load()
            {
                return GetOwner()->GetInstanceScript() != NULL;
            }

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                // m_originalCaster to allow stacking from different casters, meh
                if (GetTargetApplication()->GetRemoveMode() == AURA_REMOVE_BY_DEATH)
                    GetTarget()->CastSpell((Unit*)NULL, SPELL_HARVESTED_SOUL, true, NULL, NULL, GetTarget()->GetInstanceScript()->GetData64(DATA_THE_LICH_KING));
            }

            void Register()
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_the_lich_king_in_frostmourne_room_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_the_lich_king_in_frostmourne_room_AuraScript();
        }
};

class spell_the_lich_king_summon_spirit_bomb : public SpellScriptLoader
{
    public:
        spell_the_lich_king_summon_spirit_bomb() : SpellScriptLoader("spell_the_lich_king_summon_spirit_bomb") { }

        class spell_the_lich_king_summon_spirit_bomb_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_the_lich_king_summon_spirit_bomb_SpellScript);

            void HandleScript(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);
                GetHitUnit()->CastSpell((Unit*)NULL, uint32(GetEffectValue()), true);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_the_lich_king_summon_spirit_bomb_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_the_lich_king_summon_spirit_bomb_SpellScript();
        }
};

class spell_the_lich_king_trigger_vile_spirit : public SpellScriptLoader
{
    public:
        spell_the_lich_king_trigger_vile_spirit() : SpellScriptLoader("spell_the_lich_king_trigger_vile_spirit") { }

        class spell_the_lich_king_trigger_vile_spirit_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_the_lich_king_trigger_vile_spirit_SpellScript);

            void TeleportOutside()
            {
                Creature* target = GetHitCreature();
                if (!target)
                    return;

                Position dest;
                Position offset;
                TerenasSpawnHeroic.GetPositionOffsetTo(*target, offset);
                GetCaster()->GetPosition(&dest);
                dest.RelocateOffset(offset);
                target->NearTeleportTo(dest.GetPositionX(), dest.GetPositionY(), dest.GetPositionZ(), dest.GetOrientation());
            }

            void Register()
            {
                OnHit += SpellHitFn(spell_the_lich_king_trigger_vile_spirit_SpellScript::TeleportOutside);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_the_lich_king_trigger_vile_spirit_SpellScript();
        }
};

class spell_the_lich_king_jump : public SpellScriptLoader
{
    public:
        spell_the_lich_king_jump() : SpellScriptLoader("spell_the_lich_king_jump") { }

        class spell_the_lich_king_jump_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_the_lich_king_jump_SpellScript);

            void HandleScript(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);
                GetHitUnit()->RemoveAurasDueToSpell(SPELL_RAISE_DEAD);
                GetHitUnit()->CastSpell((Unit*)NULL, SPELL_JUMP_2, true);
                if (Creature* creature = GetHitCreature())
                    creature->AI()->DoAction(ACTION_BREAK_FROSTMOURNE);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_the_lich_king_jump_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_the_lich_king_jump_SpellScript();
        }
};

class spell_the_lich_king_jump_remove_aura : public SpellScriptLoader
{
    public:
        spell_the_lich_king_jump_remove_aura() : SpellScriptLoader("spell_the_lich_king_jump_remove_aura") { }

        class spell_the_lich_king_jump_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_the_lich_king_jump_SpellScript);

            void HandleScript(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);
                GetHitUnit()->RemoveAurasDueToSpell(uint32(GetEffectValue()));
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_the_lich_king_jump_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_the_lich_king_jump_SpellScript();
        }
};

class spell_the_lich_king_play_movie : public SpellScriptLoader
{
    public:
        spell_the_lich_king_play_movie() : SpellScriptLoader("spell_the_lich_king_play_movie") { }

        class spell_the_lich_king_play_movie_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_the_lich_king_play_movie_SpellScript);

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sMovieStore.LookupEntry(MOVIE_FALL_OF_THE_LICH_KING))
                    return false;
                return true;
            }

            void HandleScript(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);
                if (Player* player = GetHitPlayer())
                    player->SendMovieStart(MOVIE_FALL_OF_THE_LICH_KING);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_the_lich_king_play_movie_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_the_lich_king_play_movie_SpellScript();
        }
};

class spell_the_lich_king_wings_of_the_damned : public SpellScriptLoader
{
    public:
        spell_the_lich_king_wings_of_the_damned() : SpellScriptLoader("spell_the_lich_king_wings_of_the_damned") { }

        class spell_the_lich_king_wings_of_the_damned_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_the_lich_king_wings_of_the_damned_AuraScript);

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (AuraEffect* effect = GetAura()->GetEffect(EFFECT_0))
                    effect->SetAmount(32); // 32 is the half of 0.64 movement speed modificator
            }

            void Register()
            {
                OnEffectApply += AuraEffectApplyFn(spell_the_lich_king_wings_of_the_damned_AuraScript::OnApply, EFFECT_0, SPELL_AURA_MOD_MINIMUM_SPEED, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_the_lich_king_wings_of_the_damned_AuraScript();
        }
};

class achievement_been_waiting_long_time : public AchievementCriteriaScript
{
    public:
        achievement_been_waiting_long_time() : AchievementCriteriaScript("achievement_been_waiting_long_time") { }

        bool OnCheck(Player* /*source*/, Unit* target)
        {
            if (!target)
                return false;

            return target->GetAI()->GetData(DATA_PLAGUE_STACK) >= 30;
        }
};

class achievement_neck_deep_in_vile : public AchievementCriteriaScript
{
    public:
        achievement_neck_deep_in_vile() : AchievementCriteriaScript("achievement_neck_deep_in_vile") { }

        bool OnCheck(Player* /*source*/, Unit* target)
        {
            if (!target)
                return false;

            return !target->GetAI()->GetData(DATA_VILE);
        }
};

void AddSC_boss_the_lich_king()
{
    new boss_the_lich_king();
    new npc_tirion_fordring_tft();
    new npc_shambling_horror_icc();
    new npc_raging_spirit();
    new npc_valkyr_shadowguard();
    new npc_strangulate_vehicle();
    new npc_terenas_menethil();
    new npc_spirit_warden();
    new npc_spirit_bomb();
    new npc_broken_frostmourne();
    new npc_vile_spirit();
    new spell_the_lich_king_infest();
    new spell_the_lich_king_necrotic_plague();
    new spell_the_lich_king_necrotic_plague_jump();
    new spell_the_lich_king_shadow_trap_visual();
    new spell_the_lich_king_shadow_trap_periodic();
    new spell_the_lich_king_quake();
    new spell_the_lich_king_ice_burst_target_search();
    new spell_the_lich_king_raging_spirit();
    new spell_the_lich_king_defile();
    new spell_the_lich_king_summon_into_air();
    new spell_the_lich_king_soul_reaper();
    new spell_the_lich_king_valkyr_target_search();
    new spell_the_lich_king_eject_all_passengers();
    new spell_the_lich_king_cast_back_to_caster();
    new spell_the_lich_king_life_siphon();
    new spell_the_lich_king_vile_spirits();
    new spell_the_lich_king_vile_spirits_visual();
    new spell_the_lich_king_vile_spirit_move_target_search();
    new spell_the_lich_king_vile_spirit_damage_target_search();
    new spell_the_lich_king_harvest_soul();
    new spell_the_lich_king_lights_favor();
    new spell_the_lich_king_soul_rip();
    new spell_the_lich_king_restore_soul();
    new spell_the_lich_king_in_frostmourne_room();
    new spell_the_lich_king_summon_spirit_bomb();
    new spell_the_lich_king_trigger_vile_spirit();
    new spell_the_lich_king_jump();
    new spell_the_lich_king_jump_remove_aura();
    new spell_trigger_spell_from_caster("spell_the_lich_king_mass_resurrection", SPELL_MASS_RESURRECTION_REAL);
    new spell_the_lich_king_play_movie();
    new spell_the_lich_king_wings_of_the_damned();
    new achievement_been_waiting_long_time();
    new achievement_neck_deep_in_vile();
}
