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
SDName: Icecrown
SD%Complete: 100
SDComment: Quest support: 12807
SDCategory: Icecrown
EndScriptData */

/* ContentData
npc_arete
EndContentData */

#include "ScriptPCH.h"
#include "Vehicles.h"

/*######
## npc_arete
######*/

#define GOSSIP_ARETE_ITEM1 "Lord-Commander, I would hear your tale."
#define GOSSIP_ARETE_ITEM2 "<You nod slightly but do not complete the motion as the Lord-Commander narrows his eyes before he continues.>"
#define GOSSIP_ARETE_ITEM3 "I thought that they now called themselves the Scarlet Onslaught?"
#define GOSSIP_ARETE_ITEM4 "Where did the grand admiral go?"
#define GOSSIP_ARETE_ITEM5 "That's fine. When do I start?"
#define GOSSIP_ARETE_ITEM6 "Let's finish this!"
#define GOSSIP_ARETE_ITEM7 "That's quite a tale, Lord-Commander."

enum eArete
{
    GOSSIP_TEXTID_ARETE1        = 13525,
    GOSSIP_TEXTID_ARETE2        = 13526,
    GOSSIP_TEXTID_ARETE3        = 13527,
    GOSSIP_TEXTID_ARETE4        = 13528,
    GOSSIP_TEXTID_ARETE5        = 13529,
    GOSSIP_TEXTID_ARETE6        = 13530,
    GOSSIP_TEXTID_ARETE7        = 13531,

    QUEST_THE_STORY_THUS_FAR    = 12807
};

class npc_arete : public CreatureScript
{
public:
    npc_arete() : CreatureScript("npc_arete") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->isQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        if (player->GetQuestStatus(QUEST_THE_STORY_THUS_FAR) == QUEST_STATUS_INCOMPLETE)
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ARETE_ITEM1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
            player->SEND_GOSSIP_MENU(GOSSIP_TEXTID_ARETE1, creature->GetGUID());
            return true;
        }

        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*uiSender*/, uint32 uiAction)
    {
        player->PlayerTalkClass->ClearMenus();
        switch (uiAction)
        {
            case GOSSIP_ACTION_INFO_DEF+1:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ARETE_ITEM2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                player->SEND_GOSSIP_MENU(GOSSIP_TEXTID_ARETE2, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+2:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ARETE_ITEM3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
                player->SEND_GOSSIP_MENU(GOSSIP_TEXTID_ARETE3, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+3:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ARETE_ITEM4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
                player->SEND_GOSSIP_MENU(GOSSIP_TEXTID_ARETE4, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+4:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ARETE_ITEM5, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
                player->SEND_GOSSIP_MENU(GOSSIP_TEXTID_ARETE5, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+5:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ARETE_ITEM6, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6);
                player->SEND_GOSSIP_MENU(GOSSIP_TEXTID_ARETE6, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+6:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ARETE_ITEM7, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 7);
                player->SEND_GOSSIP_MENU(GOSSIP_TEXTID_ARETE7, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+7:
                player->CLOSE_GOSSIP_MENU();
                player->AreaExploredOrEventHappens(QUEST_THE_STORY_THUS_FAR);
                break;
        }

        return true;
    }
};

/*######
## npc_argent_squire
######*/

enum eArgentSquire
{
    QUEST_THE_ASPIRANT_S_CHALLENGE_H                    = 13680,
    QUEST_THE_ASPIRANT_S_CHALLENGE_A                    = 13679,

    QUEST_THE_VALIANT_S_CHALLENGE_SM                    = 13731,
    QUEST_THE_VALIANT_S_CHALLENGE_UC                    = 13729,
    QUEST_THE_VALIANT_S_CHALLENGE_TB                    = 13728,
    QUEST_THE_VALIANT_S_CHALLENGE_SJ                    = 13727,
    QUEST_THE_VALIANT_S_CHALLENGE_OG                    = 13726,
    QUEST_THE_VALIANT_S_CHALLENGE_DA                    = 13725,
    QUEST_THE_VALIANT_S_CHALLENGE_EX                    = 13724,
    QUEST_THE_VALIANT_S_CHALLENGE_GN                    = 13723,
    QUEST_THE_VALIANT_S_CHALLENGE_IF                    = 13713,
    QUEST_THE_VALIANT_S_CHALLENGE_SW                    = 13699,

    QUEST_THE_BLACK_KNGIHT_S_FALL                       = 13664,

    NPC_SQUIRE_DAVID                                    = 33447,
    NPC_SQUIRE_DANNY                                    = 33518,
    NPC_SQUIRE_CAVIN                                    = 33522,

    NPC_ARGENT_VALIANT                                  = 33448,
    NPC_ARGENT_CHAMPION                                 = 33707,
    NPC_BLACK_KNIGHT                                    = 33785,

    GOSSIP_TEXTID_SQUIRE                                = 14407
};

#define GOSSIP_SQUIRE_ITEM_1 "I am ready to fight!"
#define GOSSIP_SQUIRE_ITEM_2 "How do the Argent Crusader raiders fight?"

class npc_argent_squire : public CreatureScript
{
public:
    npc_argent_squire() : CreatureScript("npc_argent_squire") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {

        // Squire David handles Aspirant Stuff
        if (creature->GetEntry() == NPC_SQUIRE_DAVID)
        {
            if (player->GetQuestStatus(QUEST_THE_ASPIRANT_S_CHALLENGE_H) == QUEST_STATUS_INCOMPLETE ||
                    player->GetQuestStatus(QUEST_THE_ASPIRANT_S_CHALLENGE_A) == QUEST_STATUS_INCOMPLETE)//We need more info about it.
            {
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SQUIRE_ITEM_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SQUIRE_ITEM_2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
            }
        }

        // Squire Danny handles Valiant Stuff
        if (creature->GetEntry() == NPC_SQUIRE_DANNY)
        {
            if (player->GetQuestStatus(QUEST_THE_VALIANT_S_CHALLENGE_SM) == QUEST_STATUS_INCOMPLETE ||
                    player->GetQuestStatus(QUEST_THE_VALIANT_S_CHALLENGE_UC) == QUEST_STATUS_INCOMPLETE ||
                    player->GetQuestStatus(QUEST_THE_VALIANT_S_CHALLENGE_TB) == QUEST_STATUS_INCOMPLETE ||
                    player->GetQuestStatus(QUEST_THE_VALIANT_S_CHALLENGE_SJ) == QUEST_STATUS_INCOMPLETE ||
                    player->GetQuestStatus(QUEST_THE_VALIANT_S_CHALLENGE_OG) == QUEST_STATUS_INCOMPLETE ||
                    player->GetQuestStatus(QUEST_THE_VALIANT_S_CHALLENGE_DA) == QUEST_STATUS_INCOMPLETE ||
                    player->GetQuestStatus(QUEST_THE_VALIANT_S_CHALLENGE_EX) == QUEST_STATUS_INCOMPLETE ||
                    player->GetQuestStatus(QUEST_THE_VALIANT_S_CHALLENGE_GN) == QUEST_STATUS_INCOMPLETE ||
                    player->GetQuestStatus(QUEST_THE_VALIANT_S_CHALLENGE_IF) == QUEST_STATUS_INCOMPLETE ||
                    player->GetQuestStatus(QUEST_THE_VALIANT_S_CHALLENGE_SW) == QUEST_STATUS_INCOMPLETE)
            {
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SQUIRE_ITEM_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
            }
        }

        // Squire Danny handles the Black Knight
        if (creature->GetEntry() == NPC_SQUIRE_CAVIN)
        {
            if (player->GetQuestStatus(QUEST_THE_BLACK_KNGIHT_S_FALL) == QUEST_STATUS_INCOMPLETE)
            {
                // placeholder
            }
        }

        player->SEND_GOSSIP_MENU(GOSSIP_TEXTID_SQUIRE, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*uiSender*/, uint32 uiAction)
    {
        player->PlayerTalkClass->ClearMenus();
        if (uiAction == GOSSIP_ACTION_INFO_DEF+1)
        {
            player->CLOSE_GOSSIP_MENU();
            if (creature->GetEntry() == NPC_SQUIRE_DAVID)
                creature->SummonCreature(NPC_ARGENT_VALIANT, 8575.451f, 952.472f, 547.554f, 0.38f);
            else if (creature->GetEntry() == NPC_SQUIRE_DANNY)
                creature->SummonCreature(NPC_ARGENT_CHAMPION, 8534.675781f, 1069.993042f, 552.022827f, 1.274804f);

        }
        return true;
    }
};

/*######
## npc_argent_combatant
######*/

enum eArgentCombatant
{
    SPELL_CHARGE                = 63010,
    SPELL_SHIELD_BREAKER        = 65147,
    SPELL_DEFEND                = 62719,
    SPELL_THRUST                = 62544,

    NPC_ARGENT_VALIANT_CREDIT   = 38595,
    NPC_ARGENT_CHAMPION_CREDIT  = 33708,
};

class npc_argent_combatant : public CreatureScript
{
public:
    npc_argent_combatant() : CreatureScript("npc_argent_combatant") { }

    struct npc_argent_combatantAI : public ScriptedAI
    {
        npc_argent_combatantAI(Creature* creature) : ScriptedAI(creature)
        {
            if (creature->GetEntry() == NPC_ARGENT_VALIANT)
            {
                creature->GetMotionMaster()->MovePoint(0, 8599.258f, 963.951f, 547.553f);
                creature->SetHomePosition(8599.258f, 963.951f, 547.553f, 0.18f);
            }
            if (creature->GetEntry() == NPC_ARGENT_CHAMPION)
            {
                creature->GetMotionMaster()->MovePoint(0, 8557.131836f, 1109.635742f, 556.787476f);
                creature->SetHomePosition(8557.131836f, 1109.635742f, 556.787476f, 1.27f);
            }
            creature->setFaction(35); //wrong faction in db?
        }

        uint32 uiChargeTimer;
        uint32 uiShieldBreakerTimer;
        uint32 uiShieldTimer;
        uint32 uiThrustTimer;
        bool bCharge;

        void Reset()
        {
            uiChargeTimer = 12000;
            uiShieldBreakerTimer = 10000;
            uiShieldTimer = 4000;
            uiThrustTimer = 2000;
            bCharge = false;
        }

        void EnterCombat(Unit* /*who*/)
        {
            for (uint8 i = 0; i < 3; ++i)
                DoCast(me, SPELL_DEFEND, true);
        }

        void MovementInform(uint32 uiType, uint32 /*uiId*/)
        {
            if (uiType != POINT_MOTION_TYPE)
                return;

            // charge after moving away from the victim
            if (me->isInCombat() && bCharge)
            {
                me->GetMotionMaster()->Clear();
                // but only after rangecheck
                if (me->GetDistance(me->getVictim()) > 5.0f && me->GetDistance(me->getVictim()) <= 30.0f)
                    DoCastVictim(SPELL_CHARGE);
                me->GetMotionMaster()->MoveChase(me->getVictim());
                uiChargeTimer = 7000;
                bCharge = false;
            }
            else
                me->setFaction(14);
        }

        void DamageTaken(Unit* pDoneBy, uint32& uiDamage)
        {
            if (uiDamage >= me->GetHealth() && pDoneBy->GetTypeId() == TYPEID_PLAYER)
            {
                uiDamage = 0;
                if (me->GetEntry() == NPC_ARGENT_VALIANT)
                    CAST_PLR(pDoneBy)->KilledMonsterCredit(NPC_ARGENT_VALIANT_CREDIT, 0);
                if (me->GetEntry() == NPC_ARGENT_CHAMPION)
                    CAST_PLR(pDoneBy)->KilledMonsterCredit(NPC_ARGENT_CHAMPION_CREDIT, 0);
                me->setFaction(35);
                me->DespawnOrUnsummon(5000);
                me->SetHomePosition(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation());
                EnterEvadeMode();
            }
        }

        void UpdateAI(const uint32 uiDiff)
        {
            if (!UpdateVictim())
                return;

            if (uiShieldTimer <= uiDiff)
            {
                me->CastSpell(me, SPELL_DEFEND);
                uiShieldTimer = 4000;
            } else uiShieldTimer -= uiDiff;

            if (uiChargeTimer <= uiDiff && !bCharge)
            {
                // move away for charge...
                float angle = me->GetAngle(me->getVictim());
                float x = me->GetPositionX() + 20.0f * cos(angle);
                float y = me->GetPositionY() + 20.0f * sin(angle);
                me->GetMotionMaster()->MovePoint(0, x, y, me->GetPositionZ());
                bCharge = true;
            } else uiChargeTimer -= uiDiff;

            // prevent shieldbreaker while moving away, npc is not facing player at that time
            if (bCharge)
                return;

            if (uiShieldBreakerTimer <= uiDiff)
            {
                DoCastVictim(SPELL_SHIELD_BREAKER);
                uiShieldBreakerTimer = 10000;
            } else uiShieldBreakerTimer -= uiDiff;

            if (me->IsWithinMeleeRange(me->getVictim()))
            {
                if (uiThrustTimer <= uiDiff)
                {
                    DoCastVictim(SPELL_THRUST);
                    uiThrustTimer = 2000;
                }
                else uiThrustTimer -= uiDiff;
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_argent_combatantAI(creature);
    }
};

/*#####
## npc_argent_faction_rider
######*/

enum eArgentFactionRiders
{
    NPC_EXODAR_VALIANT          = 33562,
    NPC_DARNASSUS_VALIANT       = 33559,
    NPC_GNOMEREGAN_VALIANT      = 33558,
    NPC_IRONFORGE_VALIANT       = 33564,
    NPC_STORMWIND_VALIANT       = 33561,
    NPC_SILVERMOON_VALIANT      = 33382,
    NPC_THUNDER_BLUFF_VALIANT   = 33383,
    NPC_UNDERCITY_VALIANT       = 33384,
    NPC_ORGRIMMAR_VALIANT       = 33306,
    NPC_SENJIN_VALIANT          = 33285,
    NPC_EXODAR_CHAMPION         = 33739,
    NPC_DARNASSUS_CHAMPION      = 33738,
    NPC_STORMWIND_CHAMPION      = 33747,
    NPC_IRONFORGE_CHAMPION      = 33743,
    NPC_GNOMEREGAN_CHAMPION     = 33740,
    NPC_SILVERMOON_CHAMPION     = 33746,
    NPC_THUNDER_BLUFF_CHAMPION  = 33748,
    NPC_ORGRIMMAR_CHAMPION      = 33744,
    NPC_SENJIN_CHAMPION         = 33745,
    NPC_UNDERCITY_CHAMPION      = 33749,

    QUEST_AMONG_CHAMPIONS_H_DK  = 13814,
    QUEST_AMONG_CHAMPIONS_H     = 13811,
    QUEST_AMONG_CHAMPIONS_A_DK  = 13793,
    QUEST_AMONG_CHAMPIONS_A     = 13790,
    QUEST_GRAND_MELEE_SM        = 13787,
    QUEST_GRAND_MELEE_UC        = 13782,
    QUEST_GRAND_MELEE_TB        = 13777,
    QUEST_GRAND_MELEE_SJ        = 13772,
    QUEST_GRAND_MELEE_OG        = 13767,
    QUEST_GRAND_MELEE_DA        = 13761,
    QUEST_GRAND_MELEE_EX        = 13756,
    QUEST_GRAND_MELEE_GN        = 13750,
    QUEST_GRAND_MELEE_IF        = 13745,
    QUEST_GRAND_MELEE_SW        = 13665,

    SPELL_BESTED_DARNASSUS          = 64805,
    SPELL_BESTED_EXODAR             = 64808,
    SPELL_BESTED_GNOMEREGAN         = 64809,
    SPELL_BESTED_IRONFORGE          = 64810,
    SPELL_BESTED_ORGRIMMAR          = 64811,
    SPELL_BESTED_SENJIN             = 64812,
    SPELL_BESTED_SILVERMOON         = 64813,
    SPELL_BESTED_STORMWIND          = 64814,
    SPELL_BESTED_THUNDER_BLUFF      = 64815,
    SPELL_BESTED_UNDERCITY          = 64816,
    SPELL_MOUNTED_MELEE_VICTORY_C   = 63596,
    SPELL_MOUNTED_MELEE_VICTORY_V   = 62724,
    SPELL_READYJOUST_POSE_EFFECT    = 64723,

    ITEM_MARK_OF_CHAMPION       = 45500,
    ITEM_MARK_OF_VALIANT        = 45127,

    EVENT_START                 = 1,

    TYPE_VALIANT_ALLIANCE       = 1,
    TYPE_VALIANT_HORDE          = 2,
    TYPE_CHAMPION               = 3,
    TYPE_OTHER                  = 4,

    DATA_PLAYER                 = 1,
    DATA_TYPE                   = 2,
    DATA_DEFEATED               = 3,

    GOSSIP_TEXTID_CHAMPION      = 14421,
    GOSSIP_TEXTID_VALIANT       = 14384,
};

#define GOSSIP_FACTION_RIDER_1 "I am ready to fight!"

class npc_argent_faction_rider : public CreatureScript
{
public:
    npc_argent_faction_rider() : CreatureScript("npc_argent_faction_rider") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        // prevent gossip when defeated
        if (creature->GetAI()->GetData(DATA_DEFEATED) > 0)
            return false;

        uint32 type = creature->GetAI()->GetData(DATA_TYPE);

        // valiants can only be challenged by own faction, while champions fight every faction
        switch (type)
        {
            case TYPE_CHAMPION:
            {
                if (player->GetItemCount(ITEM_MARK_OF_CHAMPION, true) == 4)
                    return false;

                if (player->GetQuestStatus(QUEST_AMONG_CHAMPIONS_H_DK) == QUEST_STATUS_INCOMPLETE ||
                        player->GetQuestStatus(QUEST_AMONG_CHAMPIONS_H) == QUEST_STATUS_INCOMPLETE ||
                        player->GetQuestStatus(QUEST_AMONG_CHAMPIONS_A_DK) == QUEST_STATUS_INCOMPLETE ||
                        player->GetQuestStatus(QUEST_AMONG_CHAMPIONS_A) == QUEST_STATUS_INCOMPLETE)
                {
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_FACTION_RIDER_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
                }
                player->SEND_GOSSIP_MENU(GOSSIP_TEXTID_CHAMPION, creature->GetGUID());
                break;
            }
            case TYPE_VALIANT_ALLIANCE:
            {
                if (player->GetItemCount(ITEM_MARK_OF_VALIANT, true) == 3)
                    return false;

                if (player->GetQuestStatus(QUEST_GRAND_MELEE_EX) == QUEST_STATUS_INCOMPLETE ||
                        player->GetQuestStatus(QUEST_GRAND_MELEE_DA) == QUEST_STATUS_INCOMPLETE ||
                        player->GetQuestStatus(QUEST_GRAND_MELEE_GN) == QUEST_STATUS_INCOMPLETE ||
                        player->GetQuestStatus(QUEST_GRAND_MELEE_IF) == QUEST_STATUS_INCOMPLETE ||
                        player->GetQuestStatus(QUEST_GRAND_MELEE_SW) == QUEST_STATUS_INCOMPLETE)
                {
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_FACTION_RIDER_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
                }
                player->SEND_GOSSIP_MENU(GOSSIP_TEXTID_VALIANT, creature->GetGUID());
                break;
            }
            case TYPE_VALIANT_HORDE:
            {
                if (player->GetItemCount(ITEM_MARK_OF_VALIANT, true) == 3)
                    return false;

                if (player->GetQuestStatus(QUEST_GRAND_MELEE_SM) == QUEST_STATUS_INCOMPLETE ||
                        player->GetQuestStatus(QUEST_GRAND_MELEE_UC) == QUEST_STATUS_INCOMPLETE ||
                        player->GetQuestStatus(QUEST_GRAND_MELEE_TB) == QUEST_STATUS_INCOMPLETE ||
                        player->GetQuestStatus(QUEST_GRAND_MELEE_SJ) == QUEST_STATUS_INCOMPLETE ||
                        player->GetQuestStatus(QUEST_GRAND_MELEE_OG) == QUEST_STATUS_INCOMPLETE)
                {
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_FACTION_RIDER_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
                }
                player->SEND_GOSSIP_MENU(GOSSIP_TEXTID_VALIANT, creature->GetGUID());
                break;
            }
        }
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*uiSender*/, uint32 uiAction)
    {
        player->PlayerTalkClass->ClearMenus();
        if (uiAction == GOSSIP_ACTION_INFO_DEF+1)
        {
            player->CLOSE_GOSSIP_MENU();

            if (!player->GetVehicle())
                return false;

            creature->GetAI()->SetData(DATA_PLAYER, player->GetGUID());
            creature->GetAI()->DoAction(EVENT_START);
        }
        return true;
    }

    struct npc_argent_faction_riderAI : public ScriptedAI
    {
        npc_argent_faction_riderAI(Creature* creature) : ScriptedAI(creature) { }

        uint32 uiChargeTimer;
        uint32 uiShieldBreakerTimer;
        uint32 uiShieldTimer;
        uint32 uiThrustTimer;
        bool bCharge;
        bool bDefeated;
        Position arenaCenter;

        uint32 challengeeGUID;

        void Reset()
        {
            me->m_CombatDistance = 100.0f; // lawl, copied from zuldrak.cpp
            me->setFaction(35);
            me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            DoCast(me, SPELL_READYJOUST_POSE_EFFECT, true);

            uiChargeTimer = 7000;
            uiShieldBreakerTimer = 10000;
            uiShieldTimer = 4000;
            uiThrustTimer = 2000;
            bCharge = false;
            bDefeated = false;

            challengeeGUID = 0;

            if (GetCustomType() == TYPE_CHAMPION)
                arenaCenter.Relocate(8428.757f, 945.349f, 544.675f);
            else if (GetCustomType() == TYPE_VALIANT_ALLIANCE)
                arenaCenter.Relocate(8656.402f, 722.827f, 547.523f);
            else if (GetCustomType() == TYPE_VALIANT_HORDE)
                arenaCenter.Relocate(8334.375f, 721.165f, 553.702f);

        }

        uint32 GetData(uint32 type)
        {
            if (type == DATA_TYPE)
                return GetCustomType();

            if (type == DATA_DEFEATED)
                return bDefeated ? 1 : 0;

            return 0;
        }

        void SetData(uint32 type, uint32 data)
        {
            if (type == DATA_PLAYER)
                challengeeGUID = data;
        }

        void DoAction(int32 const type)
        {
            if (type == EVENT_START)
            {
                // check valid player
                Player* challengee = ObjectAccessor::GetPlayer(*me, challengeeGUID);
                if (!challengee)
                    return;

                // check for cooldown
                bool playerCooldown;
                switch (me->GetEntry())
                {
                    case NPC_EXODAR_CHAMPION:
                        playerCooldown = challengee->HasAura(SPELL_BESTED_EXODAR);
                        break;
                    case NPC_DARNASSUS_CHAMPION:
                        playerCooldown = challengee->HasAura(SPELL_BESTED_DARNASSUS);
                        break;
                    case NPC_STORMWIND_CHAMPION:
                        playerCooldown = challengee->HasAura(SPELL_BESTED_STORMWIND);
                        break;
                    case NPC_IRONFORGE_CHAMPION:
                        playerCooldown = challengee->HasAura(SPELL_BESTED_IRONFORGE);
                        break;
                    case NPC_GNOMEREGAN_CHAMPION:
                        playerCooldown = challengee->HasAura(SPELL_BESTED_GNOMEREGAN);
                        break;
                    case NPC_SILVERMOON_CHAMPION:
                        playerCooldown = challengee->HasAura(SPELL_BESTED_SILVERMOON);
                        break;
                    case NPC_THUNDER_BLUFF_CHAMPION:
                        playerCooldown = challengee->HasAura(SPELL_BESTED_THUNDER_BLUFF);
                        break;
                    case NPC_ORGRIMMAR_CHAMPION:
                        playerCooldown = challengee->HasAura(SPELL_BESTED_ORGRIMMAR);
                        break;
                    case NPC_SENJIN_CHAMPION:
                        playerCooldown = challengee->HasAura(SPELL_BESTED_SENJIN);
                        break;
                    case NPC_UNDERCITY_CHAMPION:
                        playerCooldown = challengee->HasAura(SPELL_BESTED_UNDERCITY);
                        break;
                    default:
                        playerCooldown = false;
                        break;
                }

                if (playerCooldown)
                    return;

                // remove gossip flag
                me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);

                // remove pose aura, otherwise no walking animation
                me->RemoveAura(SPELL_READYJOUST_POSE_EFFECT);

                // move towards arena center
                float angle = me->GetAngle(&arenaCenter);
                float x = me->GetPositionX() + 22.0f * cos(angle);
                float y = me->GetPositionY() + 22.0f * sin(angle);
                me->GetMotionMaster()->MovePoint(0, x, y, me->GetPositionZ());
            }

        }

        uint32 GetCustomType()
        {
            switch (me->GetEntry())
            {
                case NPC_EXODAR_CHAMPION:
                case NPC_DARNASSUS_CHAMPION:
                case NPC_STORMWIND_CHAMPION:
                case NPC_IRONFORGE_CHAMPION:
                case NPC_GNOMEREGAN_CHAMPION:
                case NPC_SILVERMOON_CHAMPION:
                case NPC_THUNDER_BLUFF_CHAMPION:
                case NPC_ORGRIMMAR_CHAMPION:
                case NPC_SENJIN_CHAMPION:
                case NPC_UNDERCITY_CHAMPION:
                    return TYPE_CHAMPION;
                case NPC_EXODAR_VALIANT:
                case NPC_DARNASSUS_VALIANT:
                case NPC_GNOMEREGAN_VALIANT:
                case NPC_IRONFORGE_VALIANT:
                case NPC_STORMWIND_VALIANT:
                    return TYPE_VALIANT_ALLIANCE;
                case NPC_SILVERMOON_VALIANT:
                case NPC_THUNDER_BLUFF_VALIANT:
                case NPC_UNDERCITY_VALIANT:
                case NPC_ORGRIMMAR_VALIANT:
                case NPC_SENJIN_VALIANT:
                    return TYPE_VALIANT_HORDE;
                default:
                    return TYPE_OTHER;
            }
        }

        void EnterCombat(Unit* /*who*/)
        {
            uint8 stackAmount;
            if (GetCustomType() == TYPE_CHAMPION)
                stackAmount = 3;
            else
                stackAmount = 2;

            for (uint8 i = 0; i < stackAmount; ++i)
                DoCast(me, SPELL_DEFEND, true);
        }

        void MovementInform(uint32 uiType, uint32 /*uiId*/)
        {
            if (uiType != POINT_MOTION_TYPE)
                return;

            // charge after moving away from the victim
            if (me->isInCombat() && bCharge)
            {
                me->GetMotionMaster()->Clear();
                // but only after rangecheck
                if (me->GetDistance(me->getVictim()) > 5.0f && me->GetDistance(me->getVictim()) <= 30.0f)
                    DoCastVictim(SPELL_CHARGE);
                me->GetMotionMaster()->MoveChase(me->getVictim());
                uiChargeTimer = GetCustomType() == TYPE_CHAMPION ? 6500 : 7500;
                bCharge = false;
            }
            else
            {
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
                me->setFaction(14);

                if (Player* player = ObjectAccessor::GetPlayer(*me, challengeeGUID))
                {
                    if (player->GetVehicle())
                        AttackStart(player->GetVehicle()->GetBase());
                    else
                        AttackStart(player);
                }
            }
        }

        void DamageTaken(Unit* who, uint32& damage)
        {
            if (damage >= me->GetHealth() && who->GetTypeId() == TYPEID_PLAYER && !bDefeated)
            {
                bDefeated = true;
                damage = 0;
                GrantCredit(who);
                me->setFaction(35);
                me->DespawnOrUnsummon(5000);
                me->SetHomePosition(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation());
                EnterEvadeMode();
            }
        }

        void GrantCredit(Unit* who)
        {
            Player* player;
            if (!(player = who->ToPlayer()))
                return;

            switch (GetCustomType())
            {
                case TYPE_CHAMPION:
                {
                    who->CastSpell(who, SPELL_MOUNTED_MELEE_VICTORY_C, true);
                    uint32 creditSpell;
                    switch (me->GetEntry())
                    {
                        case NPC_EXODAR_CHAMPION:
                            creditSpell = SPELL_BESTED_EXODAR;
                            break;
                        case NPC_DARNASSUS_CHAMPION:
                            creditSpell = SPELL_BESTED_DARNASSUS;
                            break;
                        case NPC_STORMWIND_CHAMPION:
                            creditSpell = SPELL_BESTED_STORMWIND;
                            break;
                        case NPC_IRONFORGE_CHAMPION:
                            creditSpell = SPELL_BESTED_IRONFORGE;
                            break;
                        case NPC_GNOMEREGAN_CHAMPION:
                            creditSpell = SPELL_BESTED_GNOMEREGAN;
                            break;
                        case NPC_SILVERMOON_CHAMPION:
                            creditSpell = SPELL_BESTED_SILVERMOON;
                            break;
                        case NPC_THUNDER_BLUFF_CHAMPION:
                            creditSpell = SPELL_BESTED_THUNDER_BLUFF;
                            break;
                        case NPC_ORGRIMMAR_CHAMPION:
                            creditSpell = SPELL_BESTED_ORGRIMMAR;
                            break;
                        case NPC_SENJIN_CHAMPION:
                            creditSpell = SPELL_BESTED_SENJIN;
                            break;
                        case NPC_UNDERCITY_CHAMPION:
                            creditSpell = SPELL_BESTED_UNDERCITY;
                            break;
                    }
                    who->CastSpell(who, creditSpell, false);
                    who->CastSpell(who, creditSpell, false); // second cast for criteria check...which is checked before aura is applied...HILARIOUS!
                    break;
                }
                case TYPE_VALIANT_ALLIANCE:
                case TYPE_VALIANT_HORDE:
                {
                    who->CastSpell(who, SPELL_MOUNTED_MELEE_VICTORY_V, true);
                    break;
                }
            }

        }

        void UpdateAI(const uint32 uiDiff)
        {
            if (!UpdateVictim())
                return;

            if (uiShieldTimer <= uiDiff)
            {
                me->CastSpell(me, SPELL_DEFEND);
                uiShieldTimer = GetCustomType() == TYPE_CHAMPION ? 3500 : 4500;
            } else uiShieldTimer -= uiDiff;

            if (uiChargeTimer <= uiDiff && !bCharge)
            {
                // move away for charge...
                float angle = me->GetAngle(me->getVictim());
                float x = me->GetPositionX() + 20.0f * cos(angle);
                float y = me->GetPositionY() + 20.0f * sin(angle);
                me->GetMotionMaster()->MovePoint(0, x, y, me->GetPositionZ());
                bCharge = true;
            } else uiChargeTimer -= uiDiff;

            if (uiShieldBreakerTimer <= uiDiff)
            {
                DoCastVictim(SPELL_SHIELD_BREAKER);
                uiShieldBreakerTimer = GetCustomType() == TYPE_CHAMPION ? 9000 : 10000;
            } else uiShieldBreakerTimer -= uiDiff;

            if (me->IsWithinMeleeRange(me->getVictim()))
            {
                if (uiThrustTimer <= uiDiff)
                {
                    DoCastVictim(SPELL_THRUST);
                    uiThrustTimer = GetCustomType() == TYPE_CHAMPION ? 1800 : 2000;
                }
                else uiThrustTimer -= uiDiff;
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_argent_faction_riderAI(creature);
    }
};

/*######
## npc_guardian_pavilion
######*/

enum eGuardianPavilion
{
    SPELL_TRESPASSER_H                            = 63987,
    AREA_SUNREAVER_PAVILION                       = 4676,

    AREA_SILVER_COVENANT_PAVILION                 = 4677,
    SPELL_TRESPASSER_A                            = 63986,
};

class npc_guardian_pavilion : public CreatureScript
{
public:
    npc_guardian_pavilion() : CreatureScript("npc_guardian_pavilion") { }

    struct npc_guardian_pavilionAI : public Scripted_NoMovementAI
    {
        npc_guardian_pavilionAI(Creature* creature) : Scripted_NoMovementAI(creature) {}

        void MoveInLineOfSight(Unit* who)
        {
            if (me->GetAreaId() != AREA_SUNREAVER_PAVILION && me->GetAreaId() != AREA_SILVER_COVENANT_PAVILION)
                return;

            if (!who || who->GetTypeId() != TYPEID_PLAYER || !me->IsHostileTo(who) || !me->isInBackInMap(who, 5.0f))
                return;

            if (who->HasAura(SPELL_TRESPASSER_H) || who->HasAura(SPELL_TRESPASSER_A))
                return;

            if (who->ToPlayer()->GetTeamId() == TEAM_ALLIANCE)
                who->CastSpell(who, SPELL_TRESPASSER_H, true);
            else
                who->CastSpell(who, SPELL_TRESPASSER_A, true);

        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_guardian_pavilionAI(creature);
    }
};

/*######
## npc_vereth_the_cunning
######*/

enum eVerethTheCunning
{
    NPC_GEIST_RETURN_BUNNY_KC   = 31049,
    NPC_LITHE_STALKER           = 30894,
    SPELL_SUBDUED_LITHE_STALKER = 58151,
};

class npc_vereth_the_cunning : public CreatureScript
{
public:
    npc_vereth_the_cunning() : CreatureScript("npc_vereth_the_cunning") { }

    struct npc_vereth_the_cunningAI : public ScriptedAI
    {
        npc_vereth_the_cunningAI(Creature* creature) : ScriptedAI(creature) {}

        void MoveInLineOfSight(Unit* who)
        {
            ScriptedAI::MoveInLineOfSight(who);

            if (who->GetEntry() == NPC_LITHE_STALKER && me->IsWithinDistInMap(who, 10.0f))
            {
                if (Unit* owner = who->GetCharmer())
                {
                    if (who->HasAura(SPELL_SUBDUED_LITHE_STALKER))
                        {
                            owner->ToPlayer()->KilledMonsterCredit(NPC_GEIST_RETURN_BUNNY_KC, 0);
                            who->ToCreature()->DisappearAndDie();

                    }
                }
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_vereth_the_cunningAI(creature);
    }
};

/*######
* npc_tournament_training_dummy
######*/
enum TournamentDummy
{
    NPC_CHARGE_TARGET         = 33272,
    NPC_MELEE_TARGET          = 33229,
    NPC_RANGED_TARGET         = 33243,

    SPELL_CHARGE_CREDIT       = 62658,
    SPELL_MELEE_CREDIT        = 62672,
    SPELL_RANGED_CREDIT       = 62673,

    SPELL_PLAYER_THRUST       = 62544,
    SPELL_PLAYER_BREAK_SHIELD = 62626,
    SPELL_PLAYER_CHARGE       = 62874,

    SPELL_RANGED_DEFEND       = 62719,
    SPELL_CHARGE_DEFEND       = 64100,
    SPELL_VULNERABLE          = 62665,

    SPELL_COUNTERATTACK       = 62709,

    EVENT_DUMMY_RECAST_DEFEND = 1,
    EVENT_DUMMY_RESET         = 2,
};

class npc_tournament_training_dummy : public CreatureScript
{
    public:
        npc_tournament_training_dummy(): CreatureScript("npc_tournament_training_dummy"){}

        struct npc_tournament_training_dummyAI : Scripted_NoMovementAI
        {
            npc_tournament_training_dummyAI(Creature* creature) : Scripted_NoMovementAI(creature) {}

            EventMap events;
            bool isVulnerable;

            void Reset()
            {
                me->SetControlled(true, UNIT_STATE_STUNNED);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                isVulnerable = false;

                // Cast Defend spells to max stack size
                switch (me->GetEntry())
                {
                    case NPC_CHARGE_TARGET:
                        DoCast(SPELL_CHARGE_DEFEND);
                        break;
                    case NPC_RANGED_TARGET:
                        me->CastCustomSpell(SPELL_RANGED_DEFEND, SPELLVALUE_AURA_STACK, 3, me);
                        break;
                }

                events.Reset();
                events.ScheduleEvent(EVENT_DUMMY_RECAST_DEFEND, 5000);
            }

            void EnterEvadeMode()
            {
                if (!_EnterEvadeMode())
                    return;

                Reset();
            }

            void DamageTaken(Unit* /*attacker*/, uint32& damage)
            {
                damage = 0;
                events.RescheduleEvent(EVENT_DUMMY_RESET, 10000);
            }

            void SpellHit(Unit* caster, SpellInfo const* spell)
            {
                switch (me->GetEntry())
                {
                    case NPC_CHARGE_TARGET:
                        if (spell->Id == SPELL_PLAYER_CHARGE)
                            if (isVulnerable)
                                DoCast(caster, SPELL_CHARGE_CREDIT, true);
                        break;
                    case NPC_MELEE_TARGET:
                        if (spell->Id == SPELL_PLAYER_THRUST)
                        {
                            DoCast(caster, SPELL_MELEE_CREDIT, true);

                            if (Unit* target = caster->GetVehicleBase())
                                DoCast(target, SPELL_COUNTERATTACK, true);
                        }
                        break;
                    case NPC_RANGED_TARGET:
                        if (spell->Id == SPELL_PLAYER_BREAK_SHIELD)
                            if (isVulnerable)
                                DoCast(caster, SPELL_RANGED_CREDIT, true);
                        break;
                }

                if (spell->Id == SPELL_PLAYER_BREAK_SHIELD)
                    if (!me->HasAura(SPELL_CHARGE_DEFEND) && !me->HasAura(SPELL_RANGED_DEFEND))
                        isVulnerable = true;
            }

            void UpdateAI(uint32 const diff)
            {
                events.Update(diff);

                switch (events.ExecuteEvent())
                {
                    case EVENT_DUMMY_RECAST_DEFEND:
                        switch (me->GetEntry())
                        {
                            case NPC_CHARGE_TARGET:
                            {
                                if (!me->HasAura(SPELL_CHARGE_DEFEND))
                                    DoCast(SPELL_CHARGE_DEFEND);
                                break;
                            }
                            case NPC_RANGED_TARGET:
                            {
                                Aura* defend = me->GetAura(SPELL_RANGED_DEFEND);
                                if (!defend || defend->GetStackAmount() < 3 || defend->GetDuration() <= 8000)
                                    DoCast(SPELL_RANGED_DEFEND);
                                break;
                            }
                        }
                        isVulnerable = false;
                        events.ScheduleEvent(EVENT_DUMMY_RECAST_DEFEND, 5000);
                        break;
                    case EVENT_DUMMY_RESET:
                        if (UpdateVictim())
                        {
                            EnterEvadeMode();
                            events.ScheduleEvent(EVENT_DUMMY_RESET, 10000);
                        }
                        break;
                }

                if (!UpdateVictim())
                    return;

                if (!me->HasUnitState(UNIT_STATE_STUNNED))
                    me->SetControlled(true, UNIT_STATE_STUNNED);
            }

            void MoveInLineOfSight(Unit* /*who*/){}
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_tournament_training_dummyAI(creature);
        }

};

void AddSC_icecrown()
{
    new npc_arete;
    new npc_argent_squire;
    new npc_argent_combatant;
    new npc_argent_faction_rider;
    new npc_guardian_pavilion;
    new npc_vereth_the_cunning;
    new npc_tournament_training_dummy;
}
