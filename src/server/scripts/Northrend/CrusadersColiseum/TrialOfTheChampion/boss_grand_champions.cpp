/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2006-2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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

/* ScriptData
SDName: boss_grand_champions
SD%Complete: 50 %
SDComment: Is missing the ai to make the npcs look for a new mount and use it.
SDCategory: Trial Of the Champion
EndScriptData */

#include "ScriptPCH.h"
#include "ScriptedEscortAI.h"
#include "Vehicle.h"
#include "trial_of_the_champion.h"

enum eSpells
{
    //Vehicle
    SPELL_THRUST                    = 68505,
    SPELL_CHARGE                    = 63010,
    SPELL_SHIELD_BREAKER            = 68504,
    SPELL_DEFEND                    = 62552,

    // Marshal Jacob Alerius && Mokra the Skullcrusher || Warrior
    SPELL_MORTAL_STRIKE             = 67542,
    SPELL_BLADESTORM                = 67541,
    SPELL_INTERCEPT                 = 67540,
    SPELL_ROLLING_THROW             = 47115, //not implemented in the AI yet...

    // Ambrose Boltspark && Eressea Dawnsinger || Mage
    SPELL_FIREBALL                  = 66042,
    SPELL_FIREBALL_H                = 68310,
    SPELL_BLAST_WAVE                = 66044,
    SPELL_BLAST_WAVE_H              = 68312,
    SPELL_HASTE                     = 66045,
    SPELL_POLYMORPH                 = 66043,
    SPELL_POLYMORPH_H               = 68311,

    // Colosos && Runok Wildmane || Shaman
    SPELL_CHAIN_LIGHTNING           = 67529,
    SPELL_CHAIN_LIGHTNING_H         = 68319,
    SPELL_EARTH_SHIELD              = 67530,
    SPELL_HEALING_WAVE              = 67528,
    SPELL_HEALING_WAVE_H            = 68318,
    SPELL_HEX_OF_MENDING            = 67534,

    // Jaelyne Evensong && Zul'tore || Hunter
    SPELL_DISENGAGE                 = 68340, //not implemented in the AI yet...
    SPELL_LIGHTNING_ARROWS          = 66085,
    SPELL_MULTI_SHOT                = 49047,
    SPELL_SHOOT                     = 65868,
    SPELL_SHOOT_H                   = 67988,

    // Lana Stouthammer Evensong && Deathstalker Visceri || Rouge
    SPELL_EVISCERATE                = 67709,
    SPELL_EVISCERATE_H              = 68317,
    SPELL_FAN_OF_KNIVES             = 67706,
    SPELL_POISON_BOTTLE             = 67701,

    // Achievement Credit
    SPELL_GRAND_CHAMPIONS_CREDIT    = 68572,
};
enum eEnums
{
    SAY_START_1                      = -1999939,
    SAY_START_2                      = -1999937
};

enum eSeat
{
    SEAT_ID_0                       = 0
};

/*
struct Point
{
    float x, y, z;
};

const Point MovementPoint[] =
{
  {746.84f, 623.15f, 411.41f},
  {747.96f, 620.29f, 411.09f},
  {750.23f, 618.35f, 411.09f}
};
*/
void AggroAllPlayers(Creature* temp)
{
    Map::PlayerList const &PlList = temp->GetMap()->GetPlayers();

    if (PlList.isEmpty())
            return;

    for (Map::PlayerList::const_iterator i = PlList.begin(); i != PlList.end(); ++i)
    {
        if (Player* player = i->getSource())
        {
            if (player->isGameMaster())
                continue;

            if (player->isAlive())
            {
                temp->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_PC);
                temp->SetReactState(REACT_AGGRESSIVE);
                temp->SetInCombatWith(player);
                player->SetInCombatWith(temp);
                temp->AddThreat(player, 0.0f);
            }
        }
    }
}

bool GrandChampionsOutVehicle(Creature* me)
{
    InstanceScript* instance = me->GetInstanceScript();

    if (!instance)
        return false;

    Creature* pGrandChampion1 = Unit::GetCreature(*me, instance->GetData64(DATA_GRAND_CHAMPION_1));
    Creature* pGrandChampion2 = Unit::GetCreature(*me, instance->GetData64(DATA_GRAND_CHAMPION_2));
    Creature* pGrandChampion3 = Unit::GetCreature(*me, instance->GetData64(DATA_GRAND_CHAMPION_3));

    if (pGrandChampion1 && pGrandChampion2 && pGrandChampion3)
    {
        if (!pGrandChampion1->HasUnitMovementFlag(MOVEMENTFLAG_ONTRANSPORT) &&
            !pGrandChampion1->GetVehicle() &&
            !pGrandChampion2->HasUnitMovementFlag(MOVEMENTFLAG_ONTRANSPORT) &&
            !pGrandChampion2->GetVehicle() &&
            !pGrandChampion3->HasUnitMovementFlag(MOVEMENTFLAG_ONTRANSPORT) &&
            !pGrandChampion3->GetVehicle())
            return true;
    }

    return false;
}

/*
* Generic AI for vehicles used by npcs in ToC, it needs more improvements.  *
* Script Complete: 25%.                                                     *
*/

class generic_vehicleAI_toc5 : public CreatureScript
{
public:
    generic_vehicleAI_toc5() : CreatureScript("generic_vehicleAI_toc5") { }

    struct generic_vehicleAI_toc5AI : public npc_escortAI
    {
        generic_vehicleAI_toc5AI(Creature* creature) : npc_escortAI(creature)
        {
            SetDespawnAtEnd(false);
            uiWaypointPath = 0;

            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;

        bool combatEntered;
        uint32 combatCheckTimer;

        uint32 uiShieldBreakerTimer;
        uint32 uiDefendTimer;
        uint32 uiChargeTimer;
        uint32 uiThrustTimer;

        uint32 uiWaypointPath;

        void Reset()
        {
            combatCheckTimer = 500;
            uiThrustTimer = 3000;
            uiChargeTimer = 5000;
            uiShieldBreakerTimer = 8000;
            uiDefendTimer = urand(30000, 60000);
        }

        void SetData(uint32 uiType, uint32 /*uiData*/)
        {
            switch (uiType)
            {
                case 1:
                    AddWaypoint(0, 746.45f, 647.03f, 411.57f);
                    AddWaypoint(1, 771.434f, 642.606f, 411.9f);
                    AddWaypoint(2, 779.807f, 617.535f, 411.716f);
                    AddWaypoint(3, 771.098f, 594.635f, 411.625f);
                    AddWaypoint(4, 746.887f, 583.425f, 411.668f);
                    AddWaypoint(5, 715.176f, 583.782f, 412.394f);
                    AddWaypoint(6, 720.719f, 591.141f, 411.737f);
                    uiWaypointPath = 1;
                    break;
                case 2:
                    AddWaypoint(0, 746.45f, 647.03f, 411.57f);
                    AddWaypoint(1, 771.434f, 642.606f, 411.9f);
                    AddWaypoint(2, 779.807f, 617.535f, 411.716f);
                    AddWaypoint(3, 771.098f, 594.635f, 411.625f);
                    AddWaypoint(4, 746.887f, 583.425f, 411.668f);
                    AddWaypoint(5, 746.16f, 571.678f, 412.389f);
                    AddWaypoint(6, 746.887f, 583.425f, 411.668f);
                    uiWaypointPath = 2;
                    break;
                case 3:
                    AddWaypoint(0, 746.45f, 647.03f, 411.57f);
                    AddWaypoint(1, 771.434f, 642.606f, 411.9f);
                    AddWaypoint(2, 779.807f, 617.535f, 411.716f);
                    AddWaypoint(3, 771.098f, 594.635f, 411.625f);
                    AddWaypoint(4, 777.759f, 584.577f, 412.393f);
                    AddWaypoint(5, 772.48f, 592.99f, 411.68f);
                    uiWaypointPath = 3;
                    break;
                case 4:
                    combatEntered = true;
                    break;
            }

            if (uiType <= 3)
                Start(false, true, 0, NULL);
        }

        void WaypointReached(uint32 i)
        {
            switch (i)
            {
                case 2:
                    if ((instance && uiWaypointPath == 3) || uiWaypointPath == 2)
                        instance->SetData(DATA_MOVEMENT_DONE, instance->GetData(DATA_MOVEMENT_DONE)+1);
                    break;
                case 3:
                    if (instance)
                        instance->SetData(DATA_MOVEMENT_DONE, instance->GetData(DATA_MOVEMENT_DONE)+1);
                    break;
            }
        }

        void EnterCombat(Unit* /*who*/)
        {
            DoCastSpellDefend();
            me->CallForHelp(VISIBLE_RANGE);
        }

        void DoCastSpellDefend()
        {
            for (uint8 i = 0; i < 3; ++i)
                DoCast(me, SPELL_DEFEND, true);
        }

        void SpellHit(Unit* source, const SpellInfo* spell)
        {

            uint32 defendAuraStackAmount = 0;

            if (me->HasAura(SPELL_DEFEND))
                if (Aura* defendAura = me->GetAura(SPELL_DEFEND))
                    defendAuraStackAmount = defendAura->GetStackAmount();

            // Shield-Break by player vehicle
            if (spell->Id == 62575)
            {
                source->DealDamage(me, uint32(2000 * (1 - 0.3f * defendAuraStackAmount)));
                source->SendSpellNonMeleeDamageLog(me, 62575, uint32(2000 * (1 - 0.3f * defendAuraStackAmount)), SPELL_SCHOOL_MASK_NORMAL, 0, 0, true, 0, false);

                if (me->HasAura(SPELL_DEFEND))
                    me->RemoveAuraFromStack(SPELL_DEFEND);
            }
    
            // Charge by player vehicle
            if (spell->Id == 68282)
            {
                source->DealDamage(me, uint32(20000 * (1 - 0.3f * defendAuraStackAmount)));
                source->SendSpellNonMeleeDamageLog(me, 68282, uint32(20000 * (1 - 0.3f * defendAuraStackAmount)), SPELL_SCHOOL_MASK_NORMAL, 0, 0, true, 0, false);

                if (source->GetMotionMaster())
                    source->GetMotionMaster()->MoveCharge(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ());

                if (me->HasAura(SPELL_DEFEND))
                    me->RemoveAuraFromStack(SPELL_DEFEND);
            }
        }

        bool StayInCombatAndCleanup(bool combat, bool cleanup)
        {
            if (me->GetMap())
            {
                Map::PlayerList const& players = me->GetMap()->GetPlayers();
                bool foundtarget = false;

                if (me->GetMap()->IsDungeon() && !players.isEmpty())
                {
                    for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
                    {
                        Player* player = itr->getSource();
                        if (player && !player->isGameMaster() && player->isAlive())
                        {
                            // Handle combat variable
                            if (combat)
                            {
                                if (combatEntered)
                                {
                                    me->SetInCombatWith(player);
                                    player->SetInCombatWith(me);
                                    me->AddThreat(player, 0.0f);

                                    foundtarget = true;

                                    if (Vehicle* pVehicle = player->GetVehicle())
                                    {
                                        if (Unit* vehicleCreature = pVehicle->GetBase())
                                        {
                                            me->SetInCombatWith(vehicleCreature);
                                            vehicleCreature->SetInCombatWith(me);
                                            me->AddThreat(vehicleCreature, 0.0f);
                                        }
                                    }
                                }
                            }

                            // Handle cleanup variable
                            if (cleanup)
                                if (player->HasAura(SPELL_DEFEND))
                                    player->RemoveAurasDueToSpell(SPELL_DEFEND);
                        }
                    }
                }

                if (combatEntered && combat && !foundtarget)
                {
                    me->SetFullHealth();
                    return false;
                }
            }

            return true;
        }

        void EnterEvadeMode()
        {
            // Try to stay in combat, otherwise reset
            if (!StayInCombatAndCleanup(true, false))
                ScriptedAI::EnterEvadeMode();
        }

        void UpdateAI(const uint32 uiDiff)
        {
            // Try to keep players clean of defend aura
            if (combatEntered)
            {
                if (combatCheckTimer <= uiDiff)
                {
                    StayInCombatAndCleanup(false, true);
                    combatCheckTimer = 1000;
                }else combatCheckTimer -= uiDiff;
            }

            npc_escortAI::UpdateAI(uiDiff);

            if (!UpdateVictim())
                return;

            if (uiDefendTimer <= uiDiff)
            {
                DoCastSpellDefend();
                uiDefendTimer = urand(30000, 45000);
            }else uiDefendTimer -= uiDiff;

            if (uiShieldBreakerTimer <= uiDiff)
            {
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                {
                    if (target->GetTypeId() == TYPEID_PLAYER && me->GetDistance(target) > 10.0f && me->GetDistance(target) < 30.0f)
                    {
                        if (target->GetVehicle())
                        {
                            if (Unit* vehTarget = target->GetVehicle()->GetBase())
                            {
                                DoCast(vehTarget, SPELL_SHIELD_BREAKER);
                                vehTarget->RemoveAuraFromStack(SPELL_DEFEND);
                            }
                        }
                    }
                    else if (target->GetTypeId() == TYPEID_UNIT && me->GetDistance(target) > 8.0f && me->GetDistance(target) < 25.0f)
                    {
                        DoCast(target, SPELL_SHIELD_BREAKER);
                        target->RemoveAuraFromStack(SPELL_DEFEND);
                    }
                }

                uiShieldBreakerTimer = urand(15000, 20000);
            }else uiShieldBreakerTimer -= uiDiff;

            if (uiChargeTimer <= uiDiff)
            {
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                {
                    if (target->GetTypeId() == TYPEID_PLAYER && me->GetDistance(target) > 8.0f && me->GetDistance(target) < 25.0f)
                    {
                        if (target->GetVehicle())
                        {
                            if (Unit* vehTarget = target->GetVehicle()->GetBase())
                            {
                                DoCast(vehTarget, SPELL_CHARGE);

                                if (vehTarget->HasAura(SPELL_DEFEND))
                                    vehTarget->RemoveAuraFromStack(SPELL_DEFEND);
                            }
                        }
                    }
                    else if (target->GetTypeId() == TYPEID_UNIT && me->GetDistance(target) > 8.0f && me->GetDistance(target) < 25.0f)
                    {
                        DoCast(target, SPELL_CHARGE);

                        if (target->HasAura(SPELL_DEFEND))
                            target->RemoveAuraFromStack(SPELL_DEFEND);
                    }
                }

                uiChargeTimer = urand(10000, 30000);
            }else uiChargeTimer -= uiDiff;

            if (uiThrustTimer <= uiDiff)
            {
                if (me->getVictim() && me->GetDistance(me->getVictim()) < 5.0f)
                    DoCast(me->getVictim(), SPELL_THRUST);

                uiThrustTimer = urand(8000, 14000);
            }else uiThrustTimer -= uiDiff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new generic_vehicleAI_toc5AI(creature);
    }
};

class boss_warrior_toc5 : public CreatureScript
{
public:
    boss_warrior_toc5() : CreatureScript("boss_warrior_toc5") { }

    // Marshal Jacob Alerius && Mokra the Skullcrusher || Warrior
    struct boss_warrior_toc5AI : public ScriptedAI
    {
        boss_warrior_toc5AI(Creature* creature) : ScriptedAI(creature)
        {
            instance = creature->GetInstanceScript();

            bDone = false;
            bHome = false;
            bCredited = false;

            uiPhase = 0;
            uiPhaseTimer = 0;

            me->SetReactState(REACT_PASSIVE);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_PC);
        }

        InstanceScript* instance;

        uint8 uiPhase;
        uint32 uiPhaseTimer;

        uint32 uiBladeStormTimer;
        uint32 uiInterceptTimer;
        uint32 uiMortalStrikeTimer;
        uint32 uiAttackTimer;
        uint32 uiResetTimer;

        bool bDone;
        bool bHome;
        bool bCredited;

        void Reset()
        {
            uiBladeStormTimer = urand(15000, 20000);
            uiInterceptTimer  = 7000;
            uiMortalStrikeTimer = urand(8000, 12000);
        }

        void JustReachedHome()
        {
            ScriptedAI::JustReachedHome();

            if (!bHome)
                return;

            uiPhaseTimer = 15000;
            uiPhase = 1;

            bHome = false;
        }

        void UpdateAI(const uint32 uiDiff)
        {
            if (!bDone && GrandChampionsOutVehicle(me))
            {
                bDone = true;

                DoScriptText(SAY_START_2, me);

                if (instance && me->GetGUID() == instance->GetData64(DATA_GRAND_CHAMPION_1))
                    me->SetHomePosition(739.678f, 662.541f, 412.393f, 4.49f);
                else if (instance && me->GetGUID() == instance->GetData64(DATA_GRAND_CHAMPION_2))
                    me->SetHomePosition(746.71f, 661.02f, 411.69f, 4.6f);
                else if (instance && me->GetGUID() == instance->GetData64(DATA_GRAND_CHAMPION_3))
                    me->SetHomePosition(754.34f, 660.70f, 412.39f, 4.79f);

                EnterEvadeMode();
                bHome = true;
            }

            if (uiPhaseTimer <= uiDiff)
            {
                if (uiPhase == 1)
                {
                    AggroAllPlayers(me);
                    uiPhase = 0;
                }
            }else uiPhaseTimer -= uiDiff;

            if (!UpdateVictim() || me->HasUnitMovementFlag(MOVEMENTFLAG_ONTRANSPORT) || me->GetVehicle())
                return;

            if (uiInterceptTimer <= uiDiff)
            {
                Map::PlayerList const& players = me->GetMap()->GetPlayers();
                if (me->GetMap()->IsDungeon() && !players.isEmpty())
                {
                    for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
                    {
                        Player* player = itr->getSource();
                        if (player && !player->isGameMaster() && me->IsInRange(player, 8.0f, 25.0f, false))
                        {
                            DoResetThreat();
                            me->AddThreat(player, 5.0f);
                            DoCast(player, SPELL_INTERCEPT);
                            break;
                        }
                    }
                }
                uiInterceptTimer = 7000;
            } else uiInterceptTimer -= uiDiff;

            if (uiBladeStormTimer <= uiDiff)
            {
                DoCastVictim(SPELL_BLADESTORM);
                uiBladeStormTimer = urand(15000, 20000);
            } else uiBladeStormTimer -= uiDiff;

            if (uiMortalStrikeTimer <= uiDiff)
            {
                DoCastVictim(SPELL_MORTAL_STRIKE);
                uiMortalStrikeTimer = urand(8000, 12000);
            } else uiMortalStrikeTimer -= uiDiff;

            DoMeleeAttackIfReady();
        }

        void DamageTaken(Unit* /*who*/, uint32& damage)
        {
            if (!bCredited && damage >= me->GetHealth())
            {
                bCredited = true;
                // Instance encounter counting mechanics
                DoCastToAllHostilePlayers(SPELL_GRAND_CHAMPIONS_CREDIT);
            }
        }

        void JustDied(Unit* /*killer*/)
        {
            if (instance)
                instance->SetData(BOSS_GRAND_CHAMPIONS, DONE);
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_warrior_toc5AI(creature);
    }
};

class boss_mage_toc5 : public CreatureScript
{
public:
    boss_mage_toc5() : CreatureScript("boss_mage_toc5") { }

    // Ambrose Boltspark && Eressea Dawnsinger || Mage
    struct boss_mage_toc5AI : public ScriptedAI
    {
        boss_mage_toc5AI(Creature* creature) : ScriptedAI(creature)
        {
            instance = creature->GetInstanceScript();

            bDone = false;
            bHome = false;
            bCredited = false;

            uiPhase = 0;
            uiPhaseTimer = 0;

            me->SetReactState(REACT_PASSIVE);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_PC);
        }

        InstanceScript* instance;

        uint8 uiPhase;
        uint32 uiPhaseTimer;

        uint32 uiFireBallTimer;
        uint32 uiBlastWaveTimer;
        uint32 uiHasteTimer;
        uint32 uiPolymorphTimer;

        bool bDone;
        bool bHome;
        bool bCredited;

        void Reset()
        {
            uiFireBallTimer = 5000;
            uiPolymorphTimer  = 8000;
            uiBlastWaveTimer = 12000;
            uiHasteTimer = 22000;
        }

        void JustReachedHome()
        {
            ScriptedAI::JustReachedHome();

            if (!bHome)
                return;

            uiPhaseTimer = 15000;
            uiPhase = 1;

            bHome = false;
        }

        void UpdateAI(const uint32 uiDiff)
        {
            if (!bDone && GrandChampionsOutVehicle(me))
            {
                bDone = true;

                if (instance && me->GetGUID() == instance->GetData64(DATA_GRAND_CHAMPION_1))
                    me->SetHomePosition(739.678f, 662.541f, 412.393f, 4.49f);
                else if (instance && me->GetGUID() == instance->GetData64(DATA_GRAND_CHAMPION_2))
                    me->SetHomePosition(746.71f, 661.02f, 411.69f, 4.6f);
                else if (instance && me->GetGUID() == instance->GetData64(DATA_GRAND_CHAMPION_3))
                    me->SetHomePosition(754.34f, 660.70f, 412.39f, 4.79f);

                if (instance)
                    instance->SetData(BOSS_GRAND_CHAMPIONS, IN_PROGRESS);

                EnterEvadeMode();
                bHome = true;
            }

            if (uiPhaseTimer <= uiDiff)
            {
                if (uiPhase == 1)
                {
                    AggroAllPlayers(me);
                    uiPhase = 0;
                }
            }else uiPhaseTimer -= uiDiff;

            if (!UpdateVictim() || me->HasUnitMovementFlag(MOVEMENTFLAG_ONTRANSPORT) || me->GetVehicle())
                return;

            if (uiFireBallTimer <= uiDiff)
            {
                DoCastVictim(SPELL_FIREBALL);
                uiFireBallTimer = 5000;
            } else uiFireBallTimer -= uiDiff;

            if (uiPolymorphTimer <= uiDiff)
            {
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                    DoCast(target, SPELL_POLYMORPH);
                uiPolymorphTimer = 8000;
            } else uiPolymorphTimer -= uiDiff;

            if (uiBlastWaveTimer <= uiDiff)
            {
                DoCastAOE(SPELL_BLAST_WAVE, false);
                uiBlastWaveTimer = 13000;
            } else uiBlastWaveTimer -= uiDiff;

            if (uiHasteTimer <= uiDiff)
            {
                me->InterruptNonMeleeSpells(true);

                DoCast(me, SPELL_HASTE);
                uiHasteTimer = 22000;
            } else uiHasteTimer -= uiDiff;

            DoMeleeAttackIfReady();
        }

        void DamageTaken(Unit* /*who*/, uint32& damage)
        {
            if (!bCredited && damage >= me->GetHealth())
            {
                bCredited = true;
                // Instance encounter counting mechanics
                DoCastToAllHostilePlayers(SPELL_GRAND_CHAMPIONS_CREDIT);
            }
        }

        void JustDied(Unit* /*killer*/)
        {
            DoScriptText(SAY_START_1, me);
            if (instance)
                instance->SetData(BOSS_GRAND_CHAMPIONS, DONE);
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_mage_toc5AI(creature);
    }
};

class boss_shaman_toc5 : public CreatureScript
{
public:
    boss_shaman_toc5() : CreatureScript("boss_shaman_toc5") { }

    // Colosos && Runok Wildmane || Shaman
    struct boss_shaman_toc5AI : public ScriptedAI
    {
        boss_shaman_toc5AI(Creature* creature) : ScriptedAI(creature)
        {
            instance = creature->GetInstanceScript();

            bDone = false;
            bHome = false;
            bCredited = false;

            uiPhase = 0;
            uiPhaseTimer = 0;

            me->SetReactState(REACT_PASSIVE);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_PC);
        }

        InstanceScript* instance;

        uint8 uiPhase;
        uint32 uiPhaseTimer;

        uint32 uiChainLightningTimer;
        uint32 uiEartShieldTimer;
        uint32 uiHealingWaveTimer;
        uint32 uiHexMendingTimer;

        bool bDone;
        bool bHome;
        bool bCredited;

        void Reset()
        {
            uiChainLightningTimer = 16000;
            uiHealingWaveTimer = 12000;
            uiEartShieldTimer = urand(30000, 35000);
            uiHexMendingTimer = urand(20000, 25000);
        }

        void EnterCombat(Unit* who)
        {
            DoCast(me, SPELL_EARTH_SHIELD);
            DoCast(who, SPELL_HEX_OF_MENDING);
        };

        void JustReachedHome()
        {
            ScriptedAI::JustReachedHome();

            if (!bHome)
                return;

            uiPhaseTimer = 15000;
            uiPhase = 1;

            bHome = false;
        }

        void UpdateAI(const uint32 uiDiff)
        {
            if (!bDone && GrandChampionsOutVehicle(me))
            {
                bDone = true;

                if (instance && me->GetGUID() == instance->GetData64(DATA_GRAND_CHAMPION_1))
                    me->SetHomePosition(739.678f, 662.541f, 412.393f, 4.49f);
                else if (instance && me->GetGUID() == instance->GetData64(DATA_GRAND_CHAMPION_2))
                    me->SetHomePosition(746.71f, 661.02f, 411.69f, 4.6f);
                else if (instance && me->GetGUID() == instance->GetData64(DATA_GRAND_CHAMPION_3))
                    me->SetHomePosition(754.34f, 660.70f, 412.39f, 4.79f);

                if (instance)
                    instance->SetData(BOSS_GRAND_CHAMPIONS, IN_PROGRESS);

                EnterEvadeMode();
                bHome = true;
            }

            if (uiPhaseTimer <= uiDiff)
            {
                if (uiPhase == 1)
                {
                    AggroAllPlayers(me);
                    uiPhase = 0;
                }
            }else uiPhaseTimer -= uiDiff;

            if (!UpdateVictim() || me->HasUnitMovementFlag(MOVEMENTFLAG_ONTRANSPORT) || me->GetVehicle())
                return;

            if (uiChainLightningTimer <= uiDiff)
            {
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                    DoCast(target, SPELL_CHAIN_LIGHTNING);

                uiChainLightningTimer = 16000;
            } else uiChainLightningTimer -= uiDiff;

            if (uiHealingWaveTimer <= uiDiff)
            {
                bool bChance = urand(0, 1);

                if (!bChance)
                {
                    if (Unit* pFriend = DoSelectLowestHpFriendly(40))
                        DoCast(pFriend, SPELL_HEALING_WAVE);
                } else
                    DoCast(me, SPELL_HEALING_WAVE);

                uiHealingWaveTimer = 12000;
            } else uiHealingWaveTimer -= uiDiff;

            if (uiEartShieldTimer <= uiDiff)
            {
                DoCast(me, SPELL_EARTH_SHIELD);

                uiEartShieldTimer = urand(30000, 35000);
            } else uiEartShieldTimer -= uiDiff;

            if (uiHexMendingTimer <= uiDiff)
            {
                DoCastVictim(SPELL_HEX_OF_MENDING, true);

                uiHexMendingTimer = urand(20000, 25000);
            } else uiHexMendingTimer -= uiDiff;

            DoMeleeAttackIfReady();
        }

        void DamageTaken(Unit* /*who*/, uint32& damage)
        {
            if (!bCredited && damage >= me->GetHealth())
            {
                bCredited = true;
                // Instance encounter counting mechanics
                DoCastToAllHostilePlayers(SPELL_GRAND_CHAMPIONS_CREDIT);
            }
        }

        void JustDied(Unit* /*killer*/)
        {
            DoScriptText(SAY_START_1, me);
            if (instance)
                instance->SetData(BOSS_GRAND_CHAMPIONS, DONE);
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_shaman_toc5AI(creature);
    }
};


class boss_hunter_toc5 : public CreatureScript
{
public:
    boss_hunter_toc5() : CreatureScript("boss_hunter_toc5") { }

        // Jaelyne Evensong && Zul'tore || Hunter
    struct boss_hunter_toc5AI : public ScriptedAI
    {
        boss_hunter_toc5AI(Creature* creature) : ScriptedAI(creature)
        {
            instance = creature->GetInstanceScript();

            bDone = false;
            bHome = false;
            bCredited = false;

            uiPhase = 0;
            uiPhaseTimer = 0;

            me->SetReactState(REACT_PASSIVE);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_PC);
        }

        InstanceScript* instance;

        uint8 uiPhase;
        uint32 uiPhaseTimer;

        uint32 uiShootTimer;
        uint32 uiDisengageCooldown;
        uint32 uiMultiShotTimer;
        uint32 uiLightningArrowsTimer;

        uint64 uiTargetGUID;

        bool bShoot;
        bool bDone;
        bool bHome;
        bool bCredited;

        void Reset()
        {
            uiShootTimer = 12000;
            uiMultiShotTimer = 0;
            uiLightningArrowsTimer = 7000;
            uiDisengageCooldown = 10000;

            uiTargetGUID = 0;

            bShoot = false;
        }

        void JustReachedHome()
        {
            ScriptedAI::JustReachedHome();

            if (!bHome)
                return;

            uiPhaseTimer = 15000;
            uiPhase = 1;

            bHome = false;
        }

        void UpdateAI(const uint32 uiDiff)
        {
            if (!bDone && GrandChampionsOutVehicle(me))
            {
                bDone = true;

                if (instance && me->GetGUID() == instance->GetData64(DATA_GRAND_CHAMPION_1))
                    me->SetHomePosition(739.678f, 662.541f, 412.393f, 4.49f);
                else if (instance && me->GetGUID() == instance->GetData64(DATA_GRAND_CHAMPION_2))
                    me->SetHomePosition(746.71f, 661.02f, 411.69f, 4.6f);
                else if (instance && me->GetGUID() == instance->GetData64(DATA_GRAND_CHAMPION_3))
                    me->SetHomePosition(754.34f, 660.70f, 412.39f, 4.79f);

                if (instance)
                    instance->SetData(BOSS_GRAND_CHAMPIONS, IN_PROGRESS);

                EnterEvadeMode();
                bHome = true;
            }

            if (uiPhaseTimer <= uiDiff)
            {
                if (uiPhase == 1)
                {
                    AggroAllPlayers(me);
                    uiPhase = 0;
                }
            }else uiPhaseTimer -= uiDiff;

            if (!UpdateVictim() || me->HasUnitMovementFlag(MOVEMENTFLAG_ONTRANSPORT) || me->GetVehicle())
                return;

            if (uiDisengageCooldown <= uiDiff)
            {
                if (me->IsWithinDistInMap(me->getVictim(), 5) && uiDisengageCooldown == 0)
                {
                    DoCast(me, SPELL_DISENGAGE);
                    uiDisengageCooldown = 35000;
                }
                uiDisengageCooldown = 20000;
            }else uiDisengageCooldown -= uiDiff;

            if (uiLightningArrowsTimer <= uiDiff)
            {
                DoCastAOE(SPELL_LIGHTNING_ARROWS, false);
                uiLightningArrowsTimer = 7000;
            } else uiLightningArrowsTimer -= uiDiff;

            if (uiShootTimer <= uiDiff)
            {
                if (Unit* target = SelectTarget(SELECT_TARGET_FARTHEST, 0, 30.0f))
                {
                    uiTargetGUID = target->GetGUID();
                    DoCast(target, SPELL_SHOOT);
                }
                uiShootTimer = 12000;
                uiMultiShotTimer = 3000;
                bShoot = true;
            } else uiShootTimer -= uiDiff;

            if (bShoot && uiMultiShotTimer <= uiDiff)
            {
                me->InterruptNonMeleeSpells(true);
                Unit* target = Unit::GetUnit(*me, uiTargetGUID);

                if (target && me->IsInRange(target, 5.0f, 30.0f, false))
                {
                    DoCast(target, SPELL_MULTI_SHOT);
                }
                else
                {
                    Map::PlayerList const& players = me->GetMap()->GetPlayers();
                    if (me->GetMap()->IsDungeon() && !players.isEmpty())
                    {
                        for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
                        {
                            Player* player = itr->getSource();
                            if (player && !player->isGameMaster() && me->IsInRange(player, 5.0f, 30.0f, false))
                            {
                                DoCast(player, SPELL_MULTI_SHOT);
                                break;
                            }
                        }
                    }
                }
                bShoot = false;
            } else uiMultiShotTimer -= uiDiff;

            DoMeleeAttackIfReady();
        }

        void DamageTaken(Unit* /*who*/, uint32& damage)
        {
            if (!bCredited && damage >= me->GetHealth())
            {
                bCredited = true;
                // Instance encounter counting mechanics
                DoCastToAllHostilePlayers(SPELL_GRAND_CHAMPIONS_CREDIT);
            }
        }

        void JustDied(Unit* /*killer*/)
        {
            DoScriptText(SAY_START_1, me);
            if (instance)
                instance->SetData(BOSS_GRAND_CHAMPIONS, DONE);
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_hunter_toc5AI(creature);
    }
};


class boss_rouge_toc5 : public CreatureScript
{
public:
    boss_rouge_toc5() : CreatureScript("boss_rouge_toc5") { }

    // Lana Stouthammer Evensong && Deathstalker Visceri || Rouge
    struct boss_rouge_toc5AI : public ScriptedAI
    {
        boss_rouge_toc5AI(Creature* creature) : ScriptedAI(creature)
        {
            instance = creature->GetInstanceScript();

            bDone = false;
            bHome = false;
            bCredited = false;

            uiPhase = 0;
            uiPhaseTimer = 0;

            me->SetReactState(REACT_PASSIVE);
            // THIS IS A HACK, SHOULD BE REMOVED WHEN THE EVENT IS FULL SCRIPTED
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_PC);
        }

        InstanceScript* instance;

        uint8 uiPhase;
        uint32 uiPhaseTimer;
        uint32 uiEviscerateTimer;
        uint32 uiFanKivesTimer;
        uint32 uiPosionBottleTimer;

        bool bDone;
        bool bHome;
        bool bCredited;

        void Reset()
        {
            uiEviscerateTimer = 8000;
            uiFanKivesTimer   = 14000;
            uiPosionBottleTimer = 19000;
        }

        void JustReachedHome()
        {
            ScriptedAI::JustReachedHome();

            if (!bHome)
                return;

            uiPhaseTimer = 15000;
            uiPhase = 1;

            bHome = false;
        }

        void UpdateAI(const uint32 uiDiff)
        {
            if (!bDone && GrandChampionsOutVehicle(me))
            {
                bDone = true;

                if (instance && me->GetGUID() == instance->GetData64(DATA_GRAND_CHAMPION_1))
                    me->SetHomePosition(739.678f, 662.541f, 412.393f, 4.49f);
                else if (instance && me->GetGUID() == instance->GetData64(DATA_GRAND_CHAMPION_2))
                    me->SetHomePosition(746.71f, 661.02f, 411.69f, 4.6f);
                else if (instance && me->GetGUID() == instance->GetData64(DATA_GRAND_CHAMPION_3))
                    me->SetHomePosition(754.34f, 660.70f, 412.39f, 4.79f);

                if (instance)
                    instance->SetData(BOSS_GRAND_CHAMPIONS, IN_PROGRESS);

                EnterEvadeMode();
                bHome = true;
            }

            if (uiPhaseTimer <= uiDiff)
            {
                if (uiPhase == 1)
                {
                    AggroAllPlayers(me);
                    uiPhase = 0;
                }
            } else uiPhaseTimer -= uiDiff;

            if (!UpdateVictim() || me->HasUnitMovementFlag(MOVEMENTFLAG_ONTRANSPORT) || me->GetVehicle())
                return;

            if (uiEviscerateTimer <= uiDiff)
            {
                DoCast(me->getVictim(), SPELL_EVISCERATE);
                uiEviscerateTimer = 8000;
            } else uiEviscerateTimer -= uiDiff;

            if (uiFanKivesTimer <= uiDiff)
            {
                DoCastAOE(SPELL_FAN_OF_KNIVES, false);
                uiFanKivesTimer = 14000;
            } else uiFanKivesTimer -= uiDiff;

            if (uiPosionBottleTimer <= uiDiff)
            {
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                    DoCast(target, SPELL_POISON_BOTTLE);
                uiPosionBottleTimer = 19000;
            } else uiPosionBottleTimer -= uiDiff;

            DoMeleeAttackIfReady();
        }

        void DamageTaken(Unit* /*who*/, uint32& damage)
        {
            if (!bCredited && damage >= me->GetHealth())
            {
                bCredited = true;
                // Instance encounter counting mechanics
                DoCastToAllHostilePlayers(SPELL_GRAND_CHAMPIONS_CREDIT);
            }
        }

        void JustDied(Unit* /*killer*/)
        {
            if (instance)
                instance->SetData(BOSS_GRAND_CHAMPIONS, DONE);
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_rouge_toc5AI(creature);
    }
};

class achievement_toc5_grand_champions : public AchievementCriteriaScript
{
    public:
        uint32 creature_entry;

        achievement_toc5_grand_champions(const char* name, uint32 original_entry) : AchievementCriteriaScript(name) {
            creature_entry = original_entry;
        }

        bool OnCheck(Player* source, Unit* target)
        {
            if (!target)
                return false;

            if (Creature* creature = target->ToCreature())
                if (creature->GetOriginalEntry() == creature_entry)
                    return true;

            return false;
        }
};

void AddSC_boss_grand_champions()
{
    new generic_vehicleAI_toc5();
    new boss_warrior_toc5();
    new boss_mage_toc5();
    new boss_shaman_toc5();
    new boss_hunter_toc5();
    new boss_rouge_toc5();
    new achievement_toc5_grand_champions("achievement_toc5_champions_mokra", NPC_MOKRA);
    new achievement_toc5_grand_champions("achievement_toc5_champions_eressea", NPC_ERESSEA);
    new achievement_toc5_grand_champions("achievement_toc5_champions_runok", NPC_RUNOK);
    new achievement_toc5_grand_champions("achievement_toc5_champions_zultore", NPC_ZULTORE);
    new achievement_toc5_grand_champions("achievement_toc5_champions_visceri", NPC_VISCERI);
    new achievement_toc5_grand_champions("achievement_toc5_champions_alerius", NPC_JACOB);
    new achievement_toc5_grand_champions("achievement_toc5_champions_ambrose", NPC_AMBROSE);
    new achievement_toc5_grand_champions("achievement_toc5_champions_colosos", NPC_COLOSOS);
    new achievement_toc5_grand_champions("achievement_toc5_champions_jaelyne", NPC_JAELYNE);
    new achievement_toc5_grand_champions("achievement_toc5_champions_lana", NPC_LANA);
}
