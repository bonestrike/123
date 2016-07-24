#include "ScriptPCH.h"
#include "Language.h"

class Professions_NPC : public CreatureScript
{
        public:
                Professions_NPC () : CreatureScript("Professions_NPC") {}

                void CreatureWhisperBasedOnBool(const char *text, Creature *_creature, Player *pPlayer, bool value)
                {
                        if (value)
                                _creature->MonsterWhisper(text, pPlayer->ToPlayer());
                }

                uint32 PlayerMaxLevel() const
                {
                        return sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL);
                }

                bool PlayerHasItemOrSpell(const Player *plr, uint32 itemId, uint32 spellId) const
                {
                        return plr->HasItemCount(itemId, 1, true) || plr->HasSpell(spellId);
                }

                bool OnGossipHello(Player *pPlayer, Creature* _creature)
                {
                        pPlayer->ADD_GOSSIP_ITEM(9, "[Professions] ->", GOSSIP_SENDER_MAIN, 196);
                        pPlayer->PlayerTalkClass->SendGossipMenu(907, _creature->GetGUID());
                        return true;
                }

                bool PlayerAlreadyHasTwoProfessions(const Player *pPlayer) const
                {
                        uint32 skillCount = 0;

                        if (pPlayer->HasSkill(SKILL_MINING))
                                skillCount++;
                        if (pPlayer->HasSkill(SKILL_SKINNING))
                                skillCount++;
                        if (pPlayer->HasSkill(SKILL_HERBALISM))
                                skillCount++;

                        if (skillCount >= 2)
                                return true;

                        for (uint32 i = 1; i < sSkillLineStore.GetNumRows(); ++i)
                        {
                                SkillLineEntry const *SkillInfo = sSkillLineStore.LookupEntry(i);
                                if (!SkillInfo)
                                        continue;

                                if (SkillInfo->categoryId == SKILL_CATEGORY_SECONDARY)
                                        continue;

                                if ((SkillInfo->categoryId != SKILL_CATEGORY_PROFESSION) || !SkillInfo->canLink)
                                        continue;

                                const uint32 skillID = SkillInfo->id;
                                if (pPlayer->HasSkill(skillID))
                                        skillCount++;

                                if (skillCount >= 2)
                                        return true;
                        }
                        return false;
                }

                bool LearnAllRecipesInProfession(Player *pPlayer, SkillType skill)
                {
                        ChatHandler handler(pPlayer->GetSession());
                        char* skill_name;

                        SkillLineEntry const *SkillInfo = sSkillLineStore.LookupEntry(skill);
                        skill_name = SkillInfo->name[handler.GetSessionDbcLocale()];

                        if (!SkillInfo)
                        {
                                // sLog->outError(LOG_FILTER_PLAYER_SKILLS, "Profession NPC: received non-valid skill ID (LearnAllRecipesInProfession)");
								return false;
                        }

                        LearnSkillRecipesHelper(pPlayer, SkillInfo->id);

                        pPlayer->SetSkill(SkillInfo->id, pPlayer->GetSkillStep(SkillInfo->id), 450, 450);
                        handler.PSendSysMessage(LANG_COMMAND_LEARN_ALL_RECIPES, skill_name);

                        return true;
                }

                void LearnSkillRecipesHelper(Player *player, uint32 skill_id)
                {
                        uint32 classmask = player->getClassMask();

                        for (uint32 j = 0; j < sSkillLineAbilityStore.GetNumRows(); ++j)
                        {
                                SkillLineAbilityEntry const *skillLine = sSkillLineAbilityStore.LookupEntry(j);
                                if (!skillLine)
                                        continue;

                                // wrong skill
                                if (skillLine->skillId != skill_id)
                                        continue;

                                // not high rank
                                if (skillLine->forward_spellid)
                                        continue;

                                // skip racial skills
                                if (skillLine->racemask != 0)
                                        continue;

                                // skip wrong class skills
                                if (skillLine->classmask && (skillLine->classmask & classmask) == 0)
                                        continue;

                                SpellInfo const * spellInfo = sSpellMgr->GetSpellInfo(skillLine->spellId);
                                if (!spellInfo || !SpellMgr::IsSpellValid(spellInfo, player, false))
                                        continue;

                                player->learnSpell(skillLine->spellId, false);
                        }
                }

                bool IsSecondarySkill(SkillType skill) const
                {
                        return skill == SKILL_COOKING || skill == SKILL_FIRST_AID;
                }

                void CompleteLearnProfession(Player *pPlayer, Creature *pCreature, SkillType skill)
                {
                        if (PlayerAlreadyHasTwoProfessions(pPlayer) && !IsSecondarySkill(skill))
                                pCreature->MonsterWhisper("You already know two professions!", pPlayer->ToPlayer());
                        else
                        {
                                if (!LearnAllRecipesInProfession(pPlayer, skill))
                                        pCreature->MonsterWhisper("Internal error occured!", pPlayer->ToPlayer());
                        }
                }

                bool OnGossipSelect(Player* pPlayer, Creature* _creature, uint32 uiSender, uint32 uiAction)
                { 
                        pPlayer->PlayerTalkClass->ClearMenus();

                        if (uiSender == GOSSIP_SENDER_MAIN)
                        {

                                switch (uiAction)
                                {
                                        case 196:
                                                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|cffff0000 ALCHEMY", GOSSIP_SENDER_MAIN, 1);
												pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|cffffffff ---------------", GOSSIP_SENDER_MAIN, 20);
                                                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|cffff0000 BLACKSMITHING", GOSSIP_SENDER_MAIN, 2);
												pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|cffffffff ---------------", GOSSIP_SENDER_MAIN, 21);
                                                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|cffff0000 LEATHERWORKING", GOSSIP_SENDER_MAIN, 3);
												pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|cffffffff ---------------", GOSSIP_SENDER_MAIN, 22);
                                                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|cffff0000 TAILORING", GOSSIP_SENDER_MAIN, 4);
												pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|cffffffff ---------------", GOSSIP_SENDER_MAIN, 23);
                                                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|cffff0000 ENGINEERING", GOSSIP_SENDER_MAIN, 5);
												pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|cffffffff ---------------", GOSSIP_SENDER_MAIN, 24);
                                                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|cffff0000 ENCHANTING", GOSSIP_SENDER_MAIN, 6);
												pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|cffffffff ---------------", GOSSIP_SENDER_MAIN, 25);
                                                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|cffff0000 JEWELCRAFTING", GOSSIP_SENDER_MAIN, 7);
												pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|cffffffff ---------------", GOSSIP_SENDER_MAIN, 26);
                                                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|cffff0000 INSCRIPTION", GOSSIP_SENDER_MAIN, 8);
												pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|cffffffff ---------------", GOSSIP_SENDER_MAIN, 27);
                                                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|cffff0000 COOKING", GOSSIP_SENDER_MAIN, 9);
												pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|cffffffff ---------------", GOSSIP_SENDER_MAIN, 28);
                                                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|cffff0000 FIRST AID", GOSSIP_SENDER_MAIN, 10);
												pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|cffffffff ---------------", GOSSIP_SENDER_MAIN, 29);
												pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|cffff0000 HERBALISM", GOSSIP_SENDER_MAIN, 11);
												pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|cffffffff ---------------", GOSSIP_SENDER_MAIN, 30);
												pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|cffff0000 SKINNING", GOSSIP_SENDER_MAIN, 12);
												pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|cffffffff ---------------", GOSSIP_SENDER_MAIN, 31);
												pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|cffff0000 MINING", GOSSIP_SENDER_MAIN, 13);

                                                pPlayer->PlayerTalkClass->SendGossipMenu(907, _creature->GetGUID());
                                                break;
                                        case 1:
                                                if(pPlayer->HasSkill(SKILL_ALCHEMY))
                                                {
                                                        pPlayer->PlayerTalkClass->SendCloseGossip();
                                                        break;
                                                }

                                                CompleteLearnProfession(pPlayer, _creature, SKILL_ALCHEMY);

                                                pPlayer->PlayerTalkClass->SendCloseGossip();
                                                break;
                                        case 2:
                                                if(pPlayer->HasSkill(SKILL_BLACKSMITHING))
                                                {
                                                        pPlayer->PlayerTalkClass->SendCloseGossip();
                                                        break;
                                                }
                                                CompleteLearnProfession(pPlayer, _creature, SKILL_BLACKSMITHING);

                                                pPlayer->PlayerTalkClass->SendCloseGossip();
                                                break;
                                        case 3:
                                                if(pPlayer->HasSkill(SKILL_LEATHERWORKING))
                                                {
                                                        pPlayer->PlayerTalkClass->SendCloseGossip();
                                                        break;
                                                }
                                                CompleteLearnProfession(pPlayer, _creature, SKILL_LEATHERWORKING);

                                                pPlayer->PlayerTalkClass->SendCloseGossip();
                                                break;
                                        case 4:
                                                if(pPlayer->HasSkill(SKILL_TAILORING))
                                                {
                                                        pPlayer->PlayerTalkClass->SendCloseGossip();
                                                        break;
                                                }
                                                CompleteLearnProfession(pPlayer, _creature, SKILL_TAILORING);

                                                pPlayer->PlayerTalkClass->SendCloseGossip();
                                                break;
                                        case 5:
                                                if(pPlayer->HasSkill(SKILL_ENGINEERING))
                                                {
                                                        pPlayer->PlayerTalkClass->SendCloseGossip();
                                                        break;
                                                }
                                                CompleteLearnProfession(pPlayer, _creature, SKILL_ENGINEERING);

                                                pPlayer->PlayerTalkClass->SendCloseGossip();
                                                break;
                                        case 6:
                                                if(pPlayer->HasSkill(SKILL_ENCHANTING))
                                                {
                                                        pPlayer->PlayerTalkClass->SendCloseGossip();
                                                        break;
                                                }
                                                CompleteLearnProfession(pPlayer, _creature, SKILL_ENCHANTING);

                                                pPlayer->PlayerTalkClass->SendCloseGossip();
                                                break;
                                        case 7:
                                                if(pPlayer->HasSkill(SKILL_JEWELCRAFTING))
                                                {
                                                        pPlayer->PlayerTalkClass->SendCloseGossip();
                                                        break;
                                                }
                                                CompleteLearnProfession(pPlayer, _creature, SKILL_JEWELCRAFTING);

                                                pPlayer->PlayerTalkClass->SendCloseGossip();
                                                break;
                                        case 8:
                                                if(pPlayer->HasSkill(SKILL_INSCRIPTION))
                                                {
                                                        pPlayer->PlayerTalkClass->SendCloseGossip();
                                                        break;
                                                }
                                                CompleteLearnProfession(pPlayer, _creature, SKILL_INSCRIPTION);

                                                pPlayer->PlayerTalkClass->SendCloseGossip();
                                                break;
                                        case 9:
                                                if(pPlayer->HasSkill(SKILL_COOKING))
                                                {
                                                        pPlayer->PlayerTalkClass->SendCloseGossip();
                                                        break;
                                                }
                                                CompleteLearnProfession(pPlayer, _creature, SKILL_COOKING);

                                                pPlayer->PlayerTalkClass->SendCloseGossip();
                                                break;
                                        case 10:
                                                if(pPlayer->HasSkill(SKILL_FIRST_AID))
                                                {
                                                        pPlayer->PlayerTalkClass->SendCloseGossip();
                                                        break;
                                                }
                                                CompleteLearnProfession(pPlayer, _creature, SKILL_FIRST_AID);

                                                pPlayer->PlayerTalkClass->SendCloseGossip();
                                                break;
										case 11:
											if(pPlayer->HasSkill(SKILL_HERBALISM))
											{
												pPlayer->PlayerTalkClass->SendCloseGossip();
												break;
											}

											CompleteLearnProfession(pPlayer, _creature, SKILL_HERBALISM);
											pPlayer->PlayerTalkClass->SendCloseGossip();
											break;
										case 12:
											if(pPlayer->HasSkill(SKILL_SKINNING))
											{
												pPlayer->PlayerTalkClass->SendCloseGossip();
												break;
											}

											CompleteLearnProfession(pPlayer, _creature, SKILL_SKINNING);
											pPlayer->PlayerTalkClass->SendCloseGossip();
											break;
										case 13:
											if(pPlayer->HasSkill(SKILL_MINING))
											{
												pPlayer->PlayerTalkClass->SendCloseGossip();
												break;
											}

											CompleteLearnProfession(pPlayer, _creature, SKILL_MINING);
											pPlayer->PlayerTalkClass->SendCloseGossip();
											break;
                                }


                        }
                        return true;
                }
};

void AddSC_Professions_NPC()
{
    new Professions_NPC();
}