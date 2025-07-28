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
 **************************************************************************/

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
#include <time.h>
#include "merc.h"
#include "interp.h"
#include "recycle.h"
#include "tables.h"

/* RT code to delete yourself */

void do_delet( CHAR_DATA *ch, char *argument)
{
    printf_to_char(ch, "You must type the full command to delete yourself.\n\r");
}

void do_delete( CHAR_DATA *ch, char *argument)
{
   char strsave[MAX_INPUT_LENGTH];

   if (IS_NPC(ch))
	return;
  
   if (ch->pcdata->confirm_delete)
   {
	if (argument[0] != '\0')
	{
	    printf_to_char(ch, "Delete status removed.\n\r");
	    ch->pcdata->confirm_delete = FALSE;
	    return;
	}
	else
	{
    	    sprintf( strsave, "%s%s", PLAYER_DIR, capitalize( ch->name ) );
	    wiznet("$N turns $Mself into line noise.",ch,NULL,0,0,0);
	    stop_fighting(ch,TRUE);
	    do_function(ch, &do_quit, "");
	    unlink(strsave);
	    return;
 	}
    }

    if (argument[0] != '\0')
    {
	printf_to_char(ch, "Just type delete. No argument.\n\r");
	return;
    }

    printf_to_char(ch, "Type delete again to confirm this command.\n\r");
    printf_to_char(ch, "WARNING: this command is irreversible.\n\r");
    printf_to_char(ch, "Typing delete with an argument will undo delete status.\n\r");
    ch->pcdata->confirm_delete = TRUE;
    wiznet("$N is contemplating deletion.",ch,NULL,0,0,get_trust(ch));
}
	    

/* RT code to display channel status */

void do_channels( CHAR_DATA *ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];

    /* lists all channels and their status */
    printf_to_char(ch, "   channel     status\n\r");
    printf_to_char(ch, "---------------------\n\r");
 
    printf_to_char(ch, "gossip         ");
    if (!IS_SET(ch->comm,COMM_NOGOSSIP))
      printf_to_char(ch, "ON\n\r");
    else
      printf_to_char(ch, "OFF\n\r");

    printf_to_char(ch, "auction        ");
    if (!IS_SET(ch->comm,COMM_NOAUCTION))
      printf_to_char(ch, "ON\n\r");
    else
      printf_to_char(ch, "OFF\n\r");

    printf_to_char(ch, "music          ");
    if (!IS_SET(ch->comm,COMM_NOMUSIC))
      printf_to_char(ch, "ON\n\r");
    else
      printf_to_char(ch, "OFF\n\r");

    printf_to_char(ch, "Q/A            ");
    if (!IS_SET(ch->comm,COMM_NOQUESTION))
      printf_to_char(ch, "ON\n\r");
    else
      printf_to_char(ch, "OFF\n\r");

    printf_to_char(ch, "Quote          ");
    if (!IS_SET(ch->comm,COMM_NOQUOTE))
	printf_to_char(ch, "ON\n\r");
    else
	printf_to_char(ch, "OFF\n\r");

    printf_to_char(ch, "grats          ");
    if (!IS_SET(ch->comm,COMM_NOGRATS))
      printf_to_char(ch, "ON\n\r");
    else
      printf_to_char(ch, "OFF\n\r");

    if (IS_IMMORTAL(ch))
    {
      printf_to_char(ch, "god channel    ");
      if(!IS_SET(ch->comm,COMM_NOWIZ))
        printf_to_char(ch, "ON\n\r");
      else
        printf_to_char(ch, "OFF\n\r");
    }

    printf_to_char(ch, "shouts         ");
    if (!IS_SET(ch->comm,COMM_SHOUTSOFF))
      printf_to_char(ch, "ON\n\r");
    else
      printf_to_char(ch, "OFF\n\r");

    printf_to_char(ch, "tells          ");
    if (!IS_SET(ch->comm,COMM_DEAF))
	printf_to_char(ch, "ON\n\r");
    else
	printf_to_char(ch, "OFF\n\r");

    printf_to_char(ch, "quiet mode     ");
    if (IS_SET(ch->comm,COMM_QUIET))
      printf_to_char(ch, "ON\n\r");
    else
      printf_to_char(ch, "OFF\n\r");

    if (IS_SET(ch->comm,COMM_AFK))
	printf_to_char(ch, "You are AFK.\n\r");

    if (IS_SET(ch->comm,COMM_SNOOP_PROOF))
	printf_to_char(ch, "You are immune to snooping.\n\r");
   
    if (ch->lines != PAGELEN)
    {
	if (ch->lines)
	{
	    sprintf(buf,"You display %d lines of scroll.\n\r",ch->lines+2);
	    printf_to_char(ch, buf);
 	}
	else
	    printf_to_char(ch, "Scroll buffering is off.\n\r");
    }

    if (ch->prompt != NULL)
    {
	sprintf(buf,"Your current prompt is: %s\n\r",ch->prompt);
	printf_to_char(ch, buf);
    }

    if (IS_SET(ch->comm,COMM_NOSHOUT))
      printf_to_char(ch, "You cannot shout.\n\r");
  
    if (IS_SET(ch->comm,COMM_NOTELL))
      printf_to_char(ch, "You cannot use tell.\n\r");
 
    if (IS_SET(ch->comm,COMM_NOCHANNELS))
     printf_to_char(ch, "You cannot use channels.\n\r");

    if (IS_SET(ch->comm,COMM_NOEMOTE))
      printf_to_char(ch, "You cannot show emotions.\n\r");

}

/* RT deaf blocks out all shouts */

void do_deaf( CHAR_DATA *ch, char *argument)
{
    
   if (IS_SET(ch->comm,COMM_DEAF))
   {
     printf_to_char(ch, "You can now hear tells again.\n\r");
     REMOVE_BIT(ch->comm,COMM_DEAF);
   }
   else 
   {
     printf_to_char(ch, "From now on, you won't hear tells.\n\r");
     SET_BIT(ch->comm,COMM_DEAF);
   }
}

/* RT quiet blocks out all communication */

void do_quiet ( CHAR_DATA *ch, char * argument)
{
    if (IS_SET(ch->comm,COMM_QUIET))
    {
      printf_to_char(ch, "Quiet mode removed.\n\r");
      REMOVE_BIT(ch->comm,COMM_QUIET);
    }
   else
   {
     printf_to_char(ch, "From now on, you will only hear says and emotes.\n\r");
     SET_BIT(ch->comm,COMM_QUIET);
   }
}

/* afk command */

void do_afk ( CHAR_DATA *ch, char * argument)
{
    if (IS_SET(ch->comm,COMM_AFK))
    {
      printf_to_char(ch, "AFK mode removed. Type 'replay' to see tells.\n\r");
      REMOVE_BIT(ch->comm,COMM_AFK);
    }
   else
   {
     printf_to_char(ch, "You are now in AFK mode.\n\r");
     SET_BIT(ch->comm,COMM_AFK);
   }
}

void do_replay (CHAR_DATA *ch, char *argument)
{
    if (IS_NPC(ch))
    {
	printf_to_char(ch, "You can't replay.\n\r");
	return;
    }

    if (buf_string(ch->pcdata->buffer)[0] == '\0')
    {
	printf_to_char(ch, "You have no tells to replay.\n\r");
	return;
    }

    page_to_char(buf_string(ch->pcdata->buffer),ch);
    clear_buf(ch->pcdata->buffer);
}

/* RT auction rewritten in ROM style */
void do_auction( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;

    if (argument[0] == '\0' )
    {
      if (IS_SET(ch->comm,COMM_NOAUCTION))
      {
	printf_to_char(ch, "Auction channel is now ON.\n\r");
	REMOVE_BIT(ch->comm,COMM_NOAUCTION);
      }
      else
      {
	printf_to_char(ch, "Auction channel is now OFF.\n\r");
	SET_BIT(ch->comm,COMM_NOAUCTION);
      }
    }
    else  /* auction message sent, turn auction on if it is off */
    {
	if (IS_SET(ch->comm,COMM_QUIET))
	{
	  printf_to_char(ch, "You must turn off quiet mode first.\n\r");
	  return;
	}

	if (IS_SET(ch->comm,COMM_NOCHANNELS))
	{
	  printf_to_char(ch, "The gods have revoked your channel priviliges.\n\r");
	  return;
	}

	REMOVE_BIT(ch->comm,COMM_NOAUCTION);
    }

    sprintf( buf, "You auction '%s'\n\r", argument );
    printf_to_char(ch, buf);
    for ( d = descriptor_list; d != NULL; d = d->next )
    {
	CHAR_DATA *victim;

	victim = d->original ? d->original : d->character;

	if ( d->connected == CON_PLAYING &&
	     d->character != ch &&
	     !IS_SET(victim->comm,COMM_NOAUCTION) &&
	     !IS_SET(victim->comm,COMM_QUIET) )
	{
	    act_new("$n auctions '$t'",
		    ch,argument,d->character,TO_VICT,POS_DEAD);
 	}
    }
}

/* RT chat replaced with ROM gossip */
void do_gossip( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
 
    if (argument[0] == '\0' )
    {
      if (IS_SET(ch->comm,COMM_NOGOSSIP))
      {
        printf_to_char(ch, "Gossip channel is now ON.\n\r");
        REMOVE_BIT(ch->comm,COMM_NOGOSSIP);
      }
      else
      {
        printf_to_char(ch, "Gossip channel is now OFF.\n\r");
        SET_BIT(ch->comm,COMM_NOGOSSIP);
      }
    }
    else  /* gossip message sent, turn gossip on if it isn't already */
    {
        if (IS_SET(ch->comm,COMM_QUIET))
        {
          printf_to_char(ch, "You must turn off quiet mode first.\n\r");
          return;
        }
 
        if (IS_SET(ch->comm,COMM_NOCHANNELS))
        {
          printf_to_char(ch, "The gods have revoked your channel priviliges.\n\r");
          return;
 
       	}

      REMOVE_BIT(ch->comm,COMM_NOGOSSIP);
 
      sprintf( buf, "You gossip '%s'\n\r", argument );
      printf_to_char(ch, buf);
      for ( d = descriptor_list; d != NULL; d = d->next )
      {
        CHAR_DATA *victim;
 
        victim = d->original ? d->original : d->character;
 
        if ( d->connected == CON_PLAYING &&
             d->character != ch &&
             !IS_SET(victim->comm,COMM_NOGOSSIP) &&
             !IS_SET(victim->comm,COMM_QUIET) )
        {
          act_new( "$n gossips '$t'", 
		   ch,argument, d->character, TO_VICT,POS_SLEEPING );
        }
      }
    }
}

void do_grats( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
 
    if (argument[0] == '\0' )
    {
      if (IS_SET(ch->comm,COMM_NOGRATS))
      {
        printf_to_char(ch, "Grats channel is now ON.\n\r");
        REMOVE_BIT(ch->comm,COMM_NOGRATS);
      }
      else
      {
        printf_to_char(ch, "Grats channel is now OFF.\n\r");
        SET_BIT(ch->comm,COMM_NOGRATS);
      }
    }
    else  /* grats message sent, turn grats on if it isn't already */
    {
        if (IS_SET(ch->comm,COMM_QUIET))
        {
          printf_to_char(ch, "You must turn off quiet mode first.\n\r");
          return;
        }
 
        if (IS_SET(ch->comm,COMM_NOCHANNELS))
        {
          printf_to_char(ch, "The gods have revoked your channel priviliges.\n\r");
          return;
 
        }
 
      REMOVE_BIT(ch->comm,COMM_NOGRATS);
 
      sprintf( buf, "You grats '%s'\n\r", argument );
      printf_to_char(ch, buf);
      for ( d = descriptor_list; d != NULL; d = d->next )
      {
        CHAR_DATA *victim;
 
        victim = d->original ? d->original : d->character;
 
        if ( d->connected == CON_PLAYING &&
             d->character != ch &&
             !IS_SET(victim->comm,COMM_NOGRATS) &&
             !IS_SET(victim->comm,COMM_QUIET) )
        {
          act_new( "$n grats '$t'",
                   ch,argument, d->character, TO_VICT,POS_SLEEPING );
        }
      }
    }
}

void do_quote( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
 
    if (argument[0] == '\0' )
    {
      if (IS_SET(ch->comm,COMM_NOQUOTE))
      {
        printf_to_char(ch, "Quote channel is now ON.\n\r");
        REMOVE_BIT(ch->comm,COMM_NOQUOTE);
      }
      else
      {
        printf_to_char(ch, "Quote channel is now OFF.\n\r");
        SET_BIT(ch->comm,COMM_NOQUOTE);
      }
    }
    else  /* quote message sent, turn quote on if it isn't already */
    {
        if (IS_SET(ch->comm,COMM_QUIET))
        {
          printf_to_char(ch, "You must turn off quiet mode first.\n\r");
          return;
        }
 
        if (IS_SET(ch->comm,COMM_NOCHANNELS))
        {
          printf_to_char(ch, "The gods have revoked your channel priviliges.\n\r");
          return;
 
        }
 
      REMOVE_BIT(ch->comm,COMM_NOQUOTE);
 
      sprintf( buf, "You quote '%s'\n\r", argument );
      printf_to_char(ch, buf);
      for ( d = descriptor_list; d != NULL; d = d->next )
      {
        CHAR_DATA *victim;
 
        victim = d->original ? d->original : d->character;
 
        if ( d->connected == CON_PLAYING &&
             d->character != ch &&
             !IS_SET(victim->comm,COMM_NOQUOTE) &&
             !IS_SET(victim->comm,COMM_QUIET) )
        {
          act_new( "$n quotes '$t'",
                   ch,argument, d->character, TO_VICT,POS_SLEEPING );
        }
      }
    }
}

/* RT question channel */
void do_question( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
 
    if (argument[0] == '\0' )
    {
      if (IS_SET(ch->comm,COMM_NOQUESTION))
      {
        printf_to_char(ch, "Q/A channel is now ON.\n\r");
        REMOVE_BIT(ch->comm,COMM_NOQUESTION);
      }
      else
      {
        printf_to_char(ch, "Q/A channel is now OFF.\n\r");
        SET_BIT(ch->comm,COMM_NOQUESTION);
      }
    }
    else  /* question sent, turn Q/A on if it isn't already */
    {
        if (IS_SET(ch->comm,COMM_QUIET))
        {
          printf_to_char(ch, "You must turn off quiet mode first.\n\r");
          return;
        }
 
        if (IS_SET(ch->comm,COMM_NOCHANNELS))
        {
          printf_to_char(ch, "The gods have revoked your channel priviliges.\n\r");
          return;
	}
 
        REMOVE_BIT(ch->comm,COMM_NOQUESTION);
 
      sprintf( buf, "You question '%s'\n\r", argument );
      printf_to_char(ch, buf);
      for ( d = descriptor_list; d != NULL; d = d->next )
      {
        CHAR_DATA *victim;
 
        victim = d->original ? d->original : d->character;
 
        if ( d->connected == CON_PLAYING &&
             d->character != ch &&
             !IS_SET(victim->comm,COMM_NOQUESTION) &&
             !IS_SET(victim->comm,COMM_QUIET) )
        {
	  act_new("$n questions '$t'",
	 	  ch,argument,d->character,TO_VICT,POS_SLEEPING);
        }
      }
    }
}

/* RT answer channel - uses same line as questions */
void do_answer( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
 
    if (argument[0] == '\0' )
    {
      if (IS_SET(ch->comm,COMM_NOQUESTION))
      {
        printf_to_char(ch, "Q/A channel is now ON.\n\r");
        REMOVE_BIT(ch->comm,COMM_NOQUESTION);
      }
      else
      {
        printf_to_char(ch, "Q/A channel is now OFF.\n\r");
        SET_BIT(ch->comm,COMM_NOQUESTION);
      }
    }
    else  /* answer sent, turn Q/A on if it isn't already */
    {
        if (IS_SET(ch->comm,COMM_QUIET))
        {
          printf_to_char(ch, "You must turn off quiet mode first.\n\r");
          return;
        }
 
        if (IS_SET(ch->comm,COMM_NOCHANNELS))
        {
          printf_to_char(ch, "The gods have revoked your channel priviliges.\n\r");
          return;
	}
 
        REMOVE_BIT(ch->comm,COMM_NOQUESTION);
 
      sprintf( buf, "You answer '%s'\n\r", argument );
      printf_to_char(ch, buf);
      for ( d = descriptor_list; d != NULL; d = d->next )
      {
        CHAR_DATA *victim;
 
        victim = d->original ? d->original : d->character;
 
        if ( d->connected == CON_PLAYING &&
             d->character != ch &&
             !IS_SET(victim->comm,COMM_NOQUESTION) &&
             !IS_SET(victim->comm,COMM_QUIET) )
        {
	  act_new("$n answers '$t'",
		  ch,argument,d->character,TO_VICT,POS_SLEEPING);
        }
      }
    }
}

/* RT music channel */
void do_music( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
 
    if (argument[0] == '\0' )
    {
      if (IS_SET(ch->comm,COMM_NOMUSIC))
      {
        printf_to_char(ch, "Music channel is now ON.\n\r");
        REMOVE_BIT(ch->comm,COMM_NOMUSIC);
      }
      else
      {
        printf_to_char(ch, "Music channel is now OFF.\n\r");
        SET_BIT(ch->comm,COMM_NOMUSIC);
      }
    }
    else  /* music sent, turn music on if it isn't already */
    {
        if (IS_SET(ch->comm,COMM_QUIET))
        {
          printf_to_char(ch, "You must turn off quiet mode first.\n\r");
          return;
        }
 
        if (IS_SET(ch->comm,COMM_NOCHANNELS))
        {
          printf_to_char(ch, "The gods have revoked your channel priviliges.\n\r");
          return;
	}
 
        REMOVE_BIT(ch->comm,COMM_NOMUSIC);
 
      sprintf( buf, "You MUSIC: '%s'\n\r", argument );
      printf_to_char(ch, buf);
      sprintf( buf, "$n MUSIC: '%s'", argument );
      for ( d = descriptor_list; d != NULL; d = d->next )
      {
        CHAR_DATA *victim;
 
        victim = d->original ? d->original : d->character;
 
        if ( d->connected == CON_PLAYING &&
             d->character != ch &&
             !IS_SET(victim->comm,COMM_NOMUSIC) &&
             !IS_SET(victim->comm,COMM_QUIET) )
        {
	    act_new("$n MUSIC: '$t'",
		    ch,argument,d->character,TO_VICT,POS_SLEEPING);
        }
      }
    }
}

/* clan channels */
void do_clantalk( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;

    if (!is_clan(ch) || clan_table[ch->clan].independent)
    {
	printf_to_char(ch, "You aren't in a clan.\n\r");
	return;
    }
    if ( argument[0] == '\0' )
    {
      if (IS_SET(ch->comm,COMM_NOCLAN))
      {
        printf_to_char(ch, "Clan channel is now ON\n\r");
        REMOVE_BIT(ch->comm,COMM_NOCLAN);
      }
      else
      {
        printf_to_char(ch, "Clan channel is now OFF\n\r");
        SET_BIT(ch->comm,COMM_NOCLAN);
      }
      return;
    }

        if (IS_SET(ch->comm,COMM_NOCHANNELS))
        {
         printf_to_char(ch, "The gods have revoked your channel priviliges.\n\r");
          return;
        }

        REMOVE_BIT(ch->comm,COMM_NOCLAN);

      sprintf( buf, "You clan '%s'\n\r", argument );
      printf_to_char(ch, buf);
      sprintf( buf, "$n clans '%s'", argument );
    for ( d = descriptor_list; d != NULL; d = d->next )
    {
        if ( d->connected == CON_PLAYING &&
             d->character != ch &&
	     is_same_clan(ch,d->character) &&
             !IS_SET(d->character->comm,COMM_NOCLAN) &&
	     !IS_SET(d->character->comm,COMM_QUIET) )
        {
            act_new("$n clans '$t'",ch,argument,d->character,TO_VICT,POS_DEAD);
        }
    }

    return;
}

void do_immtalk( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;

    if ( argument[0] == '\0' )
    {
      if (IS_SET(ch->comm,COMM_NOWIZ))
      {
	printf_to_char(ch, "Immortal channel is now ON\n\r");
	REMOVE_BIT(ch->comm,COMM_NOWIZ);
      }
      else
      {
	printf_to_char(ch, "Immortal channel is now OFF\n\r");
	SET_BIT(ch->comm,COMM_NOWIZ);
      } 
      return;
    }

    REMOVE_BIT(ch->comm,COMM_NOWIZ);

    sprintf( buf, "$n: %s", argument );
    act_new("$n: $t",ch,argument,NULL,TO_CHAR,POS_DEAD);
    for ( d = descriptor_list; d != NULL; d = d->next )
    {
	if ( d->connected == CON_PLAYING && 
	     IS_IMMORTAL(d->character) && 
             !IS_SET(d->character->comm,COMM_NOWIZ) )
	{
	    act_new("$n: $t",ch,argument,d->character,TO_VICT,POS_DEAD);
	}
    }

    return;
}



void do_say( CHAR_DATA *ch, char *argument )
{
    if ( argument[0] == '\0' )
    {
	printf_to_char(ch, "Say what?\n\r");
	return;
    }

    act( "$n says '$T'", ch, NULL, argument, TO_ROOM );
    act( "You say '$T'", ch, NULL, argument, TO_CHAR );
    return;
}



void do_shout( CHAR_DATA *ch, char *argument )
{
    DESCRIPTOR_DATA *d;

    if (argument[0] == '\0' )
    {
      	if (IS_SET(ch->comm,COMM_SHOUTSOFF))
      	{
            printf_to_char(ch, "You can hear shouts again.\n\r");
            REMOVE_BIT(ch->comm,COMM_SHOUTSOFF);
      	}
      	else
      	{
            printf_to_char(ch, "You will no longer hear shouts.\n\r");
            SET_BIT(ch->comm,COMM_SHOUTSOFF);
      	}
      	return;
    }

    if ( IS_SET(ch->comm, COMM_NOSHOUT) )
    {
        printf_to_char(ch, "You can't shout.\n\r");
        return;
    }
 
    REMOVE_BIT(ch->comm,COMM_SHOUTSOFF);

    WAIT_STATE( ch, 12 );

    act( "You shout '$T'", ch, NULL, argument, TO_CHAR );
    for ( d = descriptor_list; d != NULL; d = d->next )
    {
	CHAR_DATA *victim;

	victim = d->original ? d->original : d->character;

	if ( d->connected == CON_PLAYING &&
	     d->character != ch &&
	     !IS_SET(victim->comm, COMM_SHOUTSOFF) &&
	     !IS_SET(victim->comm, COMM_QUIET) ) 
	{
	    act("$n shouts '$t'",ch,argument,d->character,TO_VICT);
	}
    }

    return;
}



void do_tell( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH],buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;

    if ( IS_SET(ch->comm, COMM_NOTELL) || IS_SET(ch->comm,COMM_DEAF))
    {
	printf_to_char(ch, "Your message didn't get through.\n\r");
	return;
    }

    if ( IS_SET(ch->comm, COMM_QUIET) )
    {
	printf_to_char(ch, "You must turn off quiet mode first.\n\r");
	return;
    }

    if (IS_SET(ch->comm,COMM_DEAF))
    {
	printf_to_char(ch, "You must turn off deaf mode first.\n\r");
	return;
    }

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
	printf_to_char(ch, "Tell whom what?\n\r");
	return;
    }

    /*
     * Can tell to PC's anywhere, but NPC's only in same room.
     * -- Furey
     */
    if ( ( victim = get_char_world( ch, arg ) ) == NULL
    || ( IS_NPC(victim) && victim->in_room != ch->in_room ) )
    {
	printf_to_char(ch, "They aren't here.\n\r");
	return;
    }

    if ( victim->desc == NULL && !IS_NPC(victim))
    {
	act("$N seems to have misplaced $S link...try again later.",
	    ch,NULL,victim,TO_CHAR);
        sprintf(buf,"%s tells you '%s'\n\r",PERS(ch,victim),argument);
        buf[0] = UPPER(buf[0]);
        add_buf(victim->pcdata->buffer,buf);
	return;
    }

    if ( !(IS_IMMORTAL(ch) && ch->level > LEVEL_IMMORTAL) && !IS_AWAKE(victim) )
    {
	act( "$E can't hear you.", ch, 0, victim, TO_CHAR );
	return;
    }
  
    if ((IS_SET(victim->comm,COMM_QUIET) || IS_SET(victim->comm,COMM_DEAF))
    && !IS_IMMORTAL(ch))
    {
	act( "$E is not receiving tells.", ch, 0, victim, TO_CHAR );
  	return;
    }

    if (IS_SET(victim->comm,COMM_AFK))
    {
	if (IS_NPC(victim))
	{
	    act("$E is AFK, and not receiving tells.",ch,NULL,victim,TO_CHAR);
	    return;
	}

	act("$E is AFK, but your tell will go through when $E returns.",
	    ch,NULL,victim,TO_CHAR);
	sprintf(buf,"%s tells you '%s'\n\r",PERS(ch,victim),argument);
	buf[0] = UPPER(buf[0]);
	add_buf(victim->pcdata->buffer,buf);
	return;
    }

    act( "You tell $N '$t'", ch, argument, victim, TO_CHAR );
    act_new("$n tells you '$t'",ch,argument,victim,TO_VICT,POS_DEAD);
    victim->reply	= ch;

    return;
}



void do_reply( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char buf[MAX_STRING_LENGTH];

    if ( IS_SET(ch->comm, COMM_NOTELL) )
    {
	printf_to_char(ch, "Your message didn't get through.\n\r");
	return;
    }

    if ( ( victim = ch->reply ) == NULL )
    {
	printf_to_char(ch, "They aren't here.\n\r");
	return;
    }

    if ( victim->desc == NULL && !IS_NPC(victim))
    {
        act("$N seems to have misplaced $S link...try again later.",
            ch,NULL,victim,TO_CHAR);
        sprintf(buf,"%s tells you '%s'\n\r",PERS(ch,victim),argument);
        buf[0] = UPPER(buf[0]);
        add_buf(victim->pcdata->buffer,buf);
        return;
    }

    if ( !IS_IMMORTAL(ch) && !IS_AWAKE(victim) )
    {
	act( "$E can't hear you.", ch, 0, victim, TO_CHAR );
	return;
    }

    if ((IS_SET(victim->comm,COMM_QUIET) || IS_SET(victim->comm,COMM_DEAF))
    &&  !IS_IMMORTAL(ch) && !IS_IMMORTAL(victim))
    {
        act_new( "$E is not receiving tells.", ch, 0, victim, TO_CHAR,POS_DEAD);
        return;
    }

    if (!IS_IMMORTAL(victim) && !IS_AWAKE(ch))
    {
	printf_to_char(ch, "In your dreams, or what?\n\r");
	return;
    }

    if (IS_SET(victim->comm,COMM_AFK))
    {
        if (IS_NPC(victim))
        {
            act_new("$E is AFK, and not receiving tells.",
		ch,NULL,victim,TO_CHAR,POS_DEAD);
            return;
        }
 
        act_new("$E is AFK, but your tell will go through when $E returns.",
            ch,NULL,victim,TO_CHAR,POS_DEAD);
        sprintf(buf,"%s tells you '%s'\n\r",PERS(ch,victim),argument);
	buf[0] = UPPER(buf[0]);
        add_buf(victim->pcdata->buffer,buf);
        return;
    }

    act_new("You tell $N '$t'",ch,argument,victim,TO_CHAR,POS_DEAD);
    act_new("$n tells you '$t'",ch,argument,victim,TO_VICT,POS_DEAD);
    victim->reply	= ch;

    return;
}



void do_yell( CHAR_DATA *ch, char *argument )
{
    DESCRIPTOR_DATA *d;

    if ( IS_SET(ch->comm, COMM_NOSHOUT) )
    {
        printf_to_char(ch, "You can't yell.\n\r");
        return;
    }
 
    if ( argument[0] == '\0' )
    {
	printf_to_char(ch, "Yell what?\n\r");
	return;
    }


    act("You yell '$t'",ch,argument,NULL,TO_CHAR);
    for ( d = descriptor_list; d != NULL; d = d->next )
    {
	if ( d->connected == CON_PLAYING
	&&   d->character != ch
	&&   d->character->in_room != NULL
	&&   d->character->in_room->area == ch->in_room->area 
        &&   !IS_SET(d->character->comm,COMM_QUIET) )
	{
	    act("$n yells '$t'",ch,argument,d->character,TO_VICT);
	}
    }

    return;
}


void do_emote( CHAR_DATA *ch, char *argument )
{
    if ( !IS_NPC(ch) && IS_SET(ch->comm, COMM_NOEMOTE) )
    {
        printf_to_char(ch, "You can't show your emotions.\n\r");
        return;
    }
 
    if ( argument[0] == '\0' )
    {
        printf_to_char(ch, "Emote what?\n\r");
        return;
    }
 
    act( "$n $T", ch, NULL, argument, TO_ROOM );
    act( "$n $T", ch, NULL, argument, TO_CHAR );
    return;
}


void do_pmote( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *vch;
    char *letter,*name;
    char last[MAX_INPUT_LENGTH], temp[MAX_STRING_LENGTH];
    int matches = 0;

    if ( !IS_NPC(ch) && IS_SET(ch->comm, COMM_NOEMOTE) )
    {
        printf_to_char(ch, "You can't show your emotions.\n\r");
        return;
    }
 
    if ( argument[0] == '\0' )
    {
        printf_to_char(ch, "Emote what?\n\r");
        return;
    }
 
    act( "$n $t", ch, argument, NULL, TO_CHAR );

    for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
    {
	if (vch->desc == NULL || vch == ch)
	    continue;

	if ((letter = strstr(argument,vch->name)) == NULL)
	{
	    act("$N $t",vch,argument,ch,TO_CHAR);
	    continue;
	}

	strcpy(temp,argument);
	temp[strlen(argument) - strlen(letter)] = '\0';
   	last[0] = '\0';
 	name = vch->name;
	
	for (; *letter != '\0'; letter++)
	{ 
	    if (*letter == '\'' && matches == strlen(vch->name))
	    {
		strcat(temp,"r");
		continue;
	    }

	    if (*letter == 's' && matches == strlen(vch->name))
	    {
		matches = 0;
		continue;
	    }
	    
 	    if (matches == strlen(vch->name))
	    {
		matches = 0;
	    }

	    if (*letter == *name)
	    {
		matches++;
		name++;
		if (matches == strlen(vch->name))
		{
		    strcat(temp,"you");
		    last[0] = '\0';
		    name = vch->name;
		    continue;
		}
		strncat(last,letter,1);
		continue;
	    }

	    matches = 0;
	    strcat(temp,last);
	    strncat(temp,letter,1);
	    last[0] = '\0';
	    name = vch->name;
	}

	act("$N $t",vch,temp,ch,TO_CHAR);
    }
	
    return;
}


/*
 * All the posing stuff.
 */
struct	pose_table_type
{
    char *   self;
    char *   others;
};

const	struct	pose_table_type	pose_table	[]	=
{
	{
	    "You sizzle with energy.",
	    "$n sizzles with energy."
    },
    {
	    "You feel very holy.",
	    "$n looks very holy."
    },
    {
	    "You perform a small card trick.",
	    "$n performs a small card trick."
    },
    {
	    "You show your bulging muscles.",
	    "$n shows $s bulging muscles."
    },
	{
	    "You turn into a butterfly, then return to your normal shape.",
	    "$n turns into a butterfly, then returns to $s normal shape."
    },
    {
	    "You nonchalantly turn wine into water.",
	    "$n nonchalantly turns wine into water."
    },
    {
	    "You wiggle your ears alternately.",
	    "$n wiggles $s ears alternately."
    },
    {
	    "You crack nuts between your fingers.",
	    "$n cracks nuts between $s fingers."
	},
    {
	    "Blue sparks fly from your fingers.",
	    "Blue sparks fly from $n's fingers."
    },
    {
	    "A halo appears over your head.",
	    "A halo appears over $n's head."
    },
    {
	    "You nimbly tie yourself into a knot.",
	    "$n nimbly ties $mself into a knot."
    },
    {
	    "You grizzle your teeth and look mean.",
	    "$n grizzles $s teeth and looks mean."
	},
    {
	    "Little red lights dance in your eyes.",
	    "Little red lights dance in $n's eyes."
    },
    {
	    "You recite words of wisdom.",
	    "$n recites words of wisdom."
    },
    {
	    "You juggle with daggers, apples, and eyeballs.",
	    "$n juggles with daggers, apples, and eyeballs."
    },
    {
	    "You hit your head, and your eyes roll.",
	    "$n hits $s head, and $s eyes roll."
	},
    {
	    "A slimy green monster appears before you and bows.",
	    "A slimy green monster appears before $n and bows."
    },
    {
	    "Deep in prayer, you levitate.",
	    "Deep in prayer, $n levitates."
    },
    {
	    "You steal the underwear off every person in the room.",
	    "Your underwear is gone!  $n stole it!"
    },
    {
	    "Crunch, crunch -- you munch a bottle.",
	    "Crunch, crunch -- $n munches a bottle."
	},
    {
	    "You turn everybody into a little pink elephant.",
	    "You are turned into a little pink elephant by $n."
    },
    {
	    "An angel consults you.",
	    "An angel consults $n."
    },
    {
	    "The dice roll ... and you win again.",
	    "The dice roll ... and $n wins again."
    },
    {
	    "... 98, 99, 100 ... you do pushups.",
	    "... 98, 99, 100 ... $n does pushups."
	},
    {
	    "A small ball of light dances on your fingertips.",
	    "A small ball of light dances on $n's fingertips."
    },
    {
	    "Your body glows with an unearthly light.",
	    "$n's body glows with an unearthly light."
    },
    {
	    "You count the money in everyone's pockets.",
	    "Check your money, $n is counting it."
    },
    {
	    "Arnold Schwarzenegger admires your physique.",
	    "Arnold Schwarzenegger admires $n's physique."
	},
    {
	    "Smoke and fumes leak from your nostrils.",
	    "Smoke and fumes leak from $n's nostrils."
    },
    {
	    "A spot light hits you.",
	    "A spot light hits $n."
    },
    {
	    "You balance a pocket knife on your tongue.",
	    "$n balances a pocket knife on your tongue."
    },
    {
	    "Watch your feet, you are juggling granite boulders.",
	    "Watch your feet, $n is juggling granite boulders."
	},
    {
	    "The light flickers as you rap in magical languages.",
	    "The light flickers as $n raps in magical languages."
    },
    {
	    "Everyone levitates as you pray.",
	    "You levitate as $n prays."
    },
    {
	    "You produce a coin from everyone's ear.",
	    "$n produces a coin from your ear."
    },
    {
	    "Oomph!  You squeeze water out of a granite boulder.",
	    "Oomph!  $n squeezes water out of a granite boulder."
	},
    {
	    "Your head disappears.",
	    "$n's head disappears."
    },
    {
	    "A cool breeze refreshes you.",
	    "A cool breeze refreshes $n."
    },
    {
	    "You step behind your shadow.",
	    "$n steps behind $s shadow."
    },
    {
	    "You pick your teeth with a spear.",
	    "$n picks $s teeth with a spear."
	},
    {
	    "A fire elemental singes your hair.",
	    "A fire elemental singes $n's hair."
    },
    {
	    "The sun pierces through the clouds to illuminate you.",
	    "The sun pierces through the clouds to illuminate $n."
    },
    {
	    "Your eyes dance with greed.",
	    "$n's eyes dance with greed."
    },
    {
	    "Everyone is swept off their foot by your hug.",
	    "You are swept off your feet by $n's hug."
	},
    {
	    "The sky changes color to match your eyes.",
	    "The sky changes color to match $n's eyes."
    },
    {
	    "The ocean parts before you.",
	    "The ocean parts before $n."
    },
    {
	    "You deftly steal everyone's weapon.",
	    "$n deftly steals your weapon.",
    },
    {
	    "Your karate chop splits a tree.",
	    "$n's karate chop splits a tree."
	},
    {
	    "The stones dance to your command.",
	    "The stones dance to $n's command."
    },
    {
	    "A thunder cloud kneels to you.",
	    "A thunder cloud kneels to $n."
    },
    {
	    "The Grey Mouser buys you a beer.",
	    "The Grey Mouser buys $n a beer."
    },
    {
	    "A strap of your armor breaks over your mighty thews.",
	    "A strap of $n's armor breaks over $s mighty thews."
	},
    {
	    "The heavens and grass change colour as you smile.",
	    "The heavens and grass change colour as $n smiles."
    },
    {
	    "The Burning Man speaks to you.",
	    "The Burning Man speaks to $n."
    },
    {
	    "Everyone's pocket explodes with your fireworks.",
	    "Your pocket explodes with $n's fireworks."
    },
    {
	    "A boulder cracks at your frown.",
	    "A boulder cracks at $n's frown."
	},
    {
	    "Everyone's clothes are transparent, and you are laughing.",
	    "Your clothes are transparent, and $n is laughing."
    },
    {
	    "An eye in a pyramid winks at you.",
	    "An eye in a pyramid winks at $n."
    },
    {
	    "Everyone discovers your dagger a centimeter from their eye.",
	    "You discover $n's dagger a centimeter from your eye."
    },
    {
	    "Mercenaries arrive to do your bidding.",
	    "Mercenaries arrive to do $n's bidding."
	},
    {
	    "A black hole swallows you.",
	    "A black hole swallows $n."
    },
    {
	    "Valentine Michael Smith offers you a glass of water.",
	    "Valentine Michael Smith offers $n a glass of water."
    },
    {
	    "Where did you go?",
	    "Where did $n go?"
    },
    {
	    "Four matched Percherons bring in your chariot.",
	    "Four matched Percherons bring in $n's chariot."
	},
    {
	    "The world shimmers in time with your whistling.",
	    "The world shimmers in time with $n's whistling."
    },
    {
	    "The great god Mota gives you a staff.",
	    "The great god Mota gives $n a staff."
    },
    {
	    "Click.",
	    "Click."
    },
    {
	    "Atlas asks you to relieve him.",
	    "Atlas asks $n to relieve him."
	}
};


void do_pose( CHAR_DATA *ch, char *argument )
{
    int level = ch->level;
    int pose;

    if ( level < 0 )
        level = 0;

    pose = number_range(0, sizeof(pose_table)/sizeof(pose_table[0]) - 1);

    act(pose_table[pose].self, ch, NULL, NULL, TO_CHAR);
    act(pose_table[pose].others, ch, NULL, NULL, TO_ROOM);
}



void do_bug( CHAR_DATA *ch, char *argument )
{
    append_file( ch, BUG_FILE, argument );
    printf_to_char(ch, "Bug logged.\n\r");
    return;
}

void do_typo( CHAR_DATA *ch, char *argument )
{
    append_file( ch, TYPO_FILE, argument );
    printf_to_char(ch, "Typo logged.\n\r");
    return;
}

void do_rent( CHAR_DATA *ch, char *argument )
{
    printf_to_char(ch, "There is no rent here.  Just save and quit.\n\r");
    return;
}


void do_qui( CHAR_DATA *ch, char *argument )
{
    printf_to_char(ch, "If you want to QUIT, you have to spell it out.\n\r");
    return;
}



void do_quit( CHAR_DATA *ch, char *argument )
{
    DESCRIPTOR_DATA *d,*d_next;
    int id;

    if ( IS_NPC(ch) )
	return;

    if ( ch->position == POS_FIGHTING )
    {
	printf_to_char(ch, "No way! You are fighting.\n\r");
	return;
    }

    if ( ch->position  < POS_STUNNED  )
    {
	printf_to_char(ch, "You're not DEAD yet.\n\r");
	return;
    }
    printf_to_char(ch, "Alas, all good things must come to an end.\n\r");
    act( "$n has left the game.", ch, NULL, NULL, TO_ROOM );
    sprintf( log_buf, "%s has quit.", ch->name );
    log_string( log_buf );
     wiznet("$N rejoins the real world.",ch,NULL,WIZ_LOGINS,0,get_trust(ch));

    /*
     * After extract_char the ch is no longer valid!
     */
    save_char_obj( ch );
    id = ch->id;
    d = ch->desc;
    extract_char( ch, TRUE );
    if ( d != NULL )
	close_socket( d );

    /* toast evil cheating bastards */
    for (d = descriptor_list; d != NULL; d = d_next)
    {
	CHAR_DATA *tch;

	d_next = d->next;
	tch = d->original ? d->original : d->character;
	if (tch && tch->id == id)
	{
	    extract_char(tch,TRUE);
	    close_socket(d);
	} 
    }

    return;
}



void do_save( CHAR_DATA *ch, char *argument )
{
    if ( IS_NPC(ch) )
	return;

    save_char_obj( ch );
    printf_to_char(ch, "Saving. Remember that Kurgan has also automatic saving.\n\r");
    WAIT_STATE(ch,4 * PULSE_VIOLENCE);
    return;
}



void do_follow( CHAR_DATA *ch, char *argument )
{
/* RT changed to allow unlimited following and follow the NOFOLLOW rules */
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	printf_to_char(ch, "Follow whom?\n\r");
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	printf_to_char(ch, "They aren't here.\n\r");
	return;
    }

    if ( IS_AFFECTED(ch, AFF_CHARM) && ch->master != NULL )
    {
	act( "But you'd rather follow $N!", ch, NULL, ch->master, TO_CHAR );
	return;
    }

    if ( victim == ch )
    {
	if ( ch->master == NULL )
	{
	    printf_to_char(ch, "You already follow yourself.\n\r");
	    return;
	}
	stop_follower(ch);
	return;
    }

    if (!IS_NPC(victim) && IS_SET(victim->act,PLR_NOFOLLOW) && !IS_IMMORTAL(ch))
    {
	act("$N doesn't seem to want any followers.\n\r",
             ch,NULL,victim, TO_CHAR);
        return;
    }

    REMOVE_BIT(ch->act,PLR_NOFOLLOW);
    
    if ( ch->master != NULL )
	stop_follower( ch );

    add_follower( ch, victim );
    return;
}


void add_follower( CHAR_DATA *ch, CHAR_DATA *master )
{
    if ( ch->master != NULL )
    {
	bugf("Add_follower: non-null master.");
	return;
    }

    ch->master        = master;
    ch->leader        = NULL;

    if ( can_see( master, ch ) )
	act( "$n now follows you.", ch, NULL, master, TO_VICT );

    act( "You now follow $N.",  ch, NULL, master, TO_CHAR );

    return;
}



void stop_follower( CHAR_DATA *ch )
{
    if ( ch->master == NULL )
    {
	bugf("Stop_follower: null master.");
	return;
    }

    if ( IS_AFFECTED(ch, AFF_CHARM) )
    {
	REMOVE_BIT( ch->affected_by, AFF_CHARM );
	affect_strip( ch, gsn_charm_person );
    }

    if ( can_see( ch->master, ch ) && ch->in_room != NULL)
    {
	act( "$n stops following you.",     ch, NULL, ch->master, TO_VICT    );
    	act( "You stop following $N.",      ch, NULL, ch->master, TO_CHAR    );
    }
    if (ch->master->pet == ch)
	ch->master->pet = NULL;

    ch->master = NULL;
    ch->leader = NULL;
    return;
}

/* nukes charmed monsters and pets */
void nuke_pets( CHAR_DATA *ch )
{    
    CHAR_DATA *pet;

    if ((pet = ch->pet) != NULL)
    {
    	stop_follower(pet);
    	if (pet->in_room != NULL)
    	    act("$N slowly fades away.",ch,NULL,pet,TO_NOTVICT);
    	extract_char(pet,TRUE);
    }
    ch->pet = NULL;

    return;
}



void die_follower( CHAR_DATA *ch )
{
    CHAR_DATA *fch;

    if ( ch->master != NULL )
    {
    	if (ch->master->pet == ch)
    	    ch->master->pet = NULL;
	stop_follower( ch );
    }

    ch->leader = NULL;

    for ( fch = char_list; fch != NULL; fch = fch->next )
    {
	if ( fch->master == ch )
	    stop_follower( fch );
	if ( fch->leader == ch )
	    fch->leader = fch;
    }

    return;
}



void do_order( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH],arg2[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    CHAR_DATA *och;
    CHAR_DATA *och_next;
    bool found;
    bool fAll;

    argument = one_argument( argument, arg );
    one_argument(argument,arg2);

    if (!str_cmp(arg2,"delete"))
    {
        printf_to_char(ch, "That will NOT be done.\n\r");
        return;
    }

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
	printf_to_char(ch, "Order whom to do what?\n\r");
	return;
    }

    if ( IS_AFFECTED( ch, AFF_CHARM ) )
    {
	printf_to_char(ch, "You feel like taking, not giving, orders.\n\r");
	return;
    }

    if ( !str_cmp( arg, "all" ) )
    {
	fAll   = TRUE;
	victim = NULL;
    }
    else
    {
	fAll   = FALSE;
	if ( ( victim = get_char_room( ch, arg ) ) == NULL )
	{
	    printf_to_char(ch, "They aren't here.\n\r");
	    return;
	}

	if ( victim == ch )
	{
	    printf_to_char(ch, "Aye aye, right away!\n\r");
	    return;
	}

	if (!IS_AFFECTED(victim, AFF_CHARM) || victim->master != ch 
	||  (IS_IMMORTAL(victim) && victim->trust >= ch->trust))
	{
	    printf_to_char(ch, "Do it yourself!\n\r");
	    return;
	}
    }

    found = FALSE;
    for ( och = ch->in_room->people; och != NULL; och = och_next )
    {
	och_next = och->next_in_room;

	if ( IS_AFFECTED(och, AFF_CHARM)
	&&   och->master == ch
	&& ( fAll || och == victim ) )
	{
	    found = TRUE;
	    sprintf( buf, "$n orders you to '%s'.", argument );
	    act( buf, ch, NULL, och, TO_VICT );
	    interpret( och, argument );
	}
    }

    if ( found )
    {
	WAIT_STATE(ch,PULSE_VIOLENCE);
	printf_to_char(ch, "Ok.\n\r");
    }
    else
	printf_to_char(ch, "You have no followers here.\n\r");
    return;
}



void do_group( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	CHAR_DATA *gch;
	CHAR_DATA *leader;

	leader = (ch->leader != NULL) ? ch->leader : ch;
	sprintf( buf, "%s's group:\n\r", PERS(leader, ch) );
	printf_to_char(ch, buf);

	for ( gch = char_list; gch != NULL; gch = gch->next )
	{
	    if ( is_same_group( gch, ch ) )
	    {
		sprintf( buf,
		"[%2d] %-16s %4d/%4d hp %4d/%4d mana %4d/%4d mv %5d xp\n\r",
		    gch->level,
		    capitalize( PERS(gch, ch) ),
		    gch->hit,   gch->max_hit,
		    gch->mana,  gch->max_mana,
		    gch->move,  gch->max_move,
		    gch->exp    );
		printf_to_char(ch, buf);
	    }
	}
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	printf_to_char(ch, "They aren't here.\n\r");
	return;
    }

    if ( ch->master != NULL || ( ch->leader != NULL && ch->leader != ch ) )
    {
	printf_to_char(ch, "But you are following someone else!\n\r");
	return;
    }

    if ( victim->master != ch && ch != victim )
    {
	act_new("$N isn't following you.",ch,NULL,victim,TO_CHAR,POS_SLEEPING);
	return;
    }
    
    if (IS_AFFECTED(victim,AFF_CHARM))
    {
        printf_to_char(ch, "You can't remove charmed mobs from your group.\n\r");
        return;
    }
    
    if (IS_AFFECTED(ch,AFF_CHARM))
    {
    	act_new("You like your master too much to leave $m!",
	    ch,NULL,victim,TO_VICT,POS_SLEEPING);
    	return;
    }

    if ( is_same_group( victim, ch ) && ch != victim )
    {
	victim->leader = NULL;
	act_new("$n removes $N from $s group.",
	    ch,NULL,victim,TO_NOTVICT,POS_RESTING);
	act_new("$n removes you from $s group.",
	    ch,NULL,victim,TO_VICT,POS_SLEEPING);
	act_new("You remove $N from your group.",
	    ch,NULL,victim,TO_CHAR,POS_SLEEPING);
	return;
    }

    victim->leader = ch;
    act_new("$N joins $n's group.",ch,NULL,victim,TO_NOTVICT,POS_RESTING);
    act_new("You join $n's group.",ch,NULL,victim,TO_VICT,POS_SLEEPING);
    act_new("$N joins your group.",ch,NULL,victim,TO_CHAR,POS_SLEEPING);
    return;
}



/*
 * 'Split' originally by Gnort, God of Chaos.
 */
void do_split( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg1[MAX_INPUT_LENGTH],arg2[MAX_INPUT_LENGTH];
    CHAR_DATA *gch;
    int members;
    int amount_gold = 0, amount_silver = 0;
    int share_gold, share_silver;
    int extra_gold, extra_silver;

    argument = one_argument( argument, arg1 );
	       one_argument( argument, arg2 );

    if ( arg1[0] == '\0' )
    {
	printf_to_char(ch, "Split how much?\n\r");
	return;
    }
    
    amount_silver = atoi( arg1 );

    if (arg2[0] != '\0')
	amount_gold = atoi(arg2);

    if ( amount_gold < 0 || amount_silver < 0)
    {
	printf_to_char(ch, "Your group wouldn't like that.\n\r");
	return;
    }

    if ( amount_gold == 0 && amount_silver == 0 )
    {
	printf_to_char(ch, "You hand out zero coins, but no one notices.\n\r");
	return;
    }

    if ( ch->gold <  amount_gold || ch->silver < amount_silver)
    {
	printf_to_char(ch, "You don't have that much to split.\n\r");
	return;
    }
  
    members = 0;
    for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
    {
	if ( is_same_group( gch, ch ) && !IS_AFFECTED(gch,AFF_CHARM))
	    members++;
    }

    if ( members < 2 )
    {
	printf_to_char(ch, "Just keep it all.\n\r");
	return;
    }
	    
    share_silver = amount_silver / members;
    extra_silver = amount_silver % members;

    share_gold   = amount_gold / members;
    extra_gold   = amount_gold % members;

    if ( share_gold == 0 && share_silver == 0 )
    {
	printf_to_char(ch, "Don't even bother, cheapskate.\n\r");
	return;
    }

    ch->silver	-= amount_silver;
    ch->silver	+= share_silver + extra_silver;
    ch->gold 	-= amount_gold;
    ch->gold 	+= share_gold + extra_gold;

    if (share_silver > 0)
    {
	sprintf(buf,
	    "You split %d silver coins. Your share is %d silver.\n\r",
 	    amount_silver,share_silver + extra_silver);
	printf_to_char(ch, buf);
    }

    if (share_gold > 0)
    {
	sprintf(buf,
	    "You split %d gold coins. Your share is %d gold.\n\r",
	     amount_gold,share_gold + extra_gold);
	printf_to_char(ch, buf);
    }

    if (share_gold == 0)
    {
	sprintf(buf,"$n splits %d silver coins. Your share is %d silver.",
		amount_silver,share_silver);
    }
    else if (share_silver == 0)
    {
	sprintf(buf,"$n splits %d gold coins. Your share is %d gold.",
		amount_gold,share_gold);
    }
    else
    {
	sprintf(buf,
"$n splits %d silver and %d gold coins, giving you %d silver and %d gold.\n\r",
	 amount_silver,amount_gold,share_silver,share_gold);
    }

    for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
    {
	if ( gch != ch && is_same_group(gch,ch) && !IS_AFFECTED(gch,AFF_CHARM))
	{
	    act( buf, ch, NULL, gch, TO_VICT );
	    gch->gold += share_gold;
	    gch->silver += share_silver;
	}
    }

    return;
}



void do_gtell( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *gch;

    if ( argument[0] == '\0' )
    {
	printf_to_char(ch, "Tell your group what?\n\r");
	return;
    }

    if ( IS_SET( ch->comm, COMM_NOTELL ) )
    {
	printf_to_char(ch, "Your message didn't get through!\n\r");
	return;
    }

    for ( gch = char_list; gch != NULL; gch = gch->next )
    {
	if ( is_same_group( gch, ch ) )
	    act_new("$n tells the group '$t'",
		ch,argument,gch,TO_VICT,POS_SLEEPING);
    }

    return;
}



/*
 * It is very important that this be an equivalence relation:
 * (1) A ~ A
 * (2) if A ~ B then B ~ A
 * (3) if A ~ B  and B ~ C, then A ~ C
 */
bool is_same_group( CHAR_DATA *ach, CHAR_DATA *bch )
{
    if ( ach == NULL || bch == NULL)
	return FALSE;

    if ( ach->leader != NULL ) ach = ach->leader;
    if ( bch->leader != NULL ) bch = bch->leader;
    return ach == bch;
}
