/*
 * Copyright (C) 2010-2012 Project SkyFire <http://www.projectskyfire.org/>
 * Copyright (C) 2010-2012 Oregon <http://www.oregoncore.com/>
 * Copyright (C) 2006-2008 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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

/* ScriptData
SDName: Ghostlands
SD%Complete: 100
SDComment: Quest support: 9692, 9212. Obtain Budd's Guise of Zul'aman. Vendor Rathis Tomber
SDCategory: Ghostlands
EndScriptData */

/* ContentData
npc_blood_knight_dawnstar
npc_budd_nedreck
npc_rathis_tomber
npc_ranger_lilatha
EndContentData */

#include "ScriptPCH.h"
#include "ScriptedEscortAI.h"

/*######
## npc_blood_knight_dawnstar
######*/

#define GOSSIP_H_BKD "Take Blood Knight Insignia"

bool GossipHello_npc_blood_knight_dawnstar(Player* player, Creature* creature)
{
    if (player->GetQuestStatus(9692) == QUEST_STATUS_INCOMPLETE && !player->HasItemCount(24226, 1, true))
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_H_BKD, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

    player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());

    return true;
}

bool GossipSelect_npc_blood_knight_dawnstar(Player* player, Creature* /*creature*/, uint32 /*uiSender*/, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF+1)
    {
        ItemPosCountVec dest;
        uint8 msg = player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 24226, 1, false);
        if (msg == EQUIP_ERR_OK)
        {
            player->StoreNewItem(dest, 24226, 1, true);
            player->PlayerTalkClass->ClearMenus();
        }
    }
    return true;
}

/*######
## npc_budd_nedreck
######*/

#define GOSSIP_HBN "You gave the crew disguises?"

bool GossipHello_npc_budd_nedreck(Player* player, Creature* creature)
{
    if (creature->isQuestGiver())
        player->PrepareQuestMenu(creature->GetGUID());

    if (player->GetQuestStatus(11166) == QUEST_STATUS_INCOMPLETE)
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_HBN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);

    player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
    return true;
}

bool GossipSelect_npc_budd_nedreck(Player* player, Creature* creature, uint32 /*uiSender*/, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF)
    {
        player->CLOSE_GOSSIP_MENU();
        creature->CastSpell(player, 42540, false);
    }
    return true;
}

/*######
## npc_rathis_tomber
######*/

bool GossipHello_npc_rathis_tomber(Player* player, Creature* creature)
{
    if (creature->isQuestGiver())
        player->PrepareQuestMenu(creature->GetGUID());

    if (creature->isVendor() && player->GetQuestRewardStatus(9152))
    {
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, GOSSIP_TEXT_BROWSE_GOODS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE);
        player->SEND_GOSSIP_MENU(8432, creature->GetGUID());
    }else
    player->SEND_GOSSIP_MENU(8431, creature->GetGUID());

    return true;
}

bool GossipSelect_npc_rathis_tomber(Player* player, Creature* creature, uint32 /*uiSender*/, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_TRADE)
        player->SEND_VENDORLIST(creature->GetGUID());
    return true;
}

/*#####
## go_gilded_brazier (Paladin First Trail quest (9678))
#####*/

bool GOHello_gilded_brazier(Player* player, GameObject* _GO)
{
    if (player->GetQuestStatus(9678) == QUEST_STATUS_INCOMPLETE)
    {
        Creature *Stillblade = player->SummonCreature(17716, 8106.11f, -7542.06f, 151.775f, 3.02598f, TEMPSUMMON_DEAD_DESPAWN, 60000);
        if (Stillblade)
            ((CreatureAI*)Stillblade->AI())->AttackStart(player);
    }
    return true;
};

/*######
## npc_ranger_lilatha
######*/

enum eEnums
{
    SAY_START           = -1000140,
    SAY_PROGRESS1       = -1000141,
    SAY_PROGRESS2       = -1000142,
    SAY_PROGRESS3       = -1000143,
    SAY_END1            = -1000144,
    SAY_END2            = -1000145,
    SAY_CAPTAIN_ANSWER  = -1000146,

    QUEST_ESCAPE_FROM_THE_CATACOMBS     = 9212,
    GO_CAGE             = 181152,
    NPC_CAPTAIN_HELIOS  = 16220,
    FACTION_SMOON_E     = 1603,
};

struct npc_ranger_lilathaAI : public npc_escortAI
{
    npc_ranger_lilathaAI(Creature *c) : npc_escortAI(c) {}

    void WaypointReached(uint32 i)
    {
        Player* player = GetPlayerForEscort();

        if (!player)
            return;

        switch (i)
        {
        case 0:
            {
            me->SetUInt32Value(UNIT_FIELD_BYTES_1, 0);
            if (GameObject* Cage = me->FindNearestGameObject(GO_CAGE, 20))
                Cage->SetGoState(GO_STATE_ACTIVE);
            DoScriptText(SAY_START, me, player);
            break;
            }
        case 5:
            DoScriptText(SAY_PROGRESS1, me, player);
        case 11:
            DoScriptText(SAY_PROGRESS2, me, player);
            me->SetOrientation(4.762841f);
            break;
        case 18:
            {
            DoScriptText(SAY_PROGRESS3, me, player);
            Creature* Summ1 = me->SummonCreature(16342, 7627.083984f, -7532.538086f, 152.128616f, 1.082733f, TEMPSUMMON_DEAD_DESPAWN, 0);
            Creature* Summ2 = me->SummonCreature(16343, 7620.432129f, -7532.550293f, 152.454865f, 0.827478f, TEMPSUMMON_DEAD_DESPAWN, 0);
            if (Summ1 && Summ2)
            {
                Summ1->Attack(me, true);
                Summ2->Attack(player, true);
            }
            me->AI()->AttackStart(Summ1);
            break;
            }
        case 19: me->RemoveUnitMovementFlag(MOVEFLAG_WALK_MODE); break;
        case 25: me->AddUnitMovementFlag(MOVEFLAG_WALK_MODE); break;
        case 30:
            if (player && player->GetTypeId() == TYPEID_PLAYER)
                CAST_PLR(player)->GroupEventHappens(QUEST_ESCAPE_FROM_THE_CATACOMBS, me);
            break;
        case 32:
            me->SetOrientation(2.978281f);
            DoScriptText(SAY_END1, me, player);
            break;
        case 33:
            me->SetOrientation(5.858011f);
            DoScriptText(SAY_END2, me, player);
            Unit* CaptainHelios = me->FindNearestCreature(NPC_CAPTAIN_HELIOS, 50);
            if (CaptainHelios)
            DoScriptText(SAY_CAPTAIN_ANSWER, CaptainHelios, player);
            break;
        }
    }

    void Reset()
    {
        if (GameObject* Cage = me->FindNearestGameObject(GO_CAGE, 20))
            Cage->SetGoState(GO_STATE_READY);
    }
};

bool QuestAccept_npc_ranger_lilatha(Player* player, Creature* creature, Quest const* quest)
{
    if (quest->GetQuestId() == QUEST_ESCAPE_FROM_THE_CATACOMBS)
    {
        creature->setFaction(113);

        if (npc_escortAI* pEscortAI = CAST_AI(npc_ranger_lilathaAI, creature->AI()))
            pEscortAI->Start(true, false, player->GetGUID());
    }
    return true;
}

CreatureAI* GetAI_npc_ranger_lilathaAI(Creature* creature)
{
    return  new npc_ranger_lilathaAI(creature);
}

void AddSC_ghostlands()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "npc_blood_knight_dawnstar";
    newscript->pGossipHello = &GossipHello_npc_blood_knight_dawnstar;
    newscript->pGossipSelect = &GossipSelect_npc_blood_knight_dawnstar;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_budd_nedreck";
    newscript->pGossipHello = &GossipHello_npc_budd_nedreck;
    newscript->pGossipSelect = &GossipSelect_npc_budd_nedreck;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_rathis_tomber";
    newscript->pGossipHello = &GossipHello_npc_rathis_tomber;
    newscript->pGossipSelect = &GossipSelect_npc_rathis_tomber;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_gilded_brazier";
    newscript->pGOHello = &GOHello_gilded_brazier;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_ranger_lilatha";
    newscript->GetAI = &GetAI_npc_ranger_lilathaAI;
    newscript->pQuestAccept = &QuestAccept_npc_ranger_lilatha;
    newscript->RegisterSelf();
}
