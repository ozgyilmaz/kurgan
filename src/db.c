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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>
#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <mysql/mysql.h>
#endif

#include "merc.h"
#include "db.h"
#include "recycle.h"
#include "music.h"
#include "lookup.h"


#if !defined(macintosh)
extern	int	_filbuf		args( (FILE *) );
#endif

#if !defined(OLD_RAND)
#if !defined(linux)
long random();
#endif
void srandom(unsigned int);
int getpid();
time_t time(time_t *tloc);
#endif


/* externals for counting purposes */
extern	OBJ_DATA	*obj_free;
extern	CHAR_DATA	*char_free;
extern  DESCRIPTOR_DATA *descriptor_free;
extern	PC_DATA		*pcdata_free;
extern  AFFECT_DATA	*affect_free;

/*
 * Globals.
 */
HELP_DATA *		help_first;
HELP_DATA *		help_last;

SHOP_DATA *		shop_first;
SHOP_DATA *		shop_last;


char			bug_buf		[2*MAX_INPUT_LENGTH];
CHAR_DATA *		char_list;
char *			help_greeting;
char			log_buf		[2*MAX_INPUT_LENGTH];
KILL_DATA		kill_table	[MAX_LEVEL];
OBJ_DATA *		object_list;
TIME_INFO_DATA		time_info;
WEATHER_DATA		weather_info;

sh_int			gsn_backstab;
sh_int			gsn_dodge;
sh_int			gsn_envenom;
sh_int			gsn_hide;
sh_int			gsn_peek;
sh_int			gsn_pick_lock;
sh_int			gsn_sneak;
sh_int			gsn_steal;

sh_int			gsn_disarm;
sh_int			gsn_enhanced_damage;
sh_int			gsn_kick;
sh_int			gsn_parry;
sh_int			gsn_rescue;
sh_int			gsn_second_attack;
sh_int			gsn_third_attack;

sh_int			gsn_blindness;
sh_int			gsn_charm_person;
sh_int			gsn_curse;
sh_int			gsn_invis;
sh_int			gsn_mass_invis;
sh_int			gsn_poison;
sh_int			gsn_plague;
sh_int			gsn_sleep;
sh_int			gsn_sanctuary;
sh_int			gsn_fly;
/* new gsns */

sh_int  		gsn_axe;
sh_int  		gsn_dagger;
sh_int  		gsn_flail;
sh_int  		gsn_mace;
sh_int  		gsn_polearm;
sh_int			gsn_shield_block;
sh_int  		gsn_spear;
sh_int  		gsn_sword;
sh_int  		gsn_whip;

sh_int  		gsn_bash;
sh_int  		gsn_berserk;
sh_int  		gsn_dirt;
sh_int  		gsn_hand_to_hand;
sh_int  		gsn_trip;

sh_int  		gsn_fast_healing;
sh_int  		gsn_haggle;
sh_int  		gsn_lore;
sh_int  		gsn_meditation;

sh_int  		gsn_scrolls;
sh_int  		gsn_staves;
sh_int  		gsn_wands;
sh_int  		gsn_recall;

ini_t * config;

/*
 * Locals.
 */
MOB_INDEX_DATA *	mob_index_hash		[MAX_KEY_HASH];
OBJ_INDEX_DATA *	obj_index_hash		[MAX_KEY_HASH];
ROOM_INDEX_DATA *	room_index_hash		[MAX_KEY_HASH];
char *			string_hash		[MAX_KEY_HASH];

AREA_DATA *		area_first;
AREA_DATA *		area_last;

char *			string_space;
char *			top_string;
char			str_empty	[1];
int   db_run;
char * db_server;
char * db_name;
char * db_user;
char * db_password;

int			top_affect;
int			top_area;
int			top_ed;
int			top_exit;
int			top_help;
int			top_mob_index;
int			top_obj_index;
int			top_reset;
int			top_room;
int			top_shop;
int 			mobile_count = 0;
int			newmobs = 0;
int			newobjs = 0;


/*
 * Memory management.
 * Increase MAX_STRING if you have too.
 * Tune the others only if you understand what you're doing.
 */
#define			MAX_STRING	1413120
#define			MAX_PERM_BLOCK	131072
#define			MAX_MEM_LIST	11

void *			rgFreeList	[MAX_MEM_LIST];
const int		rgSizeList	[MAX_MEM_LIST]	=
{
    16, 32, 64, 128, 256, 1024, 2048, 4096, 8192, 16384, 32768-64
};

int			nAllocString;
int			sAllocString;
int			nAllocPerm;
int			sAllocPerm;



/*
 * Semi-locals.
 */
bool			fBootDb;
FILE *			fpArea;
char			strArea[MAX_INPUT_LENGTH];



/*
 * Local booting procedures.
*/
void  init_random_number_generator  args( ( void ) );
void	load_area	args( ( FILE *fp ) );
void	load_helps	args( ( FILE *fp ) );
void	load_old_mob	args( ( FILE *fp ) );
void 	load_mobiles	args( ( FILE *fp ) );
void	load_old_obj	args( ( FILE *fp ) );
void 	load_objects	args( ( FILE *fp ) );
void	load_resets	args( ( FILE *fp ) );
void	load_rooms	args( ( FILE *fp ) );
void	load_shops	args( ( FILE *fp ) );
void 	load_socials	args( ( FILE *fp ) );
void	load_specials	args( ( FILE *fp ) );
void	load_notes	args( ( void ) );
void	load_bans	args( ( void ) );

void	fix_exits	args( ( void ) );

void	reset_area	args( ( AREA_DATA * pArea ) );

/*
 * Big mama top level function.
 */
void boot_db( void )
{

    /*
     * Init some data space stuff.
     */
    {
	if ( ( string_space = (char *)calloc( 1, MAX_STRING ) ) == NULL )
	{
	    bug( "Boot_db: can't alloc %d string space.", MAX_STRING );
	    exit( 1 );
	}
	top_string	= string_space;
	fBootDb		= TRUE;
    }

    /*
     * Init random number generator.
     */
    {
        init_random_number_generator( );
    }

/*
 * init ini file
 */
config = ini_load("../kurgan.ini");

db_run = atoi(ini_get(config, "database", "db_run"));

if (db_run == 1 || db_run == 2) {
  db_server = (char*)ini_get(config, "database", "db_server");
  if(!db_server)
  {
    bugf( "'No valid 'db_server'.\n\r" );
    exit( 1 );
  }
  db_name = (char*)ini_get(config, "database", "db_name");
  if(!db_name)
  {
    bugf( "No valid 'db_name'.\n\r" );
    exit( 1 );
  }
  db_user = (char*)ini_get(config, "database", "db_user");
  if(!db_user)
  {
    bugf( "No valid 'db_user'.\n\r" );
    exit( 1 );
  }
  db_password = (char*)ini_get(config, "database", "db_password");
  if(!db_password)
  {
    bugf( "No valid 'db_password'.\n\r" );
    exit( 1 );
  }
}

    /*
     * Set time and weather.
     */
    {
	long lhour, lday, lmonth;

	lhour		= (current_time - 650336715)
			/ (PULSE_TICK / PULSE_PER_SECOND);
	time_info.hour	= lhour  % 24;
	lday		= lhour  / 24;
	time_info.day	= lday   % 35;
	lmonth		= lday   / 35;
	time_info.month	= lmonth % 17;
	time_info.year	= lmonth / 17;

	     if ( time_info.hour <  5 ) weather_info.sunlight = SUN_DARK;
	else if ( time_info.hour <  6 ) weather_info.sunlight = SUN_RISE;
	else if ( time_info.hour < 19 ) weather_info.sunlight = SUN_LIGHT;
	else if ( time_info.hour < 20 ) weather_info.sunlight = SUN_SET;
	else                            weather_info.sunlight = SUN_DARK;

	weather_info.change	= 0;
	weather_info.mmhg	= 960;
	if ( time_info.month >= 7 && time_info.month <=12 )
	    weather_info.mmhg += number_range( 1, 50 );
	else
	    weather_info.mmhg += number_range( 1, 80 );

	     if ( weather_info.mmhg <=  980 ) weather_info.sky = SKY_LIGHTNING;
	else if ( weather_info.mmhg <= 1000 ) weather_info.sky = SKY_RAINING;
	else if ( weather_info.mmhg <= 1020 ) weather_info.sky = SKY_CLOUDY;
	else                                  weather_info.sky = SKY_CLOUDLESS;

    }

    /*
     * Assign gsn's for skills which have them.
     */
    {
	int sn;

	for ( sn = 0; sn < MAX_SKILL; sn++ )
	{
	    if ( skill_table[sn].pgsn != NULL )
		*skill_table[sn].pgsn = sn;
	}
    }


if (db_run == 0 || db_run == 1)
{
	FILE *fpList;

  if(db_run == 1)
  {
  logf("'db_run' is not set. Application will run with *.are files.");
  logf("db tables will be filled with freshly read data.");
  }

	if ( ( fpList = fopen( AREA_LIST, "r" ) ) == NULL )
	{
	    perror( AREA_LIST );
	    exit( 1 );
	}

	for ( ; ; )
	{
	    strcpy( strArea, fread_word( fpList ) );
	    if ( strArea[0] == '$' )
		break;

	    if ( strArea[0] == '-' )
	    {
		fpArea = stdin;
	    }
	    else
	    {
		if ( ( fpArea = fopen( strArea, "r" ) ) == NULL )
		{
		    perror( strArea );
		    exit( 1 );
		}
	    }

	    for ( ; ; )
	    {
		char *word;

		if ( fread_letter( fpArea ) != '#' )
		{
		    bug( "Boot_db: # not found.", 0 );
		    exit( 1 );
		}

		word = fread_word( fpArea );

		     if ( word[0] == '$'               )                 break;
		else if ( !str_cmp( word, "AREA"     ) ) load_area    (fpArea);
		else if ( !str_cmp( word, "HELPS"    ) ) load_helps   (fpArea);
		else if ( !str_cmp( word, "MOBOLD"   ) ) load_old_mob (fpArea);
		else if ( !str_cmp( word, "MOBILES"  ) ) load_mobiles (fpArea);
		else if ( !str_cmp( word, "OBJOLD"   ) ) load_old_obj (fpArea);
	  else if ( !str_cmp( word, "OBJECTS"  ) ) load_objects (fpArea);
		else if ( !str_cmp( word, "RESETS"   ) ) load_resets  (fpArea);
		else if ( !str_cmp( word, "ROOMS"    ) ) load_rooms   (fpArea);
		else if ( !str_cmp( word, "SHOPS"    ) ) load_shops   (fpArea);
		else if ( !str_cmp( word, "SOCIALS"  ) ) load_socials (fpArea);
		else if ( !str_cmp( word, "SPECIALS" ) ) load_specials(fpArea);
		else
		{
		    bug( "Boot_db: bad section name.", 0 );
		    exit( 1 );
		}
	    }

	    if ( fpArea != stdin )
		fclose( fpArea );
	    fpArea = NULL;
	}
	fclose( fpList );
    }
    // End of reading area files.

// Dont read area files but read the database.
else if (db_run == 2)
{
  logf("Mud will run with data served by database.\n\r");
  mysql_read_help();
  mysql_read_social();
  mysql_read_area();
  mysql_read_room();
  mysql_read_room_exit();
  mysql_read_room_extra_description();
  mysql_read_mobile();
  mysql_read_object();
  mysql_read_object_extra_description();
  mysql_read_object_affect_data();
  mysql_read_reset();
  mysql_read_shop();
}
else
{
  bugf("No valid value for 'db_run'. Exiting.\n\r");
  exit( 1 );
}
    /*
     * Fix up exits.
     * Declare db booting over.
     * Reset all areas once.
     * Load up the songs, notes and ban files.
     */
    {
	fix_exits( );
	fBootDb	= FALSE;
	area_update( );
	load_notes( );
	load_bans();
	load_songs();
    }

    return;
}



/*
 * Snarf an 'area' header line.
 */
void load_area( FILE *fp )
{
    AREA_DATA *pArea;

    pArea		= (AREA_DATA *)alloc_perm( sizeof(*pArea) );
    pArea->reset_first	= NULL;
    pArea->reset_last	= NULL;
    pArea->file_name	= fread_string(fp);
    pArea->name		= fread_string( fp );
    fread_letter(fp);
    pArea->low_range	= fread_number(fp);
    pArea->high_range	= fread_number(fp);
    fread_letter(fp);
    pArea->writer	= str_dup( fread_word(fp) );
    pArea->credits	= fread_string( fp );
    pArea->min_vnum	= fread_number(fp);
    pArea->max_vnum	= fread_number(fp);
    pArea->age		= 15;
    pArea->nplayer	= 0;
    pArea->empty	= FALSE;

    if (db_run == 1)
    {
    mysql_write_area( pArea );
    }

    if ( area_first == NULL )
	area_first = pArea;
    if ( area_last  != NULL )
	area_last->next = pArea;
    area_last	= pArea;
    pArea->next	= NULL;

    top_area++;
    return;
}



/*
 * Snarf a help section.
 */
void load_helps( FILE *fp )
{
    HELP_DATA *pHelp;

    for ( ; ; )
    {
	pHelp		= (HELP_DATA *)alloc_perm( sizeof(*pHelp) );
	pHelp->level	= fread_number( fp );
	pHelp->keyword	= fread_string( fp );
	if ( pHelp->keyword[0] == '$' )
	    break;
	pHelp->text	= fread_string( fp );
  if (db_run == 1)
  {
  mysql_write_help( pHelp );
}
	if ( !str_cmp( pHelp->keyword, "greeting" ) )
	    help_greeting = pHelp->text;

	if ( help_first == NULL )
	    help_first = pHelp;
	if ( help_last  != NULL )
	    help_last->next = pHelp;

	help_last	= pHelp;
	pHelp->next	= NULL;
	top_help++;
    }

    return;
}



/*
 * Snarf a mob section.  old style
 */
void load_old_mob( FILE *fp )
{
    MOB_INDEX_DATA *pMobIndex;
    /* for race updating */
    int race;
    char name[MAX_STRING_LENGTH];

    if ( area_last == NULL )
    {
    	bug( "Load_old_mob: no #AREA seen yet.", 0 );
    	exit( 1 );
    }

    for ( ; ; )
    {
	sh_int vnum;
	char letter;
	int iHash;

	letter				= fread_letter( fp );
	if ( letter != '#' )
	{
	    bug( "Load_mobiles: # not found.", 0 );
	    exit( 1 );
	}

	vnum				= fread_number( fp );
	if ( vnum == 0 )
	    break;

	fBootDb = FALSE;
	if ( get_mob_index( vnum ) != NULL )
	{
	    bug( "Load_mobiles: vnum %d duplicated.", vnum );
	    exit( 1 );
	}
	fBootDb = TRUE;

	pMobIndex			= (MOB_INDEX_DATA *) alloc_perm( sizeof(*pMobIndex) );
	pMobIndex->vnum			= vnum;
	pMobIndex->new_format		= FALSE;
	pMobIndex->player_name		= fread_string( fp );
	pMobIndex->short_descr		= fread_string( fp );
	pMobIndex->long_descr		= fread_string( fp );
	pMobIndex->description		= fread_string( fp );

	pMobIndex->long_descr[0]	= UPPER(pMobIndex->long_descr[0]);
	pMobIndex->description[0]	= UPPER(pMobIndex->description[0]);

	pMobIndex->act			= fread_flag( fp ) | ACT_IS_NPC;
	pMobIndex->affected_by		= fread_flag( fp );
	pMobIndex->pShop		= NULL;
	pMobIndex->alignment		= fread_number( fp );
	letter				= fread_letter( fp );
	pMobIndex->level		= fread_number( fp );

	/*
	 * The unused stuff is for imps who want to use the old-style
	 * stats-in-files method.
	 */
					  fread_number( fp );	/* Unused */
					  fread_number( fp );	/* Unused */
					  fread_number( fp );	/* Unused */
	/* 'd'		*/		  fread_letter( fp );	/* Unused */
					  fread_number( fp );	/* Unused */
	/* '+'		*/		  fread_letter( fp );	/* Unused */
					  fread_number( fp );	/* Unused */
					  fread_number( fp );	/* Unused */
	/* 'd'		*/		  fread_letter( fp );	/* Unused */
					  fread_number( fp );	/* Unused */
	/* '+'		*/		  fread_letter( fp );	/* Unused */
					  fread_number( fp );	/* Unused */
        pMobIndex->wealth               = fread_number( fp )/20;
	/* xp can't be used! */		  fread_number( fp );	/* Unused */
	pMobIndex->start_pos		= fread_number( fp );	/* Unused */
	pMobIndex->default_pos		= fread_number( fp );	/* Unused */

  	if (pMobIndex->start_pos < POS_SLEEPING)
	    pMobIndex->start_pos = POS_STANDING;
	if (pMobIndex->default_pos < POS_SLEEPING)
	    pMobIndex->default_pos = POS_STANDING;

	/*
	 * Back to meaningful values.
	 */
	pMobIndex->sex			= fread_number( fp );

    	/* compute the race BS */
   	one_argument(pMobIndex->player_name,name);

   	if (name[0] == '\0' || (race =  race_lookup(name)) == 0)
   	{
            /* fill in with blanks */
            pMobIndex->race = race_lookup("human");
            pMobIndex->off_flags = OFF_DODGE|OFF_DISARM|OFF_TRIP|ASSIST_VNUM;
            pMobIndex->imm_flags = 0;
            pMobIndex->res_flags = 0;
            pMobIndex->vuln_flags = 0;
            pMobIndex->form = FORM_EDIBLE|FORM_SENTIENT|FORM_BIPED|FORM_MAMMAL;
            pMobIndex->parts = PART_HEAD|PART_ARMS|PART_LEGS|PART_HEART|
                               PART_BRAINS|PART_GUTS;
    	}
    	else
    	{
            pMobIndex->race = race;
            pMobIndex->off_flags = OFF_DODGE|OFF_DISARM|OFF_TRIP|ASSIST_RACE|
                                   race_table[race].off;
            pMobIndex->imm_flags = race_table[race].imm;
            pMobIndex->res_flags = race_table[race].res;
            pMobIndex->vuln_flags = race_table[race].vuln;
            pMobIndex->form = race_table[race].form;
            pMobIndex->parts = race_table[race].parts;
    	}

	if ( letter != 'S' )
	{
	    bug( "Load_mobiles: vnum %d non-S.", vnum );
	    exit( 1 );
	}
  if (db_run == 1)
  {
  mysql_write_mobile( pMobIndex , area_last );
}

	iHash			= vnum % MAX_KEY_HASH;
	pMobIndex->next		= mob_index_hash[iHash];
	mob_index_hash[iHash]	= pMobIndex;
	top_mob_index++;
	kill_table[URANGE(0, pMobIndex->level, MAX_LEVEL-1)].number++;
    }

    return;
}

/*
 * Snarf an obj section.  old style
 */
void load_old_obj( FILE *fp )
{
    OBJ_INDEX_DATA *pObjIndex;

    if ( area_last == NULL )
    {
    	bug( "Load_old_mob: no #AREA seen yet.", 0 );
    	exit( 1 );
    }

    for ( ; ; )
    {
	sh_int vnum;
	char letter;
	int iHash;

	letter				= fread_letter( fp );
	if ( letter != '#' )
	{
	    bug( "Load_objects: # not found.", 0 );
	    exit( 1 );
	}

	vnum				= fread_number( fp );
	if ( vnum == 0 )
	    break;

	fBootDb = FALSE;
	if ( get_obj_index( vnum ) != NULL )
	{
	    bug( "Load_objects: vnum %d duplicated.", vnum );
	    exit( 1 );
	}
	fBootDb = TRUE;

	pObjIndex			= (OBJ_INDEX_DATA *)alloc_perm( sizeof(*pObjIndex) );
	pObjIndex->vnum			= vnum;
	pObjIndex->new_format		= FALSE;
	pObjIndex->reset_num	 	= 0;
	pObjIndex->name			= fread_string( fp );
	pObjIndex->short_descr		= fread_string( fp );
	pObjIndex->description		= fread_string( fp );
	/* Action description */	  fread_string( fp );

	pObjIndex->short_descr[0]	= LOWER(pObjIndex->short_descr[0]);
	pObjIndex->description[0]	= UPPER(pObjIndex->description[0]);
	pObjIndex->material		= str_dup("");

	pObjIndex->item_type		= fread_number( fp );
	pObjIndex->extra_flags		= fread_flag( fp );
	pObjIndex->wear_flags		= fread_flag( fp );
	pObjIndex->value[0]		= fread_number( fp );
	pObjIndex->value[1]		= fread_number( fp );
	pObjIndex->value[2]		= fread_number( fp );
	pObjIndex->value[3]		= fread_number( fp );
	pObjIndex->value[4]		= 0;
	pObjIndex->level		= 0;
	pObjIndex->condition 		= 100;
	pObjIndex->weight		= fread_number( fp );
	pObjIndex->cost			= fread_number( fp );	/* Unused */
	/* Cost per day */		  fread_number( fp );


	if (pObjIndex->item_type == ITEM_WEAPON)
	{
	    if (is_name((char*)"two",pObjIndex->name)
	    ||  is_name((char*)"two-handed",pObjIndex->name)
	    ||  is_name((char*)"claymore",pObjIndex->name))
		SET_BIT(pObjIndex->value[4],WEAPON_TWO_HANDS);
	}

	for ( ; ; )
	{
	    char letter;

	    letter = fread_letter( fp );

	    if ( letter == 'A' )
	    {
		AFFECT_DATA *paf;

		paf			= (AFFECT_DATA *)alloc_perm( sizeof(*paf) );
		paf->where		= TO_OBJECT;
		paf->type		= -1;
		paf->level		= 20; /* RT temp fix */
		paf->duration		= -1;
		paf->location		= fread_number( fp );
		paf->modifier		= fread_number( fp );
		paf->bitvector		= 0;
		paf->next		= pObjIndex->affected;
		pObjIndex->affected	= paf;
		top_affect++;
    if (db_run == 1)
    {
    mysql_write_object_affect_data( paf, pObjIndex->vnum );
  }
	    }

	    else if ( letter == 'E' )
	    {
		EXTRA_DESCR_DATA *ed;

		ed			= (EXTRA_DESCR_DATA *) alloc_perm( sizeof(*ed) );
		ed->keyword		= fread_string( fp );
		ed->description		= fread_string( fp );
		ed->next		= pObjIndex->extra_descr;
		pObjIndex->extra_descr	= ed;
		top_ed++;
    if (db_run == 1)
    {
    mysql_write_object_extra_description( ed, pObjIndex->vnum );
  }
	    }

	    else
	    {
		ungetc( letter, fp );
		break;
	    }
	}

        /* fix armors */
        if (pObjIndex->item_type == ITEM_ARMOR)
        {
            pObjIndex->value[1] = pObjIndex->value[0];
            pObjIndex->value[2] = pObjIndex->value[1];
        }

	/*
	 * Translate spell "slot numbers" to internal "skill numbers."
	 */
	switch ( pObjIndex->item_type )
	{
	case ITEM_PILL:
	case ITEM_POTION:
	case ITEM_SCROLL:
	    pObjIndex->value[1] = slot_lookup( pObjIndex->value[1] );
	    pObjIndex->value[2] = slot_lookup( pObjIndex->value[2] );
	    pObjIndex->value[3] = slot_lookup( pObjIndex->value[3] );
	    pObjIndex->value[4] = slot_lookup( pObjIndex->value[4] );
	    break;

	case ITEM_STAFF:
	case ITEM_WAND:
	    pObjIndex->value[3] = slot_lookup( pObjIndex->value[3] );
	    break;
	}

	iHash			= vnum % MAX_KEY_HASH;
	pObjIndex->next		= obj_index_hash[iHash];
	obj_index_hash[iHash]	= pObjIndex;
	top_obj_index++;
  if (db_run == 1)
  {
  mysql_write_object( pObjIndex , area_last );
}
    }

    return;
}





/*
 * Snarf a reset section.
 */
void load_resets( FILE *fp )
{
    RESET_DATA *pReset;

    if ( area_last == NULL )
    {
	bug( "Load_resets: no #AREA seen yet.", 0 );
	exit( 1 );
    }

    for ( ; ; )
    {
	ROOM_INDEX_DATA *pRoomIndex;
	EXIT_DATA *pexit;
	char letter;
	OBJ_INDEX_DATA *temp_index;

	if ( ( letter = fread_letter( fp ) ) == 'S' )
	    break;

	if ( letter == '*' )
	{
	    fread_to_eol( fp );
	    continue;
	}

	pReset		= (RESET_DATA *)alloc_perm( sizeof(*pReset) );
	pReset->command	= letter;
	/* if_flag */	  fread_number( fp );
	pReset->arg1	= fread_number( fp );
	pReset->arg2	= fread_number( fp );
	pReset->arg3	= (letter == 'G' || letter == 'R')
			    ? 0 : fread_number( fp );
	pReset->arg4	= (letter == 'P' || letter == 'M')
			    ? fread_number(fp) : 0;
			  fread_to_eol( fp );
        if (db_run == 1)
        {
  mysql_write_reset( pReset );
}
	/*
	 * Validate parameters.
	 * We're calling the index functions for the side effect.
	 */
	switch ( letter )
	{
	default:
	    bug( "Load_resets: bad command '%c'.", letter );
	    exit( 1 );
	    break;

	case 'M':
	    get_mob_index  ( pReset->arg1 );
	    get_room_index ( pReset->arg3 );
	    break;

	case 'O':
	    temp_index = get_obj_index  ( pReset->arg1 );
	    temp_index->reset_num++;
	    get_room_index ( pReset->arg3 );
	    break;

	case 'P':
	    temp_index = get_obj_index  ( pReset->arg1 );
	    temp_index->reset_num++;
	    get_obj_index  ( pReset->arg3 );
	    break;

	case 'G':
	case 'E':
	    temp_index = get_obj_index  ( pReset->arg1 );
	    temp_index->reset_num++;
	    break;

	case 'D':
	    pRoomIndex = get_room_index( pReset->arg1 );

	    if ( pReset->arg2 < 0
	    ||   pReset->arg2 > 5
	    || ( pexit = pRoomIndex->exit[pReset->arg2] ) == NULL
	    || !IS_SET( pexit->exit_info, EX_ISDOOR ) )
	    {
		bug( "Load_resets: 'D': exit %d not door.", pReset->arg2 );
		exit( 1 );
	    }

	    if ( pReset->arg3 < 0 || pReset->arg3 > 2 )
	    {
		bug( "Load_resets: 'D': bad 'locks': %d.", pReset->arg3 );
		exit( 1 );
	    }

	    break;

	case 'R':
	    pRoomIndex		= get_room_index( pReset->arg1 );

	    if ( pReset->arg2 < 0 || pReset->arg2 > 6 )
	    {
		bug( "Load_resets: 'R': bad exit %d.", pReset->arg2 );
		exit( 1 );
	    }

	    break;
	}

	if ( area_last->reset_first == NULL )
	    area_last->reset_first	= pReset;
	if ( area_last->reset_last  != NULL )
	    area_last->reset_last->next	= pReset;

	area_last->reset_last	= pReset;
	pReset->next		= NULL;
	top_reset++;
    }

    return;
}



/*
 * Snarf a room section.
 */
void load_rooms( FILE *fp )
{
    ROOM_INDEX_DATA *pRoomIndex;

    if ( area_last == NULL )
    {
    	bug( "Load_rooms: no #AREA seen yet.", 0 );
    	exit( 1 );
    }

    for ( ; ; )
    {
	sh_int vnum;
	char letter;
	int door;
	int iHash;

	letter				= fread_letter( fp );
	if ( letter != '#' )
	{
	    bug( "Load_rooms: # not found.", 0 );
	    exit( 1 );
	}

	vnum				= fread_number( fp );
	if ( vnum == 0 )
	    break;

	fBootDb = FALSE;
	if ( get_room_index( vnum ) != NULL )
	{
	    bug( "Load_rooms: vnum %d duplicated.", vnum );
	    exit( 1 );
	}
	fBootDb = TRUE;

	pRoomIndex			= (ROOM_INDEX_DATA *)alloc_perm( sizeof(*pRoomIndex) );
	pRoomIndex->owner		= str_dup("");
	pRoomIndex->people		= NULL;
	pRoomIndex->contents		= NULL;
	pRoomIndex->extra_descr		= NULL;
	pRoomIndex->area		= area_last;
	pRoomIndex->vnum		= vnum;
	pRoomIndex->name		= fread_string( fp );
	pRoomIndex->description		= fread_string( fp );
	/* Area number */		  fread_number( fp );
	pRoomIndex->room_flags		= fread_flag( fp );
	/* horrible hack */
  	if ( 3000 <= vnum && vnum < 3400)
	   SET_BIT(pRoomIndex->room_flags,ROOM_LAW);
	pRoomIndex->sector_type		= fread_number( fp );
	pRoomIndex->light		= 0;
	for ( door = 0; door <= 5; door++ )
	    pRoomIndex->exit[door] = NULL;

	/* defaults */
	pRoomIndex->heal_rate = 100;
	pRoomIndex->mana_rate = 100;

	for ( ; ; )
	{
	    letter = fread_letter( fp );

	    if ( letter == 'S' )
		break;

	    if ( letter == 'H') /* healing room */
		pRoomIndex->heal_rate = fread_number(fp);

	    else if ( letter == 'M') /* mana room */
		pRoomIndex->mana_rate = fread_number(fp);

	   else if ( letter == 'C') /* clan */
	   {
		if (pRoomIndex->clan)
	  	{
		    bug("Load_rooms: duplicate clan fields.",0);
		    exit(1);
		}
		pRoomIndex->clan = clan_lookup(fread_string(fp));
	    }


	    else if ( letter == 'D' )
	    {
		EXIT_DATA *pexit;
		int locks;

		door = fread_number( fp );
		if ( door < 0 || door > 5 )
		{
		    bug( "Fread_rooms: vnum %d has bad door number.", vnum );
		    exit( 1 );
		}

		pexit			= (EXIT_DATA *)alloc_perm( sizeof(*pexit) );
		pexit->description	= fread_string( fp );
		pexit->keyword		= fread_string( fp );
		pexit->exit_info	= 0;
		locks			= fread_number( fp );
		pexit->key		= fread_number( fp );
		pexit->u1.vnum		= fread_number( fp );

		switch ( locks )
		{
		case 1: pexit->exit_info = EX_ISDOOR;                break;
		case 2: pexit->exit_info = EX_ISDOOR | EX_PICKPROOF; break;
		case 3: pexit->exit_info = EX_ISDOOR | EX_NOPASS;    break;
		case 4: pexit->exit_info = EX_ISDOOR|EX_NOPASS|EX_PICKPROOF;
			break;
		}

		pRoomIndex->exit[door]	= pexit;
		pRoomIndex->old_exit[door] = pexit;
    if (db_run == 1)
    {
    mysql_write_exit( pexit, pRoomIndex->vnum, door );
  }
		top_exit++;
	    }
	    else if ( letter == 'E' )
	    {
		EXTRA_DESCR_DATA *ed;

		ed			= (EXTRA_DESCR_DATA *)alloc_perm( sizeof(*ed) );
		ed->keyword		= fread_string( fp );
		ed->description		= fread_string( fp );
		ed->next		= pRoomIndex->extra_descr;
		pRoomIndex->extra_descr	= ed;
    if (db_run == 1)
    {
    mysql_write_room_extra_description(ed, pRoomIndex->vnum);
  }
		top_ed++;
	    }

	    else if (letter == 'O')
	    {
		if (pRoomIndex->owner[0] != '\0')
		{
		    bug("Load_rooms: duplicate owner.",0);
		    exit(1);
		}

		pRoomIndex->owner = fread_string(fp);
	    }

	    else
	    {
		bug( "Load_rooms: vnum %d has flag not 'DES'.", vnum );
		exit( 1 );
	    }
	}
  if (db_run == 1)
  {
  mysql_write_room(pRoomIndex);
}

	iHash			= vnum % MAX_KEY_HASH;
	pRoomIndex->next	= room_index_hash[iHash];
	room_index_hash[iHash]	= pRoomIndex;
	top_room++;
    }

    return;
}



/*
 * Snarf a shop section.
 */
void load_shops( FILE *fp )
{
    SHOP_DATA *pShop;

    for ( ; ; )
    {
	MOB_INDEX_DATA *pMobIndex;
	int iTrade;

	pShop			= (SHOP_DATA *)alloc_perm( sizeof(*pShop) );
	pShop->keeper		= fread_number( fp );
	if ( pShop->keeper == 0 )
	    break;
	for ( iTrade = 0; iTrade < MAX_TRADE; iTrade++ )
	    pShop->buy_type[iTrade]	= fread_number( fp );
	pShop->profit_buy	= fread_number( fp );
	pShop->profit_sell	= fread_number( fp );
	pShop->open_hour	= fread_number( fp );
	pShop->close_hour	= fread_number( fp );
				  fread_to_eol( fp );
	pMobIndex		= get_mob_index( pShop->keeper );
	pMobIndex->pShop	= pShop;
  if (db_run == 1)
  {
  mysql_write_shop( pShop );
}
	if ( shop_first == NULL )
	    shop_first = pShop;
	if ( shop_last  != NULL )
	    shop_last->next = pShop;

	shop_last	= pShop;
	pShop->next	= NULL;
	top_shop++;
    }

    return;
}


/*
 * Snarf spec proc declarations.
 */
void load_specials( FILE *fp )
{
  char *temp;

    for ( ; ; )
    {
	MOB_INDEX_DATA *pMobIndex;
	char letter;

	switch ( letter = fread_letter( fp ) )
	{
	default:
	    bug( "Load_specials: letter '%c' not *MS.", letter );
	    exit( 1 );

	case 'S':
	    return;

	case '*':
	    break;

	case 'M':
	    pMobIndex		= get_mob_index	( fread_number ( fp ) );
      temp = fread_word(fp);
	    pMobIndex->spec_fun	= spec_lookup	( temp );
	    if ( pMobIndex->spec_fun == 0 )
	    {
		bug( "Load_specials: 'M': vnum %d.", pMobIndex->vnum );
		exit( 1 );
	    }
      if (db_run == 1)
      {
      mysql_write_mobile_special( pMobIndex, temp );
    }
	    break;
	}

	fread_to_eol( fp );
    }
}


/*
 * Translate all room exits from virtual to real.
 * Has to be done after all rooms are read in.
 * Check for bad reverse exits.
 */
void fix_exits( void )
{
    extern const sh_int rev_dir [];
    char buf[MAX_STRING_LENGTH];
    ROOM_INDEX_DATA *pRoomIndex;
    ROOM_INDEX_DATA *to_room;
    EXIT_DATA *pexit;
    EXIT_DATA *pexit_rev;
    int iHash;
    int door;

    for ( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
    {
	for ( pRoomIndex  = room_index_hash[iHash];
	      pRoomIndex != NULL;
	      pRoomIndex  = pRoomIndex->next )
	{
	    bool fexit;

	    fexit = FALSE;
	    for ( door = 0; door <= 5; door++ )
	    {
		if ( ( pexit = pRoomIndex->exit[door] ) != NULL )
		{
		    if ( pexit->u1.vnum <= 0
		    || get_room_index(pexit->u1.vnum) == NULL)
			pexit->u1.to_room = NULL;
		    else
		    {
		   	fexit = TRUE;
			pexit->u1.to_room = get_room_index( pexit->u1.vnum );
		    }
		}
	    }
	    if (!fexit)
		SET_BIT(pRoomIndex->room_flags,ROOM_NO_MOB);
	}
    }

    for ( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
    {
	for ( pRoomIndex  = room_index_hash[iHash];
	      pRoomIndex != NULL;
	      pRoomIndex  = pRoomIndex->next )
	{
	    for ( door = 0; door <= 5; door++ )
	    {
		if ( ( pexit     = pRoomIndex->exit[door]       ) != NULL
		&&   ( to_room   = pexit->u1.to_room            ) != NULL
		&&   ( pexit_rev = to_room->exit[rev_dir[door]] ) != NULL
		&&   pexit_rev->u1.to_room != pRoomIndex
		&&   (pRoomIndex->vnum < 1200 || pRoomIndex->vnum > 1299))
		{
		    sprintf( buf, "Fix_exits: %d:%d -> %d:%d -> %d.",
			pRoomIndex->vnum, door,
			to_room->vnum,    rev_dir[door],
			(pexit_rev->u1.to_room == NULL)
			    ? 0 : pexit_rev->u1.to_room->vnum );
		    bug( buf, 0 );
		}
	    }
	}
    }

    return;
}



/*
 * Repopulate areas periodically.
 */
void area_update( void )
{
    AREA_DATA *pArea;
    char buf[MAX_STRING_LENGTH];

    for ( pArea = area_first; pArea != NULL; pArea = pArea->next )
    {

	if ( ++pArea->age < 3 )
	    continue;

	/*
	 * Check age and reset.
	 * Note: Mud School resets every 3 minutes (not 15).
	 */
	if ( (!pArea->empty && (pArea->nplayer == 0 || pArea->age >= 15))
	||    pArea->age >= 31)
	{
	    ROOM_INDEX_DATA *pRoomIndex;

	    reset_area( pArea );
	    sprintf(buf,"%s has just been reset.",pArea->name);
	    wiznet(buf,NULL,NULL,WIZ_RESETS,0,0);

	    pArea->age = number_range( 0, 3 );
	    pRoomIndex = get_room_index( ROOM_VNUM_SCHOOL );
	    if ( pRoomIndex != NULL && pArea == pRoomIndex->area )
		pArea->age = 15 - 2;
	    else if (pArea->nplayer == 0)
		pArea->empty = TRUE;
	}
    }

    return;
}



/*
 * Reset one area.
 */
void reset_area( AREA_DATA *pArea )
{
    RESET_DATA *pReset;
    CHAR_DATA *mob;
    bool last;
    int level;

    mob 	= NULL;
    last	= TRUE;
    level	= 0;
	
    for ( pReset = pArea->reset_first; pReset != NULL; pReset = pReset->next )
    {
	ROOM_INDEX_DATA *pRoomIndex;
	MOB_INDEX_DATA *pMobIndex;
	OBJ_INDEX_DATA *pObjIndex;
	OBJ_INDEX_DATA *pObjToIndex;
	EXIT_DATA *pexit;
	OBJ_DATA *obj;
	OBJ_DATA *obj_to;
	int count, limit;

	switch ( pReset->command )
	{
	default:
	    bug( "Reset_area: bad command %c.", pReset->command );
	    break;

	case 'M':
	    if ( ( pMobIndex = get_mob_index( pReset->arg1 ) ) == NULL )
	    {
		bug( "Reset_area: 'M': bad vnum %d.", pReset->arg1 );
		continue;
	    }

	    if ( ( pRoomIndex = get_room_index( pReset->arg3 ) ) == NULL )
	    {
		bug( "Reset_area: 'R': bad vnum %d.", pReset->arg3 );
		continue;
	    }

	    if ( pMobIndex->count >= pReset->arg2 )
	    {
		last = FALSE;
		break;
	    }

	    count = 0;
	    for (mob = pRoomIndex->people; mob != NULL; mob = mob->next_in_room)
		if (mob->pIndexData == pMobIndex)
		{
		    count++;
		    if (count >= pReset->arg4)
		    {
		    	last = FALSE;
		    	break;
		    }
		}

	    if (count >= pReset->arg4)
		break;

	    mob = create_mobile( pMobIndex );

	    /*
	     * Check for pet shop.
	     */
	    {
		ROOM_INDEX_DATA *pRoomIndexPrev;
		pRoomIndexPrev = get_room_index( pRoomIndex->vnum - 1 );
		if ( pRoomIndexPrev != NULL
		&&   IS_SET(pRoomIndexPrev->room_flags, ROOM_PET_SHOP) )
		    SET_BIT(mob->act, ACT_PET);
	    }

	    /* set area */
	    mob->zone = pRoomIndex->area;

	    char_to_room( mob, pRoomIndex );
	    level = URANGE( 0, mob->level - 2, LEVEL_HERO - 1 );
	    last  = TRUE;
	    break;

	case 'O':
	    if ( ( pObjIndex = get_obj_index( pReset->arg1 ) ) == NULL )
	    {
		bug( "Reset_area: 'O': bad vnum %d.", pReset->arg1 );
		continue;
	    }

	    if ( ( pRoomIndex = get_room_index( pReset->arg3 ) ) == NULL )
	    {
		bug( "Reset_area: 'R': bad vnum %d.", pReset->arg3 );
		continue;
	    }

	    if ( pArea->nplayer > 0
	    ||   count_obj_list( pObjIndex, pRoomIndex->contents ) > 0 )
	    {
		last = FALSE;
		break;
	    }

	    obj       = create_object( pObjIndex, UMIN(number_fuzzy(level),
						       LEVEL_HERO - 1) );
	    obj->cost = 0;
	    obj_to_room( obj, pRoomIndex );
	    last = TRUE;
	    break;

	case 'P':
	    if ( ( pObjIndex = get_obj_index( pReset->arg1 ) ) == NULL )
	    {
		bug( "Reset_area: 'P': bad vnum %d.", pReset->arg1 );
		continue;
	    }

	    if ( ( pObjToIndex = get_obj_index( pReset->arg3 ) ) == NULL )
	    {
		bug( "Reset_area: 'P': bad vnum %d.", pReset->arg3 );
		continue;
	    }

            if (pReset->arg2 > 50) /* old format */
                limit = 6;
            else if (pReset->arg2 == -1) /* no limit */
                limit = 999;
            else
                limit = pReset->arg2;

	    if (pArea->nplayer > 0
	    || (obj_to = get_obj_type( pObjToIndex ) ) == NULL
	    || (obj_to->in_room == NULL && !last)
	    || ( pObjIndex->count >= limit && number_range(0,4) != 0)
	    || (count = count_obj_list(pObjIndex,obj_to->contains))
		> pReset->arg4 )
	    {
		last = FALSE;
		break;
	    }

	    while (count < pReset->arg4)
	    {
	        obj = create_object( pObjIndex, number_fuzzy(obj_to->level) );
	    	obj_to_obj( obj, obj_to );
		count++;
		if (pObjIndex->count >= limit)
		    break;
	    }
	    /* fix object lock state! */
	    obj_to->value[1] = obj_to->pIndexData->value[1];
	    last = TRUE;
	    break;

	case 'G':
	case 'E':
	    if ( ( pObjIndex = get_obj_index( pReset->arg1 ) ) == NULL )
	    {
		bug( "Reset_area: 'E' or 'G': bad vnum %d.", pReset->arg1 );
		continue;
	    }

	    if ( !last )
		break;

	    if ( mob == NULL )
	    {
		bug( "Reset_area: 'E' or 'G': null mob for vnum %d.",
		    pReset->arg1 );
		last = FALSE;
		break;
	    }

	    if ( mob->pIndexData->pShop != NULL )
	    {
		int olevel = 0,i,j;

		if (!pObjIndex->new_format)
		    switch ( pObjIndex->item_type )
		{
		case ITEM_PILL:
		case ITEM_POTION:
		case ITEM_SCROLL:
		    olevel = 53;
		    for (i = 1; i < 5; i++)
		    {
			if (pObjIndex->value[i] > 0)
			{
		    	    for (j = 0; j < 4; j++)
			    {
				olevel = UMIN(olevel,
				         skill_table[pObjIndex->value[i]].
						     skill_level[j]);
			    }
			}
		    }

		    olevel = UMAX(0,(olevel * 3 / 4) - 2);
		    break;
		case ITEM_WAND:		olevel = number_range( 10, 20 ); break;
		case ITEM_STAFF:	olevel = number_range( 15, 25 ); break;
		case ITEM_ARMOR:	olevel = number_range(  5, 15 ); break;
		case ITEM_WEAPON:	olevel = number_range(  5, 15 ); break;
		case ITEM_TREASURE:	olevel = number_range( 10, 20 ); break;
		}

		obj = create_object( pObjIndex, olevel );
		SET_BIT( obj->extra_flags, ITEM_INVENTORY );
	    }

	    else
	    {
		if (pReset->arg2 > 50) /* old format */
		    limit = 6;
		else if (pReset->arg2 == -1) /* no limit */
		    limit = 999;
		else
		    limit = pReset->arg2;

		if (pObjIndex->count < limit || number_range(0,4) == 0)
		{
		    obj=create_object(pObjIndex,UMIN(number_fuzzy(level),
		    LEVEL_HERO - 1));
		    /* error message if it is too high */
		    if (obj->level > mob->level + 3
		    ||  (obj->item_type == ITEM_WEAPON
		    &&   pReset->command == 'E'
		    &&   obj->level < mob->level -5 && obj->level < 45))
			fprintf(stderr,
			    "Err: obj %s (%d) -- %d, mob %s (%d) -- %d\n",
			    obj->short_descr,obj->pIndexData->vnum,obj->level,
			    mob->short_descr,mob->pIndexData->vnum,mob->level);
		}
		else
		    break;
	    }
	    obj_to_char( obj, mob );
	    if ( pReset->command == 'E' )
		equip_char( mob, obj, pReset->arg3 );
	    last = TRUE;
	    break;

	case 'D':
	    if ( ( pRoomIndex = get_room_index( pReset->arg1 ) ) == NULL )
	    {
		bug( "Reset_area: 'D': bad vnum %d.", pReset->arg1 );
		continue;
	    }

	    if ( ( pexit = pRoomIndex->exit[pReset->arg2] ) == NULL )
		break;

	    switch ( pReset->arg3 )
	    {
	    case 0:
		REMOVE_BIT( pexit->exit_info, EX_CLOSED );
		REMOVE_BIT( pexit->exit_info, EX_LOCKED );
		break;

	    case 1:
		SET_BIT(    pexit->exit_info, EX_CLOSED );
		REMOVE_BIT( pexit->exit_info, EX_LOCKED );
		break;

	    case 2:
		SET_BIT(    pexit->exit_info, EX_CLOSED );
		SET_BIT(    pexit->exit_info, EX_LOCKED );
		break;
	    }

	    last = TRUE;
	    break;

	case 'R':
	    if ( ( pRoomIndex = get_room_index( pReset->arg1 ) ) == NULL )
	    {
		bug( "Reset_area: 'R': bad vnum %d.", pReset->arg1 );
		continue;
	    }

	    {
		int d0;
		int d1;

		for ( d0 = 0; d0 < pReset->arg2 - 1; d0++ )
		{
		    d1                   = number_range( d0, pReset->arg2-1 );
		    pexit                = pRoomIndex->exit[d0];
		    pRoomIndex->exit[d0] = pRoomIndex->exit[d1];
		    pRoomIndex->exit[d1] = pexit;
		}
	    }
	    break;
	}
    }

    return;
}



/*
 * Create an instance of a mobile.
 */
CHAR_DATA *create_mobile( MOB_INDEX_DATA *pMobIndex )
{
    CHAR_DATA *mob;
    int i;
    AFFECT_DATA af;

    mobile_count++;

    if ( pMobIndex == NULL )
    {
	bug( "Create_mobile: NULL pMobIndex.", 0 );
	exit( 1 );
    }

    mob = new_char();

    mob->pIndexData	= pMobIndex;

    mob->name		= pMobIndex->player_name;
    mob->id		= get_mob_id();
    mob->short_descr	= pMobIndex->short_descr;
    mob->long_descr	= pMobIndex->long_descr;
    mob->description	= pMobIndex->description;
    mob->spec_fun	= pMobIndex->spec_fun;
    mob->prompt		= NULL;

    if (pMobIndex->wealth == 0)
    {
	mob->silver = 0;
	mob->gold   = 0;
    }
    else
    {
	long wealth;

	wealth = number_range(pMobIndex->wealth/2, 3 * pMobIndex->wealth/2);
	mob->gold = number_range(wealth/200,wealth/100);
	mob->silver = wealth - (mob->gold * 100);
    }

    if (pMobIndex->new_format)
    /* load in new style */
    {
	/* read from prototype */
 	mob->group		= pMobIndex->group;
	mob->act 		= pMobIndex->act;
	mob->comm		= COMM_NOCHANNELS|COMM_NOSHOUT|COMM_NOTELL;
	mob->affected_by	= pMobIndex->affected_by;
	mob->alignment		= pMobIndex->alignment;
	mob->level		= pMobIndex->level;
	mob->hitroll		= pMobIndex->hitroll;
	mob->damroll		= pMobIndex->damage[DICE_BONUS];
	mob->max_hit		= dice(pMobIndex->hit[DICE_NUMBER],
				       pMobIndex->hit[DICE_TYPE])
				  + pMobIndex->hit[DICE_BONUS];
	mob->hit		= mob->max_hit;
	mob->max_mana		= dice(pMobIndex->mana[DICE_NUMBER],
				       pMobIndex->mana[DICE_TYPE])
				  + pMobIndex->mana[DICE_BONUS];
	mob->mana		= mob->max_mana;
	mob->damage[DICE_NUMBER]= pMobIndex->damage[DICE_NUMBER];
	mob->damage[DICE_TYPE]	= pMobIndex->damage[DICE_TYPE];
	mob->dam_type		= pMobIndex->dam_type;
        if (mob->dam_type == 0)
    	    switch(number_range(1,3))
            {
                case (1): mob->dam_type = 3;        break;  /* slash */
                case (2): mob->dam_type = 7;        break;  /* pound */
                case (3): mob->dam_type = 11;       break;  /* pierce */
            }
	for (i = 0; i < 4; i++)
	    mob->armor[i]	= pMobIndex->ac[i];
	mob->off_flags		= pMobIndex->off_flags;
	mob->imm_flags		= pMobIndex->imm_flags;
	mob->res_flags		= pMobIndex->res_flags;
	mob->vuln_flags		= pMobIndex->vuln_flags;
	mob->start_pos		= pMobIndex->start_pos;
	mob->default_pos	= pMobIndex->default_pos;
	mob->sex		= pMobIndex->sex;
        if (mob->sex == 3) /* random sex */
            mob->sex = number_range(1,2);
	mob->race		= pMobIndex->race;
	mob->form		= pMobIndex->form;
	mob->parts		= pMobIndex->parts;
	mob->size		= pMobIndex->size;
	mob->material		= str_dup(pMobIndex->material);

	/* computed on the spot */

    	for (i = 0; i < MAX_STATS; i ++)
            mob->perm_stat[i] = UMIN(25,11 + mob->level/4);

        if (IS_SET(mob->act,ACT_WARRIOR))
        {
            mob->perm_stat[STAT_STR] += 3;
            mob->perm_stat[STAT_INT] -= 1;
            mob->perm_stat[STAT_CON] += 2;
        }

        if (IS_SET(mob->act,ACT_THIEF))
        {
            mob->perm_stat[STAT_DEX] += 3;
            mob->perm_stat[STAT_INT] += 1;
            mob->perm_stat[STAT_WIS] -= 1;
        }

        if (IS_SET(mob->act,ACT_CLERIC))
        {
            mob->perm_stat[STAT_WIS] += 3;
            mob->perm_stat[STAT_DEX] -= 1;
            mob->perm_stat[STAT_STR] += 1;
        }

        if (IS_SET(mob->act,ACT_MAGE))
        {
            mob->perm_stat[STAT_INT] += 3;
            mob->perm_stat[STAT_STR] -= 1;
            mob->perm_stat[STAT_DEX] += 1;
        }

        if (IS_SET(mob->off_flags,OFF_FAST))
            mob->perm_stat[STAT_DEX] += 2;

        mob->perm_stat[STAT_STR] += mob->size - SIZE_MEDIUM;
        mob->perm_stat[STAT_CON] += (mob->size - SIZE_MEDIUM) / 2;

	/* let's get some spell action */
	if (IS_AFFECTED(mob,AFF_SANCTUARY))
	{
	    af.where	 = TO_AFFECTS;
	    af.type      = skill_lookup("sanctuary");
	    af.level     = mob->level;
	    af.duration  = -1;
	    af.location  = APPLY_NONE;
	    af.modifier  = 0;
	    af.bitvector = AFF_SANCTUARY;
	    affect_to_char( mob, &af );
	}

	if (IS_AFFECTED(mob,AFF_HASTE))
	{
	    af.where	 = TO_AFFECTS;
	    af.type      = skill_lookup("haste");
    	    af.level     = mob->level;
      	    af.duration  = -1;
    	    af.location  = APPLY_DEX;
    	    af.modifier  = 1 + (mob->level >= 18) + (mob->level >= 25) +
			   (mob->level >= 32);
    	    af.bitvector = AFF_HASTE;
    	    affect_to_char( mob, &af );
	}

	if (IS_AFFECTED(mob,AFF_PROTECT_EVIL))
	{
	    af.where	 = TO_AFFECTS;
	    af.type	 = skill_lookup("protection evil");
	    af.level	 = mob->level;
	    af.duration	 = -1;
	    af.location	 = APPLY_SAVES;
	    af.modifier	 = -1;
	    af.bitvector = AFF_PROTECT_EVIL;
	    affect_to_char(mob,&af);
	}

        if (IS_AFFECTED(mob,AFF_PROTECT_GOOD))
        {
	    af.where	 = TO_AFFECTS;
            af.type      = skill_lookup("protection good");
            af.level     = mob->level;
            af.duration  = -1;
            af.location  = APPLY_SAVES;
            af.modifier  = -1;
            af.bitvector = AFF_PROTECT_GOOD;
            affect_to_char(mob,&af);
        }
    }
    else /* read in old format and convert */
    {
	mob->act		= pMobIndex->act;
	mob->affected_by	= pMobIndex->affected_by;
	mob->alignment		= pMobIndex->alignment;
	mob->level		= pMobIndex->level;
	mob->hitroll		= pMobIndex->hitroll;
	mob->damroll		= 0;
	mob->max_hit		= mob->level * 8 + number_range(
					mob->level * mob->level/4,
					mob->level * mob->level);
	mob->max_hit *= 10/9;
	mob->hit		= mob->max_hit;
	mob->max_mana		= 100 + dice(mob->level,10);
	mob->mana		= mob->max_mana;
	switch(number_range(1,3))
	{
	    case (1): mob->dam_type = 3; 	break;  /* slash */
	    case (2): mob->dam_type = 7;	break;  /* pound */
	    case (3): mob->dam_type = 11;	break;  /* pierce */
	}
	for (i = 0; i < 3; i++)
	    mob->armor[i]	= interpolate(mob->level,100,-100);
	mob->armor[3]		= interpolate(mob->level,100,0);
	mob->race		= pMobIndex->race;
	mob->off_flags		= pMobIndex->off_flags;
	mob->imm_flags		= pMobIndex->imm_flags;
	mob->res_flags		= pMobIndex->res_flags;
	mob->vuln_flags		= pMobIndex->vuln_flags;
	mob->start_pos		= pMobIndex->start_pos;
	mob->default_pos	= pMobIndex->default_pos;
	mob->sex		= pMobIndex->sex;
	mob->form		= pMobIndex->form;
	mob->parts		= pMobIndex->parts;
	mob->size		= SIZE_MEDIUM;
	mob->material		= (char*)"";

        for (i = 0; i < MAX_STATS; i ++)
            mob->perm_stat[i] = 11 + mob->level/4;
    }

    mob->position = mob->start_pos;


    /* link the mob to the world list */
    mob->next		= char_list;
    char_list		= mob;
    pMobIndex->count++;
    return mob;
}

/* duplicate a mobile exactly -- except inventory */
void clone_mobile(CHAR_DATA *parent, CHAR_DATA *clone)
{
    int i;
    AFFECT_DATA *paf;

    if ( parent == NULL || clone == NULL || !IS_NPC(parent))
	return;

    /* start fixing values */
    clone->name 	= str_dup(parent->name);
    clone->version	= parent->version;
    clone->short_descr	= str_dup(parent->short_descr);
    clone->long_descr	= str_dup(parent->long_descr);
    clone->description	= str_dup(parent->description);
    clone->group	= parent->group;
    clone->sex		= parent->sex;
    clone->race		= parent->race;
    clone->level	= parent->level;
    clone->trust	= 0;
    clone->timer	= parent->timer;
    clone->wait		= parent->wait;
    clone->hit		= parent->hit;
    clone->max_hit	= parent->max_hit;
    clone->mana		= parent->mana;
    clone->max_mana	= parent->max_mana;
    clone->move		= parent->move;
    clone->max_move	= parent->max_move;
    clone->gold		= parent->gold;
    clone->silver	= parent->silver;
    clone->exp		= parent->exp;
    clone->act		= parent->act;
    clone->comm		= parent->comm;
    clone->imm_flags	= parent->imm_flags;
    clone->res_flags	= parent->res_flags;
    clone->vuln_flags	= parent->vuln_flags;
    clone->invis_level	= parent->invis_level;
    clone->affected_by	= parent->affected_by;
    clone->position	= parent->position;
    clone->practice	= parent->practice;
    clone->train	= parent->train;
    clone->saving_throw	= parent->saving_throw;
    clone->alignment	= parent->alignment;
    clone->hitroll	= parent->hitroll;
    clone->damroll	= parent->damroll;
    clone->wimpy	= parent->wimpy;
    clone->form		= parent->form;
    clone->parts	= parent->parts;
    clone->size		= parent->size;
    clone->material	= str_dup(parent->material);
    clone->off_flags	= parent->off_flags;
    clone->dam_type	= parent->dam_type;
    clone->start_pos	= parent->start_pos;
    clone->default_pos	= parent->default_pos;
    clone->spec_fun	= parent->spec_fun;

    for (i = 0; i < 4; i++)
    	clone->armor[i]	= parent->armor[i];

    for (i = 0; i < MAX_STATS; i++)
    {
	clone->perm_stat[i]	= parent->perm_stat[i];
	clone->mod_stat[i]	= parent->mod_stat[i];
    }

    for (i = 0; i < 3; i++)
	clone->damage[i]	= parent->damage[i];

    /* now add the affects */
    for (paf = parent->affected; paf != NULL; paf = paf->next)
        affect_to_char(clone,paf);

}




/*
 * Create an instance of an object.
 */
OBJ_DATA *create_object( OBJ_INDEX_DATA *pObjIndex, int level )
{
    AFFECT_DATA *paf;
    OBJ_DATA *obj;
    int i;

    if ( pObjIndex == NULL )
    {
	bug( "Create_object: NULL pObjIndex.", 0 );
	exit( 1 );
    }

    obj = new_obj();

    obj->pIndexData	= pObjIndex;
    obj->in_room	= NULL;
    obj->enchanted	= FALSE;

    if (pObjIndex->new_format)
	obj->level = pObjIndex->level;
    else
	obj->level		= UMAX(0,level);
    obj->wear_loc	= -1;

    obj->name		= pObjIndex->name;
    obj->short_descr	= pObjIndex->short_descr;
    obj->description	= pObjIndex->description;
    obj->material	= str_dup(pObjIndex->material);
    obj->item_type	= pObjIndex->item_type;
    obj->extra_flags	= pObjIndex->extra_flags;
    obj->wear_flags	= pObjIndex->wear_flags;
    obj->value[0]	= pObjIndex->value[0];
    obj->value[1]	= pObjIndex->value[1];
    obj->value[2]	= pObjIndex->value[2];
    obj->value[3]	= pObjIndex->value[3];
    obj->value[4]	= pObjIndex->value[4];
    obj->weight		= pObjIndex->weight;

    if (level == -1 || pObjIndex->new_format)
	obj->cost	= pObjIndex->cost;
    else
    	obj->cost	= number_fuzzy( 10 )
			* number_fuzzy( level ) * number_fuzzy( level );

    /*
     * Mess with object properties.
     */
    switch ( obj->item_type )
    {
    default:
	bug( "Read_object: vnum %d bad type.", pObjIndex->vnum );
	break;

    case ITEM_LIGHT:
	if (obj->value[2] == 999)
		obj->value[2] = -1;
	break;

    case ITEM_FURNITURE:
    case ITEM_TRASH:
    case ITEM_CONTAINER:
    case ITEM_DRINK_CON:
    case ITEM_KEY:
    case ITEM_FOOD:
    case ITEM_BOAT:
    case ITEM_CORPSE_NPC:
    case ITEM_CORPSE_PC:
    case ITEM_FOUNTAIN:
    case ITEM_MAP:
    case ITEM_CLOTHING:
    case ITEM_PORTAL:
	if (!pObjIndex->new_format)
	    obj->cost /= 5;
	break;

    case ITEM_TREASURE:
    case ITEM_WARP_STONE:
    case ITEM_ROOM_KEY:
    case ITEM_GEM:
    case ITEM_JEWELRY:
	break;

    case ITEM_JUKEBOX:
	for (i = 0; i < 5; i++)
	   obj->value[i] = -1;
	break;

    case ITEM_SCROLL:
	if (level != -1 && !pObjIndex->new_format)
	    obj->value[0]	= number_fuzzy( obj->value[0] );
	break;

    case ITEM_WAND:
    case ITEM_STAFF:
	if (level != -1 && !pObjIndex->new_format)
	{
	    obj->value[0]	= number_fuzzy( obj->value[0] );
	    obj->value[1]	= number_fuzzy( obj->value[1] );
	    obj->value[2]	= obj->value[1];
	}
	if (!pObjIndex->new_format)
	    obj->cost *= 2;
	break;

    case ITEM_WEAPON:
	if (level != -1 && !pObjIndex->new_format)
	{
	    obj->value[1] = number_fuzzy( number_fuzzy( 1 * level / 4 + 2 ) );
	    obj->value[2] = number_fuzzy( number_fuzzy( 3 * level / 4 + 6 ) );
	}
	break;

    case ITEM_ARMOR:
	if (level != -1 && !pObjIndex->new_format)
	{
	    obj->value[0]	= number_fuzzy( level / 5 + 3 );
	    obj->value[1]	= number_fuzzy( level / 5 + 3 );
	    obj->value[2]	= number_fuzzy( level / 5 + 3 );
	}
	break;

    case ITEM_POTION:
    case ITEM_PILL:
	if (level != -1 && !pObjIndex->new_format)
	    obj->value[0] = number_fuzzy( number_fuzzy( obj->value[0] ) );
	break;

    case ITEM_MONEY:
	if (!pObjIndex->new_format)
	    obj->value[0]	= obj->cost;
	break;
    }

    for (paf = pObjIndex->affected; paf != NULL; paf = paf->next)
	if ( paf->location == APPLY_SPELL_AFFECT )
	    affect_to_obj(obj,paf);

    obj->next		= object_list;
    object_list		= obj;
    pObjIndex->count++;

    return obj;
}

/* duplicate an object exactly -- except contents */
void clone_object(OBJ_DATA *parent, OBJ_DATA *clone)
{
    int i;
    AFFECT_DATA *paf;
    EXTRA_DESCR_DATA *ed,*ed_new;

    if (parent == NULL || clone == NULL)
	return;

    /* start fixing the object */
    clone->name 	= str_dup(parent->name);
    clone->short_descr 	= str_dup(parent->short_descr);
    clone->description	= str_dup(parent->description);
    clone->item_type	= parent->item_type;
    clone->extra_flags	= parent->extra_flags;
    clone->wear_flags	= parent->wear_flags;
    clone->weight	= parent->weight;
    clone->cost		= parent->cost;
    clone->level	= parent->level;
    clone->condition	= parent->condition;
    clone->material	= str_dup(parent->material);
    clone->timer	= parent->timer;

    for (i = 0;  i < 5; i ++)
	clone->value[i]	= parent->value[i];

    /* affects */
    clone->enchanted	= parent->enchanted;

    for (paf = parent->affected; paf != NULL; paf = paf->next)
	affect_to_obj(clone,paf);

    /* extended desc */
    for (ed = parent->extra_descr; ed != NULL; ed = ed->next)
    {
        ed_new                  = new_extra_descr();
        ed_new->keyword    	= str_dup( ed->keyword);
        ed_new->description     = str_dup( ed->description );
        ed_new->next           	= clone->extra_descr;
        clone->extra_descr  	= ed_new;
    }

}



/*
 * Clear a new character.
 */
void clear_char( CHAR_DATA *ch )
{
    static CHAR_DATA ch_zero;
    int i;

    *ch				= ch_zero;
    ch->name			= &str_empty[0];
    ch->short_descr		= &str_empty[0];
    ch->long_descr		= &str_empty[0];
    ch->description		= &str_empty[0];
    ch->prompt                  = &str_empty[0];
    ch->logon			= current_time;
    ch->lines			= PAGELEN;
    for (i = 0; i < 4; i++)
    	ch->armor[i]		= 100;
    ch->position		= POS_STANDING;
    ch->hit			= 20;
    ch->max_hit			= 20;
    ch->mana			= 100;
    ch->max_mana		= 100;
    ch->move			= 100;
    ch->max_move		= 100;
    ch->on			= NULL;
    for (i = 0; i < MAX_STATS; i ++)
    {
	ch->perm_stat[i] = 13;
	ch->mod_stat[i] = 0;
    }
    return;
}

/*
 * Get an extra description from a list.
 */
char *get_extra_descr( const char *name, EXTRA_DESCR_DATA *ed )
{
    for ( ; ed != NULL; ed = ed->next )
    {
	if ( is_name( (char *) name, ed->keyword ) )
	    return ed->description;
    }
    return NULL;
}



/*
 * Translates mob virtual number to its mob index struct.
 * Hash table lookup.
 */
MOB_INDEX_DATA *get_mob_index( int vnum )
{
    MOB_INDEX_DATA *pMobIndex;

    for ( pMobIndex  = mob_index_hash[vnum % MAX_KEY_HASH];
	  pMobIndex != NULL;
	  pMobIndex  = pMobIndex->next )
    {
	if ( pMobIndex->vnum == vnum )
	    return pMobIndex;
    }

    if ( fBootDb )
    {
	bug( "Get_mob_index: bad vnum %d.", vnum );
	exit( 1 );
    }

    return NULL;
}



/*
 * Translates mob virtual number to its obj index struct.
 * Hash table lookup.
 */
OBJ_INDEX_DATA *get_obj_index( int vnum )
{
    OBJ_INDEX_DATA *pObjIndex;

    for ( pObjIndex  = obj_index_hash[vnum % MAX_KEY_HASH];
	  pObjIndex != NULL;
	  pObjIndex  = pObjIndex->next )
    {
	if ( pObjIndex->vnum == vnum )
	    return pObjIndex;
    }

    if ( fBootDb )
    {
	bug( "Get_obj_index: bad vnum %d.", vnum );
	exit( 1 );
    }

    return NULL;
}



/*
 * Translates mob virtual number to its room index struct.
 * Hash table lookup.
 */
ROOM_INDEX_DATA *get_room_index( int vnum )
{
    ROOM_INDEX_DATA *pRoomIndex;

    for ( pRoomIndex  = room_index_hash[vnum % MAX_KEY_HASH];
	  pRoomIndex != NULL;
	  pRoomIndex  = pRoomIndex->next )
    {
	if ( pRoomIndex->vnum == vnum )
	    return pRoomIndex;
    }

    if ( fBootDb )
    {
	bug( "Get_room_index: bad vnum %d.", vnum );
	exit( 1 );
    }

    return NULL;
}



/*
 * Read a letter from a file.
 */
char fread_letter( FILE *fp )
{
    char c;

    do
    {
	c = getc( fp );
    }
    while ( isspace(c) );

    return c;
}



/*
 * Read a number from a file.
 */
int fread_number( FILE *fp )
{
    int number;
    bool sign;
    char c;

    do
    {
	c = getc( fp );
    }
    while ( isspace(c) );

    number = 0;

    sign   = FALSE;
    if ( c == '+' )
    {
	c = getc( fp );
    }
    else if ( c == '-' )
    {
	sign = TRUE;
	c = getc( fp );
    }

    if ( !isdigit(c) )
    {
	bug( "Fread_number: bad format.", 0 );
	exit( 1 );
    }

    while ( isdigit(c) )
    {
	number = number * 10 + c - '0';
	c      = getc( fp );
    }

    if ( sign )
	number = 0 - number;

    if ( c == '|' )
	number += fread_number( fp );
    else if ( c != ' ' )
	ungetc( c, fp );

    return number;
}

long fread_flag( FILE *fp)
{
    int number;
    char c;
    bool negative = FALSE;

    do
    {
	c = getc(fp);
    }
    while ( isspace(c));

    if (c == '-')
    {
	negative = TRUE;
	c = getc(fp);
    }

    number = 0;

    if (!isdigit(c))
    {
	while (('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z'))
	{
	    number += flag_convert(c);
	    c = getc(fp);
	}
    }

    while (isdigit(c))
    {
	number = number * 10 + c - '0';
	c = getc(fp);
    }

    if (c == '|')
	number += fread_flag(fp);

    else if  ( c != ' ')
	ungetc(c,fp);

    if (negative)
	return -1 * number;

    return number;
}

long flag_convert(char letter )
{
    long bitsum = 0;
    char i;

    if ('A' <= letter && letter <= 'Z')
    {
	bitsum = 1;
	for (i = letter; i > 'A'; i--)
	    bitsum *= 2;
    }
    else if ('a' <= letter && letter <= 'z')
    {
	bitsum = 67108864; /* 2^26 */
	for (i = letter; i > 'a'; i --)
	    bitsum *= 2;
    }

    return bitsum;
}



char *mysql_fread_string( char *str )
{
    char *plast, *arr;
    int loop;

    plast = top_string + sizeof(char *);
    if ( plast > &string_space[MAX_STRING - MAX_STRING_LENGTH] )
    {
	bug( "Fread_string: MAX_STRING %d exceeded.", MAX_STRING );
	exit( 1 );
    }

    loop = 0;

    if ( strlen(str) == 0 )
	     return &str_empty[0];

    arr = (char*)alloc_mem( strlen(str) * sizeof(char ) );
    strcpy(arr,str);

    while(arr[loop] != '\0')
    {
        *plast++ = arr[loop];
        loop++;
    }
    *plast++ = '\0';

		union
		{
		    char *	pc;
		    char	rgc[sizeof(char *)];
		} u1;
		size_t ic;
		int iHash;
		char *pHash;
		char *pHashPrev;
		char *pString;

		iHash     = UMIN( MAX_KEY_HASH - 1, plast - 1 - top_string );
		for ( pHash = string_hash[iHash]; pHash; pHash = pHashPrev )
		{
		    for ( ic = 0; ic < sizeof(char *); ic++ )
			u1.rgc[ic] = pHash[ic];
		    pHashPrev = u1.pc;
		    pHash    += sizeof(char *);

		    if ( top_string[sizeof(char *)] == pHash[0]
		    &&   !strcmp( top_string+sizeof(char *)+1, pHash+1 ) )
			return pHash;
		}

		if ( fBootDb )
		{
		    pString		= top_string;
		    top_string		= plast;
		    u1.pc		= string_hash[iHash];
		    for ( ic = 0; ic < sizeof(char *); ic++ )
			pString[ic] = u1.rgc[ic];
		    string_hash[iHash]	= pString;

		    nAllocString += 1;
		    sAllocString += top_string - pString;
		    return pString + sizeof(char *);
		}
		else
		{
		    return str_dup( top_string + sizeof(char *) );
		}
}

/*
 * Read and allocate space for a string from a file.
 * These strings are read-only and shared.
 * Strings are hashed:
 *   each string prepended with hash pointer to prev string,
 *   hash code is simply the string length.
 *   this function takes 40% to 50% of boot-up time.
 */
char *fread_string( FILE *fp )
{
    char *plast;
    char c;

    plast = top_string + sizeof(char *);
    if ( plast > &string_space[MAX_STRING - MAX_STRING_LENGTH] )
    {
	bug( "Fread_string: MAX_STRING %d exceeded.", MAX_STRING );
	exit( 1 );
    }

    /*
     * Skip blanks.
     * Read first char.
     */
    do
    {
	c = getc( fp );
    }
    while ( isspace(c) );

    if ( ( *plast++ = c ) == '~' )
	return &str_empty[0];

    for ( ;; )
    {
        /*
         * Back off the char type lookup,
         *   it was too dirty for portability.
         *   -- Furey
         */

	switch ( *plast = getc(fp) )
	{
        default:
            plast++;
            break;

        case EOF:
	/* temp fix */
            bug( "Fread_string: EOF", 0 );
	    return NULL;
            /* exit( 1 ); */
            break;

        case '\n':
            plast++;
            *plast++ = '\r';
            break;

        case '\r':
            break;

        case '~':
            plast++;
	    {
		union
		{
		    char *	pc;
		    char	rgc[sizeof(char *)];
		} u1;
		size_t ic;
		int iHash;
		char *pHash;
		char *pHashPrev;
		char *pString;

		plast[-1] = '\0';
		iHash     = UMIN( MAX_KEY_HASH - 1, plast - 1 - top_string );
		for ( pHash = string_hash[iHash]; pHash; pHash = pHashPrev )
		{
		    for ( ic = 0; ic < sizeof(char *); ic++ )
			u1.rgc[ic] = pHash[ic];
		    pHashPrev = u1.pc;
		    pHash    += sizeof(char *);

		    if ( top_string[sizeof(char *)] == pHash[0]
		    &&   !strcmp( top_string+sizeof(char *)+1, pHash+1 ) )
			return pHash;
		}

		if ( fBootDb )
		{
		    pString		= top_string;
		    top_string		= plast;
		    u1.pc		= string_hash[iHash];
		    for ( ic = 0; ic < sizeof(char *); ic++ )
			pString[ic] = u1.rgc[ic];
		    string_hash[iHash]	= pString;

		    nAllocString += 1;
		    sAllocString += top_string - pString;
		    return pString + sizeof(char *);
		}
		else
		{
		    return str_dup( top_string + sizeof(char *) );
		}
	    }
	}
    }
}

char *fread_string_eol( FILE *fp )
{
    static bool char_special[256-EOF];
    char *plast;
    char c;

    if ( char_special[EOF-EOF] != TRUE )
    {
        char_special[EOF -  EOF] = TRUE;
        char_special['\n' - EOF] = TRUE;
        char_special['\r' - EOF] = TRUE;
    }

    plast = top_string + sizeof(char *);
    if ( plast > &string_space[MAX_STRING - MAX_STRING_LENGTH] )
    {
        bug( "Fread_string: MAX_STRING %d exceeded.", MAX_STRING );
        exit( 1 );
    }

    /*
     * Skip blanks.
     * Read first char.
     */
    do
    {
        c = getc( fp );
    }
    while ( isspace(c) );

    if ( ( *plast++ = c ) == '\n')
        return &str_empty[0];

    for ( ;; )
    {
        if ( !char_special[ ( *plast++ = getc( fp ) ) - EOF ] )
            continue;

        switch ( plast[-1] )
        {
        default:
            break;

        case EOF:
            bug( "Fread_string_eol  EOF", 0 );
            exit( 1 );
            break;

        case '\n':  case '\r':
            {
                union
                {
                    char *      pc;
                    char        rgc[sizeof(char *)];
                } u1;
                size_t ic;
                int iHash;
                char *pHash;
                char *pHashPrev;
                char *pString;

                plast[-1] = '\0';
                iHash     = UMIN( MAX_KEY_HASH - 1, plast - 1 - top_string );
                for ( pHash = string_hash[iHash]; pHash; pHash = pHashPrev )
                {
                    for ( ic = 0; ic < sizeof(char *); ic++ )
                        u1.rgc[ic] = pHash[ic];
                    pHashPrev = u1.pc;
                    pHash    += sizeof(char *);

                    if ( top_string[sizeof(char *)] == pHash[0]
                    &&   !strcmp( top_string+sizeof(char *)+1, pHash+1 ) )
                        return pHash;
                }

                if ( fBootDb )
                {
                    pString             = top_string;
                    top_string          = plast;
                    u1.pc               = string_hash[iHash];
                    for ( ic = 0; ic < sizeof(char *); ic++ )
                        pString[ic] = u1.rgc[ic];
                    string_hash[iHash]  = pString;

                    nAllocString += 1;
                    sAllocString += top_string - pString;
                    return pString + sizeof(char *);
                }
                else
                {
                    return str_dup( top_string + sizeof(char *) );
                }
            }
        }
    }
}



/*
 * Read to end of line (for comments).
 */
void fread_to_eol( FILE *fp )
{
    char c;

    do
    {
	c = getc( fp );
    }
    while ( c != '\n' && c != '\r' );

    do
    {
	c = getc( fp );
    }
    while ( c == '\n' || c == '\r' );

    ungetc( c, fp );
    return;
}



/*
 * Read one word (into static buffer).
 */
char *fread_word( FILE *fp )
{
    static char word[MAX_INPUT_LENGTH];
    char *pword;
    char cEnd;

    do
    {
	cEnd = getc( fp );
    }
    while ( isspace( cEnd ) );

    if ( cEnd == '\'' || cEnd == '"' )
    {
	pword   = word;
    }
    else
    {
	word[0] = cEnd;
	pword   = word+1;
	cEnd    = ' ';
    }

    for ( ; pword < word + MAX_INPUT_LENGTH; pword++ )
    {
	*pword = getc( fp );
	if ( cEnd == ' ' ? isspace(*pword) : *pword == cEnd )
	{
	    if ( cEnd == ' ' )
		ungetc( *pword, fp );
	    *pword = '\0';
	    return word;
	}
    }

    bug( "Fread_word: word too long.", 0 );
    exit( 1 );
    return NULL;
}

void *alloc_mem( int sMem )
{
    void *pMem;
    long *magic;
    long iList;

    sMem += sizeof(*magic);

    for ( iList = 0; iList < MAX_MEM_LIST; iList++ )
    {
        if ( sMem <= rgSizeList[iList] )
            break;
    }

    if ( iList == MAX_MEM_LIST )
    {
        bug( "Alloc_mem: size %d too large.", sMem );
        exit( 1 );
    }

    if ( rgFreeList[iList] == NULL )
    {
        pMem              = alloc_perm( rgSizeList[iList] );
    }
    else
    {
        pMem              = rgFreeList[iList];
        rgFreeList[iList] = * ((void **) rgFreeList[iList]);
    }

    magic = (long *) pMem;
    *magic = MAGIC_NUM;
    pMem = (void *) ((long) pMem + (long) (sizeof(*magic)));

    return pMem;
}


/*
 * Free some memory.
 * Recycle it back onto the free list for blocks of that size.
 */
void free_mem( void *pMem, int sMem )
{
    long iList;
    long *magic;

    pMem = (void *) ((long) pMem - (long) sizeof(*magic));
    magic = (long *) pMem;

    if (*magic != MAGIC_NUM)
    {
        bug("Attempt to recyle invalid memory of size %d.",sMem);
        bug((char*) pMem + sizeof(*magic),0);
        return;
    }

    *magic = 0;
    sMem += sizeof(*magic);

    for ( iList = 0; iList < MAX_MEM_LIST; iList++ )
    {
        if ( sMem <= rgSizeList[iList] )
            break;
    }

    if ( iList == MAX_MEM_LIST )
    {
        bug( "Free_mem: size %d too large.", sMem );
        exit( 1 );
    }

    * ((void **) pMem) = rgFreeList[iList];
    rgFreeList[iList]  = pMem;

    return;
}



/*
 * Allocate some permanent memory.
 * Permanent memory is never freed,
 *   pointers into it may be copied safely.
 */
void *alloc_perm( int sMem )
{
    static char *pMemPerm;
    static int iMemPerm;
    void *pMem;

    while ( sMem % sizeof(long) != 0 )
	sMem++;
    if ( sMem > MAX_PERM_BLOCK )
    {
	bug( "Alloc_perm: %d too large.", sMem );
	exit( 1 );
    }

    if ( pMemPerm == NULL || iMemPerm + sMem > MAX_PERM_BLOCK )
    {
	iMemPerm = 0;
	if ( ( pMemPerm = (char *)calloc( 1, MAX_PERM_BLOCK ) ) == NULL )
	{
	    perror( "Alloc_perm" );
	    exit( 1 );
	}
    }

    pMem        = pMemPerm + iMemPerm;
    iMemPerm   += sMem;
    nAllocPerm += 1;
    sAllocPerm += sMem;
    return pMem;
}



/*
 * Duplicate a string into dynamic memory.
 * Fread_strings are read-only and shared.
 */
char *str_dup( const char *str )
{
    char *str_new;

    if ( str[0] == '\0' )
	return &str_empty[0];

    if ( str >= string_space && str < top_string )
	return (char *) str;

    str_new = (char *)alloc_mem( strlen(str) + 1 );
    strcpy( str_new, str );
    return str_new;
}



/*
 * Free a string.
 * Null is legal here to simplify callers.
 * Read-only shared strings are not touched.
 */
void free_string( char *pstr )
{
    if ( pstr == NULL
    ||   pstr == &str_empty[0]
    || ( pstr >= string_space && pstr < top_string ) )
	return;

    free_mem( pstr, strlen(pstr) + 1 );
    return;
}



void do_areas( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    AREA_DATA *pArea1;
    AREA_DATA *pArea2;
    int iArea;
    int iAreaHalf;

    if (argument[0] != '\0')
    {
	send_to_char("No argument is used with this command.\n\r",ch);
	return;
    }

    iAreaHalf = (top_area + 1) / 2;
    pArea1    = area_first;
    pArea2    = area_first;
    for ( iArea = 0; iArea < iAreaHalf; iArea++ )
	pArea2 = pArea2->next;

    for ( iArea = 0; iArea < iAreaHalf; iArea++ )
    {
	sprintf( buf, "%-39s%-39s\n\r",
	    pArea1->credits, (pArea2 != NULL) ? pArea2->credits : "" );
	send_to_char( buf, ch );
	pArea1 = pArea1->next;
	if ( pArea2 != NULL )
	    pArea2 = pArea2->next;
    }

    return;
}



void do_memory( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];

    sprintf( buf, "Affects %5d\n\r", top_affect    ); send_to_char( buf, ch );
    sprintf( buf, "Areas   %5d\n\r", top_area      ); send_to_char( buf, ch );
    sprintf( buf, "ExDes   %5d\n\r", top_ed        ); send_to_char( buf, ch );
    sprintf( buf, "Exits   %5d\n\r", top_exit      ); send_to_char( buf, ch );
    sprintf( buf, "Helps   %5d\n\r", top_help      ); send_to_char( buf, ch );
    sprintf( buf, "Socials %5d\n\r", social_count  ); send_to_char( buf, ch );
    sprintf( buf, "Mobs    %5d(%d new format)\n\r", top_mob_index,newmobs );
    send_to_char( buf, ch );
    sprintf( buf, "(in use)%5d\n\r", mobile_count  ); send_to_char( buf, ch );
    sprintf( buf, "Objs    %5d(%d new format)\n\r", top_obj_index,newobjs );
    send_to_char( buf, ch );
    sprintf( buf, "Resets  %5d\n\r", top_reset     ); send_to_char( buf, ch );
    sprintf( buf, "Rooms   %5d\n\r", top_room      ); send_to_char( buf, ch );
    sprintf( buf, "Shops   %5d\n\r", top_shop      ); send_to_char( buf, ch );

    sprintf( buf, "Strings %5d strings of %7d bytes (max %d).\n\r",
	nAllocString, sAllocString, MAX_STRING );
    send_to_char( buf, ch );

    sprintf( buf, "Perms   %5d blocks  of %7d bytes.\n\r",
	nAllocPerm, sAllocPerm );
    send_to_char( buf, ch );

    return;
}

void do_dump( CHAR_DATA *ch, char *argument )
{
    int count,count2,num_pcs,aff_count;
    CHAR_DATA *fch;
    MOB_INDEX_DATA *pMobIndex;
    PC_DATA *pc;
    OBJ_DATA *obj;
    OBJ_INDEX_DATA *pObjIndex;
    ROOM_INDEX_DATA *room;
    EXIT_DATA *exit;
    DESCRIPTOR_DATA *d;
    AFFECT_DATA *af;
    FILE *fp;
    int vnum,nMatch = 0;

    /* open file */
    fclose(fpReserve);
    fp = fopen("mem.dmp","w");

    /* report use of data structures */

    num_pcs = 0;
    aff_count = 0;

    /* mobile prototypes */
    fprintf(fp,"MobProt	%4d (%8d bytes)\n",
	top_mob_index, (int)(top_mob_index * (sizeof(*pMobIndex))));

    /* mobs */
    count = 0;  count2 = 0;
    for (fch = char_list; fch != NULL; fch = fch->next)
    {
	count++;
	if (fch->pcdata != NULL)
	    num_pcs++;
	for (af = fch->affected; af != NULL; af = af->next)
	    aff_count++;
    }
    for (fch = char_free; fch != NULL; fch = fch->next)
	count2++;

    fprintf(fp,"Mobs	%4d (%8d bytes), %2d free (%d bytes)\n",
	count, (int)(count * (sizeof(*fch))), count2, (int)(count2 * (sizeof(*fch))));

    /* pcdata */
    count = 0;
    for (pc = pcdata_free; pc != NULL; pc = pc->next)
	count++;

    fprintf(fp,"Pcdata	%4d (%8d bytes), %2d free (%d bytes)\n",
	num_pcs, (int)(num_pcs * (sizeof(*pc))), count, (int)(count * (sizeof(*pc))));

    /* descriptors */
    count = 0; count2 = 0;
    for (d = descriptor_list; d != NULL; d = d->next)
	count++;
    for (d= descriptor_free; d != NULL; d = d->next)
	count2++;

    fprintf(fp, "Descs	%4d (%8d bytes), %2d free (%d bytes)\n",
	count, (int)(count * (sizeof(*d))), count2, (int)(count2 * (sizeof(*d))));

    /* object prototypes */
    for ( vnum = 0; nMatch < top_obj_index; vnum++ )
        if ( ( pObjIndex = get_obj_index( vnum ) ) != NULL )
        {
	    for (af = pObjIndex->affected; af != NULL; af = af->next)
		aff_count++;
            nMatch++;
        }

    fprintf(fp,"ObjProt	%4d (%8d bytes)\n",
	top_obj_index, (int)(top_obj_index * (sizeof(*pObjIndex))));


    /* objects */
    count = 0;  count2 = 0;
    for (obj = object_list; obj != NULL; obj = obj->next)
    {
	count++;
	for (af = obj->affected; af != NULL; af = af->next)
	    aff_count++;
    }
    for (obj = obj_free; obj != NULL; obj = obj->next)
	count2++;

    fprintf(fp,"Objs	%4d (%8d bytes), %2d free (%d bytes)\n",
	count, (int)(count * (sizeof(*obj))), count2, (int)(count2 * (sizeof(*obj))));

    /* affects */
    count = 0;
    for (af = affect_free; af != NULL; af = af->next)
	count++;

    fprintf(fp,"Affects	%4d (%8d bytes), %2d free (%d bytes)\n",
	aff_count, (int)(aff_count * (sizeof(*af))), count, (int)(count * (sizeof(*af))));

    /* rooms */
    fprintf(fp,"Rooms	%4d (%8d bytes)\n",
	top_room, (int)(top_room * (sizeof(*room))));

     /* exits */
    fprintf(fp,"Exits	%4d (%8d bytes)\n",
	top_exit, (int)(top_exit * (sizeof(*exit))));

    fclose(fp);

    /* start printing out mobile data */
    fp = fopen("mob.dmp","w");

    fprintf(fp,"\nMobile Analysis\n");
    fprintf(fp,  "---------------\n");
    nMatch = 0;
    for (vnum = 0; nMatch < top_mob_index; vnum++)
	if ((pMobIndex = get_mob_index(vnum)) != NULL)
	{
	    nMatch++;
	    fprintf(fp,"#%-4d %3d active %3d killed     %s\n",
		pMobIndex->vnum,pMobIndex->count,
		pMobIndex->killed,pMobIndex->short_descr);
	}
    fclose(fp);

    /* start printing out object data */
    fp = fopen("obj.dmp","w");

    fprintf(fp,"\nObject Analysis\n");
    fprintf(fp,  "---------------\n");
    nMatch = 0;
    for (vnum = 0; nMatch < top_obj_index; vnum++)
	if ((pObjIndex = get_obj_index(vnum)) != NULL)
	{
	    nMatch++;
	    fprintf(fp,"#%-4d %3d active %3d reset      %s\n",
		pObjIndex->vnum,pObjIndex->count,
		pObjIndex->reset_num,pObjIndex->short_descr);
	}

    /* close file */
    fclose(fp);
    fpReserve = fopen( NULL_FILE, "r" );
}



/*
 * Stick a little fuzz on a number.
 */
int number_fuzzy( int number )
{
    switch ( number_range(0,3) )
    {
    case 0:  number -= 1; break;
    case 3:  number += 1; break;
    }

    return UMAX( 1, number );
}



/*
 * New number_range() function to solve modula problem.
 */
int number_range( int from, int to )
{
	int x;
	int diff;

	diff = to - from;

	// in case "to" is lower than "from".
	if( diff <= 0 )
	{
		return from;
	}

	do {
	x = random();
	} while (x >= (RAND_MAX - (RAND_MAX % (diff+1))));

	x %= (diff+1);

	return x + from;
}



/*
 * Generate a percentile roll.
 */
 int number_percent( void )
 {
     return number_range(1,100);
 }



/*
 * Generate a random door.
 */
 int number_door( void )
 {
     return number_range(0,5);
 }

void init_random_number_generator( )
{
  srandom(time(NULL)^getpid());
  return;
}


/*
 * Roll some dice.
 */
int dice( int number, int size )
{
    int idice;
    int sum;

    switch ( size )
    {
    case 0: return 0;
    case 1: return number;
    }

    for ( idice = 0, sum = 0; idice < number; idice++ )
	sum += number_range( 1, size );

    return sum;
}



/*
 * Simple linear interpolation.
 */
int interpolate( int level, int value_00, int value_32 )
{
    return value_00 + level * (value_32 - value_00) / 32;
}



/*
 * Removes the tildes from a string.
 * Used for player-entered strings that go into disk files.
 */
void smash_tilde( char *str )
{
    for ( ; *str != '\0'; str++ )
    {
	if ( *str == '~' )
	    *str = '-';
    }

    return;
}



/*
 * Compare strings, case insensitive.
 * Return TRUE if different
 *   (compatibility with historical functions).
 */
bool str_cmp( const char *astr, const char *bstr )
{
    if ( astr == NULL )
    {
	bug( "Str_cmp: null astr.", 0 );
	return TRUE;
    }

    if ( bstr == NULL )
    {
	bug( "Str_cmp: null bstr.", 0 );
	return TRUE;
    }

    for ( ; *astr || *bstr; astr++, bstr++ )
    {
	if ( LOWER(*astr) != LOWER(*bstr) )
	    return TRUE;
    }

    return FALSE;
}



/*
 * Compare strings, case insensitive, for prefix matching.
 * Return TRUE if astr not a prefix of bstr
 *   (compatibility with historical functions).
 */
bool str_prefix( const char *astr, const char *bstr )
{
    if ( astr == NULL )
    {
	bug( "Strn_cmp: null astr.", 0 );
	return TRUE;
    }

    if ( bstr == NULL )
    {
	bug( "Strn_cmp: null bstr.", 0 );
	return TRUE;
    }

    for ( ; *astr; astr++, bstr++ )
    {
	if ( LOWER(*astr) != LOWER(*bstr) )
	    return TRUE;
    }

    return FALSE;
}



/*
 * Compare strings, case insensitive, for match anywhere.
 * Returns TRUE is astr not part of bstr.
 *   (compatibility with historical functions).
 */
bool str_infix( const char *astr, const char *bstr )
{
    int sstr1;
    int sstr2;
    int ichar;
    char c0;

    if ( ( c0 = LOWER(astr[0]) ) == '\0' )
	return FALSE;

    sstr1 = strlen(astr);
    sstr2 = strlen(bstr);

    for ( ichar = 0; ichar <= sstr2 - sstr1; ichar++ )
    {
	if ( c0 == LOWER(bstr[ichar]) && !str_prefix( astr, bstr + ichar ) )
	    return FALSE;
    }

    return TRUE;
}



/*
 * Compare strings, case insensitive, for suffix matching.
 * Return TRUE if astr not a suffix of bstr
 *   (compatibility with historical functions).
 */
bool str_suffix( const char *astr, const char *bstr )
{
    int sstr1;
    int sstr2;

    sstr1 = strlen(astr);
    sstr2 = strlen(bstr);
    if ( sstr1 <= sstr2 && !str_cmp( astr, bstr + sstr2 - sstr1 ) )
	return FALSE;
    else
	return TRUE;
}



/*
 * Returns an initial-capped string.
 */
char *capitalize( const char *str )
{
    static char strcap[MAX_STRING_LENGTH];
    int i;

    for ( i = 0; str[i] != '\0'; i++ )
	strcap[i] = LOWER(str[i]);
    strcap[i] = '\0';
    strcap[0] = UPPER(strcap[0]);
    return strcap;
}


/*
 * Append a string to a file.
 */
void append_file( CHAR_DATA *ch, const char *file, char *str )
{
    FILE *fp;

    if ( IS_NPC(ch) || str[0] == '\0' )
	return;

    fclose( fpReserve );
    if ( ( fp = fopen( file, "a" ) ) == NULL )
    {
	perror( file );
	send_to_char( "Could not open the file!\n\r", ch );
    }
    else
    {
	fprintf( fp, "[%5d] %s: %s\n",
	    ch->in_room ? ch->in_room->vnum : 0, ch->name, str );
	fclose( fp );
    }

    fpReserve = fopen( NULL_FILE, "r" );
    return;
}



/*
 * Reports a bug.
 */
void bug( const char *str, int param )
{
    char buf[MAX_STRING_LENGTH];

    if ( fpArea != NULL )
    {
	int iLine;
	int iChar;

	if ( fpArea == stdin )
	{
	    iLine = 0;
	}
	else
	{
	    iChar = ftell( fpArea );
	    fseek( fpArea, 0, 0 );
	    for ( iLine = 0; ftell( fpArea ) < iChar; iLine++ )
	    {
		while ( getc( fpArea ) != '\n' )
		    ;
	    }
	    fseek( fpArea, iChar, 0 );
	}

	sprintf( buf, "[*****] FILE: %s LINE: %d", strArea, iLine );
	log_string( buf );
/* RT removed because we don't want bugs shutting the mud
	if ( ( fp = fopen( "shutdown.txt", "a" ) ) != NULL )
	{
	    fprintf( fp, "[*****] %s\n", buf );
	    fclose( fp );
	}
*/
    }

    strcpy( buf, "[*****] BUG: " );
    sprintf( buf + strlen(buf), str, param );
    log_string( buf );
/* RT removed due to bug-file spamming
    fclose( fpReserve );
    if ( ( fp = fopen( BUG_FILE, "a" ) ) != NULL )
    {
	fprintf( fp, "%s\n", buf );
	fclose( fp );
    }
    fpReserve = fopen( NULL_FILE, "r" );
*/

    return;
}



/*
 * Writes a string to the log.
 */
void log_string( const char *str )
{
    char *strtime;

    strtime                    = ctime( &current_time );
    strtime[strlen(strtime)-1] = '\0';
    fprintf( stderr, "%s :: %s\n", strtime, str );
    return;
}



/*
 * This function is here to aid in debugging.
 * If the last expression in a function is another function call,
 *   gcc likes to generate a JMP instead of a CALL.
 * This is called "tail chaining."
 * It hoses the debugger call stack for that call.
 * So I make this the last call in certain critical functions,
 *   where I really need the call stack to be right for debugging!
 *
 * If you don't understand this, then LEAVE IT ALONE.
 * Don't remove any calls to tail_chain anywhere.
 *
 * -- Furey
 */
void tail_chain( void )
{
    return;
}

/*************************************************
*                                                *
*                Mysql functions                 *
*                                                *
**************************************************/

/***************************************************************************
 DB connection
***************************************************************************/
MYSQL *DB_CONNECT()
{
 MYSQL *hnd;

 if ( (hnd = mysql_init(NULL)) == NULL )
 {
   bugf("Couldn't init mysql.\n\r");
   return NULL;
 }

 if ( mysql_real_connect(hnd,db_server,db_user,db_password,db_name,0,NULL,0) == NULL )
 {
   bugf("Couldn't connect mysql database %s on %s with %s.\n\r", db_name, db_server, db_user);
   return NULL;
 }

 return hnd;
}

/***************************************************************************
 Adding escape character in front of single quote characters
***************************************************************************/
void mysql_escape_string(char *query, char *escaped)
{
 char 	*point_orig, *point_escaped;
 if(!query)
 {
   return;
 }
 point_escaped = escaped;
 for(point_orig=query;*point_orig;point_orig++)
 {
   if( *point_orig == '\'' )
   {
     *point_escaped = '\\';
     point_escaped++;
   }
   *point_escaped = *point_orig;
   point_escaped++;
 }
 return;
}

/***************************************************************************
 Where the WRITE queries are executed.
***************************************************************************/
void mysql_execute_query(char *query, int i)
{
 MYSQL *hnd;

 hnd = DB_CONNECT();

 if( hnd == NULL )
   return;

 if(mysql_real_query( hnd , query , strlen( query ) ) != 0 )
 {
     bugf("mysql_real_query() error: %s\n\r",query);
 }

 mysql_close( hnd );
}

/***************************************************************************
 A sample logging with MySQL. Used in do_say() for now.
***************************************************************************/
void mysql_log_communication( char *channel , CHAR_DATA *ch, CHAR_DATA *victim , int room_vnum , char *data )
{
 char datetime[100];
 char query[ MAX_STRING_LENGTH ];

 strftime(datetime, sizeof(datetime), "%Y-%m-%d %X", localtime(&current_time));

 sprintf( query , "INSERT INTO `pc_communication` VALUES( NULL , '%s' , '%s' , '%s' ,'%s' , '%d' , '%s' )" , \
  (char*)datetime , channel , (ch!=NULL)?ch->name:(char*)"" , (victim!=NULL)?victim->name:(char*)"",room_vnum , data );

 mysql_execute_query(query,0);

 return;
}

/***************************************************************************
 Writing helps in DB.
***************************************************************************/
void mysql_write_help( HELP_DATA *pHelp )
{
 char query[ MAX_STRING_LENGTH ];
 char arg1[MAX_STRING_LENGTH];
 char arg2[MAX_STRING_LENGTH];

 memset(arg1,0,sizeof(arg1));
 memset(arg2,0,sizeof(arg2));

 mysql_escape_string(pHelp->keyword,arg1);
 mysql_escape_string(pHelp->text,arg2);

 sprintf( query , "INSERT INTO `help` VALUES( NULL , '%d' , '%s' , '%s' )" , \
 pHelp->level, arg1, arg2 );

 mysql_execute_query(query,0);

 return;
}

/***************************************************************************
 Writing socials in DB.
***************************************************************************/
void mysql_write_social( char *name, char *char_no_arg, char *others_no_arg, char *char_found, char *others_found, char *vict_found, char *char_not_found, char *char_auto, char *others_auto )
{
 char query[ MAX_STRING_LENGTH ];
 char arg1[MAX_STRING_LENGTH];
 char arg2[MAX_STRING_LENGTH];
 char arg3[MAX_STRING_LENGTH];
 char arg4[MAX_STRING_LENGTH];
 char arg5[MAX_STRING_LENGTH];
 char arg6[MAX_STRING_LENGTH];
 char arg7[MAX_STRING_LENGTH];
 char arg8[MAX_STRING_LENGTH];
 char arg9[MAX_STRING_LENGTH];

 memset(arg1,0,sizeof(arg1));
 memset(arg2,0,sizeof(arg2));
 memset(arg3,0,sizeof(arg3));
 memset(arg4,0,sizeof(arg4));
 memset(arg5,0,sizeof(arg5));
 memset(arg6,0,sizeof(arg6));
 memset(arg7,0,sizeof(arg7));
 memset(arg8,0,sizeof(arg8));
 memset(arg9,0,sizeof(arg9));

 mysql_escape_string(name,arg1);
 mysql_escape_string(char_no_arg,arg2);
 mysql_escape_string(others_no_arg,arg3);
 mysql_escape_string(char_found,arg4);
 mysql_escape_string(others_found,arg5);
 mysql_escape_string(vict_found,arg6);
 mysql_escape_string(char_not_found,arg7);
 mysql_escape_string(char_auto,arg8);
 mysql_escape_string(others_auto,arg9);

 sprintf( query , "INSERT INTO `social` VALUES( NULL , '%s' , '%s' , '%s' , '%s' , '%s' , '%s' , '%s' , '%s' , '%s' )" , \
 arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9 );

 mysql_execute_query(query,0);

 return;
}

/***************************************************************************
 Writing #AREA info in DB.
***************************************************************************/
void mysql_write_area( AREA_DATA *pArea )
{
 char query[ MAX_STRING_LENGTH ];
 char arg1[MAX_STRING_LENGTH];
 char arg2[MAX_STRING_LENGTH];
 char arg3[MAX_STRING_LENGTH];
 char arg4[MAX_STRING_LENGTH];

 memset(arg1,0,sizeof(arg1));
 memset(arg2,0,sizeof(arg2));
 memset(arg3,0,sizeof(arg3));
 memset(arg4,0,sizeof(arg3));

 mysql_escape_string(pArea->file_name,arg1);
 mysql_escape_string(pArea->name,arg2);
 mysql_escape_string(pArea->writer,arg3);
 mysql_escape_string(pArea->credits,arg4);

 sprintf( query , "INSERT INTO `area` VALUES( NULL , '%s' , '%s' , '%s' , '%s' ,'%d' , '%d' , '%d' , '%d' )" , arg1 , arg2 , arg3 , arg4 , pArea->low_range ,pArea->high_range , pArea->min_vnum , pArea->max_vnum );

 mysql_execute_query(query,0);

 return;
}

/***************************************************************************
 Writing #ROOM info in DB.
***************************************************************************/
void mysql_write_room( ROOM_INDEX_DATA *pRoomIndex )
{
 char query[ MAX_STRING_LENGTH ];
 char arg1[MAX_STRING_LENGTH];
 char arg2[MAX_STRING_LENGTH];
 char arg3[MAX_STRING_LENGTH];

 memset(arg1,0,sizeof(arg1));
 memset(arg2,0,sizeof(arg2));
 memset(arg3,0,sizeof(arg3));

 mysql_escape_string(pRoomIndex->name,arg1);
 mysql_escape_string(pRoomIndex->description,arg2);
 mysql_escape_string(pRoomIndex->owner,arg3);

 sprintf( query , "INSERT INTO `room` VALUES( NULL , '%s' , '%d' , '%s' , '%s' , '%s' , '%d' , '%d' , '%d' , '%d' , '%d' , '%d' )" , \
 pRoomIndex->area->file_name,pRoomIndex->vnum, arg1, arg2, arg3, pRoomIndex->room_flags,
 pRoomIndex->light, pRoomIndex->sector_type, pRoomIndex->heal_rate, pRoomIndex->mana_rate, pRoomIndex->clan );

 mysql_execute_query(query,1);

 return;
}

/***************************************************************************
 Writing D0-D5 info in DB.
***************************************************************************/
void mysql_write_exit( EXIT_DATA *pexit, int room_vnum, int door )
{
 char query[ MAX_STRING_LENGTH ];
 char arg1[MAX_STRING_LENGTH];
 char arg2[MAX_STRING_LENGTH];

 memset(arg1,0,sizeof(arg1));
 memset(arg2,0,sizeof(arg2));

 mysql_escape_string(pexit->description,arg1);
 mysql_escape_string(pexit->keyword,arg2);

 sprintf( query , "INSERT INTO `room_exit` VALUES( NULL , '%d' , '%d' , '%s' , '%s' , '%d' , '%d' , '%d' )" , \
 room_vnum, door, arg1, arg2, pexit->exit_info, pexit->key, pexit->u1.vnum );

 mysql_execute_query(query,0);

 return;
}

/***************************************************************************
 Writing extra descriptions of rooms in DB.
***************************************************************************/
void mysql_write_room_extra_description( EXTRA_DESCR_DATA *ed, int room_vnum )
{
 char query[ MAX_STRING_LENGTH ];
 char arg1[MAX_STRING_LENGTH];
 char arg2[MAX_STRING_LENGTH];

 memset(arg1,0,sizeof(arg1));
 memset(arg2,0,sizeof(arg2));

 mysql_escape_string(ed->keyword,arg1);
 mysql_escape_string(ed->description,arg2);

 sprintf( query , "INSERT INTO `room_extra_description` VALUES( NULL , '%d' , '%s' , '%s' )" , \
 room_vnum, arg1, arg2 );

 mysql_execute_query(query,0);

 return;
}

/***************************************************************************
 Writing shops in DB.
***************************************************************************/
void mysql_write_shop( SHOP_DATA *pShop )
{
 char query[ MAX_STRING_LENGTH ];

 sprintf( query , "INSERT INTO `shop` VALUES( NULL , '%d' , '%d' , '%d' , '%d' , '%d' , '%d' , '%d' , '%d' , '%d' , '%d' )" , \
 pShop->keeper, pShop->buy_type[0], pShop->buy_type[1], pShop->buy_type[2], pShop->buy_type[3], pShop->buy_type[4],
   pShop->profit_buy, pShop->profit_sell , pShop->open_hour, pShop->close_hour );

 mysql_execute_query(query,0);

 return;
}

/***************************************************************************
 Writing resets in DB.
***************************************************************************/
void mysql_write_reset( RESET_DATA *pReset )
{
 char query[ MAX_STRING_LENGTH ];

 sprintf( query , "INSERT INTO `reset` VALUES( NULL , '%s' , '%c' , '%d' , '%d' , '%d' , '%d' )" , \
 area_last->file_name, pReset->command, pReset->arg1, pReset->arg2, pReset->arg3, pReset->arg4 );

 mysql_execute_query(query,0);

 return;
}

/***************************************************************************
 Writing mobiles in DB.
***************************************************************************/
void mysql_write_mobile( MOB_INDEX_DATA *pMobIndex , AREA_DATA *pArea )
{
 char query[ MAX_STRING_LENGTH ];
 char arg1[MAX_STRING_LENGTH];
 char arg2[MAX_STRING_LENGTH];
 char arg3[MAX_STRING_LENGTH];
 char arg4[MAX_STRING_LENGTH];
 char arg5[MAX_STRING_LENGTH];

 memset(arg1,0,sizeof(arg1));
 memset(arg2,0,sizeof(arg2));
 memset(arg3,0,sizeof(arg3));
 memset(arg4,0,sizeof(arg4));
 memset(arg5,0,sizeof(arg5));

 mysql_escape_string(pMobIndex->player_name,arg1);
 mysql_escape_string(pMobIndex->short_descr,arg2);
 mysql_escape_string(pMobIndex->long_descr,arg3);
 mysql_escape_string(pMobIndex->description,arg4);
 mysql_escape_string(pMobIndex->material,arg5);

 sprintf( query , "INSERT INTO `mobile` VALUES( NULL , '%s' , '%d' , '%d' , '%d' , '%s' , '%s' , '%s' , '%s' , \
 '%ld' , '%ld' , '%d' , '%d' , '%d' , '%d' , '%d' , '%d' , '%d' , '%d' , '%d' , '%d' , '%d' , '%d' , \
 '%d' , '%d' , '%d' , '%d' , '%d' , '%ld' , '%ld' , '%ld' , '%ld' , '%d' , '%d' , '%d' , '%d' , '%ld' , \
 '%ld' , '%ld' , '%d' , '%s' , '' )" , \
 pArea->file_name, pMobIndex->vnum, pMobIndex->group, pMobIndex->new_format, arg1, arg2, arg3,
   arg4, pMobIndex->act, pMobIndex->affected_by, pMobIndex->alignment, pMobIndex->level, pMobIndex->hitroll,
 pMobIndex->hit[0], pMobIndex->hit[1], pMobIndex->hit[2], pMobIndex->mana[0], pMobIndex->mana[1], pMobIndex->mana[2],
 pMobIndex->damage[0], pMobIndex->damage[1], pMobIndex->damage[2], pMobIndex->ac[0], pMobIndex->ac[1], pMobIndex->ac[2],
 pMobIndex->ac[3], pMobIndex->dam_type, pMobIndex->off_flags, pMobIndex->imm_flags, pMobIndex->res_flags, pMobIndex->vuln_flags,
 pMobIndex->start_pos, pMobIndex->default_pos, pMobIndex->sex, pMobIndex->race, pMobIndex->wealth, pMobIndex->form,
 pMobIndex->parts, pMobIndex->size, arg5);

 mysql_execute_query(query,0);

 return;
}

/***************************************************************************
 Writing affect data of objects in DB.
***************************************************************************/
void mysql_write_object_affect_data( AFFECT_DATA *paf, int object_vnum  )
{
 char query[ MAX_STRING_LENGTH ];

 sprintf( query , "INSERT INTO `object_affect_data` VALUES( NULL , '%d' , '%d' , '%d' , '%d' , '%d' , '%d' , '%d' , '%d' , '%d' )" , \
 object_vnum, paf->valid, paf->where, paf->type, paf->level, paf->duration, paf->location, paf->modifier, paf->bitvector );

 mysql_execute_query(query,0);

 return;
}

/***************************************************************************
 Writing extra descriptions of objects in DB.
***************************************************************************/
void mysql_write_object_extra_description( EXTRA_DESCR_DATA *ed, int object_vnum )
{
 char query[ MAX_STRING_LENGTH ];
 char arg1[MAX_STRING_LENGTH];
 char arg2[MAX_STRING_LENGTH];

 memset(arg1,0,sizeof(arg1));
 memset(arg2,0,sizeof(arg2));

 mysql_escape_string(ed->keyword,arg1);
 mysql_escape_string(ed->description,arg2);

 sprintf( query , "INSERT INTO `object_extra_description` VALUES( NULL , '%d' , '%s' , '%s' )" , \
 object_vnum, arg1, arg2 );

 mysql_execute_query(query,0);

 return;
}

/***************************************************************************
 Writing objects in DB.
***************************************************************************/
void mysql_write_object( OBJ_INDEX_DATA *pObjIndex , AREA_DATA *pArea )
{
 char query[ MAX_STRING_LENGTH ];
 char arg1[MAX_STRING_LENGTH];
 char arg2[MAX_STRING_LENGTH];
 char arg3[MAX_STRING_LENGTH];
 char arg4[MAX_STRING_LENGTH];

 memset(arg1,0,sizeof(arg1));
 memset(arg2,0,sizeof(arg2));
 memset(arg3,0,sizeof(arg3));
 memset(arg4,0,sizeof(arg4));

 mysql_escape_string(pObjIndex->name,arg1);
 mysql_escape_string(pObjIndex->short_descr,arg2);
 mysql_escape_string(pObjIndex->description,arg3);
 mysql_escape_string(pObjIndex->material,arg4);

 sprintf( query , "INSERT INTO `object` VALUES( NULL , '%s' , '%d' , '%d' , '%s' , '%s' , '%s' , '%d' , '%s' , \
 '%d' , '%d' , '%d' , '%d' , '%d' , '%d' , '%d' , '%d' , '%d' , '%d' , '%d' , '%d')" , \
 pArea->file_name, pObjIndex->vnum, pObjIndex->new_format, arg1, arg2, arg3, pObjIndex->reset_num,
   arg4, pObjIndex->item_type, pObjIndex->extra_flags, pObjIndex->wear_flags, pObjIndex->level, pObjIndex->condition,
 pObjIndex->weight, pObjIndex->cost, pObjIndex->value[0], pObjIndex->value[1], pObjIndex->value[2], pObjIndex->value[3],
 pObjIndex->value[4]);

 mysql_execute_query(query,0);

 return;
}

/***************************************************************************
 Writing mobile spec_funs in DB.
***************************************************************************/
void mysql_write_mobile_special( MOB_INDEX_DATA *pMobIndex, char *name )
{
 char query[ MAX_STRING_LENGTH ];
 char arg1[MAX_STRING_LENGTH];

 memset(arg1,0,sizeof(arg1));

 mysql_escape_string(name,arg1);

 sprintf( query , "UPDATE `mobile` SET `spec_fun` = '%s' WHERE `vnum` = %d" , arg1, pMobIndex->vnum );

 mysql_execute_query(query,0);

 return;
}

void mysql_read_help()
{
 MYSQL_RES *result;
 MYSQL *hnd;
 MYSQL_ROW row;
 char query[ MAX_STRING_LENGTH ];

 hnd = DB_CONNECT();

 if( hnd == NULL )
   return;

 sprintf( query , "SELECT * FROM help" );

 if( mysql_real_query( hnd , query , strlen( query ) ) )
 {
   return;
 }

 result = mysql_use_result( hnd );

 logf("Reading helps from DB.");

 while ( (row = mysql_fetch_row( result )) != NULL )
 {
   mysql_load_help(row);
 }

 mysql_close( hnd );
 return;

}


void mysql_load_help(MYSQL_ROW row)
{
  HELP_DATA *pHelp;
pHelp		= (HELP_DATA *)alloc_perm( sizeof(*pHelp) );
pHelp->level	= atoi((char*)row[1]);
pHelp->keyword	= mysql_fread_string((char*)row[2]);
pHelp->text	= mysql_fread_string((char*)row[3]);
if ( !str_cmp( pHelp->keyword, "greeting" ) )
    help_greeting = pHelp->text;

if ( help_first == NULL )
    help_first = pHelp;
if ( help_last  != NULL )
    help_last->next = pHelp;
help_last	= pHelp;
pHelp->next	= NULL;
top_help++;

  return;
}

void mysql_read_social()
{
 MYSQL_RES *result;
 MYSQL *hnd;
 MYSQL_ROW row;
 char query[ MAX_STRING_LENGTH ];

 hnd = DB_CONNECT();

 if( hnd == NULL )
   return;

 sprintf( query , "SELECT * FROM social" );

 if( mysql_real_query( hnd , query , strlen( query ) ) )
 {
   return;
 }

 result = mysql_use_result( hnd );

 logf("Reading socials from DB.");

 while ( (row = mysql_fetch_row( result )) != NULL )
 {
   mysql_load_social(row);
 }

 mysql_close( hnd );
 return;

}

void mysql_load_social(MYSQL_ROW row)
{

    	struct social_type social;

        /* clear social */
	social.char_no_arg = NULL;
	social.others_no_arg = NULL;
	social.char_found = NULL;
	social.others_found = NULL;
	social.vict_found = NULL;
	social.char_not_found = NULL;
	social.char_auto = NULL;
	social.others_auto = NULL;

  strcpy(social.name,(char*)row[1]);
  social.char_no_arg = mysql_fread_string((char*)row[2]);
	social.others_no_arg = mysql_fread_string((char*)row[3]);
	social.char_found = mysql_fread_string((char*)row[4]);
	social.others_found = mysql_fread_string((char*)row[5]);
	social.vict_found = mysql_fread_string((char*)row[6]);
	social.char_not_found = mysql_fread_string((char*)row[7]);
	social.char_auto = mysql_fread_string((char*)row[8]);
	social.others_auto = mysql_fread_string((char*)row[9]);

	social_table[social_count] = social;
    	social_count++;

   return;
}


void mysql_read_area()
{
 MYSQL_RES *result;
 MYSQL *hnd;
 MYSQL_ROW row;
 char query[ MAX_STRING_LENGTH ];

 hnd = DB_CONNECT();

 if( hnd == NULL )
   return;

 sprintf( query , "SELECT * FROM area" );

 if( mysql_real_query( hnd , query , strlen( query ) ) )
 {
   return;
 }

 result = mysql_use_result( hnd );

 logf("Reading areas from DB.");

 while ( (row = mysql_fetch_row( result )) != NULL )
 {
   mysql_load_area(row);
 }

 mysql_close( hnd );
 return;

}


void mysql_load_area(MYSQL_ROW row)
{
    AREA_DATA *pArea;

    pArea		= (AREA_DATA *)alloc_perm( sizeof(*pArea) );
    pArea->reset_first	= NULL;
    pArea->reset_last	= NULL;
    pArea->file_name	= mysql_fread_string((char*)row[1]);
    pArea->name		= mysql_fread_string((char*)row[2]);
   pArea->writer	= mysql_fread_string((char*)row[3]);
   pArea->credits	= mysql_fread_string((char*)row[4]);
    pArea->low_range	= atoi((char*)row[5]);
    pArea->high_range	= atoi((char*)row[6]);
    pArea->min_vnum	= atoi((char*)row[7]);
    pArea->max_vnum	= atoi((char*)row[8]);
    pArea->age		= 15;
    pArea->nplayer	= 0;
    pArea->empty	= FALSE;

    if ( area_first == NULL )
 area_first = pArea;
    if ( area_last  != NULL )
 area_last->next = pArea;
    area_last	= pArea;
    pArea->next	= NULL;

    top_area++;
    return;
}


void mysql_read_room()
{
 MYSQL_RES *result;
 MYSQL *hnd;
 MYSQL_ROW row;
 char query[ MAX_STRING_LENGTH ];

 hnd = DB_CONNECT();

 if( hnd == NULL )
   return;

 sprintf( query , "SELECT * FROM room" );

 if( mysql_real_query( hnd , query , strlen( query ) ) )
 {
   return;
 }

 result = mysql_use_result( hnd );

 logf("Reading rooms from DB.");

 while ( (row = mysql_fetch_row( result )) != NULL )
 {
   mysql_load_room(row);
 }

 mysql_close( hnd );
 return;

}

void mysql_load_room(MYSQL_ROW row)
{
    ROOM_INDEX_DATA *pRoomIndex;

    if ( area_last == NULL )
    {
 bug( "Load_resets: no #AREA seen yet.", 0 );
 exit( 1 );
    }


 sh_int vnum;

 int door;
 int iHash;

 vnum				= atoi((char*)row[2]);
 if ( vnum == 0 )
     return;

 fBootDb = FALSE;
 if ( get_room_index( vnum ) != NULL )
 {
     bug( "Load_rooms: vnum %d duplicated.", vnum );
     exit( 1 );
 }
 fBootDb = TRUE;

 pRoomIndex			= (ROOM_INDEX_DATA *)alloc_perm( sizeof(*pRoomIndex) );
 pRoomIndex->people		= NULL;
 pRoomIndex->contents		= NULL;
 pRoomIndex->extra_descr		= NULL;
 pRoomIndex->area		= area_lookup((char*)row[1]);
 pRoomIndex->vnum		= vnum;
 pRoomIndex->name		= mysql_fread_string((char*)row[3]);
 pRoomIndex->description		= mysql_fread_string((char*)row[4]);
 pRoomIndex->room_flags		= atoi((char*)row[6]);
 /* horrible hack */
   if ( 3000 <= vnum && vnum < 3400)
    SET_BIT(pRoomIndex->room_flags,ROOM_LAW);
 pRoomIndex->sector_type		= atoi((char*)row[8]);
 pRoomIndex->light		= atoi((char*)row[7]);
 for ( door = 0; door <= 5; door++ )
     pRoomIndex->exit[door] = NULL;

 /* defaults */
 pRoomIndex->heal_rate = atoi((char*)row[9]);
 pRoomIndex->mana_rate = atoi((char*)row[10]);
 pRoomIndex->clan = clan_lookup((char*)row[11]);
 pRoomIndex->owner = mysql_fread_string((char*)row[5]);

 iHash			= vnum % MAX_KEY_HASH;
 pRoomIndex->next	= room_index_hash[iHash];
 room_index_hash[iHash]	= pRoomIndex;
 top_room++;

    return;
}

void mysql_read_room_exit()
{
 MYSQL_RES *result;
 MYSQL *hnd;
 MYSQL_ROW row;
 int query_return_value;
 char query[ MAX_STRING_LENGTH ];

 hnd = DB_CONNECT();

 if( hnd == NULL )
   return;

 sprintf( query , "SELECT * FROM room_exit" );

query_return_value = mysql_real_query( hnd , query , strlen( query ) );

 if( query_return_value != 0 )
 {
  bugf("Query error: %d.\n\r",query_return_value);
   return;
 }

 result = mysql_use_result( hnd );

 logf("Reading exits from DB.");

 while ( (row = mysql_fetch_row( result )) != NULL )
 {
   mysql_load_room_exit(row);
 }

 mysql_close( hnd );
 return;

}


void mysql_load_room_exit(MYSQL_ROW row)
{
    EXIT_DATA *pexit;
    ROOM_INDEX_DATA *pRoomIndex;
    int door;

    pRoomIndex = get_room_index( atoi((char*)row[1]) );

    if(pRoomIndex == NULL)
      {
        bug( "No room with vnum %d for exit.", atoi((char*)row[1]) );
        return;
      }

    door = atoi((char*)row[2]);
    pexit			= (EXIT_DATA *)alloc_perm( sizeof(*pexit) );
    pexit->description	= mysql_fread_string((char*)row[3]);
		pexit->keyword		= mysql_fread_string((char*)row[4]);
		pexit->exit_info	= atoi((char*)row[5]);
		pexit->key		= atoi((char*)row[6]);
		pexit->u1.vnum		= atoi((char*)row[7]);
    pexit->exit_info = atoi((char*)row[5]);
    pRoomIndex->exit[door]	= pexit;
		pRoomIndex->old_exit[door] = pexit;
		top_exit++;

    return;
}

void mysql_read_room_extra_description()
{
 MYSQL_RES *result;
 MYSQL *hnd;
 MYSQL_ROW row;
 char query[ MAX_STRING_LENGTH ];

 hnd = DB_CONNECT();

 if( hnd == NULL )
   return;

 sprintf( query , "SELECT * FROM room_extra_description" );

 if( mysql_real_query( hnd , query , strlen( query ) ) )
 {
   return;
 }

 result = mysql_use_result( hnd );

 logf("Reading room extra descriptions from DB.");

 while ( (row = mysql_fetch_row( result )) != NULL )
 {
   mysql_load_room_extra_description(row);
 }

 mysql_close( hnd );
 return;

}

void mysql_load_room_extra_description(MYSQL_ROW row)
{
    EXTRA_DESCR_DATA *ed;
    ROOM_INDEX_DATA *pRoomIndex;

    pRoomIndex = get_room_index( atoi((char*)row[1]) );

    if(pRoomIndex == NULL)
      {
        bug( "No room with vnum %d for room extra description.", atoi((char*)row[1]) );
        return;
      }

    //door = atoi((char*)row[2]);
    ed			= (EXTRA_DESCR_DATA *)alloc_perm( sizeof(*ed) );
    ed->keyword		= mysql_fread_string((char*)row[2]);
		ed->description		= mysql_fread_string((char*)row[3]);
		ed->next		= pRoomIndex->extra_descr;
		pRoomIndex->extra_descr	= ed;
		top_ed++;

    return;
}

void mysql_read_mobile()
{
 MYSQL_RES *result;
 MYSQL *hnd;
 MYSQL_ROW row;
 char query[ MAX_STRING_LENGTH ];

 hnd = DB_CONNECT();

 if( hnd == NULL )
   return;

 sprintf( query , "SELECT * FROM mobile" );

 if( mysql_real_query( hnd , query , strlen( query ) ) )
 {
   return;
 }

 result = mysql_use_result( hnd );

 logf("Reading mobiles from DB.");

 while ( (row = mysql_fetch_row( result )) != NULL )
 {
   mysql_load_mobile(row);
 }

 mysql_close( hnd );
 return;

}

void mysql_load_mobile(MYSQL_ROW row)
{
  MOB_INDEX_DATA *pMobIndex;

  if ( area_last == NULL )
  {
    bug( "Load_mobiles: no #AREA seen yet.", 0 );
    exit( 1 );
  }

      sh_int vnum;

      int iHash;

      vnum                            = atoi((char*)row[2]);

      fBootDb = FALSE;
      if ( get_mob_index( vnum ) != NULL )
      {
          bug( "Load_mobiles: vnum %d duplicated.", vnum );
          exit( 1 );
      }
      fBootDb = TRUE;

      pMobIndex                       = (MOB_INDEX_DATA *)alloc_perm( sizeof(*pMobIndex) );
      pMobIndex->vnum                 = vnum;
pMobIndex->new_format		= TRUE;
newmobs++;
      pMobIndex->player_name          = mysql_fread_string((char*)row[5]);
      pMobIndex->short_descr          = mysql_fread_string((char*)row[6]);
      pMobIndex->long_descr           = mysql_fread_string((char*)row[7]);
      pMobIndex->description          = mysql_fread_string((char*)row[8]);
      pMobIndex->race		 	= atoi((char*)row[35]);

      pMobIndex->act                  = atoi((char*)row[9]) | ACT_IS_NPC | race_table[pMobIndex->race].act;
      pMobIndex->affected_by          = atoi((char*)row[10]) | race_table[pMobIndex->race].aff;
      pMobIndex->pShop                = NULL;
      pMobIndex->alignment            = atoi((char*)row[11]);
      pMobIndex->group                = atoi((char*)row[3]);

      pMobIndex->level                = atoi((char*)row[12]);
      pMobIndex->hitroll              = atoi((char*)row[13]);

/* read hit dice */
      pMobIndex->hit[DICE_NUMBER]     = atoi((char*)row[14]);
      pMobIndex->hit[DICE_TYPE]   	= atoi((char*)row[15]);
      pMobIndex->hit[DICE_BONUS]      = atoi((char*)row[16]);

/* read mana dice */
pMobIndex->mana[DICE_NUMBER]	= atoi((char*)row[17]);
pMobIndex->mana[DICE_TYPE]	= atoi((char*)row[18]);
pMobIndex->mana[DICE_BONUS]	= atoi((char*)row[19]);

/* read damage dice */
pMobIndex->damage[DICE_NUMBER]	= atoi((char*)row[20]);
pMobIndex->damage[DICE_TYPE]	= atoi((char*)row[21]);
pMobIndex->damage[DICE_BONUS]	= atoi((char*)row[22]);
pMobIndex->dam_type		= atoi((char*)row[27]);

/* read armor class */
pMobIndex->ac[AC_PIERCE]	= atoi((char*)row[23]);
pMobIndex->ac[AC_BASH]		= atoi((char*)row[24]);
pMobIndex->ac[AC_SLASH]		= atoi((char*)row[25]);
pMobIndex->ac[AC_EXOTIC]	= atoi((char*)row[26]);

/* read flags and add in data from the race table */
pMobIndex->off_flags		= atoi((char*)row[28]) | race_table[pMobIndex->race].off;
pMobIndex->imm_flags		= atoi((char*)row[29]) | race_table[pMobIndex->race].imm;
pMobIndex->res_flags		= atoi((char*)row[30]) | race_table[pMobIndex->race].res;
pMobIndex->vuln_flags		= atoi((char*)row[31]) | race_table[pMobIndex->race].vuln;

/* vital statistics */
pMobIndex->start_pos		= atoi((char*)row[32]);
pMobIndex->default_pos		= atoi((char*)row[33]);
pMobIndex->sex			= atoi((char*)row[34]);

pMobIndex->wealth		= atoi((char*)row[36]);

pMobIndex->form			= atoi((char*)row[37]) | race_table[pMobIndex->race].form;
pMobIndex->parts		= atoi((char*)row[38]) | race_table[pMobIndex->race].parts;
/* size */
pMobIndex->size			= atoi((char*)row[39]);
pMobIndex->material		= mysql_fread_string((char*)row[40]);
pMobIndex->spec_fun = spec_lookup	( (char*)row[41] );

      iHash                   = vnum % MAX_KEY_HASH;
      pMobIndex->next         = mob_index_hash[iHash];
      mob_index_hash[iHash]   = pMobIndex;
      top_mob_index++;
      kill_table[URANGE(0, pMobIndex->level, MAX_LEVEL-1)].number++;

  return;
}

void mysql_read_object()
{
 MYSQL_RES *result;
 MYSQL *hnd;
 MYSQL_ROW row;
 char query[ MAX_STRING_LENGTH ];

 hnd = DB_CONNECT();

 if( hnd == NULL )
   return;

 sprintf( query , "SELECT * FROM object" );

 if( mysql_real_query( hnd , query , strlen( query ) ) )
 {
   return;
 }

 result = mysql_use_result( hnd );

 logf("Reading objects from DB.");

 while ( (row = mysql_fetch_row( result )) != NULL )
 {
   mysql_load_object(row);
 }

 mysql_close( hnd );
 return;

}

void mysql_load_object(MYSQL_ROW row)
{
  OBJ_INDEX_DATA *pObjIndex;

  if ( area_last == NULL )
  {
  bugf( "Load_objects: no #AREA seen yet.");
  exit( 1 );
  }

  sh_int vnum;
  int iHash;

  vnum = atoi((char*)row[2]);
  if ( vnum == 0 )
  {
    bugf( "Vnum 0. Returning.");
    return;
  }

  fBootDb = FALSE;
  if ( get_obj_index( vnum ) != NULL )
  {
    bugf( "Load_objects: vnum %d duplicated. Exiting.", vnum );
    exit( 1 );
  }
  fBootDb = TRUE;

  pObjIndex                       = (OBJ_INDEX_DATA *)alloc_perm( sizeof(*pObjIndex) );
  pObjIndex->vnum                 = vnum;
  pObjIndex->new_format           = TRUE;
  pObjIndex->reset_num		= 0;
  newobjs++;
  pObjIndex->name                 = mysql_fread_string((char*)row[4]);
  pObjIndex->short_descr          = mysql_fread_string((char*)row[5]);
  pObjIndex->description          = mysql_fread_string((char*)row[6]);
  pObjIndex->material		          = mysql_fread_string((char*)row[8]);

  pObjIndex->item_type            = atoi((char*)row[9]);
  pObjIndex->extra_flags          = atoi((char*)row[10]);
  pObjIndex->wear_flags           = atoi((char*)row[11]);

  pObjIndex->value[0]		= atoi((char*)row[16]);
  pObjIndex->value[1]		= atoi((char*)row[17]);
  pObjIndex->value[2]		= atoi((char*)row[18]);
  pObjIndex->value[3]		= atoi((char*)row[19]);
  pObjIndex->value[4]		= atoi((char*)row[20]);

  pObjIndex->level		= atoi((char*)row[12]);
  pObjIndex->weight               = atoi((char*)row[14]);
  pObjIndex->cost                 = atoi((char*)row[15]);

  pObjIndex->condition = atoi((char*)row[13]);

  iHash                   = vnum % MAX_KEY_HASH;
  pObjIndex->next         = obj_index_hash[iHash];
  obj_index_hash[iHash]   = pObjIndex;
  top_obj_index++;

  return;
}

void mysql_read_object_extra_description()
{
 MYSQL_RES *result;
 MYSQL *hnd;
 MYSQL_ROW row;
 char query[ MAX_STRING_LENGTH ];

 hnd = DB_CONNECT();

 if( hnd == NULL )
   return;

 sprintf( query , "SELECT * FROM object_extra_description" );

 if( mysql_real_query( hnd , query , strlen( query ) ) )
 {
   return;
 }

 result = mysql_use_result( hnd );

 logf("Reading object extra descriptions from DB.");

 while ( (row = mysql_fetch_row( result )) != NULL )
 {
   mysql_load_object_extra_description(row);
 }

 mysql_close( hnd );
 return;

}

void mysql_load_object_extra_description(MYSQL_ROW row)
{
    OBJ_INDEX_DATA *pObjIndex;
    EXTRA_DESCR_DATA *ed;

    if ( ( pObjIndex = get_obj_index( atoi((char*)row[1]) ) ) == NULL )
    {
    bug( "No object has that vnum.\n\r", 0 );
    return;
    }

    ed                      = (EXTRA_DESCR_DATA *)alloc_perm( sizeof(*ed) );
    ed->keyword             = mysql_fread_string((char*)row[2]);
    ed->description         = mysql_fread_string((char*)row[3]);
    ed->next                = pObjIndex->extra_descr;
    pObjIndex->extra_descr  = ed;
    top_ed++;

    return;
}

void mysql_read_object_affect_data()
{
 MYSQL_RES *result;
 MYSQL *hnd;
 MYSQL_ROW row;
 char query[ MAX_STRING_LENGTH ];

 hnd = DB_CONNECT();

 if( hnd == NULL )
   return;

 sprintf( query , "SELECT * FROM object_affect_data" );

 if( mysql_real_query( hnd , query , strlen( query ) ) )
 {
   return;
 }

 result = mysql_use_result( hnd );

 logf("Reading object affect data from DB.");

 while ( (row = mysql_fetch_row( result )) != NULL )
 {
   mysql_load_object_affect_data(row);
 }

 mysql_close( hnd );
 return;

}

void mysql_load_object_affect_data(MYSQL_ROW row)
{
    OBJ_INDEX_DATA *pObjIndex;
    AFFECT_DATA *paf;

    if ( ( pObjIndex = get_obj_index( atoi((char*)row[1]) ) ) == NULL )
    {
    bug( "No object has that vnum.\n\r", 0 );
    return;
    }

    paf = (AFFECT_DATA *)alloc_perm( sizeof(*paf) );
    paf->where              = atoi((char*)row[3]);
    paf->type               = atoi((char*)row[4]);
    paf->level              = atoi((char*)row[5]);
    paf->duration           = atoi((char*)row[6]);
    paf->location           = atoi((char*)row[7]);
    paf->modifier           = atoi((char*)row[8]);
    paf->bitvector          = atoi((char*)row[9]);
    paf->next               = pObjIndex->affected;
    pObjIndex->affected     = paf;
    top_affect++;

    return;
}

void mysql_read_reset()
{
 MYSQL_RES *result;
 MYSQL *hnd;
 MYSQL_ROW row;
 char query[ MAX_STRING_LENGTH ];

 hnd = DB_CONNECT();

 if( hnd == NULL )
   return;

 sprintf( query , "SELECT * FROM reset" );

 if( mysql_real_query( hnd , query , strlen( query ) ) )
 {
   return;
 }

 result = mysql_use_result( hnd );

 logf("Reading resets from DB.");

 while ( (row = mysql_fetch_row( result )) != NULL )
 {
   mysql_load_reset(row);
 }

 mysql_close( hnd );
 return;

}

void mysql_load_reset(MYSQL_ROW row)
{

  RESET_DATA *pReset;
  ROOM_INDEX_DATA *pRoomIndex;
  EXIT_DATA *pexit;
  OBJ_INDEX_DATA *temp_index;
  AREA_DATA *pArea;

  if ( area_last == NULL )
  {
bug( "Load_resets: no #AREA seen yet.", 0 );
exit( 1 );
  }

  pReset		= (RESET_DATA *)alloc_perm( sizeof(*pReset) );
  pReset->command = mysql_fread_string((char*)row[2])[0];
  pReset->arg1	= atoi((char*)row[3]);
	pReset->arg2	= atoi((char*)row[4]);
  pReset->arg3	= atoi((char*)row[5]);
  pReset->arg4	= atoi((char*)row[6]);

  /*
	 * Validate parameters.
	 * We're calling the index functions for the side effect.
	 */
	switch ( pReset->command )
	{
	default:
	    bug( "Load_resets: bad command '%c'.", pReset->command );
	    exit( 1 );
	    break;

	case 'M':
	    get_mob_index  ( pReset->arg1 );
	    get_room_index ( pReset->arg3 );
	    break;

	case 'O':
	    temp_index = get_obj_index  ( pReset->arg1 );
	    temp_index->reset_num++;
	    get_room_index ( pReset->arg3 );
	    break;

	case 'P':
	    temp_index = get_obj_index  ( pReset->arg1 );
	    temp_index->reset_num++;
	    get_obj_index  ( pReset->arg3 );
	    break;

	case 'G':
	case 'E':
	    temp_index = get_obj_index  ( pReset->arg1 );
	    temp_index->reset_num++;
	    break;

	case 'D':
	    pRoomIndex = get_room_index( pReset->arg1 );

	    if ( pReset->arg2 < 0
	    ||   pReset->arg2 > 5
	    || ( pexit = pRoomIndex->exit[pReset->arg2] ) == NULL
	    || !IS_SET( pexit->exit_info, EX_ISDOOR ) )
	    {
		bug( "Load_resets: 'D': exit %d not door.", atoi((char*)row[0]) );
		//exit( 1 );
	    }

	    if ( pReset->arg3 < 0 || pReset->arg3 > 2 )
	    {
		bug( "Load_resets: 'D': bad 'locks': %d.", pReset->arg3 );
		exit( 1 );
	    }

	    break;

	case 'R':
	    pRoomIndex		= get_room_index( pReset->arg1 );

	    if ( pReset->arg2 < 0 || pReset->arg2 > 6 )
	    {
		bug( "Load_resets: 'R': bad exit %d.", pReset->arg2 );
		exit( 1 );
	    }

	    break;
	}

	pArea = area_lookup((char*)row[1]);

  if ( pArea->reset_first == NULL )
      pArea->reset_first	= pReset;
  if ( pArea->reset_last  != NULL )
      pArea->reset_last->next	= pReset;

  pArea->reset_last	= pReset;
  pReset->next		= NULL;
  top_reset++;

    return;
}

void mysql_read_shop()
{
 MYSQL_RES *result;
 MYSQL *hnd;
 MYSQL_ROW row;
 char query[ MAX_STRING_LENGTH ];

 hnd = DB_CONNECT();

 if( hnd == NULL )
   return;

 sprintf( query , "SELECT * FROM shop" );

 if( mysql_real_query( hnd , query , strlen( query ) ) )
 {
   return;
 }

 result = mysql_use_result( hnd );

 logf("Reading shops from DB.");

 while ( (row = mysql_fetch_row( result )) != NULL )
 {
   mysql_load_shop(row);
 }

 mysql_close( hnd );
 return;

}

void mysql_load_shop(MYSQL_ROW row)
{
  SHOP_DATA *pShop;

MOB_INDEX_DATA *pMobIndex;


pShop			= (SHOP_DATA *)alloc_perm( sizeof(*pShop) );
pShop->keeper		= atoi((char*)row[1]);
if ( pShop->keeper == 0 )
    return;
pShop->buy_type[0]	= atoi((char*)row[2]);
pShop->buy_type[1]	= atoi((char*)row[3]);
pShop->buy_type[2]	= atoi((char*)row[4]);
pShop->buy_type[3]	= atoi((char*)row[5]);
pShop->buy_type[4]	= atoi((char*)row[6]);
pShop->profit_buy	= atoi((char*)row[7]);
pShop->profit_sell	= atoi((char*)row[8]);
pShop->open_hour	= atoi((char*)row[9]);
pShop->close_hour	= atoi((char*)row[10]);

pMobIndex		= get_mob_index( pShop->keeper );
pMobIndex->pShop	= pShop;

if ( shop_first == NULL )
    shop_first = pShop;
if ( shop_last  != NULL )
    shop_last->next = pShop;

shop_last	= pShop;
pShop->next	= NULL;
top_shop++;

  return;
}

AREA_DATA* area_lookup( char *name )
{

  AREA_DATA *pArea;

  for ( pArea = area_first; pArea != NULL; pArea = pArea->next )
  {
    if (!strcmp(pArea->file_name,name))
    {
      return pArea;
    }
  }

    return NULL;
}
