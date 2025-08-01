/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *                                                                         *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael          *
 *  Chastain, Michael Quan, and Mitchell Tse.                              *
 *                                                                         *
 *  In order to use any part of this Merc Diku Mud, you must comply with   *
 *  both the original Diku license in 'license.doc' as well the Merc       *
 *  license in 'license.txt'.  In particular, you may not remove either of *
 *  these copyright notices.                                               *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 ***************************************************************************/
 
/***************************************************************************
*	ROM 2.4 is copyright 1993-1998 Russ Taylor			   *
*	ROM has been brought to you by the ROM consortium		   *
*	    Russ Taylor (rtaylor@hypercube.org)				   *
*	    Gabrielle Taylor (gtaylor@hypercube.org)			   *
*	    Brian Moore (zump@rom.org)					   *
*	By using this code, you have agreed to follow the terms of the	   *
*	ROM license, in the file Rom24/doc/rom.license			   *
***************************************************************************/

/***************************************************************************
 *   KURGAN MUD – Based on ROM 2.4, Merc 2.1, and DikuMUD                  *
 *                                                                         *
 *   KURGAN MUD is a customized extension by Özgür Yilmaz                  *
 ***************************************************************************/

#if defined(macintosh)
#include <types.h>
#include <time.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "merc.h"
#include "tables.h"

int flag_lookup args( ( const char *name, const struct flag_type *flag_table) );

void do_flag(CHAR_DATA *ch, char *argument)
{
    char arg1[MAX_INPUT_LENGTH],arg2[MAX_INPUT_LENGTH],arg3[MAX_INPUT_LENGTH];
    char word[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    long *flag, old = 0, new = 0, marked = 0, pos;
    char type;
    const struct flag_type *flag_table;

    argument = one_argument(argument,arg1);
    argument = one_argument(argument,arg2);
    argument = one_argument(argument,arg3);

    type = argument[0];

    if (type == '=' || type == '-' || type == '+')
        argument = one_argument(argument,word);

    if (arg1[0] == '\0')
    {
	printf_to_char(ch, "Syntax:\n\r");
	printf_to_char(ch, "  flag mob  <name> <field> <flags>\n\r");
	printf_to_char(ch, "  flag char <name> <field> <flags>\n\r");
	printf_to_char(ch, "  mob  flags: act,aff,off,imm,res,vuln,form,part\n\r");
	printf_to_char(ch, "  char flags: plr,comm,aff,imm,res,vuln,\n\r");
	printf_to_char(ch, "  +: add flag, -: remove flag, = set equal to\n\r");
	printf_to_char(ch, "  otherwise flag toggles the flags listed.\n\r");
	return;
    }

    if (arg2[0] == '\0')
    {
	printf_to_char(ch, "What do you wish to set flags on?\n\r");
	return;
    }

    if (arg3[0] == '\0')
    {
	printf_to_char(ch, "You need to specify a flag to set.\n\r");
	return;
    }

    if (argument[0] == '\0')
    {
	printf_to_char(ch, "Which flags do you wish to change?\n\r");
	return;
    }

    if (!str_prefix(arg1,"mob") || !str_prefix(arg1,"char"))
    {
	victim = get_char_world(ch,arg2);
	if (victim == NULL)
	{
	    printf_to_char(ch, "You can't find them.\n\r");
	    return;
	}

        /* select a flag to set */
	if (!str_prefix(arg3,"act"))
	{
	    if (!IS_NPC(victim))
	    {
		printf_to_char(ch, "Use plr for PCs.\n\r");
		return;
	    }

	    flag = &victim->act;
	    flag_table = act_flags;
	}

	else if (!str_prefix(arg3,"plr"))
	{
	    if (IS_NPC(victim))
	    {
		printf_to_char(ch, "Use act for NPCs.\n\r");
		return;
	    }

	    flag = &victim->act;
	    flag_table = plr_flags;
	}

 	else if (!str_prefix(arg3,"aff"))
	{
	    flag = &victim->affected_by;
	    flag_table = affect_flags;
	}

  	else if (!str_prefix(arg3,"immunity"))
	{
	    flag = &victim->imm_flags;
	    flag_table = imm_flags;
	}

	else if (!str_prefix(arg3,"resist"))
	{
	    flag = &victim->res_flags;
	    flag_table = imm_flags;
	}

	else if (!str_prefix(arg3,"vuln"))
	{
	    flag = &victim->vuln_flags;
	    flag_table = imm_flags;
	}

	else if (!str_prefix(arg3,"form"))
	{
	    if (!IS_NPC(victim))
	    {
	 	printf_to_char(ch, "Form can't be set on PCs.\n\r");
		return;
	    }

	    flag = &victim->form;
	    flag_table = form_flags;
	}

	else if (!str_prefix(arg3,"parts"))
	{
	    if (!IS_NPC(victim))
	    {
		printf_to_char(ch, "Parts can't be set on PCs.\n\r");
		return;
	    }

	    flag = &victim->parts;
	    flag_table = part_flags;
	}

	else if (!str_prefix(arg3,"comm"))
	{
	    if (IS_NPC(victim))
	    {
		printf_to_char(ch, "Comm can't be set on NPCs.\n\r");
		return;
	    }

	    flag = &victim->comm;
	    flag_table = comm_flags;
	}

	else 
	{
	    printf_to_char(ch, "That's not an acceptable flag.\n\r");
	    return;
	}

	old = *flag;
	victim->zone = NULL;

	if (type != '=')
	    new = old;

        /* mark the words */
        for (; ;)
        {
	    argument = one_argument(argument,word);

	    if (word[0] == '\0')
		break;

	    pos = flag_lookup(word,flag_table);
	    if (pos == 0)
	    {
		printf_to_char(ch, "That flag doesn't exist!\n\r");
		return;
	    }
	    else
		SET_BIT(marked,pos);
	}

	for (pos = 0; flag_table[pos].name != NULL; pos++)
	{
	    if (!flag_table[pos].settable && IS_SET(old,flag_table[pos].bit))
	    {
		SET_BIT(new,flag_table[pos].bit);
		continue;
	    }

	    if (IS_SET(marked,flag_table[pos].bit))
	    {
		switch(type)
		{
		    case '=':
		    case '+':
			SET_BIT(new,flag_table[pos].bit);
			break;
		    case '-':
			REMOVE_BIT(new,flag_table[pos].bit);
			break;
		    default:
			if (IS_SET(new,flag_table[pos].bit))
			    REMOVE_BIT(new,flag_table[pos].bit);
			else
			    SET_BIT(new,flag_table[pos].bit);
		}
	    }
	}
	*flag = new;
	return;
    }
}



    
