/* Copyright (C) 2008 - 2009 Trinity <http://www.trinitycore.org/>
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include "ScriptPCH.h"
#include "OutdoorPvPWG.h"

#define GOSSIP_HELLO_DEMO1  "Build catapult."
#define GOSSIP_HELLO_DEMO2  "Build demolisher."
#define GOSSIP_HELLO_DEMO3  "Build siege engine."
#define GOSSIP_HELLO_DEMO4  "I cannot build more!"

class npc_demolisher_engineerer : public CreatureScript
{
public:
    npc_demolisher_engineerer() : CreatureScript("npc_demolisher_engineerer") { }


bool OnGossipHello(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetGUID());

    if(pPlayer->isGameMaster() || pCreature->GetZoneScript() && pCreature->GetZoneScript()->GetData(pCreature->GetDBTableGUIDLow()))
    {
        if (pPlayer->HasAura(SPELL_CORPORAL))
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_HELLO_DEMO1, GOSSIP_SENDER_MAIN,   GOSSIP_ACTION_INFO_DEF);
        else if (pPlayer->HasAura(SPELL_LIEUTENANT))
        {
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_HELLO_DEMO1, GOSSIP_SENDER_MAIN,   GOSSIP_ACTION_INFO_DEF);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_HELLO_DEMO2, GOSSIP_SENDER_MAIN,   GOSSIP_ACTION_INFO_DEF+1);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_HELLO_DEMO3, GOSSIP_SENDER_MAIN,   GOSSIP_ACTION_INFO_DEF+2);
        }
    }
    else
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_HELLO_DEMO4, GOSSIP_SENDER_MAIN,   GOSSIP_ACTION_INFO_DEF+9);

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());
    return true;
}

bool OnGossipSelect(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    pPlayer->CLOSE_GOSSIP_MENU();
    if(pPlayer->isGameMaster() || pCreature->GetZoneScript() && pCreature->GetZoneScript()->GetData(pCreature->GetDBTableGUIDLow()))
    {
        switch(uiAction - GOSSIP_ACTION_INFO_DEF)
        {
            case 0: pPlayer->CastSpell(pPlayer, 56663, false, NULL, NULL, pCreature->GetGUID()); break;
            case 1: pPlayer->CastSpell(pPlayer, 56575, false, NULL, NULL, pCreature->GetGUID()); break;
            case 2: pPlayer->CastSpell(pPlayer, pPlayer->GetTeamId() ? 61408 : 56661, false, NULL, NULL, pCreature->GetGUID()); break;
        }
    }

    return true;
}

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_demolisher_engineererAI(pCreature);
    }

    struct npc_demolisher_engineererAI : public ScriptedAI
    {
        npc_demolisher_engineererAI(Creature* pCreature) : ScriptedAI(pCreature)
        {
            me->SetReactState(REACT_PASSIVE);
        }
    };
};

/*#########
# npc_wg_guard
##########*/

enum wg_guard_spells
{
    SPELL_STRIKE = 52532, // custom strike spell
};

class npc_wg_guard : public CreatureScript
{
public:
    npc_wg_guard() : CreatureScript("npc_wg_guard") { }

    struct npc_wg_guardAI : public CreatureAI
    {
        npc_wg_guardAI(Creature* creature) : CreatureAI(creature) {}

        uint32 combatStuckTimer;
        uint32 strikeTimer;

        void Reset()
        {
            combatStuckTimer = 5000;
            strikeTimer = 6000;
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (strikeTimer <= diff)
            {
                DoCastVictim(SPELL_STRIKE);
                strikeTimer = urand(4000, 6000);
            } else
                strikeTimer -= diff;

            if (combatStuckTimer <= diff)
            {
                bool playerInCombatRange = false;
                bool playerInThreatList = false;
                for (std::list<HostileReference*>::const_iterator itr = me->getThreatManager().getThreatList().begin(); itr != me->getThreatManager().getThreatList().end(); ++itr)
                    if ((*itr)->getTarget() && (*itr)->getTarget()->GetTypeId() == TYPEID_PLAYER)
                    {
                        playerInThreatList = true;
                        if (me->GetDistance((*itr)->getTarget()) < 60.0f)
                            playerInCombatRange = true;
                    }
                if (!playerInCombatRange && playerInThreatList)
                    EnterEvadeMode();
                combatStuckTimer = 5000;
            } else
                combatStuckTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_wg_guardAI(creature);
    }
};

void AddSC_wintergrasp()
{
    new npc_demolisher_engineerer();
    new npc_wg_guard();
}
