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
#else
#include <sys/types.h>
#include <sys/time.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include "merc.h"
#include "interp.h"
#include "magic.h"
#include "recycle.h"
#include "tables.h"
#include "lookup.h"

char *	const	where_name	[] =
{
    "<used as light>     ",
    "<worn on finger>    ",
    "<worn on finger>    ",
    "<worn around neck>  ",
    "<worn around neck>  ",
    "<worn on torso>     ",
    "<worn on head>      ",
    "<worn on legs>      ",
    "<worn on feet>      ",
    "<worn on hands>     ",
    "<worn on arms>      ",
    "<worn as shield>    ",
    "<worn about body>   ",
    "<worn about waist>  ",
    "<worn around wrist> ",
    "<worn around wrist> ",
    "<wielded>           ",
    "<held>              ",
    "<floating nearby>   ",
};


/* for  keeping track of the player count */
int max_on = 0;

/*
 * Local functions.
 */
char *	format_obj_to_char	args( ( OBJ_DATA *obj, CHAR_DATA *ch,
				    bool fShort ) );
void	show_list_to_char	args( ( OBJ_DATA *list, CHAR_DATA *ch,
				    bool fShort, bool fShowNothing ) );
void	show_char_to_char_0	args( ( CHAR_DATA *victim, CHAR_DATA *ch ) );
void	show_char_to_char_1	args( ( CHAR_DATA *victim, CHAR_DATA *ch ) );
void	show_char_to_char	args( ( CHAR_DATA *list, CHAR_DATA *ch ) );
bool	check_blind		args( ( CHAR_DATA *ch ) );



char *format_obj_to_char( OBJ_DATA *obj, CHAR_DATA *ch, bool fShort )
{
    static char buf[MAX_STRING_LENGTH];

    buf[0] = '\0';

    if (IS_IMMORTAL(ch) && !IS_NPC(ch) && IS_SET(ch->act, PLR_HOLYLIGHT))
    {
        sprintf(buf,"[Obj %d]",obj->pIndexData->vnum);
    }

    if ((fShort && (obj->short_descr == NULL || obj->short_descr[0] == '\0'))
    ||  (obj->description == NULL || obj->description[0] == '\0'))
	return buf;

    if ( IS_OBJ_STAT(obj, ITEM_INVIS)     )   strcat( buf, "(Invis) "     );
    if ( IS_AFFECTED(ch, AFF_DETECT_EVIL)
         && IS_OBJ_STAT(obj, ITEM_EVIL)   )   strcat( buf, "(Red Aura) "  );
    if (IS_AFFECTED(ch, AFF_DETECT_GOOD)
    &&  IS_OBJ_STAT(obj,ITEM_BLESS))	      strcat(buf,"(Blue Aura) "	);
    if ( IS_AFFECTED(ch, AFF_DETECT_MAGIC)
         && IS_OBJ_STAT(obj, ITEM_MAGIC)  )   strcat( buf, "(Magical) "   );
    if ( IS_OBJ_STAT(obj, ITEM_GLOW)      )   strcat( buf, "(Glowing) "   );
    if ( IS_OBJ_STAT(obj, ITEM_HUM)       )   strcat( buf, "(Humming) "   );

    if ( fShort )
    {
	if ( obj->short_descr != NULL )
	    strcat( buf, obj->short_descr );
    }
    else
    {
	if ( obj->description != NULL)
	    strcat( buf, obj->description );
    }

    return buf;
}



/*
 * Show a list to a character.
 * Can coalesce duplicated items.
 */
void show_list_to_char( OBJ_DATA *list, CHAR_DATA *ch, bool fShort, bool fShowNothing )
{
    char buf[MAX_STRING_LENGTH];
    BUFFER *output;
    char **prgpstrShow;
    int *prgnShow;
    char *pstrShow;
    OBJ_DATA *obj;
    int nShow;
    int iShow;
    int count;
    bool fCombine;

    if ( ch->desc == NULL )
	return;

    /*
     * Alloc space for output lines.
     */
    output = new_buf();

    count = 0;
    for ( obj = list; obj != NULL; obj = obj->next_content )
	count++;
    prgpstrShow	= alloc_mem( count * sizeof(char *) );
    prgnShow    = alloc_mem( count * sizeof(int)    );
    nShow	= 0;

    /*
     * Format the list of objects.
     */
    for ( obj = list; obj != NULL; obj = obj->next_content )
    { 
	if ( obj->wear_loc == WEAR_NONE && can_see_obj( ch, obj )) 
	{
	    pstrShow = format_obj_to_char( obj, ch, fShort );

	    fCombine = FALSE;

	    if ( IS_NPC(ch) || IS_SET(ch->comm, COMM_COMBINE) )
	    {
		/*
		 * Look for duplicates, case sensitive.
		 * Matches tend to be near end so run loop backwords.
		 */
		for ( iShow = nShow - 1; iShow >= 0; iShow-- )
		{
		    if ( !strcmp( prgpstrShow[iShow], pstrShow ) )
		    {
			prgnShow[iShow]++;
			fCombine = TRUE;
			break;
		    }
		}
	    }

	    /*
	     * Couldn't combine, or didn't want to.
	     */
	    if ( !fCombine )
	    {
		prgpstrShow [nShow] = str_dup( pstrShow );
		prgnShow    [nShow] = 1;
		nShow++;
	    }
	}
    }

    /*
     * Output the formatted list.
     */
    for ( iShow = 0; iShow < nShow; iShow++ )
    {
	if (prgpstrShow[iShow][0] == '\0')
	{
	    free_string(prgpstrShow[iShow]);
	    continue;
	}

	if ( IS_NPC(ch) || IS_SET(ch->comm, COMM_COMBINE) )
	{
	    if ( prgnShow[iShow] != 1 )
	    {
		sprintf( buf, "(%2d) ", prgnShow[iShow] );
		add_buf(output,buf);
	    }
	    else
	    {
		add_buf(output,"     ");
	    }
	}
	add_buf(output,prgpstrShow[iShow]);
	add_buf(output,"\n\r");
	free_string( prgpstrShow[iShow] );
    }

    if ( fShowNothing && nShow == 0 )
    {
	if ( IS_NPC(ch) || IS_SET(ch->comm, COMM_COMBINE) )
	    printf_to_char(ch, "     ");
	printf_to_char(ch, "Nothing.\n\r");
    }
    page_to_char(buf_string(output),ch);

    /*
     * Clean up.
     */
    free_buf(output);
    free_mem( prgpstrShow, count * sizeof(char *) );
    free_mem( prgnShow,    count * sizeof(int)    );

    return;
}



void show_char_to_char_0( CHAR_DATA *victim, CHAR_DATA *ch )
{
    char buf[MAX_STRING_LENGTH],message[MAX_STRING_LENGTH];

    buf[0] = '\0';

    if (IS_IMMORTAL(ch) && !IS_NPC(ch) && IS_SET(ch->act, PLR_HOLYLIGHT) && IS_NPC(victim))
    {
        sprintf(buf, " [Mob %d]", victim->pIndexData->vnum);
    }

    if ( IS_SET(victim->comm,COMM_AFK	  )   ) strcat( buf, "[AFK] "	     );
    if ( IS_AFFECTED(victim, AFF_INVISIBLE)   ) strcat( buf, "(Invis) "      );
    if ( victim->invis_level >= LEVEL_HERO    ) strcat( buf, "(Wizi) "	     );
    if ( IS_AFFECTED(victim, AFF_HIDE)        ) strcat( buf, "(Hide) "       );
    if ( IS_AFFECTED(victim, AFF_CHARM)       ) strcat( buf, "(Charmed) "    );
    if ( IS_AFFECTED(victim, AFF_PASS_DOOR)   ) strcat( buf, "(Translucent) ");
    if ( IS_AFFECTED(victim, AFF_FAERIE_FIRE) ) strcat( buf, "(Pink Aura) "  );
    if ( IS_EVIL(victim)
    &&   IS_AFFECTED(ch, AFF_DETECT_EVIL)     ) strcat( buf, "(Red Aura) "   );
    if ( IS_GOOD(victim)
    &&   IS_AFFECTED(ch, AFF_DETECT_GOOD)     ) strcat( buf, "(Golden Aura) ");
    if ( IS_AFFECTED(victim, AFF_SANCTUARY)   ) strcat( buf, "(White Aura) " );
    if ( !IS_NPC(victim) && IS_SET(victim->act, PLR_KILLER ) )
						strcat( buf, "(KILLER) "     );
    if ( !IS_NPC(victim) && IS_SET(victim->act, PLR_THIEF  ) )
						strcat( buf, "(THIEF) "      );
    if ( victim->position == victim->start_pos && victim->long_descr[0] != '\0' )
    {
	strcat( buf, victim->long_descr );
	printf_to_char(ch, buf);
	return;
    }

    strcat( buf, PERS( victim, ch ) );
    if ( !IS_NPC(victim) && !IS_SET(ch->comm, COMM_BRIEF) 
    &&   victim->position == POS_STANDING && ch->on == NULL )
	strcat( buf, victim->pcdata->title );

    switch ( victim->position )
    {
    case POS_DEAD:     strcat( buf, " is DEAD!!" );              break;
    case POS_MORTAL:   strcat( buf, " is mortally wounded." );   break;
    case POS_INCAP:    strcat( buf, " is incapacitated." );      break;
    case POS_STUNNED:  strcat( buf, " is lying here stunned." ); break;
    case POS_SLEEPING: 
	if (victim->on != NULL)
	{
	    if (IS_SET(victim->on->value[2],SLEEP_AT))
  	    {
		sprintf(message," is sleeping at %s.",
		    victim->on->short_descr);
		strcat(buf,message);
	    }
	    else if (IS_SET(victim->on->value[2],SLEEP_ON))
	    {
		sprintf(message," is sleeping on %s.",
		    victim->on->short_descr); 
		strcat(buf,message);
	    }
	    else
	    {
		sprintf(message, " is sleeping in %s.",
		    victim->on->short_descr);
		strcat(buf,message);
	    }
	}
	else 
	    strcat(buf," is sleeping here.");
	break;
    case POS_RESTING:  
        if (victim->on != NULL)
	{
            if (IS_SET(victim->on->value[2],REST_AT))
            {
                sprintf(message," is resting at %s.",
                    victim->on->short_descr);
                strcat(buf,message);
            }
            else if (IS_SET(victim->on->value[2],REST_ON))
            {
                sprintf(message," is resting on %s.",
                    victim->on->short_descr);
                strcat(buf,message);
            }
            else 
            {
                sprintf(message, " is resting in %s.",
                    victim->on->short_descr);
                strcat(buf,message);
            }
	}
        else
	    strcat( buf, " is resting here." );       
	break;
    case POS_SITTING:  
        if (victim->on != NULL)
        {
            if (IS_SET(victim->on->value[2],SIT_AT))
            {
                sprintf(message," is sitting at %s.",
                    victim->on->short_descr);
                strcat(buf,message);
            }
            else if (IS_SET(victim->on->value[2],SIT_ON))
            {
                sprintf(message," is sitting on %s.",
                    victim->on->short_descr);
                strcat(buf,message);
            }
            else
            {
                sprintf(message, " is sitting in %s.",
                    victim->on->short_descr);
                strcat(buf,message);
            }
        }
        else
	    strcat(buf, " is sitting here.");
	break;
    case POS_STANDING: 
	if (victim->on != NULL)
	{
	    if (IS_SET(victim->on->value[2],STAND_AT))
	    {
		sprintf(message," is standing at %s.",
		    victim->on->short_descr);
		strcat(buf,message);
	    }
	    else if (IS_SET(victim->on->value[2],STAND_ON))
	    {
		sprintf(message," is standing on %s.",
		   victim->on->short_descr);
		strcat(buf,message);
	    }
	    else
	    {
		sprintf(message," is standing in %s.",
		    victim->on->short_descr);
		strcat(buf,message);
	    }
	}
	else
	    strcat( buf, " is here." );               
	break;
    case POS_FIGHTING:
	strcat( buf, " is here, fighting " );
	if ( victim->fighting == NULL )
	    strcat( buf, "thin air??" );
	else if ( victim->fighting == ch )
	    strcat( buf, "YOU!" );
	else if ( victim->in_room == victim->fighting->in_room )
	{
	    strcat( buf, PERS( victim->fighting, ch ) );
	    strcat( buf, "." );
	}
	else
	    strcat( buf, "someone who left??" );
	break;
    }

    strcat( buf, "\n\r" );
    buf[0] = UPPER(buf[0]);
    printf_to_char(ch, buf);
    return;
}



void show_char_to_char_1( CHAR_DATA *victim, CHAR_DATA *ch )
{
    char buf[MAX_STRING_LENGTH];
    OBJ_DATA *obj;
    int iWear;
    int percent;
    bool found;

    if ( can_see( victim, ch ) )
    {
	if (ch == victim)
	    act( "$n looks at $mself.",ch,NULL,NULL,TO_ROOM);
	else
	{
	    act( "$n looks at you.", ch, NULL, victim, TO_VICT    );
	    act( "$n looks at $N.",  ch, NULL, victim, TO_NOTVICT );
	}
    }

    if ( victim->description[0] != '\0' )
    {
	printf_to_char(ch, victim->description);
    }
    else
    {
	act( "You see nothing special about $M.", ch, NULL, victim, TO_CHAR );
    }

    if ( victim->max_hit > 0 )
	percent = ( 100 * victim->hit ) / victim->max_hit;
    else
	percent = -1;

    strcpy( buf, PERS(victim, ch) );

    if (percent >= 100) 
	strcat( buf, " is in excellent condition.\n\r");
    else if (percent >= 90) 
	strcat( buf, " has a few scratches.\n\r");
    else if (percent >= 75) 
	strcat( buf," has some small wounds and bruises.\n\r");
    else if (percent >=  50) 
	strcat( buf, " has quite a few wounds.\n\r");
    else if (percent >= 30)
	strcat( buf, " has some big nasty wounds and scratches.\n\r");
    else if (percent >= 15)
	strcat ( buf, " looks pretty hurt.\n\r");
    else if (percent >= 0 )
	strcat (buf, " is in awful condition.\n\r");
    else
	strcat(buf, " is bleeding to death.\n\r");

    buf[0] = UPPER(buf[0]);
    printf_to_char(ch, buf);

    found = FALSE;
    for ( iWear = 0; iWear < MAX_WEAR; iWear++ )
    {
	if ( ( obj = get_eq_char( victim, iWear ) ) != NULL
	&&   can_see_obj( ch, obj ) )
	{
	    if ( !found )
	    {
		printf_to_char(ch, "\n\r");
		act( "$N is using:", ch, NULL, victim, TO_CHAR );
		found = TRUE;
	    }
	    printf_to_char(ch, where_name[iWear]);
	    printf_to_char(ch, format_obj_to_char( obj, ch, TRUE ));
	    printf_to_char(ch, "\n\r");
	}
    }

    if ( victim != ch
    &&   !IS_NPC(ch)
    &&   number_percent( ) < get_skill(ch,gsn_peek))
    {
	printf_to_char(ch, "\n\rYou peek at the inventory:\n\r");
	check_improve(ch,gsn_peek,TRUE,4);
	show_list_to_char( victim->carrying, ch, TRUE, TRUE );
    }

    return;
}



void show_char_to_char( CHAR_DATA *list, CHAR_DATA *ch )
{
    CHAR_DATA *rch;

    for ( rch = list; rch != NULL; rch = rch->next_in_room )
    {
	if ( rch == ch )
	    continue;

	if ( get_trust(ch) < rch->invis_level)
	    continue;

	if ( can_see( ch, rch ) )
	{
	    show_char_to_char_0( rch, ch );
	}
	else if ( room_is_dark( ch->in_room )
	&&        IS_AFFECTED(rch, AFF_INFRARED ) )
	{
	    printf_to_char(ch, "You see glowing red eyes watching YOU!\n\r");
	}
    }

    return;
} 



bool check_blind( CHAR_DATA *ch )
{

    if (!IS_NPC(ch) && IS_SET(ch->act,PLR_HOLYLIGHT))
	return TRUE;

    if ( IS_AFFECTED(ch, AFF_BLIND) )
    { 
	printf_to_char(ch, "You can't see a thing!\n\r"); 
	return FALSE; 
    }

    return TRUE;
}

/* changes your scroll */
void do_scroll(CHAR_DATA *ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    char buf[100];
    int lines;

    one_argument(argument,arg);
    
    if (arg[0] == '\0')
    {
	if (ch->lines == 0)
	    printf_to_char(ch, "You do not page long messages.\n\r");
	else
	{
	    sprintf(buf,"You currently display %d lines per page.\n\r",
		    ch->lines + 2);
	    printf_to_char(ch, buf);
	}
	return;
    }

    if (!is_number(arg))
    {
	printf_to_char(ch, "You must provide a number.\n\r");
	return;
    }

    lines = atoi(arg);

    if (lines == 0)
    {
        printf_to_char(ch, "Paging disabled.\n\r");
        ch->lines = 0;
        return;
    }

    if (lines < 10 || lines > 100)
    {
	printf_to_char(ch, "You must provide a reasonable number.\n\r");
	return;
    }

    sprintf(buf,"Scroll set to %d lines.\n\r",lines);
    printf_to_char(ch, buf);
    ch->lines = lines - 2;
}

/* RT does socials */
void do_socials(CHAR_DATA *ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    int iSocial;
    int col;
     
    col = 0;
   
    for (iSocial = 0; social_table[iSocial].name[0] != '\0'; iSocial++)
    {
	sprintf(buf,"%-12s",social_table[iSocial].name);
	printf_to_char(ch, buf);
	if (++col % 6 == 0)
	    printf_to_char(ch, "\n\r");
    }

    if ( col % 6 != 0)
	printf_to_char(ch, "\n\r");
    return;
}


 
/* RT Commands to replace news, motd, imotd, etc from ROM */

void do_motd(CHAR_DATA *ch, char *argument)
{
    do_function(ch, &do_help, "motd");
}

void do_imotd(CHAR_DATA *ch, char *argument)
{  
    do_function(ch, &do_help, "imotd");
}

void do_rules(CHAR_DATA *ch, char *argument)
{
    do_function(ch, &do_help, "rules");
}

void do_story(CHAR_DATA *ch, char *argument)
{
    do_function(ch, &do_help, "story");
}

void do_wizlist(CHAR_DATA *ch, char *argument)
{
    do_function(ch, &do_help, "wizlist");
}

/* RT this following section holds all the auto commands from ROM, as well as
   replacements for config */

void do_autolist(CHAR_DATA *ch, char *argument)
{
    /* lists most player flags */
    if (IS_NPC(ch))
      return;

    printf_to_char(ch, "   action     status\n\r");
    printf_to_char(ch, "---------------------\n\r");
 
    printf_to_char(ch, "autoassist     ");
    if (IS_SET(ch->act,PLR_AUTOASSIST))
        printf_to_char(ch, "ON\n\r");
    else
        printf_to_char(ch, "OFF\n\r"); 

    printf_to_char(ch, "autogold       ");
    if (IS_SET(ch->act,PLR_AUTOGOLD))
        printf_to_char(ch, "ON\n\r");
    else
        printf_to_char(ch, "OFF\n\r");

    printf_to_char(ch, "autoloot       ");
    if (IS_SET(ch->act,PLR_AUTOLOOT))
        printf_to_char(ch, "ON\n\r");
    else
        printf_to_char(ch, "OFF\n\r");

    printf_to_char(ch, "autosac        ");
    if (IS_SET(ch->act,PLR_AUTOSAC))
        printf_to_char(ch, "ON\n\r");
    else
        printf_to_char(ch, "OFF\n\r");

    printf_to_char(ch, "autosplit      ");
    if (IS_SET(ch->act,PLR_AUTOSPLIT))
        printf_to_char(ch, "ON\n\r");
    else
        printf_to_char(ch, "OFF\n\r");

    printf_to_char(ch, "prompt         ");
    if (IS_SET(ch->comm,COMM_PROMPT))
	printf_to_char(ch, "ON\n\r");
    else
	printf_to_char(ch, "OFF\n\r");

    printf_to_char(ch, "combine items  ");
    if (IS_SET(ch->comm,COMM_COMBINE))
	printf_to_char(ch, "ON\n\r");
    else
	printf_to_char(ch, "OFF\n\r");

    if (!IS_SET(ch->act,PLR_CANLOOT))
	printf_to_char(ch, "Your corpse is safe from thieves.\n\r");
    else 
        printf_to_char(ch, "Your corpse may be looted.\n\r");

    if (IS_SET(ch->act,PLR_NOSUMMON))
	printf_to_char(ch, "You cannot be summoned.\n\r");
    else
	printf_to_char(ch, "You can be summoned.\n\r");
   
    if (IS_SET(ch->act,PLR_NOFOLLOW))
	printf_to_char(ch, "You do not welcome followers.\n\r");
    else
	printf_to_char(ch, "You accept followers.\n\r");
}

void do_autoassist(CHAR_DATA *ch, char *argument)
{
    if (IS_NPC(ch))
      return;
    
    if (IS_SET(ch->act,PLR_AUTOASSIST))
    {
      printf_to_char(ch, "Autoassist removed.\n\r");
      REMOVE_BIT(ch->act,PLR_AUTOASSIST);
    }
    else
    {
      printf_to_char(ch, "You will now assist when needed.\n\r");
      SET_BIT(ch->act,PLR_AUTOASSIST);
    }
}

void do_autogold(CHAR_DATA *ch, char *argument)
{
    if (IS_NPC(ch))
      return;
 
    if (IS_SET(ch->act,PLR_AUTOGOLD))
    {
      printf_to_char(ch, "Autogold removed.\n\r");
      REMOVE_BIT(ch->act,PLR_AUTOGOLD);
    }
    else
    {
      printf_to_char(ch, "Automatic gold looting set.\n\r");
      SET_BIT(ch->act,PLR_AUTOGOLD);
    }
}

void do_autoloot(CHAR_DATA *ch, char *argument)
{
    if (IS_NPC(ch))
      return;
 
    if (IS_SET(ch->act,PLR_AUTOLOOT))
    {
      printf_to_char(ch, "Autolooting removed.\n\r");
      REMOVE_BIT(ch->act,PLR_AUTOLOOT);
    }
    else
    {
      printf_to_char(ch, "Automatic corpse looting set.\n\r");
      SET_BIT(ch->act,PLR_AUTOLOOT);
    }
}

void do_autosac(CHAR_DATA *ch, char *argument)
{
    if (IS_NPC(ch))
      return;
 
    if (IS_SET(ch->act,PLR_AUTOSAC))
    {
      printf_to_char(ch, "Autosacrificing removed.\n\r");
      REMOVE_BIT(ch->act,PLR_AUTOSAC);
    }
    else
    {
      printf_to_char(ch, "Automatic corpse sacrificing set.\n\r");
      SET_BIT(ch->act,PLR_AUTOSAC);
    }
}

void do_autosplit(CHAR_DATA *ch, char *argument)
{
    if (IS_NPC(ch))
      return;
 
    if (IS_SET(ch->act,PLR_AUTOSPLIT))
    {
      printf_to_char(ch, "Autosplitting removed.\n\r");
      REMOVE_BIT(ch->act,PLR_AUTOSPLIT);
    }
    else
    {
      printf_to_char(ch, "Automatic gold splitting set.\n\r");
      SET_BIT(ch->act,PLR_AUTOSPLIT);
    }
}

void do_brief(CHAR_DATA *ch, char *argument)
{
    if (IS_SET(ch->comm,COMM_BRIEF))
    {
      printf_to_char(ch, "Full descriptions activated.\n\r");
      REMOVE_BIT(ch->comm,COMM_BRIEF);
    }
    else
    {
      printf_to_char(ch, "Short descriptions activated.\n\r");
      SET_BIT(ch->comm,COMM_BRIEF);
    }
}

void do_show(CHAR_DATA *ch, char *argument)
{
    if (IS_SET(ch->comm,COMM_SHOW_AFFECTS))
    {
      printf_to_char(ch, "Affects will no longer be shown in score.\n\r");
      REMOVE_BIT(ch->comm,COMM_SHOW_AFFECTS);
    }
    else
    {
      printf_to_char(ch, "Affects will now be shown in score.\n\r");
      SET_BIT(ch->comm,COMM_SHOW_AFFECTS);
    }
}

void do_prompt(CHAR_DATA *ch, char *argument)
{
   char buf[MAX_STRING_LENGTH];
 
   if ( argument[0] == '\0' )
   {
	if (IS_SET(ch->comm,COMM_PROMPT))
   	{
      	    printf_to_char(ch, "You will no longer see prompts.\n\r");
      	    REMOVE_BIT(ch->comm,COMM_PROMPT);
    	}
    	else
    	{
      	    printf_to_char(ch, "You will now see prompts.\n\r");
      	    SET_BIT(ch->comm,COMM_PROMPT);
    	}
       return;
   }
 
   if( !strcmp( argument, "all" ) ) //"<%hhp %mm %vmv> "
      strcpy( buf, "Hp:%h/%H Mp:%m/%M Mv:%v/%V <%o> ");
   else
   {
      if ( strlen(argument) > 50 )
         argument[50] = '\0';
      strcpy( buf, argument );
      smash_tilde( buf );
      if (str_suffix("%c",buf))
	strcat(buf," ");
	
   }
 
   free_string( ch->prompt );
   ch->prompt = str_dup( buf );
   sprintf(buf,"Prompt set to %s\n\r",ch->prompt );
   printf_to_char(ch, buf);
   return;
}

void do_combine(CHAR_DATA *ch, char *argument)
{
    if (IS_SET(ch->comm,COMM_COMBINE))
    {
      printf_to_char(ch, "Long inventory selected.\n\r");
      REMOVE_BIT(ch->comm,COMM_COMBINE);
    }
    else
    {
      printf_to_char(ch, "Combined inventory selected.\n\r");
      SET_BIT(ch->comm,COMM_COMBINE);
    }
}

void do_noloot(CHAR_DATA *ch, char *argument)
{
    if (IS_NPC(ch))
      return;
 
    if (IS_SET(ch->act,PLR_CANLOOT))
    {
      printf_to_char(ch, "Your corpse is now safe from thieves.\n\r");
      REMOVE_BIT(ch->act,PLR_CANLOOT);
    }
    else
    {
      printf_to_char(ch, "Your corpse may now be looted.\n\r");
      SET_BIT(ch->act,PLR_CANLOOT);
    }
}

void do_nofollow(CHAR_DATA *ch, char *argument)
{
    if (IS_NPC(ch))
      return;
 
    if (IS_SET(ch->act,PLR_NOFOLLOW))
    {
      printf_to_char(ch, "You now accept followers.\n\r");
      REMOVE_BIT(ch->act,PLR_NOFOLLOW);
    }
    else
    {
      printf_to_char(ch, "You no longer accept followers.\n\r");
      SET_BIT(ch->act,PLR_NOFOLLOW);
      die_follower( ch );
    }
}

void do_nosummon(CHAR_DATA *ch, char *argument)
{
    if (IS_NPC(ch))
    {
      if (IS_SET(ch->imm_flags,IMM_SUMMON))
      {
	printf_to_char(ch, "You are no longer immune to summon.\n\r");
	REMOVE_BIT(ch->imm_flags,IMM_SUMMON);
      }
      else
      {
	printf_to_char(ch, "You are now immune to summoning.\n\r");
	SET_BIT(ch->imm_flags,IMM_SUMMON);
      }
    }
    else
    {
      if (IS_SET(ch->act,PLR_NOSUMMON))
      {
        printf_to_char(ch, "You are no longer immune to summon.\n\r");
        REMOVE_BIT(ch->act,PLR_NOSUMMON);
      }
      else
      {
        printf_to_char(ch, "You are now immune to summoning.\n\r");
        SET_BIT(ch->act,PLR_NOSUMMON);
      }
    }
}

void do_look( CHAR_DATA *ch, char *argument )
{
    char buf  [MAX_STRING_LENGTH];
    char arg1 [MAX_INPUT_LENGTH];
    char arg2 [MAX_INPUT_LENGTH];
    char arg3 [MAX_INPUT_LENGTH];
    EXIT_DATA *pexit;
    CHAR_DATA *victim;
    OBJ_DATA *obj;
    char *pdesc;
    int door;
    int number,count;

    if ( ch->desc == NULL )
	return;

    if ( ch->position < POS_SLEEPING )
    {
	printf_to_char(ch, "You can't see anything but stars!\n\r");
	return;
    }

    if ( ch->position == POS_SLEEPING )
    {
	printf_to_char(ch, "You can't see anything, you're sleeping!\n\r");
	return;
    }

    if ( !check_blind( ch ) )
	return;

    if ( !IS_NPC(ch)
    &&   !IS_SET(ch->act, PLR_HOLYLIGHT)
    &&   room_is_dark( ch->in_room ) )
    {
	printf_to_char(ch, "It is pitch black ... \n\r");
	show_char_to_char( ch->in_room->people, ch );
	return;
    }

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    number = number_argument(arg1,arg3);
    count = 0;

    if ( arg1[0] == '\0' || !str_cmp( arg1, "auto" ) )
    {
	/* 'look' or 'look auto' */
	printf_to_char(ch, "%s%s%s", CLR_ROOM_NAME, ch->in_room->name, CLR_RESET);

	if (IS_IMMORTAL(ch) && !IS_NPC(ch) && IS_SET(ch->act, PLR_HOLYLIGHT))
	{
	    printf_to_char(ch, " [Room %d]",ch->in_room->vnum);
	}

	printf_to_char(ch, "\n\r");

	if ( arg1[0] == '\0'
	|| ( !IS_NPC(ch) && !IS_SET(ch->comm, COMM_BRIEF) ) )
	{
	    printf_to_char(ch, "  ");
	    printf_to_char(ch, ch->in_room->description);
	}

    printf_to_char(ch, "\n\r");
    do_function(ch, &do_exits, "auto" );

	show_list_to_char( ch->in_room->contents, ch, FALSE, FALSE );
	show_char_to_char( ch->in_room->people,   ch );
	return;
    }

    if ( !str_cmp( arg1, "i" ) || !str_cmp(arg1, "in")  || !str_cmp(arg1,"on"))
    {
	/* 'look in' */
	if ( arg2[0] == '\0' )
	{
	    printf_to_char(ch, "Look in what?\n\r");
	    return;
	}

	if ( ( obj = get_obj_here( ch, arg2 ) ) == NULL )
	{
	    printf_to_char(ch, "You do not see that here.\n\r");
	    return;
	}

	switch ( obj->item_type )
	{
	default:
	    printf_to_char(ch, "That is not a container.\n\r");
	    break;

	case ITEM_DRINK_CON:
	    if ( obj->value[1] <= 0 )
	    {
		printf_to_char(ch, "It is empty.\n\r");
		break;
	    }

	    sprintf( buf, "It's %sfilled with  a %s liquid.\n\r",
		obj->value[1] <     obj->value[0] / 4
		    ? "less than half-" :
		obj->value[1] < 3 * obj->value[0] / 4
		    ? "about half-"     : "more than half-",
		liq_table[obj->value[2]].liq_color
		);

	    printf_to_char(ch, buf);
	    break;

	case ITEM_CONTAINER:
	case ITEM_CORPSE_NPC:
	case ITEM_CORPSE_PC:
	    if ( IS_SET(obj->value[1], CONT_CLOSED) )
	    {
		printf_to_char(ch, "It is closed.\n\r");
		break;
	    }

	    act( "$p holds:", ch, obj, NULL, TO_CHAR );
	    show_list_to_char( obj->contains, ch, TRUE, TRUE );
	    break;
	}
	return;
    }

    if ( ( victim = get_char_room( ch, arg1 ) ) != NULL )
    {
	show_char_to_char_1( victim, ch );
	return;
    }

    for ( obj = ch->carrying; obj != NULL; obj = obj->next_content )
    {
	if ( can_see_obj( ch, obj ) )
	{  /* player can see object */
	    pdesc = get_extra_descr( arg3, obj->extra_descr );
	    if (pdesc != NULL) {
            if (++count == number) {
                printf_to_char(ch, pdesc);
                return;
            } else {
                continue;
            }
        }
 	    pdesc = get_extra_descr( arg3, obj->pIndexData->extra_descr );
        if (pdesc != NULL) {
            if (++count == number) {
                printf_to_char(ch, pdesc);
                return;
            } else {
                continue;
            }
        }

	    if ( is_name( arg3, obj->name ) )
        {
	    	if (++count == number)
	    	{
	    	    printf_to_char(ch, obj->description);
	    	    printf_to_char(ch, "\n\r");
		        return;
		    }
        }
	  }
    }

    for ( obj = ch->in_room->contents; obj != NULL; obj = obj->next_content )
    {
	if ( can_see_obj( ch, obj ) )
	{
	    pdesc = get_extra_descr( arg3, obj->extra_descr );
	    if ( pdesc != NULL )
	    	if (++count == number)
	    	{
		    printf_to_char(ch, pdesc);
		    return;
	    	}

	    pdesc = get_extra_descr( arg3, obj->pIndexData->extra_descr );
	    if ( pdesc != NULL )
	    	if (++count == number)
	    	{
		    printf_to_char(ch, pdesc);
		    return;
	    	}

	    if ( is_name( arg3, obj->name ) )
		if (++count == number)
		{
		    printf_to_char(ch, obj->description);
		    printf_to_char(ch, "\n\r");
		    return;
		}
	}
    }

    pdesc = get_extra_descr(arg3,ch->in_room->extra_descr);
    if (pdesc != NULL)
    {
	if (++count == number)
	{
	    printf_to_char(ch, pdesc);
	    return;
	}
    }
    
    if (count > 0 && count != number)
    {
    	if (count == 1)
    	    sprintf(buf,"You only see one %s here.\n\r",arg3);
    	else
    	    sprintf(buf,"You only see %d of those here.\n\r",count);
    	
    	printf_to_char(ch, buf);
    	return;
    }

         if ( !str_cmp( arg1, "n" ) || !str_cmp( arg1, "north" ) ) door = 0;
    else if ( !str_cmp( arg1, "e" ) || !str_cmp( arg1, "east"  ) ) door = 1;
    else if ( !str_cmp( arg1, "s" ) || !str_cmp( arg1, "south" ) ) door = 2;
    else if ( !str_cmp( arg1, "w" ) || !str_cmp( arg1, "west"  ) ) door = 3;
    else if ( !str_cmp( arg1, "u" ) || !str_cmp( arg1, "up"    ) ) door = 4;
    else if ( !str_cmp( arg1, "d" ) || !str_cmp( arg1, "down"  ) ) door = 5;
    else
    {
	printf_to_char(ch, "You do not see that here.\n\r");
	return;
    }

    /* 'look direction' */
    if ( ( pexit = ch->in_room->exit[door] ) == NULL )
    {
	printf_to_char(ch, "Nothing special there.\n\r");
	return;
    }

    if ( pexit->description != NULL && pexit->description[0] != '\0' )
	printf_to_char(ch, pexit->description);
    else
	printf_to_char(ch, "Nothing special there.\n\r");

    if ( pexit->keyword    != NULL
    &&   pexit->keyword[0] != '\0'
    &&   pexit->keyword[0] != ' ' )
    {
	if ( IS_SET(pexit->exit_info, EX_CLOSED) )
	{
	    act( "The $d is closed.", ch, NULL, pexit->keyword, TO_CHAR );
	}
	else if ( IS_SET(pexit->exit_info, EX_ISDOOR) )
	{
	    act( "The $d is open.",   ch, NULL, pexit->keyword, TO_CHAR );
	}
    }

    return;
}

/* RT added back for the hell of it */
void do_read (CHAR_DATA *ch, char *argument )
{
    do_function(ch, &do_look, argument);
}

void do_examine( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	printf_to_char(ch, "Examine what?\n\r");
	return;
    }

    do_function(ch, &do_look, arg );

    if ( ( obj = get_obj_here( ch, arg ) ) != NULL )
    {
	switch ( obj->item_type )
	{
	default:
	    break;
	
	case ITEM_JUKEBOX:
	    do_function(ch, &do_play, "list");
	    break;

	case ITEM_MONEY:
	    if (obj->value[0] == 0)
	    {
	        if (obj->value[1] == 0)
		    sprintf(buf,"Odd...there's no coins in the pile.\n\r");
		else if (obj->value[1] == 1)
		    sprintf(buf,"Wow. One gold coin.\n\r");
		else
		    sprintf(buf,"There are %d gold coins in the pile.\n\r",
			obj->value[1]);
	    }
	    else if (obj->value[1] == 0)
	    {
		if (obj->value[0] == 1)
		    sprintf(buf,"Wow. One silver coin.\n\r");
		else
		    sprintf(buf,"There are %d silver coins in the pile.\n\r",
			obj->value[0]);
	    }
	    else
		sprintf(buf,
		    "There are %d gold and %d silver coins in the pile.\n\r",
		    obj->value[1],obj->value[0]);
	    printf_to_char(ch, buf);
	    break;

	case ITEM_DRINK_CON:
	case ITEM_CONTAINER:
	case ITEM_CORPSE_NPC:
	case ITEM_CORPSE_PC:
	    sprintf(buf,"in %s",argument);
	    do_function(ch, &do_look, buf );
	}
    }

    return;
}



/*
 * Thanks to Zrin for auto-exit part.
 */
void do_exits( CHAR_DATA *ch, char *argument )
{
    extern char * const dir_name[];
    char buf[MAX_STRING_LENGTH];
    EXIT_DATA *pexit;
    bool found;
    bool fAuto;
    int door;

    fAuto  = !str_cmp( argument, "auto" );

    if ( !check_blind( ch ) )
	return;

    if (fAuto)
	sprintf(buf,"[Exits:");
    else if (IS_IMMORTAL(ch))
	sprintf(buf,"Obvious exits from room %d:\n\r",ch->in_room->vnum);
    else
	sprintf(buf,"Obvious exits:\n\r");

    found = FALSE;
    for ( door = 0; door <= 5; door++ )
    {
	if ( ( pexit = ch->in_room->exit[door] ) != NULL
	&&   pexit->u1.to_room != NULL
	&&   can_see_room(ch,pexit->u1.to_room) 
	&&   !IS_SET(pexit->exit_info, EX_CLOSED) )
	{
	    found = TRUE;
	    if ( fAuto )
	    {
		strcat( buf, " " );
		strcat( buf, dir_name[door] );
	    }
	    else
	    {
		sprintf( buf + strlen(buf), "%-5s - %s",
		    capitalize( dir_name[door] ),
		    room_is_dark( pexit->u1.to_room )
			?  "Too dark to tell"
			: pexit->u1.to_room->name
		    );
		if (IS_IMMORTAL(ch))
		    sprintf(buf + strlen(buf), 
			" (room %d)\n\r",pexit->u1.to_room->vnum);
		else
		    sprintf(buf + strlen(buf), "\n\r");
	    }
	}
    }

    if ( !found )
	strcat( buf, fAuto ? " none" : "None.\n\r" );

    if ( fAuto )
	strcat( buf, "]\n\r" );

    printf_to_char(ch, "%s%s%s", CLR_ROOM_EXITS, buf, CLR_RESET);
    return;
}

void do_worth( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];

    if (IS_NPC(ch))
    {
	sprintf(buf,"You have %ld gold and %ld silver.\n\r",
	    ch->gold,ch->silver);
	printf_to_char(ch, buf);
	return;
    }

    sprintf(buf, 
    "You have %ld gold, %ld silver, and %d experience (%d exp to level).\n\r",
	ch->gold, ch->silver,ch->exp,
	(ch->level + 1) * exp_per_level(ch,ch->pcdata->points) - ch->exp);

    printf_to_char(ch, buf);

    return;
}


void do_score( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    int i;

    sprintf( buf,
	"You are %s%s, level %d, %d years old (%d hours).\n\r",
	ch->name,
	IS_NPC(ch) ? "" : ch->pcdata->title,
	ch->level, get_age(ch),
        ( ch->played + (int) (current_time - ch->logon) ) / 3600);
    printf_to_char(ch, buf);

    if ( get_trust( ch ) != ch->level )
    {
	sprintf( buf, "You are trusted at level %d.\n\r",
	    get_trust( ch ) );
	printf_to_char(ch, buf);
    }

    sprintf(buf, "Race: %s  Sex: %s\n\r",
	race_table[ch->race].name,
	ch->sex == 0 ? "sexless" : ch->sex == 1 ? "male" : "female");
    printf_to_char(ch, buf);
	

    sprintf( buf,
	"You have %d/%d hit, %d/%d mana, %d/%d movement.\n\r",
	ch->hit,  ch->max_hit,
	ch->mana, ch->max_mana,
	ch->move, ch->max_move);
    printf_to_char(ch, buf);

    sprintf( buf,
	"You have %d practices and %d training sessions.\n\r",
	ch->practice, ch->train);
    printf_to_char(ch, buf);

    sprintf( buf,
	"You are carrying %d/%d items with weight %ld/%d pounds.\n\r",
	ch->carry_number, can_carry_n(ch),
	get_carry_weight(ch) / 10, can_carry_w(ch) /10 );
    printf_to_char(ch, buf);

    sprintf( buf,
	"Str: %d(%d)  Int: %d(%d)  Wis: %d(%d)  Dex: %d(%d)  Con: %d(%d)\n\r",
	ch->perm_stat[STAT_STR],
	get_curr_stat(ch,STAT_STR),
	ch->perm_stat[STAT_INT],
	get_curr_stat(ch,STAT_INT),
	ch->perm_stat[STAT_WIS],
	get_curr_stat(ch,STAT_WIS),
	ch->perm_stat[STAT_DEX],
	get_curr_stat(ch,STAT_DEX),
	ch->perm_stat[STAT_CON],
	get_curr_stat(ch,STAT_CON) );
    printf_to_char(ch, buf);

    sprintf( buf,
	"You have scored %d exp, and have %ld gold and %ld silver coins.\n\r",
	ch->exp,  ch->gold, ch->silver );
    printf_to_char(ch, buf);

    /* RT shows exp to level */
    if (!IS_NPC(ch) && ch->level < LEVEL_HERO)
    {
      sprintf (buf, 
	"You need %d exp to level.\n\r",
	((ch->level + 1) * exp_per_level(ch,ch->pcdata->points) - ch->exp));
      printf_to_char(ch, buf);
     }

    sprintf( buf, "Wimpy set to %d hit points.\n\r", ch->wimpy );
    printf_to_char(ch, buf);

    if ( !IS_NPC(ch) && ch->pcdata->condition[COND_DRUNK]   > 10 )
	printf_to_char(ch, "You are drunk.\n\r");
    if ( !IS_NPC(ch) && ch->pcdata->condition[COND_THIRST] ==  0 )
	printf_to_char(ch, "You are thirsty.\n\r");
    if ( !IS_NPC(ch) && ch->pcdata->condition[COND_HUNGER]   ==  0 )
	printf_to_char(ch, "You are hungry.\n\r");

    switch ( ch->position )
    {
    case POS_DEAD:     
	printf_to_char(ch, "You are DEAD!!\n\r");
	break;
    case POS_MORTAL:
	printf_to_char(ch, "You are mortally wounded.\n\r");
	break;
    case POS_INCAP:
	printf_to_char(ch, "You are incapacitated.\n\r");
	break;
    case POS_STUNNED:
	printf_to_char(ch, "You are stunned.\n\r");
	break;
    case POS_SLEEPING:
	printf_to_char(ch, "You are sleeping.\n\r");
	break;
    case POS_RESTING:
	printf_to_char(ch, "You are resting.\n\r");
	break;
    case POS_SITTING:
	printf_to_char(ch, "You are sitting.\n\r");
	break;
    case POS_STANDING:
	printf_to_char(ch, "You are standing.\n\r");
	break;
    case POS_FIGHTING:
	printf_to_char(ch, "You are fighting.\n\r");
	break;
    }


    /* print AC values */
    if (ch->level >= 25)
    {	
	sprintf( buf,"Armor: pierce: %d  bash: %d  slash: %d  magic: %d\n\r",
		 GET_AC(ch,AC_PIERCE),
		 GET_AC(ch,AC_BASH),
		 GET_AC(ch,AC_SLASH),
		 GET_AC(ch,AC_EXOTIC));
    	printf_to_char(ch, buf);
    }

    for (i = 0; i < 4; i++)
    {
	char * temp;

	switch(i)
	{
	    case(AC_PIERCE):	temp = "piercing";	break;
	    case(AC_BASH):	temp = "bashing";	break;
	    case(AC_SLASH):	temp = "slashing";	break;
	    case(AC_EXOTIC):	temp = "magic";		break;
	    default:		temp = "error";		break;
	}
	
	printf_to_char(ch, "You are ");

	if      (GET_AC(ch,i) >=  101 ) 
	    sprintf(buf,"hopelessly vulnerable to %s.\n\r",temp);
	else if (GET_AC(ch,i) >= 80) 
	    sprintf(buf,"defenseless against %s.\n\r",temp);
	else if (GET_AC(ch,i) >= 60)
	    sprintf(buf,"barely protected from %s.\n\r",temp);
	else if (GET_AC(ch,i) >= 40)
	    sprintf(buf,"slightly armored against %s.\n\r",temp);
	else if (GET_AC(ch,i) >= 20)
	    sprintf(buf,"somewhat armored against %s.\n\r",temp);
	else if (GET_AC(ch,i) >= 0)
	    sprintf(buf,"armored against %s.\n\r",temp);
	else if (GET_AC(ch,i) >= -20)
	    sprintf(buf,"well-armored against %s.\n\r",temp);
	else if (GET_AC(ch,i) >= -40)
	    sprintf(buf,"very well-armored against %s.\n\r",temp);
	else if (GET_AC(ch,i) >= -60)
	    sprintf(buf,"heavily armored against %s.\n\r",temp);
	else if (GET_AC(ch,i) >= -80)
	    sprintf(buf,"superbly armored against %s.\n\r",temp);
	else if (GET_AC(ch,i) >= -100)
	    sprintf(buf,"almost invulnerable to %s.\n\r",temp);
	else
	    sprintf(buf,"divinely armored against %s.\n\r",temp);

	printf_to_char(ch, buf);
    }


    /* RT wizinvis and holy light */
    if ( IS_IMMORTAL(ch))
    {
      printf_to_char(ch, "Holy Light: ");
      if (IS_SET(ch->act,PLR_HOLYLIGHT))
        printf_to_char(ch, "on");
      else
        printf_to_char(ch, "off");
 
      if (ch->invis_level)
      {
        sprintf( buf, "  Invisible: level %d",ch->invis_level);
        printf_to_char(ch, buf);
      }

      if (ch->incog_level)
      {
	sprintf(buf,"  Incognito: level %d",ch->incog_level);
	printf_to_char(ch, buf);
      }
      printf_to_char(ch, "\n\r");
    }

    if ( ch->level >= 15 )
    {
	sprintf( buf, "Hitroll: %d  Damroll: %d.\n\r",
	    GET_HITROLL(ch), GET_DAMROLL(ch) );
	printf_to_char(ch, buf);
    }
    
    if ( ch->level >= 10 )
    {
	sprintf( buf, "Alignment: %d.  ", ch->alignment );
	printf_to_char(ch, buf);
    }

    printf_to_char(ch, "You are ");
         if ( ch->alignment >  900 ) printf_to_char(ch, "angelic.\n\r");
    else if ( ch->alignment >  700 ) printf_to_char(ch, "saintly.\n\r");
    else if ( ch->alignment >  350 ) printf_to_char(ch, "good.\n\r");
    else if ( ch->alignment >  100 ) printf_to_char(ch, "kind.\n\r");
    else if ( ch->alignment > -100 ) printf_to_char(ch, "neutral.\n\r");
    else if ( ch->alignment > -350 ) printf_to_char(ch, "mean.\n\r");
    else if ( ch->alignment > -700 ) printf_to_char(ch, "evil.\n\r");
    else if ( ch->alignment > -900 ) printf_to_char(ch, "demonic.\n\r");
    else                             printf_to_char(ch, "satanic.\n\r");

    if (IS_SET(ch->comm,COMM_SHOW_AFFECTS))
	do_function(ch, &do_affects, "");
}

void do_affects(CHAR_DATA *ch, char *argument )
{
    AFFECT_DATA *paf, *paf_last = NULL;
    char buf[MAX_STRING_LENGTH];
    
    if ( ch->affected != NULL )
    {
	printf_to_char(ch, "You are affected by the following spells:\n\r");
	for ( paf = ch->affected; paf != NULL; paf = paf->next )
	{
	    if (paf_last != NULL && paf->type == paf_last->type)
		if (ch->level >= 20)
		    sprintf( buf, "                      ");
		else
		    continue;
	    else
	    	sprintf( buf, "Spell: %-15s", skill_table[paf->type].name );

	    printf_to_char(ch, buf);

	    if ( ch->level >= 20 )
	    {
		sprintf( buf,
		    ": modifies %s by %d ",
		    affect_loc_name( paf->location ),
		    paf->modifier);
		printf_to_char(ch, buf);
		if ( paf->duration == -1 )
		    sprintf( buf, "permanently" );
		else
		    sprintf( buf, "for %d hours", paf->duration );
		printf_to_char(ch, buf);
	    }

	    printf_to_char(ch, "\n\r");
	    paf_last = paf;
	}
    }
    else 
	printf_to_char(ch, "You are not affected by any spells.\n\r");

    return;
}



char *	const	day_name	[] =
{
    "the Moon", "the Bull", "Deception", "Thunder", "Freedom",
    "the Great Gods", "the Sun"
};

char *	const	month_name	[] =
{
    "Winter", "the Winter Wolf", "the Frost Giant", "the Old Forces",
    "the Grand Struggle", "the Spring", "Nature", "Futility", "the Dragon",
    "the Sun", "the Heat", "the Battle", "the Dark Shades", "the Shadows",
    "the Long Shadows", "the Ancient Darkness", "the Great Evil"
};

void do_time( CHAR_DATA *ch, char *argument )
{
    extern char str_boot_time[];
    char buf[MAX_STRING_LENGTH];
    char *suf;
    int day;

    day     = time_info.day + 1;

         if ( day > 4 && day <  20 ) suf = "th";
    else if ( day % 10 ==  1       ) suf = "st";
    else if ( day % 10 ==  2       ) suf = "nd";
    else if ( day % 10 ==  3       ) suf = "rd";
    else                             suf = "th";

    sprintf( buf,
	"It is %d o'clock %s, Day of %s, %d%s the Month of %s.\n\r",
	(time_info.hour % 12 == 0) ? 12 : time_info.hour %12,
	time_info.hour >= 12 ? "pm" : "am",
	day_name[day % 7],
	day, suf,
	month_name[time_info.month]);
    printf_to_char(ch, buf);
    sprintf(buf,"ROM started up at %s\n\rThe system time is %s.\n\r",
	str_boot_time,
	(char *) ctime( &current_time )
	);

    printf_to_char(ch, buf);
    return;
}



void do_weather( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];

    static char * const sky_look[4] =
    {
	"cloudless",
	"cloudy",
	"rainy",
	"lit by flashes of lightning"
    };

    if ( !IS_OUTSIDE(ch) )
    {
	printf_to_char(ch, "You can't see the weather indoors.\n\r");
	return;
    }

    sprintf( buf, "The sky is %s and %s.\n\r",
	sky_look[weather_info.sky],
	weather_info.change >= 0
	? "a warm southerly breeze blows"
	: "a cold northern gust blows"
	);
    printf_to_char(ch, buf);
    return;
}

void do_help( CHAR_DATA *ch, char *argument )
{
    HELP_DATA *pHelp;
    BUFFER *output;
    bool found = FALSE;
    char argall[MAX_INPUT_LENGTH],argone[MAX_INPUT_LENGTH];
    int level;

    output = new_buf();

    if ( argument[0] == '\0' )
	argument = "summary";

    /* this parts handles help a b so that it returns help 'a b' */
    argall[0] = '\0';
    while (argument[0] != '\0' )
    {
	argument = one_argument(argument,argone);
	if (argall[0] != '\0')
	    strcat(argall," ");
	strcat(argall,argone);
    }

    for ( pHelp = help_first; pHelp != NULL; pHelp = pHelp->next )
    {
    	level = (pHelp->level < 0) ? -1 * pHelp->level - 1 : pHelp->level;

	if (level > get_trust( ch ) )
	    continue;

	if ( is_name( argall, pHelp->keyword ) )
	{
	    /* add seperator if found */
	    if (found)
		add_buf(output,
    "\n\r============================================================\n\r\n\r");
	    if ( pHelp->level >= 0 && str_cmp( argall, "imotd" ) )
	    {
		add_buf(output,pHelp->keyword);
		add_buf(output,"\n\r");
	    }

	    /*
	     * Strip leading '.' to allow initial blanks.
	     */
	    if ( pHelp->text[0] == '.' )
		add_buf(output,pHelp->text+1);
	    else
		add_buf(output,pHelp->text);
	    found = TRUE;
	    /* small hack :) */
	    if (ch->desc != NULL && ch->desc->connected != CON_PLAYING 
	    &&  		    ch->desc->connected != CON_GEN_GROUPS)
		break;
	}
    }

    if (!found)
    	printf_to_char(ch, "No help on that word.\n\r");
    else
	page_to_char(buf_string(output),ch);
    free_buf(output);
}


/* whois command */
void do_whois (CHAR_DATA *ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    BUFFER *output;
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
    bool found = FALSE;

    one_argument(argument,arg);
  
    if (arg[0] == '\0')
    {
	printf_to_char(ch, "You must provide a name.\n\r");
	return;
    }

    output = new_buf();

    for (d = descriptor_list; d != NULL; d = d->next)
    {
	CHAR_DATA *wch;

 	if (d->connected != CON_PLAYING || !can_see(ch,d->character))
	    continue;
	
	wch = ( d->original != NULL ) ? d->original : d->character;

 	if (!can_see(ch,wch))
	    continue;

	if (!str_prefix(arg,wch->name))
	{
	    found = TRUE;
    
	    /* a little formatting */
	    sprintf(buf, "[%2d %6s] %s%s%s%s%s%s%s%s\n\r",
		wch->level,
		wch->race < MAX_PC_RACE ? pc_race_table[wch->race].who_name : "     ",
	     wch->incog_level >= LEVEL_HERO ? "(Incog) ": "",
 	     wch->invis_level >= LEVEL_HERO ? "(Wizi) " : "",
	     clan_table[wch->clan].who_name,
	     IS_SET(wch->comm, COMM_AFK) ? "[AFK] " : "",
             IS_SET(wch->act,PLR_KILLER) ? "(KILLER) " : "",
             IS_SET(wch->act,PLR_THIEF) ? "(THIEF) " : "",
		wch->name, IS_NPC(wch) ? "" : wch->pcdata->title);
	    add_buf(output,buf);
	}
    }

    if (!found)
    {
	printf_to_char(ch, "No one of that name is playing.\n\r");
	return;
    }

    page_to_char(buf_string(output),ch);
    free_buf(output);
}


/*
 * New 'who' command originally by Alander of Rivers of Mud.
 */
void do_who( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    BUFFER *output;
    DESCRIPTOR_DATA *d; 
    int nMatch;
 
    /*
     * Now show matching chars.
     */
    nMatch = 0;
    buf[0] = '\0';
    output = new_buf();
    for ( d = descriptor_list; d != NULL; d = d->next )
    {
        CHAR_DATA *wch;
 
        /*
         * Check for match against restrictions.
         * Don't use trust as that exposes trusted mortals.
         */
        if ( d->connected != CON_PLAYING || !can_see( ch, d->character ) )
            continue;
 
        wch   = ( d->original != NULL ) ? d->original : d->character;

	if (!can_see(ch,wch))
	    continue;
 
        nMatch++;

	/*
	 * Format it up.
	 */
	sprintf( buf, "[%s%2d%s %6s] %s%s%s%s%s%s%s%s\n\r",
	    CLR_DARK_GOLDEN_ROD,
        wch->level,
        CLR_RESET,
	    wch->race < MAX_PC_RACE ? pc_race_table[wch->race].who_name : "     ",
	    wch->incog_level >= LEVEL_HERO ? "(Incog) " : "",
	    wch->invis_level >= LEVEL_HERO ? "(Wizi) " : "",
	    clan_table[wch->clan].who_name,
	    IS_SET(wch->comm, COMM_AFK) ? "[AFK] " : "",
            IS_SET(wch->act, PLR_KILLER) ? "(KILLER) " : "",
            IS_SET(wch->act, PLR_THIEF)  ? "(THIEF) "  : "",
	    wch->name,
	    IS_NPC(wch) ? "" : wch->pcdata->title );
	add_buf(output,buf);
    }

    sprintf( buf2, "\n\rPlayers found: %d\n\r", nMatch );
    add_buf(output,buf2);
    page_to_char( buf_string(output), ch );
    free_buf(output);
    return;
}

void do_count ( CHAR_DATA *ch, char *argument )
{
    int count;
    DESCRIPTOR_DATA *d;
    char buf[MAX_STRING_LENGTH];

    count = 0;

    for ( d = descriptor_list; d != NULL; d = d->next )
        if ( d->connected == CON_PLAYING && can_see( ch, d->character ) )
	    count++;

    max_on = UMAX(count,max_on);

    if (max_on == count)
        sprintf(buf,"There are %d characters on, the most so far today.\n\r",
	    count);
    else
	sprintf(buf,"There are %d characters on, the most on today was %d.\n\r",
	    count,max_on);

    printf_to_char(ch, buf);
}

void do_inventory( CHAR_DATA *ch, char *argument )
{
    printf_to_char(ch, "You are carrying:\n\r");
    show_list_to_char( ch->carrying, ch, TRUE, TRUE );
    return;
}



void do_equipment( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    int iWear;
    bool found;

    printf_to_char(ch, "You are using:\n\r");
    found = FALSE;
    for ( iWear = 0; iWear < MAX_WEAR; iWear++ )
    {
	if ( ( obj = get_eq_char( ch, iWear ) ) == NULL )
	    continue;

	printf_to_char(ch, where_name[iWear]);
	if ( can_see_obj( ch, obj ) )
	{
	    printf_to_char(ch, format_obj_to_char( obj, ch, TRUE ));
	    printf_to_char(ch, "\n\r");
	}
	else
	{
	    printf_to_char(ch, "something.\n\r");
	}
	found = TRUE;
    }

    if ( !found )
	printf_to_char(ch, "Nothing.\n\r");

    return;
}



void do_compare( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    OBJ_DATA *obj1;
    OBJ_DATA *obj2;
    int value1;
    int value2;
    char *msg;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    if ( arg1[0] == '\0' )
    {
	printf_to_char(ch, "Compare what to what?\n\r");
	return;
    }

    if ( ( obj1 = get_obj_carry( ch, arg1, ch ) ) == NULL )
    {
	printf_to_char(ch, "You do not have that item.\n\r");
	return;
    }

    if (arg2[0] == '\0')
    {
	for (obj2 = ch->carrying; obj2 != NULL; obj2 = obj2->next_content)
	{
	    if (obj2->wear_loc != WEAR_NONE
	    &&  can_see_obj(ch,obj2)
	    &&  obj1->item_type == obj2->item_type
	    &&  (obj1->wear_flags & obj2->wear_flags & ~ITEM_TAKE) != 0 )
		break;
	}

	if (obj2 == NULL)
	{
	    printf_to_char(ch, "You aren't wearing anything comparable.\n\r");
	    return;
	}
    } 

    else if ( (obj2 = get_obj_carry(ch,arg2,ch) ) == NULL )
    {
	printf_to_char(ch, "You do not have that item.\n\r");
	return;
    }

    msg		= NULL;
    value1	= 0;
    value2	= 0;

    if ( obj1 == obj2 )
    {
	msg = "You compare $p to itself.  It looks about the same.";
    }
    else if ( obj1->item_type != obj2->item_type )
    {
	msg = "You can't compare $p and $P.";
    }
    else
    {
	switch ( obj1->item_type )
	{
	default:
	    msg = "You can't compare $p and $P.";
	    break;

	case ITEM_ARMOR:
	    value1 = obj1->value[0] + obj1->value[1] + obj1->value[2];
	    value2 = obj2->value[0] + obj2->value[1] + obj2->value[2];
	    break;

	case ITEM_WEAPON:
	    if (obj1->pIndexData->new_format)
		value1 = (1 + obj1->value[2]) * obj1->value[1];
	    else
	    	value1 = obj1->value[1] + obj1->value[2];

	    if (obj2->pIndexData->new_format)
		value2 = (1 + obj2->value[2]) * obj2->value[1];
	    else
	    	value2 = obj2->value[1] + obj2->value[2];
	    break;
	}
    }

    if ( msg == NULL )
    {
	     if ( value1 == value2 ) msg = "$p and $P look about the same.";
	else if ( value1  > value2 ) msg = "$p looks better than $P.";
	else                         msg = "$p looks worse than $P.";
    }

    act( msg, ch, obj1, obj2, TO_CHAR );
    return;
}



void do_credits( CHAR_DATA *ch, char *argument )
{
    do_function(ch, &do_help, "diku" );
    return;
}



void do_where( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    DESCRIPTOR_DATA *d;
    bool found;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	printf_to_char(ch, "Players near you:\n\r");
	found = FALSE;
	for ( d = descriptor_list; d; d = d->next )
	{
	    if ( d->connected == CON_PLAYING
	    && ( victim = d->character ) != NULL
	    &&   !IS_NPC(victim)
	    &&   victim->in_room != NULL
	    &&   !IS_SET(victim->in_room->room_flags,ROOM_NOWHERE)
 	    &&   (is_room_owner(ch,victim->in_room) 
	    ||    !room_is_private(victim->in_room))
	    &&   victim->in_room->area == ch->in_room->area
	    &&   can_see( ch, victim ) )
	    {
		found = TRUE;
		sprintf( buf, "%-28s %s\n\r",
		    victim->name, victim->in_room->name );
		printf_to_char(ch, buf);
	    }
	}
	if ( !found )
	    printf_to_char(ch, "None\n\r");
    }
    else
    {
	found = FALSE;
	for ( victim = char_list; victim != NULL; victim = victim->next )
	{
	    if ( victim->in_room != NULL
	    &&   victim->in_room->area == ch->in_room->area
	    &&   !IS_AFFECTED(victim, AFF_HIDE)
	    &&   !IS_AFFECTED(victim, AFF_SNEAK)
	    &&   can_see( ch, victim )
	    &&   is_name( arg, victim->name ) )
	    {
		found = TRUE;
		sprintf( buf, "%-28s %s\n\r",
		    PERS(victim, ch), victim->in_room->name );
		printf_to_char(ch, buf);
		break;
	    }
	}
	if ( !found )
	    act( "You didn't find any $T.", ch, NULL, arg, TO_CHAR );
    }

    return;
}




void do_consider( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    char *msg;
    int diff;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	printf_to_char(ch, "Consider killing whom?\n\r");
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	printf_to_char(ch, "They're not here.\n\r");
	return;
    }

    if (is_safe(ch,victim))
    {
	printf_to_char(ch, "Don't even think about it.\n\r");
	return;
    }

    diff = victim->level - ch->level;

         if ( diff <= -10 ) msg = "You can kill $N naked and weaponless.";
    else if ( diff <=  -5 ) msg = "$N is no match for you.";
    else if ( diff <=  -2 ) msg = "$N looks like an easy kill.";
    else if ( diff <=   1 ) msg = "The perfect match!";
    else if ( diff <=   4 ) msg = "$N says 'Do you feel lucky, punk?'.";
    else if ( diff <=   9 ) msg = "$N laughs at you mercilessly.";
    else                    msg = "Death will thank you for your gift.";

    act( msg, ch, NULL, victim, TO_CHAR );
    return;
}



void set_title( CHAR_DATA *ch, char *title )
{
    char buf[MAX_STRING_LENGTH];

    if ( IS_NPC(ch) )
    {
	bugf("Set_title: NPC.");
	return;
    }

    if ( title[0] != '.' && title[0] != ',' && title[0] != '!' && title[0] != '?' )
    {
	buf[0] = ' ';
	strcpy( buf+1, title );
    }
    else
    {
	strcpy( buf, title );
    }

    free_string( ch->pcdata->title );
    ch->pcdata->title = str_dup( buf );
    return;
}



void do_title( CHAR_DATA *ch, char *argument )
{
    if ( IS_NPC(ch) )
	return;

    if ( argument[0] == '\0' )
    {
	printf_to_char(ch, "Change your title to what?\n\r");
	return;
    }

    if ( strlen(argument) > 45 )
	argument[45] = '\0';

    smash_tilde( argument );
    set_title( ch, argument );
    printf_to_char(ch, "Ok.\n\r");
}



void do_description( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];

    if ( argument[0] != '\0' )
    {
	buf[0] = '\0';
	smash_tilde( argument );

    	if (argument[0] == '-')
    	{
            int len;
            bool found = FALSE;
 
            if (ch->description == NULL || ch->description[0] == '\0')
            {
                printf_to_char(ch, "No lines left to remove.\n\r");
                return;
            }
	
  	    strcpy(buf,ch->description);
 
            for (len = strlen(buf); len > 0; len--)
            {
                if (buf[len] == '\r')
                {
                    if (!found)  /* back it up */
                    {
                        if (len > 0)
                            len--;
                        found = TRUE;
                    }
                    else /* found the second one */
                    {
                        buf[len + 1] = '\0';
			free_string(ch->description);
			ch->description = str_dup(buf);
			printf_to_char(ch, "Your description is:\n\r");
			printf_to_char(ch, ch->description ? ch->description : "(None).\n\r");
            return;
                    }
                }
            }
            buf[0] = '\0';
	    free_string(ch->description);
	    ch->description = str_dup(buf);
	    printf_to_char(ch, "Description cleared.\n\r");
	    return;
        }
	if ( argument[0] == '+' )
	{
	    if ( ch->description != NULL )
		strcat( buf, ch->description );
	    argument++;
	    while ( isspace(*argument) )
		argument++;
	}

        if ( strlen(buf) >= 1024)
	{
	    printf_to_char(ch, "Description too long.\n\r");
	    return;
	}

	strcat( buf, argument );
	strcat( buf, "\n\r" );
	free_string( ch->description );
	ch->description = str_dup( buf );
    }

    printf_to_char(ch, "Your description is:\n\r");
    printf_to_char(ch, ch->description ? ch->description : "(None).\n\r");
    return;
}



void do_report( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_INPUT_LENGTH];

    sprintf( buf,
	"You say 'I have %d/%d hp %d/%d mana %d/%d mv %d xp.'\n\r",
	ch->hit,  ch->max_hit,
	ch->mana, ch->max_mana,
	ch->move, ch->max_move,
	ch->exp   );

    printf_to_char(ch, buf);

    sprintf( buf, "$n says 'I have %d/%d hp %d/%d mana %d/%d mv %d xp.'",
	ch->hit,  ch->max_hit,
	ch->mana, ch->max_mana,
	ch->move, ch->max_move,
	ch->exp   );

    act( buf, ch, NULL, NULL, TO_ROOM );

    return;
}



void do_practice( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    int sn;

    if ( IS_NPC(ch) )
	return;

    if ( argument[0] == '\0' )
    {
	int col;

	col    = 0;
	for ( sn = 0; sn < MAX_SKILL; sn++ )
	{
	    if ( skill_table[sn].name == NULL )
		break;
	    if ( ch->level < skill_table[sn].skill_level[ch->class] 
	      || ch->pcdata->learned[sn] < 1 /* skill is not known */)
		continue;

	    sprintf( buf, "%-18s %3d%%  ",
		skill_table[sn].name, ch->pcdata->learned[sn] );
	    printf_to_char(ch, buf);
	    if ( ++col % 3 == 0 )
		printf_to_char(ch, "\n\r");
	}

	if ( col % 3 != 0 )
	    printf_to_char(ch, "\n\r");

	sprintf( buf, "You have %d practice sessions left.\n\r",
	    ch->practice );
	printf_to_char(ch, buf);
    }
    else
    {
	CHAR_DATA *mob;
	int adept;

	if ( !IS_AWAKE(ch) )
	{
	    printf_to_char(ch, "In your dreams, or what?\n\r");
	    return;
	}

	for ( mob = ch->in_room->people; mob != NULL; mob = mob->next_in_room )
	{
	    if ( IS_NPC(mob) && IS_SET(mob->act, ACT_PRACTICE) )
		break;
	}

	if ( mob == NULL )
	{
	    printf_to_char(ch, "You can't do that here.\n\r");
	    return;
	}

	if ( ch->practice <= 0 )
	{
	    printf_to_char(ch, "You have no practice sessions left.\n\r");
	    return;
	}

	if ( ( sn = find_spell( ch,argument ) ) < 0
	|| ( !IS_NPC(ch)
	&&   (ch->level < skill_table[sn].skill_level[ch->class] 
 	||    ch->pcdata->learned[sn] < 1 /* skill is not known */
	||    skill_table[sn].rating[ch->class] == 0)))
	{
	    printf_to_char(ch, "You can't practice that.\n\r");
	    return;
	}

	adept = IS_NPC(ch) ? 100 : pc_race_table[ch->race].skill_adept;

	if ( ch->pcdata->learned[sn] >= adept )
	{
	    sprintf( buf, "You are already learned at %s.\n\r",
		skill_table[sn].name );
	    printf_to_char(ch, buf);
	}
	else
	{
	    ch->practice--;
	    ch->pcdata->learned[sn] += 
		int_app[get_curr_stat(ch,STAT_INT)].learn / 
	        skill_table[sn].rating[ch->class];
	    if ( ch->pcdata->learned[sn] < adept )
	    {
		act( "You practice $T.",
		    ch, NULL, skill_table[sn].name, TO_CHAR );
		act( "$n practices $T.",
		    ch, NULL, skill_table[sn].name, TO_ROOM );
	    }
	    else
	    {
		ch->pcdata->learned[sn] = adept;
		act( "You are now learned at $T.",
		    ch, NULL, skill_table[sn].name, TO_CHAR );
		act( "$n is now learned at $T.",
		    ch, NULL, skill_table[sn].name, TO_ROOM );
	    }
	}
    }
    return;
}



/*
 * 'Wimpy' originally by Dionysos.
 */
void do_wimpy( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    int wimpy;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
	wimpy = ch->max_hit / 5;
    else
	wimpy = atoi( arg );

    if ( wimpy < 0 )
    {
	printf_to_char(ch, "Your courage exceeds your wisdom.\n\r");
	return;
    }

    if ( wimpy > ch->max_hit/2 )
    {
	printf_to_char(ch, "Such cowardice ill becomes you.\n\r");
	return;
    }

    ch->wimpy	= wimpy;
    sprintf( buf, "Wimpy set to %d hit points.\n\r", wimpy );
    printf_to_char(ch, buf);
    return;
}



void do_password( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char *pArg;
    char *pwdnew;
    char *p;
    char cEnd;

    if ( IS_NPC(ch) )
	return;

    /*
     * Can't use one_argument here because it smashes case.
     * So we just steal all its code.  Bleagh.
     */
    pArg = arg1;
    while ( isspace(*argument) )
	argument++;

    cEnd = ' ';
    if ( *argument == '\'' || *argument == '"' )
	cEnd = *argument++;

    while ( *argument != '\0' )
    {
	if ( *argument == cEnd )
	{
	    argument++;
	    break;
	}
	*pArg++ = *argument++;
    }
    *pArg = '\0';

    pArg = arg2;
    while ( isspace(*argument) )
	argument++;

    cEnd = ' ';
    if ( *argument == '\'' || *argument == '"' )
	cEnd = *argument++;

    while ( *argument != '\0' )
    {
	if ( *argument == cEnd )
	{
	    argument++;
	    break;
	}
	*pArg++ = *argument++;
    }
    *pArg = '\0';

    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
	printf_to_char(ch, "Syntax: password <old> <new>.\n\r");
	return;
    }

    if ( strcmp( crypt( arg1, ch->pcdata->pwd ), ch->pcdata->pwd ) )
    {
	WAIT_STATE( ch, 40 );
	printf_to_char(ch, "Wrong password.  Wait 10 seconds.\n\r");
	return;
    }

    if ( strlen(arg2) < 5 )
    {
	printf_to_char(ch, "New password must be at least five characters long.\n\r");
	return;
    }

    /*
     * No tilde allowed because of player file format.
     */
    pwdnew = crypt( arg2, ch->name );
    for ( p = pwdnew; *p != '\0'; p++ )
    {
	if ( *p == '~' )
	{
	    printf_to_char(ch, "New password not acceptable, try again.\n\r");
	    return;
	}
    }

    free_string( ch->pcdata->pwd );
    ch->pcdata->pwd = str_dup( pwdnew );
    save_char_obj( ch );
    printf_to_char(ch, "Ok.\n\r");
    return;
}
