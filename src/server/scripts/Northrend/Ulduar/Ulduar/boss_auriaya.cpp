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

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "SpellScript.h"
#include "ulduar.h"

enum AuriayaSpells
{
    // Auriaya
    SPELL_TERRIFYING_SCREECH        = 64386,
    SPELL_SENTINEL_BLAST_10         = 64389,
    SPELL_SENTINEL_BLAST_25         = 64678,
    SPELL_SONIC_SCREECH_10          = 64422,
    SPELL_SONIC_SCREECH_25          = 64688,
    SPELL_SUMMON_SWARMING_GUARDIAN  = 64396,
    SPELL_ACTIVATE_DEFENDER         = 64449,
    SPELL_DEFENDER_TRIGGER          = 64448,
    SPELL_SUMMON_DEFENDER           = 64447,
    SPELL_BERSERK                   = 47008,

    // Feral Defender
    SPELL_FERAL_RUSH_10             = 64496,
    SPELL_FERAL_RUSH_25             = 64674,
    SPELL_FERAL_POUNCE_10           = 64478,
    SPELL_FERAL_POUNCE_25           = 64669,
    SPELL_SEEPING_FERAL_ESSENCE_10  = 64458,
    SPELL_SEEPING_FERAL_ESSENCE_25  = 64676,
    SPELL_SUMMON_ESSENCE            = 64457,
    SPELL_FERAL_ESSENCE             = 64455,

    // Sanctum Sentry
    SPELL_SAVAGE_POUNCE_10          = 64666,
    SPELL_SAVAGE_POUNCE_25          = 64374,
    SPELL_RIP_FLESH_10              = 64375,
    SPELL_RIP_FLESH_25              = 64667,
    SPELL_STRENGHT_OF_THE_PACK      = 64369, // Triggers 64381
};

#define SPELL_SENTINEL_BLAST RAID_MODE(SPELL_SENTINEL_BLAST_10, SPELL_SENTINEL_BLAST_25)
#define SPELL_SONIC_SCREECH RAID_MODE(SPELL_SONIC_SCREECH_10, SPELL_SONIC_SCREECH_25)

#define SPELL_FERAL_RUSH RAID_MODE(SPELL_FERAL_RUSH_10, SPELL_FERAL_RUSH_25)
#define SPELL_FERAL_POUNCE RAID_MODE(SPELL_FERAL_POUNCE_10, SPELL_FERAL_POUNCE_25)

#define SPELL_SEEPING_FERAL_ESSENCE RAID_MODE(SPELL_SEEPING_FERAL_ESSENCE_10, SPELL_SEEPING_FERAL_ESSENCE_25)

#define SPELL_SAVAGE_POUNCE RAID_MODE(SPELL_SAVAGE_POUNCE_10, SPELL_SAVAGE_POUNCE_25)
#define SPELL_RIP_FLESH RAID_MODE(SPELL_RIP_FLESH_10, SPELL_RIP_FLESH_25)

enum AuriayaNPCs
{
    NPC_SANCTUM_SENTRY                           = 34014,
    NPC_FERAL_DEFENDER                           = 34035,
    NPC_FERAL_DEFENDER_TRIGGER                   = 34096,
    NPC_SEEPING_TRIGGER                          = 34098,
};

enum AuriayaYells
{
    // Yells
    SAY_AGGRO                                    = -1603050,
    SAY_SLAY_1                                   = -1603051,
    SAY_SLAY_2                                   = -1603052,
    SAY_DEATH                                    = -1603053,
    SAY_BERSERK                                  = -1603054,

    // Emotes
    EMOTE_FEAR                                   = -1603055,
    EMOTE_DEFENDER                               = -1603056,
};

enum AuriayaActions
{
    ACTION_CRAZY_CAT_LADY                        = 0,
    ACTION_RESPAWN_DEFENDER
};

#define SENTRY_NUMBER                            RAID_MODE<uint8>(2, 4)
// #define DATA_NINE_LIVES                          30763077
// #define DATA_CRAZY_CAT_LADY                      30063007

enum Data
{
    DATA_NINE_LIVES,            // Achievements 3076/3077
    DATA_CRAZY_CAT_LADY,        // Achievements 3006/3007
    MODEL_INVISIBLE = 11686
};

class boss_auriaya : public CreatureScript
{
    enum AuriayaEvents
    {
        // Auriaya
        EVENT_SONIC_SCREECH          = 1,
        EVENT_SENTINEL_BLAST,         
        EVENT_TERRIFYING_SCREECH,     
        EVENT_SUMMON_SWARMING_GUARDIAN, 
        EVENT_ACTIVATE_DEFENDER, 
        EVENT_RESPAWN_DEFENDER,
        EVENT_BERSERK, 
    };

    public:
        boss_auriaya() : CreatureScript("boss_auriaya") {}

        struct boss_auriayaAI : public BossAI
        {
            boss_auriayaAI(Creature* creature) : BossAI(creature, BOSS_AURIAYA)
            {
                // TODO: don't interrupt by taking damage -> check if this information is outdated
                SpellInfo* spell = (SpellInfo*)sSpellMgr->GetSpellInfo(SPELL_SENTINEL_BLAST);
                if (spell)
                    spell->ChannelInterruptFlags &= ~AURA_INTERRUPT_FLAG_TAKE_DAMAGE;
            }

            void Reset()
            {
                _Reset();
                defenderLives = 9;
                crazyCatLady = true;
                nineLives = false;

                // Guardians are despawned by _Reset, but since they walk around with Auriaya, summon them again.
                for (uint8 i = 0; i < SENTRY_NUMBER; i++)
                    if (Creature* sentry = me->SummonCreature(NPC_SANCTUM_SENTRY, *me, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30000))
                    {
                        sentry->GetMotionMaster()->MoveFollow(me, (i < 2) ? 0.5f : 4.0f, M_PI - i - 1.5f);
                        summons.Summon(sentry);
                    }
            }

            void EnterCombat(Unit* /*who*/)
            {
                _EnterCombat();
                DoScriptText(SAY_AGGRO, me);
                summons.DoZoneInCombat();
                events.ScheduleEvent(EVENT_SONIC_SCREECH, urand(45000, 65000));
                events.ScheduleEvent(EVENT_SENTINEL_BLAST, urand(20000, 25000));
                events.ScheduleEvent(EVENT_TERRIFYING_SCREECH, urand(20000, 30000));
                events.ScheduleEvent(EVENT_ACTIVATE_DEFENDER, urand(40000, 55000));
                events.ScheduleEvent(EVENT_SUMMON_SWARMING_GUARDIAN, urand(45000, 55000));
                events.ScheduleEvent(EVENT_BERSERK, 10*MINUTE*IN_MILLISECONDS); 
            }

            void KilledUnit(Unit* /*who*/)
            {
                DoScriptText(RAND(SAY_SLAY_1, SAY_SLAY_2), me);
            }

            void JustSummoned(Creature* summoned)
            {
                summons.Summon(summoned);

                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true))
                {
                    summoned->AI()->AttackStart(target);
                    summoned->AddThreat(target, 250.0f);
                    DoZoneInCombat(summoned);
                }

                if (summoned->GetEntry() == NPC_FERAL_DEFENDER)
                {
                    if (!summoned->isInCombat() && me->getVictim())
                        summoned->AI()->AttackStart(me->getVictim());
                    summoned->SetAuraStack(SPELL_FERAL_ESSENCE, summoned, 9);
                    DoZoneInCombat(summoned);
                }
            }

            uint32 GetData(uint32 type)
            {
                switch (type)
                {
                    case DATA_NINE_LIVES:
                        return nineLives ? 1 : 0;
                    case DATA_CRAZY_CAT_LADY:
                        return crazyCatLady ? 1 : 0;
                }
                return 0;
            }

            void SetData(uint32 id, uint32 data)
            {
                switch (id)
                {
                    case DATA_NINE_LIVES:
                        nineLives = data ? true : false;
                        break;
                    case DATA_CRAZY_CAT_LADY:
                        crazyCatLady = data ? true : false;
                        break;
                }
            }

            void SummonedCreatureDies(Creature* summon, Unit* /*killer*/)
            {
                switch (summon->GetEntry())
                {
                    case NPC_FERAL_DEFENDER:
                        --defenderLives;
                        if (!defenderLives)
                        {
                            SetData(DATA_NINE_LIVES, 1);
                            break;
                        }
                        me->SummonCreature(NPC_SEEPING_TRIGGER, *summon);
                        if (defenderLives > 0)
                            events.ScheduleEvent(EVENT_RESPAWN_DEFENDER, 30000);
                        break;
                    case NPC_SANCTUM_SENTRY:
                        SetData(DATA_CRAZY_CAT_LADY, 0);
                        break;
                    default:
                        break;
                }
                summons.Despawn(summon);
            }

            void JustDied(Unit* /*who*/)
            {
                DoScriptText(SAY_DEATH, me);
                _JustDied();
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_SONIC_SCREECH:
                            DoCast(SPELL_SONIC_SCREECH);
                            events.ScheduleEvent(EVENT_SONIC_SCREECH, urand(40000, 60000));
                            break;
                        case EVENT_TERRIFYING_SCREECH:
                            DoScriptText(EMOTE_FEAR, me);
                            DoCast(SPELL_TERRIFYING_SCREECH);
                            events.ScheduleEvent(EVENT_TERRIFYING_SCREECH, urand(20000, 30000));
                            break;
                        case EVENT_SENTINEL_BLAST:
                            DoCastAOE(SPELL_SENTINEL_BLAST);
                            events.ScheduleEvent(EVENT_SENTINEL_BLAST, urand(25000, 35000));
                            break;
                        case EVENT_ACTIVATE_DEFENDER:
                            // TODO: Check if this works correctly. Otherwise, we will summon those directly.
                            DoScriptText(EMOTE_DEFENDER, me);
                            DoCast(SPELL_DEFENDER_TRIGGER);
                            if (Creature* trigger = me->FindNearestCreature(NPC_FERAL_DEFENDER_TRIGGER, 50.0f, true))
                                DoCast(trigger, SPELL_ACTIVATE_DEFENDER, true);
                            break;
                        case EVENT_RESPAWN_DEFENDER:
                            if (Creature* Defender = me->FindNearestCreature(NPC_FERAL_DEFENDER, 500.0f, false))
                            {
                                Defender->Respawn();
                                if (defenderLives)
                                    Defender->SetAuraStack(SPELL_FERAL_ESSENCE, Defender, defenderLives);
                                Defender->SetInCombatWithZone();
                                if (!Defender->isInCombat())
                                    Defender->AI()->AttackStart(me->getVictim());
                            }
                            break;
                        case EVENT_SUMMON_SWARMING_GUARDIAN:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true))
                                DoCast(target, SPELL_SUMMON_SWARMING_GUARDIAN);
                            events.ScheduleEvent(EVENT_SUMMON_SWARMING_GUARDIAN, urand(30000, 45000));
                            break;
                        case EVENT_BERSERK:
                            DoCast(me, SPELL_BERSERK, true);
                            DoScriptText(SAY_BERSERK, me);
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

        private:
            uint8 defenderLives;
            bool crazyCatLady;
            bool nineLives;
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return GetUlduarAI<boss_auriayaAI>(creature);
        }
};

class npc_auriaya_seeping_trigger : public CreatureScript
{
    public:
        npc_auriaya_seeping_trigger() : CreatureScript("npc_auriaya_seeping_trigger") {}

        struct npc_auriaya_seeping_triggerAI : public ScriptedAI
        {
            npc_auriaya_seeping_triggerAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = me->GetInstanceScript();
            }

        void Reset()
        {
            me->SetDisplayId(MODEL_INVISIBLE);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE | UNIT_FLAG_NON_ATTACKABLE);
            me->ForcedDespawn(600000);
            DoCast(me, SPELL_SEEPING_FERAL_ESSENCE);
        }

        void UpdateAI(uint32 const /*diff*/)
        {
            if (instance->GetBossState(BOSS_AURIAYA) != IN_PROGRESS)
                me->ForcedDespawn();
        }

        private:
            InstanceScript* instance;
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_auriaya_seeping_triggerAI(creature);
        }
};

class npc_sanctum_sentry : public CreatureScript
{
    private:
        enum { EVENT_RIP = 1, EVENT_POUNCE };
    public:
        npc_sanctum_sentry() : CreatureScript("npc_sanctum_sentry") {}

        struct npc_sanctum_sentryAI : public ScriptedAI
        {
            npc_sanctum_sentryAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = me->GetInstanceScript();
            }

            void Reset()
            {
                events.ScheduleEvent(EVENT_RIP, urand(4000, 8000));
                events.ScheduleEvent(EVENT_POUNCE, urand(12000, 15000));
            }

            void EnterCombat(Unit* /*who*/)
            {
                DoCast(me, SPELL_STRENGHT_OF_THE_PACK, true);
                if (me->ToTempSummon())
                {
                    Unit* auriaya = me->ToTempSummon()->GetSummoner();
                    if (auriaya && auriaya->ToCreature() && !auriaya->isInCombat())
                        auriaya->ToCreature()->SetInCombatWithZone();
                }
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_RIP:
                            DoCastVictim(SPELL_RIP_FLESH);
                            events.ScheduleEvent(EVENT_RIP, urand(12000, 15000));
                            break;
                        case EVENT_POUNCE:                            
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true))
                            {
                                DoResetThreat();
                                me->AddThreat(target, 100.0f);
                                me->AI()->AttackStart(target);
                                DoCast(target, SPELL_SAVAGE_POUNCE);
                            }
                            events.ScheduleEvent(EVENT_POUNCE, urand(12000, 17000));
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

            // Moved "JustDied" behavior to SummonedCreatureDies
        private:
            InstanceScript* instance;
            EventMap events;
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_sanctum_sentryAI(creature);
        }
};

class npc_feral_defender : public CreatureScript
{
    private:
        enum { EVENT_FERAL_POUNCE = 1, EVENT_RUSH };
    public:
        npc_feral_defender() : CreatureScript("npc_feral_defender") {}

        struct npc_feral_defenderAI : public ScriptedAI
        {
            npc_feral_defenderAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = me->GetInstanceScript();
            }

            void Reset()
            {
                events.ScheduleEvent(EVENT_FERAL_POUNCE, 5000);
                events.ScheduleEvent(EVENT_RUSH, 10000);
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_FERAL_POUNCE:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true))
                            {
                                DoResetThreat();
                                me->AddThreat(target, 100.0f);
                                me->AI()->AttackStart(target);
                                DoCast(target, SPELL_FERAL_POUNCE);
                            }
                            events.ScheduleEvent(EVENT_FERAL_POUNCE, urand(10000, 12000));
                            break;
                        case EVENT_RUSH:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true))
                            {
                                DoResetThreat();
                                me->AddThreat(target, 100.0f);
                                me->AI()->AttackStart(target);
                                DoCast(target, SPELL_FERAL_RUSH);
                            }
                            events.ScheduleEvent(EVENT_RUSH, urand(10000, 12000));
                            break;
                    default:
                        break;
                    }
                }

                DoMeleeAttackIfReady();
            }

            void JustDied(Unit* /*who*/)
            {
                DoCast(me, SPELL_SUMMON_ESSENCE);
                // Moved other behavior to SummonedCreatureDies
            }

        private:
            InstanceScript* instance;
            EventMap events;
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_feral_defenderAI(creature);
        }
};

class SanctumSentryCheck
{
    public:
        bool operator() (Unit* unit)
        {
            if (unit->GetEntry() == NPC_SANCTUM_SENTRY)
                return false;

            return true;
        }
};

class spell_auriaya_strenght_of_the_pack : public SpellScriptLoader
{
    public:
        spell_auriaya_strenght_of_the_pack() : SpellScriptLoader("spell_auriaya_strenght_of_the_pack") {}

        class spell_auriaya_strenght_of_the_pack_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_auriaya_strenght_of_the_pack_SpellScript);

            void FilterTargets(std::list<Unit*>& unitList)
            {
                unitList.remove_if (SanctumSentryCheck());
            }

            void Register()
            {
                OnUnitTargetSelect += SpellUnitTargetFn(spell_auriaya_strenght_of_the_pack_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ALLY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_auriaya_strenght_of_the_pack_SpellScript();
        }
};

class spell_auriaya_sentinel_blast : public SpellScriptLoader
{
    public:
        spell_auriaya_sentinel_blast() : SpellScriptLoader("spell_auriaya_sentinel_blast") {}

        class spell_auriaya_sentinel_blast_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_auriaya_sentinel_blast_SpellScript);

            void FilterTargets(std::list<Unit*>& unitList)
            {
                unitList.remove_if (PlayerOrPetCheck());
            }

            void Register()
            {
                OnUnitTargetSelect += SpellUnitTargetFn(spell_auriaya_sentinel_blast_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnUnitTargetSelect += SpellUnitTargetFn(spell_auriaya_sentinel_blast_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_auriaya_sentinel_blast_SpellScript();
        }
};


class achievement_nine_lives : public AchievementCriteriaScript
{
    public:
        achievement_nine_lives() : AchievementCriteriaScript("achievement_nine_lives") {}

        bool OnCheck(Player* /*player*/, Unit* target)
        {
            if (!target)
                return false;

            if (Creature* Auriaya = target->ToCreature())
                if (Auriaya->AI()->GetData(DATA_NINE_LIVES))
                    return true;

            return false;
        }
};

class achievement_crazy_cat_lady : public AchievementCriteriaScript
{
    public:
        achievement_crazy_cat_lady() : AchievementCriteriaScript("achievement_crazy_cat_lady") {}

        bool OnCheck(Player* /*player*/, Unit* target)
        {
            if (!target)
                return false;

            if (Creature* Auriaya = target->ToCreature())
                if (Auriaya->AI()->GetData(DATA_CRAZY_CAT_LADY))
                    return true;

            return false;
        }
};

void AddSC_boss_auriaya()
{
    new boss_auriaya();
    new npc_auriaya_seeping_trigger();
    new npc_feral_defender();
    new npc_sanctum_sentry();
    new spell_auriaya_strenght_of_the_pack();
    new spell_auriaya_sentinel_blast();
    new achievement_nine_lives();
    new achievement_crazy_cat_lady();
}

#undef SPELL_SENTINEL_BLAST 
#undef SPELL_SONIC_SCREECH 

#undef SPELL_FERAL_RUSH 
#undef SPELL_FERAL_POUNCE 

#undef SPELL_SEEPING_FERAL_ESSENCE 

#undef SPELL_SAVAGE_POUNCE 
#undef SPELL_RIP_FLESH
