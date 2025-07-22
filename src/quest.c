/****************************************************************************
*  Automated Quest code written by Vassago of MOONGATE, moongate.ams.com    *
*  4000. Copyright (c) 1996 Ryan Addams, All Rights Reserved. Use of this   *
*  code is allowed provided you add a credit line to the effect of:         *
*  "Quest Code (c) 1996 Ryan Addams" to your logon screen with the rest     *
*  of the standard diku/rom credits. If you use this or a modified version  *
*  of this code, let me know via email: moongate@moongate.ams.com. Further  *
*  updates will be posted to the rom mailing list. If you'd like to get     *
*  the latest version of quest.c, please send a request to the above add-   *
*  ress. Quest Code v2.03. Please do not remove this notice from this file. *
****************************************************************************/
/***************************************************************************
 *     ANATOLIA 2.1 is copyright 1996-1997 Serdar BULUT, Ibrahim CANPUNAR  *	
 *     ANATOLIA has been brought to you by ANATOLIA consortium		   *
 *	 Serdar BULUT {Chronos}		bulut@rorqual.cc.metu.edu.tr       *	
 *	 Ibrahim Canpunar  {Asena}	canpunar@rorqual.cc.metu.edu.tr    *	
 *	 Murat BICER  {KIO}		mbicer@rorqual.cc.metu.edu.tr	   *
 *	 D.Baris ACAR {Powerman}	dbacar@rorqual.cc.metu.edu.tr	   *	
 *     By using this code, you have agreed to follow the terms of the      *
 *     ANATOLIA license, in the file Anatolia/anatolia.licence             *	
 ***************************************************************************/

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "recycle.h"

void do_tell_quest( CHAR_DATA *ch, CHAR_DATA *victim, char *argument);
CHAR_DATA *get_quest_world( CHAR_DATA *ch, MOB_INDEX_DATA *victim );

/* Object vnums for object quest 'tokens'. In Moongate, the tokens are
   things like 'the Shield of Moongate', 'the Sceptre of Moongate'. These
   items are worthless and have the rot-death flag, as they are placed
   into the world when a player receives an object quest. */

#define QUEST_OBJQUEST1 27
#define QUEST_OBJQUEST2 28
#define QUEST_OBJQUEST3 29
#define QUEST_OBJQUEST4 30

/* Local functions */

void generate_quest     args(( CHAR_DATA *ch, CHAR_DATA *questman ));
void quest_update       args(( void ));
bool chance             args(( int num ));
ROOM_INDEX_DATA         *find_location( CHAR_DATA *ch, char *arg );

/* CHANCE function. I use this everywhere in my code, very handy :> */

bool chance(int num)
{
    if (number_range(1,100) <= num) return TRUE;
    else return FALSE;
}

/* The main quest function */

void do_quest(CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *questman;
    OBJ_DATA *obj=NULL, *obj_next;
    OBJ_INDEX_DATA *questinfoobj;
    MOB_INDEX_DATA *questinfo;
    char buf [MAX_STRING_LENGTH];
    char arg1 [MAX_INPUT_LENGTH];
    char arg2 [MAX_INPUT_LENGTH];

    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);

    if (arg1[0] == '\0')
    {
        printf_to_char(ch,"QUEST commands: POINTS INFO TIME REQUEST COMPLETE LIST BUY.\n\r");
        printf_to_char(ch,"For more information, type 'HELP QUEST'.\n\r");
        return;
    }
    if (!strcmp(arg1, "info"))
    {
        if (IS_SET(ch->act, PLR_QUESTOR))
        {
            if (ch->pcdata->questmob == -1 && ch->pcdata->questgiver->short_descr != NULL)
            {
                sprintf(buf, "Your quest is ALMOST complete!\n\rGet back to %s before your time runs out!\n\r", ch->pcdata->questgiver->short_descr);
                printf_to_char(ch,buf);
            }
            else if (ch->pcdata->questobj > 0)
            {
                questinfoobj = get_obj_index (ch->pcdata->questobj);
                if (questinfoobj != NULL)
                {
                    sprintf(buf, "You are on a quest to recover the fabled %s!\n\r", questinfoobj->name);
                    printf_to_char(ch,buf);
                }
                else printf_to_char(ch,"You aren't currently on a quest.\n\r");
                return;
            }
            else if (ch->pcdata->questmob > 0)
            {
                questinfo = get_mob_index(ch->pcdata->questmob);
                if (questinfo != NULL)
                {
                    sprintf(buf, "You are on a quest to slay the dreaded %s!\n\r", questinfo->short_descr);
                    printf_to_char(ch,buf);
                }
                else printf_to_char(ch,"You aren't currently on a quest.\n\r");
                return;
            }
        }
        else
            printf_to_char(ch,"You aren't currently on a quest.\n\r");
        return;
    }
    if (!strcmp(arg1, "points"))
    {
        sprintf(buf, "You have %d quest points.\n\r",ch->pcdata->questpoints);
        printf_to_char(ch,buf);
        return;
    }
    else if (!strcmp(arg1, "time"))
    {
        if (!IS_SET(ch->act, PLR_QUESTOR))
        {
            printf_to_char(ch,"You aren't currently on a quest.\n\r");
            if (ch->pcdata->nextquest > 1)
            {
                sprintf (buf, "There are %d minutes remaining until you can go on another quest.\n\r", ch->pcdata->nextquest);
                printf_to_char(ch,buf);
            }
            else if (ch->pcdata->nextquest == 1)
            {
                sprintf (buf, "There is less than a minute remaining until you can go on another quest.\n\r");
                printf_to_char(ch,buf);
            }
        }
        else if (ch->pcdata->countdown > 0)
        {
            sprintf (buf, "Time left for current quest: %d\n\r", ch->pcdata->countdown);
            printf_to_char(ch,buf);
        }
        return;
    }

/* Checks for a character in the room with spec_questmaster set. This special
   procedure must be defined in special.c. You could instead use an
   ACT_QUESTMASTER flag instead of a special procedure. */

    for ( questman = ch->in_room->people; questman != NULL; questman = questman->next_in_room )
    {
        if (!IS_NPC(questman)) continue;
        if (questman->spec_fun == spec_lookup ("spec_questmaster")) break;
    }

    if (questman == NULL || questman->spec_fun != spec_lookup ("spec_questmaster"))
    {
        printf_to_char(ch,"You can't do that here.\n\r");
        return;
    }

    if ( questman->fighting != NULL)
    {
        printf_to_char(ch,"Wait until the fighting stops.\n\r");
        return;
    }

    ch->pcdata->questgiver = questman;

/* And, of course, you will need to change the following lines for YOUR
   quest item information. Quest items on Moongate are unbalanced, very
   very nice items, and no one has one yet, because it takes awhile to
   build up quest points :> Make the item worth their while. */

    if (!strcmp(arg1, "list"))
    {
        act( "$n asks $N for a list of quest items.", ch, NULL, questman, TO_ROOM);
        act ("You ask $N for a list of quest items.", ch, NULL, questman, TO_CHAR);
        sprintf(buf, "Current Quest Items available for Purchase:\n\r\
1000qp.........The COMFY CHAIR!!!!!!\n\r\
850qp..........Sword of Vassago\n\r\
750qp..........Amulet of Vassago\n\r\
750qp..........Shield of Vassago\n\r\
550qp..........Decanter of Endless Water\n\r\
500qp..........350,000 gold pieces\n\r\
500qp..........30 Practices\n\r\
To buy an item, type 'QUEST BUY <item>'.\n\r");
        printf_to_char(ch,buf);
        return;
    }

    else if (!strcmp(arg1, "buy"))
    {
        if (arg2[0] == '\0')
        {
            printf_to_char(ch,"To buy an item, type 'QUEST BUY <item>'.\n\r");
            return;
        }
        if (is_name(arg2, "ring"))
        {
            if (ch->pcdata->questpoints >= 750)
            {
                ch->pcdata->questpoints -= 750;
                obj = create_object(get_obj_index(QUEST_ITEM1),ch->level, FALSE);
            }
            else
            {
                sprintf(buf, "Sorry, %s, but you don't have enough quest points for that.", ch->name);
                do_tell_quest (ch, questman, buf);
                return;
            }
        }
        else if (is_name(arg2, "girth"))
        {
            if (ch->pcdata->questpoints >= 750)
            {
                ch->pcdata->questpoints -= 750;
                obj = create_object (get_obj_index(QUEST_ITEM2), ch->level, FALSE);
            }
            else
            {
                sprintf (buf, "Sorry, %s, but you don't have enough quest points for that.", ch->name);
                do_tell_quest(ch, questman, buf);
                return;
            }
        }
        else if (is_name (arg2, "weapon"))
        {
            if (ch->pcdata->questpoints >= 850)
            {
                ch->pcdata->questpoints -= 850;
                obj = create_object(get_obj_index(QUEST_ITEM3),ch->level, FALSE);
            }
            else
            {
                sprintf(buf, "Sorry, %s, but you don't have enough quest points for that.", ch->name);
                do_tell_quest(ch, questman,buf);
                return;
            }
        }
        else if (is_name(arg2, "backpack"))
        {
            if (ch->pcdata->questpoints >= 550)
            {
                ch->pcdata->questpoints -= 550;
                obj = create_object(get_obj_index(QUEST_ITEM4),ch->level, FALSE);
            }
            else
            {
                sprintf(buf, "Sorry, %s, but you don't have enough quest points for that.", ch->name);
                do_tell_quest(ch, questman,buf);
                return;
            }
        }
        else if (is_name(arg2, "practices pracs prac practice"))
        {
            if (ch->pcdata->questpoints >= 500)
            {
                ch->pcdata->questpoints -= 500;
                ch->practice += 30;
                act( "$N gives 30 practices to $n.", ch, NULL, questman, TO_ROOM );
                act( "$N gives you 30 practices.",   ch, NULL, questman, TO_CHAR );
                return;
            }
            else
            {
                sprintf(buf, "Sorry, %s, but you don't have enough quest points for that.",ch->name);
                do_tell_quest(ch, questman,buf);
                return;
            }
        }
        else if (is_name(arg2, "gold gp"))
        {
            if (ch->pcdata->questpoints >= 500)
            {
                ch->pcdata->questpoints -= 500;
                ch->gold += 350000;
                act( "$N gives 350,000 gold pieces to $n.", ch, NULL, questman, TO_ROOM );
                act( "$N has 350,000 in gold transfered from $s Swiss account to your balance.", ch, NULL, questman, TO_CHAR );
                return;
            }
            else
            {
                sprintf(buf, "Sorry, %s, but you don't have enough quest points for that.", ch->name);
                do_tell_quest(ch, questman,buf);
                return;
            }
        }
        else
        {
            sprintf(buf, "I don't have that item, %s.",ch->name);
            do_tell_quest(ch, questman, buf);
        }
        if (obj != NULL)
        {
            act( "$N gives $p to $n.", ch, obj, questman, TO_ROOM );
            act( "$N gives you $p.",   ch, obj, questman, TO_CHAR );
            obj_to_char(obj, ch);
        }
        return;
    }
    else if (!strcmp(arg1, "request"))
    {
        act( "$n asks $N for a quest.", ch, NULL, questman, TO_ROOM);
        act ("You ask $N for a quest.",ch, NULL, questman, TO_CHAR);
        if (IS_SET(ch->act, PLR_QUESTOR))
        {
            sprintf(buf, "But you're already on a quest!");
            do_tell_quest(ch, questman, buf);
            return;
        }
        if (ch->pcdata->nextquest > 0)
        {
            sprintf(buf, "You're very brave, %s, but let someone else have a chance.", ch->name);
            do_tell_quest(ch, questman, buf);
            sprintf(buf, "Come back later.");
            do_tell_quest(ch, questman, buf);
            return;
        }

        sprintf(buf, "Thank you, brave %s!",ch->name);
        do_tell_quest(ch, questman, buf);
        ch->pcdata->questmob = 0;
        ch->pcdata->questobj = 0;

        generate_quest(ch, questman);

        if (ch->pcdata->questmob > 0 || ch->pcdata->questobj > 0)
        {
            ch->pcdata->countdown = number_range(10,30);
            SET_BIT(ch->act, PLR_QUESTOR);
            sprintf(buf, "You have %d minutes to complete this quest.",ch->pcdata->countdown);
            do_tell_quest(ch, questman, buf);
            sprintf(buf, "May the gods go with you!");
            do_tell_quest(ch, questman, buf);
        }
        return;
    }
    else if (!strcmp(arg1, "complete"))
    {
        act( "$n informs $N $e has completed $s quest.", ch, NULL, questman, TO_ROOM);
        act ("You inform $N you have completed $s quest.",ch, NULL, questman, TO_CHAR);
        if (ch->pcdata->questgiver != questman)
        {
            sprintf(buf, "I never sent you on a quest! Perhaps you're thinking of someone else.");
            do_tell_quest(ch, questman,buf);
            return;
        }

        if (IS_SET(ch->act, PLR_QUESTOR))
        {
            if (ch->pcdata->questmob == -1 && ch->pcdata->countdown > 0)
            {
                int reward, pointreward, pracreward;

                reward = number_range(2500,45000);
                pointreward = number_range(25,75);

                sprintf(buf, "Congratulations on completing your quest!");
                do_tell_quest(ch, questman,buf);
                sprintf(buf,"As a reward, I am giving you %d quest points, and %d gold.", pointreward, reward);
                do_tell_quest(ch, questman,buf);
                if (chance(15))
                {
                    pracreward = number_range(1,6);
                    sprintf(buf, "You gain %d practices!\n\r",pracreward);
                    printf_to_char(ch,buf);
                    ch->practice += pracreward;
                }

                REMOVE_BIT(ch->act, PLR_QUESTOR);
                ch->pcdata->questgiver = NULL;
                ch->pcdata->countdown = 0;
                ch->pcdata->questmob = 0;
                ch->pcdata->questobj = 0;
                ch->pcdata->nextquest = number_range(1,4);
                ch->gold += reward;
                ch->pcdata->questpoints += pointreward;

                return;
            }
            else if (ch->pcdata->questobj > 0 && ch->pcdata->countdown > 0)
            {
                bool obj_found = FALSE;

                for (obj = ch->carrying; obj != NULL; obj= obj_next)
                {
                    obj_next = obj->next_content;

                    if (obj != NULL && obj->pIndexData->vnum == ch->pcdata->questobj)
                    {
                        obj_found = TRUE;
                        break;
                    }
                }
                if (obj_found == TRUE)
                {
                    int reward, pointreward, pracreward;

                    reward = number_range(2500,45000);
                    pointreward = number_range(25,75);

                    act("You hand $p to $N.",ch, obj, questman, TO_CHAR);
                    act("$n hands $p to $N.",ch, obj, questman, TO_ROOM);

                    sprintf(buf, "Congratulations on completing your quest!");
                    do_tell_quest(ch, questman,buf);
                    sprintf(buf,"As a reward, I am giving you %d quest points, and %d gold.", pointreward, reward);
                    do_tell_quest(ch, questman,buf);
                    if (chance(15))
                    {
                        pracreward = number_range(1,6);
                        sprintf(buf, "You gain %d practices!\n\r",pracreward);
                        printf_to_char(ch,buf);
                        ch->practice += pracreward;
                    }

                    REMOVE_BIT(ch->act, PLR_QUESTOR);
                    ch->pcdata->questgiver = NULL;
                    ch->pcdata->countdown = 0;
                    ch->pcdata->questmob = 0;
                    ch->pcdata->questobj = 0;
                    ch->pcdata->nextquest = 10;
                    ch->gold += reward;
                    ch->pcdata->questpoints += pointreward;
                    extract_obj(obj);
                    return;
                }
                else
                {
                    sprintf(buf, "You haven't completed the quest yet, but there is still time!");
                    do_tell_quest(ch, questman, buf);
                    return;
                }
                return;
            }
            else if ((ch->pcdata->questmob > 0 || ch->pcdata->questobj > 0) && ch->pcdata->countdown >
0)
            {
                sprintf(buf, "You haven't completed the quest yet, but there is still time!");
                do_tell_quest(ch, questman, buf);
                return;
            }
        }
        if (ch->pcdata->nextquest > 0)
            sprintf(buf,"But you didn't complete your quest in time!");
        else sprintf(buf, "You have to REQUEST a quest first, %s.",ch->name);
        do_tell_quest(ch, questman, buf);
        return;
    }

    printf_to_char(ch,"QUEST commands: POINTS INFO TIME REQUEST COMPLETE LIST BUY.\n\r");
    printf_to_char(ch,"For more information, type 'HELP QUEST'.\n\r");
    return;
}

void generate_quest(CHAR_DATA *ch, CHAR_DATA *questman)
{
    CHAR_DATA *victim;
    ROOM_INDEX_DATA *room;
    MOB_INDEX_DATA *vsearch;
    OBJ_DATA *questitem;
    char buf [MAX_STRING_LENGTH];
    int mob_buf[300],mob_count;

    /* first we have to find some mobs to kill or to put the quest object near */
    mob_count = 0;
    for (victim = char_list; victim != NULL; victim = victim->next)
    {
        if (!IS_NPC(victim))
        {
            continue;
        }

        if (    victim->level - ch->level > 4
            ||  ch->level - victim->level > 2
            ||  victim->pIndexData == NULL
            ||  victim->pIndexData->pShop != NULL
            ||  victim->in_room == NULL
            ||  (IS_GOOD(victim) && IS_GOOD(ch))
            ||  IS_SET(victim->act, ACT_PET)
            ||  IS_SET(victim->act, ACT_TRAIN)
            ||  IS_SET(victim->act, ACT_PRACTICE)
            ||  IS_SET(victim->act, ACT_IS_HEALER)
            ||  IS_SET(victim->act, ACT_IS_CHANGER)
            ||  IS_SET(victim->imm_flags, IMM_SUMMON)
            ||  IS_SET(victim->affected_by, AFF_CHARM))
        {
            continue;
        }

        mob_buf[mob_count] = victim->pIndexData->vnum;
        mob_count++;

        if (mob_count > 299)
        {
            break;
        }
    }

    if ( mob_count == 0  )
    {
        do_tell_quest(ch,questman,"Hmm... I'm sorry, but I don't have any quests for you at this time.");
        do_tell_quest(ch,questman,"Try again later.");
        ch->pcdata->nextquest = 0;
        return;
    }

    int tmp_number, i;
    victim = NULL;
    for(i=1;i<11;i++)
    {
        tmp_number = number_range(0,mob_count-1);

        vsearch = get_mob_index( mob_buf[tmp_number] );
        if(vsearch == NULL)
        {
            continue;
        }

        victim = get_quest_world( ch, vsearch );
        if(victim == NULL)
        {
            continue;
        }

        room = find_location( ch, victim->name );
        if(room == NULL)
        {
            vsearch = NULL;
            victim = NULL;
            continue;
        }

        break;
    }

    if(victim == NULL)
    {
        sprintf(buf, "I'm sorry, but I don't have any quests for you at this time.");
        do_tell_quest(ch,questman,buf);
        sprintf(buf, "Try again later.");
        do_tell_quest(ch,questman,buf);
        ch->pcdata->nextquest = 2;
        return;
    }

    /* object quest */
    if (number_percent()<40)
    {
        int objvnum = 0;

        switch(number_range(1,4))
        {
            case 1:
            objvnum = QUEST_OBJQUEST1;
            break;

            case 2:
            objvnum = QUEST_OBJQUEST2;
            break;

            case 3:
            objvnum = QUEST_OBJQUEST3;
            break;

            case 4:
            objvnum = QUEST_OBJQUEST4;
            break;
        }

        questitem = create_object( get_obj_index(objvnum), ch->level, FALSE );

        questitem->owner = str_dup(ch->name);
	    questitem->level = ch->level;

        sprintf( buf, questitem->pIndexData->extra_descr->description, ch->name );
	    questitem->extra_descr = new_extra_descr();
	    questitem->extra_descr->keyword = str_dup( questitem->pIndexData->extra_descr->keyword );
	    questitem->extra_descr->description = str_dup( buf );
	    questitem->extra_descr->next = NULL;
        questitem->cost = 0;
	    questitem->timer = 30;

        obj_to_room(questitem, room);
        ch->pcdata->questobj = questitem->pIndexData->vnum;

        sprintf(buf, "Vile pilferers have stolen %s from the royal treasury!", questitem->short_descr);
        do_tell_quest(ch,questman,buf);
        do_tell_quest(ch,questman,"My court wizardess, with her magic mirror, has pinpointed its location.");

        /* I changed my area names so that they have just the name of the area
           and none of the level stuff. You may want to comment these next two
           lines. - Vassago */

        sprintf(buf, "Look in the general area of %s for %s!", room->area->name, room->name);
        do_tell_quest(ch,questman,buf);
        return;
    }
    /* Quest to kill a mob */
    else
    {
        switch(number_range(0,1))
        {
            case 0:
            sprintf(buf, "An enemy of mine, %s, is making vile threats against the crown.", victim->short_descr);
            do_tell_quest(ch,questman,buf);
            sprintf(buf, "This threat must be eliminated!");
            do_tell_quest(ch,questman,buf);
            break;

            case 1:
            sprintf(buf, "Rune's most heinous criminal, %s, has escaped from the dungeon!", victim->short_descr);
            do_tell_quest(ch,questman,buf);
            sprintf(buf, "Since the escape, %s has murdered %d civillians!", victim->short_descr, number_range(2,20));
            do_tell_quest(ch,questman,buf);
            do_tell_quest(ch,questman,"The penalty for this crime is death, and you are to deliver the sentence!");
            break;
        }

        if (room->name != NULL)
        {
            sprintf(buf, "Seek %s out somewhere in the vicinity of %s!", victim->short_descr, room->name);
            do_tell_quest(ch,questman,buf);

            /* I changed my area names so that they have just the name of the area
            and none of the level stuff. You may want to comment these next two
            lines. - Vassago */

            sprintf(buf, "That location is in the general area of %s.", room->area->name);
            do_tell_quest(ch,questman,buf);
        }

        ch->pcdata->questmob = victim->pIndexData->vnum;
        return;
    }
}
                
/* Called from update_handler() by pulse_area */

void quest_update(void)
{
    DESCRIPTOR_DATA *d;
    CHAR_DATA *ch;

    for ( d = descriptor_list; d != NULL; d = d->next )
    {
        if (d->character != NULL && d->connected == CON_PLAYING)
        {

        ch = d->character;

        if (ch->pcdata->nextquest > 0)
        {
            ch->pcdata->nextquest--;
            if (ch->pcdata->nextquest == 0)
            {
                printf_to_char(ch,"You may now quest again.\n\r");
                return;
            }
        }
        else if (IS_SET(ch->act,PLR_QUESTOR))
        {
            if (--ch->pcdata->countdown <= 0)
            {
                char buf [MAX_STRING_LENGTH];

                ch->pcdata->nextquest = 10;
                sprintf(buf, "You have run out of time for your quest!"
                             "\n\rYou may quest again in %d minutes.\n\r", ch->pcdata->nextquest);
                printf_to_char(ch,buf);
                REMOVE_BIT(ch->act, PLR_QUESTOR);
                ch->pcdata->questgiver = NULL;
                ch->pcdata->countdown = 0;
                ch->pcdata->questmob = 0;
            }
            if (ch->pcdata->countdown > 0 && ch->pcdata->countdown < 6)
            {
                printf_to_char(ch,"Better hurry, you're almost out of time for your quest!\n\r");
                return;
            }
        }
        }
    }
    return;
}

void do_tell_quest( CHAR_DATA *ch, CHAR_DATA *victim, char *argument )
{

	    act_color("$N: $C$t$c",ch,argument,victim,TO_CHAR,POS_DEAD, CLR_LOPES_B_MAGENTA );

		return;
}

CHAR_DATA *get_quest_world( CHAR_DATA *ch, MOB_INDEX_DATA *victim)
{
    CHAR_DATA *wch;

    for ( wch = char_list; wch != NULL ; wch = wch->next )
    {
        if ( wch->in_room == NULL
		|| wch->pIndexData != victim)
            continue;

	return wch;
    }

    return NULL;
}
