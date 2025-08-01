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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#endif

#include "merc.h"
#include "db.h"
#include "recycle.h"
#include "music.h"
#include "lookup.h"
#include "magic.h"
#include "strrep.h"


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

NOTE_DATA *		note_free;

char			bug_buf		[2*MAX_INPUT_LENGTH];
CHAR_DATA *		char_list;
char *			help_greeting;
char			log_buf		[2*MAX_INPUT_LENGTH];
KILL_DATA		kill_table	[MAX_LEVEL];
NOTE_DATA *		note_list;
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
void    init_mm         args( ( void ) );
void	load_area	args( ( cJSON *json_data ) );
void	load_helps	args( ( cJSON *json_data ) );
void 	load_mobiles	args( ( cJSON *json_data ) );
void 	load_objects	args( ( cJSON *json_data ) );
void	load_resets	args( ( cJSON *json_data ) );
void	load_rooms	args( ( cJSON *json_data ) );
void	load_shops	args( ( cJSON *json_data ) );
void 	load_socials	args( ( cJSON *json_data ) );
void	load_specials	args( ( cJSON *json_data ) );
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
	if ( ( string_space = calloc( 1, MAX_STRING ) ) == NULL )
	{
	    bugf("Boot_db: can't alloc %d string space.", MAX_STRING );
	    exit( 1 );
	}
	top_string	= string_space;
	fBootDb		= TRUE;
    }

    /*
     * Init random number generator.
     */
    {
        init_mm( );
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

    /*
     * Read in all the area files.
     */
    {
	FILE *fpList;

	if ( ( fpList = fopen( AREA_LIST, "r" ) ) == NULL )
	{
	    perror( AREA_LIST );
	    exit( 1 );
	}

	for ( ; ; )
	{
		long len = 0;
        char *json_byte_data = NULL;
        cJSON *json_data = NULL;

	    strcpy( strArea, fread_word( fpList ) );

	    if ( strArea[0] == '$' )
		break;

	    if ( strArea[0] == '-' )
	    {
		fpArea = stdin;
	    }
	    else
	    {
			/* open in read binary mode */
			if ( ( fpArea = fopen(strArea,"rb") ) == NULL )
			{
				perror( strArea );
				exit( 1 );
			}
	    }

		/* get the length */
        fseek(fpArea, 0, SEEK_END);
        len = ftell(fpArea);
        fseek(fpArea, 0, SEEK_SET);

		json_byte_data = (char*)malloc(len + 1);

		size_t bytes_read = fread(json_byte_data, 1, len, fpArea);
		if (bytes_read != len) {
			fprintf(stderr, "Error: Incomoplete read! (%zu / %ld byte)\n", bytes_read, len);
		}
        json_byte_data[len] = '\0';
        fclose(fpArea);

        json_data = cJSON_Parse(json_byte_data);
        if (!json_data)
        {
            bugf((char*)"Error before: [%s]\n", cJSON_GetErrorPtr());
            exit( 1 );
        }

	    load_area( json_data );
        load_rooms( json_data );
		load_objects( json_data );
		load_mobiles( json_data );
		load_shops( json_data );
		load_specials( json_data );
		load_resets( json_data );
        load_helps( json_data );
		load_socials( json_data );

        cJSON_Delete(json_data);

	    fpArea = NULL;
	}
	fclose( fpList );
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
void load_area( cJSON *json_data )
{
    cJSON *json_areadata = NULL;

    json_areadata = cJSON_GetObjectItemCaseSensitive(json_data, "area");

    if(!json_areadata)
        return;

    AREA_DATA *pArea;
    
    pArea		= (AREA_DATA *)alloc_perm( sizeof(*pArea) );

    pArea->reset_first	= NULL;
    pArea->reset_last	= NULL;
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

    pArea->name             = str_dup( cJSON_GetObjectItemCaseSensitive( json_areadata, "name" )->valuestring  );
    pArea->credits          = str_dup( cJSON_GetObjectItemCaseSensitive( json_areadata, "writer" )->valuestring );
    pArea->file_name        = str_dup( cJSON_GetObjectItemCaseSensitive( json_areadata, "file_name" )->valuestring );
    pArea->low_range        = cJSON_GetObjectItemCaseSensitive( json_areadata, "low_range" )->valuedouble;
    pArea->high_range       = cJSON_GetObjectItemCaseSensitive( json_areadata, "high_range" )->valuedouble;
    pArea->min_vnum         = cJSON_GetObjectItemCaseSensitive( json_areadata, "min_vnum" )->valuedouble;
    pArea->max_vnum         = cJSON_GetObjectItemCaseSensitive( json_areadata, "max_vnum" )->valuedouble;

    return;
}



/*
 * Snarf a help section.
 */
void load_helps( cJSON *json_data )
{
    cJSON *json_helps = NULL;

    json_helps = cJSON_GetObjectItemCaseSensitive(json_data, "helps");

    if(!json_helps)
        return;
    
    HELP_DATA *pHelp;
    cJSON *json_help = NULL;

    cJSON_ArrayForEach(json_help, json_helps)
    {
        pHelp		= (HELP_DATA *)alloc_perm( sizeof(*pHelp) );
        pHelp->level	= cJSON_GetObjectItemCaseSensitive( json_help, "level" )->valuedouble;
	    pHelp->keyword	= str_dup( cJSON_GetObjectItemCaseSensitive( json_help, "keyword" )->valuestring );
        pHelp->text	= str_dup( cJSON_GetObjectItemCaseSensitive( json_help, "text" )->valuestring );

        if ( !str_cmp( pHelp->keyword, "GREETING" ) )
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
 * Snarf a reset section.
 */
void load_resets( cJSON *json_data )
{
	cJSON *json_resets = NULL;
	cJSON *json_reset = NULL;

    json_resets = cJSON_GetObjectItemCaseSensitive(json_data, "resets");

    if(!json_resets)
        return;

    RESET_DATA *pReset;

    if ( area_last == NULL )
    {
	bug( "Load_resets: no #AREA seen yet.", 0 );
	exit( 1 );
    }

    cJSON_ArrayForEach(json_reset, json_resets)
    {
	ROOM_INDEX_DATA *pRoomIndex;
	EXIT_DATA *pexit;
	char letter;
	OBJ_INDEX_DATA *temp_index;

	letter = (cJSON_GetObjectItemCaseSensitive( json_reset, "command" )->valuestring)[0];

	pReset		= alloc_perm( sizeof(*pReset) );
	pReset->command	= letter;
	pReset->arg1	= cJSON_GetObjectItemCaseSensitive( json_reset, "arg1" )->valuedouble;
	pReset->arg2	= cJSON_GetObjectItemCaseSensitive( json_reset, "arg2" )->valuedouble;
	pReset->arg3	= (letter == 'G' || letter == 'R')
			    ? 0 : cJSON_GetObjectItemCaseSensitive( json_reset, "arg3" )->valuedouble;
	pReset->arg4	= (letter == 'P' || letter == 'M')
			    ? cJSON_GetObjectItemCaseSensitive( json_reset, "arg4" )->valuedouble : 0;

	/*
	 * Validate parameters.
	 * We're calling the index functions for the side effect.
	 */
	switch ( letter )
	{
	default:
	    bugf("Load_resets: bad command '%c'.", letter );
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
		bugf("Load_resets: 'D': exit %d not door.", pReset->arg2 );
		exit( 1 );
	    }

	    if ( pReset->arg3 < 0 || pReset->arg3 > 2 )
	    {
		bugf("Load_resets: 'D': bad 'locks': %d.", pReset->arg3 );
		exit( 1 );
	    }

	    break;

	case 'R':
	    pRoomIndex		= get_room_index( pReset->arg1 );

	    if ( pReset->arg2 < 0 || pReset->arg2 > 6 )
	    {
		bugf("Load_resets: 'R': bad exit %d.", pReset->arg2 );
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
void load_rooms( cJSON *json_data )
{
    cJSON *json_rooms = NULL;

    json_rooms = cJSON_GetObjectItemCaseSensitive(json_data, "rooms");

    if(!json_rooms)
        return;

    cJSON *json_room = NULL;
    cJSON *json_exits = NULL;
    cJSON *json_exit = NULL;
    cJSON *json_extra_descriptions = NULL;
    cJSON *json_extra_description = NULL;
    cJSON *temp = NULL;

    cJSON_ArrayForEach(json_room, json_rooms)
    {
        ROOM_INDEX_DATA *pRoomIndex;
        int vnum;
        int door;
        int iHash;

        vnum = cJSON_GetObjectItemCaseSensitive( json_room, "vnum" )->valuedouble;

        fBootDb = FALSE;

        if ( get_room_index( vnum ) != NULL )
        {
            bug( "Load_json_rooms: vnum %d duplicated.", vnum );
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
        pRoomIndex->name		= str_dup( cJSON_GetObjectItemCaseSensitive( json_room, "name" )->valuestring );
        pRoomIndex->description		= str_dup( cJSON_GetObjectItemCaseSensitive( json_room, "description" )->valuestring );
        pRoomIndex->room_flags		= char_flag( (char *)cJSON_GetObjectItemCaseSensitive( json_room, "flags" )->valuestring, 0 );

        if ( 3000 <= vnum && vnum < 3400)
            SET_BIT(pRoomIndex->room_flags,ROOM_LAW);
        
        pRoomIndex->sector_type		= cJSON_GetObjectItemCaseSensitive( json_room, "sector" )->valuedouble;
        pRoomIndex->light		= 0;
        for ( door = 0; door <= 5; door++ )
            pRoomIndex->exit[door] = NULL;
        
        /* defaults */
        pRoomIndex->heal_rate = 100;
        pRoomIndex->mana_rate = 100;

        temp = cJSON_GetObjectItemCaseSensitive( json_room, "heal_rate" );
        if (cJSON_IsNumber(temp))
            pRoomIndex->heal_rate = temp->valuedouble;

        temp = cJSON_GetObjectItemCaseSensitive( json_room, "mana_rate" );
        if (cJSON_IsNumber(temp))
            pRoomIndex->mana_rate = temp->valuedouble;
        
        temp = cJSON_GetObjectItemCaseSensitive( json_room, "owner" );
        if (cJSON_IsString(temp))
            pRoomIndex->owner = str_dup(cJSON_Print(temp));

        json_exits = cJSON_GetObjectItemCaseSensitive(json_room, "exits");

        cJSON_ArrayForEach(json_exit, json_exits)
        {
            EXIT_DATA *pexit;
            int locks;

            door		= cJSON_GetObjectItemCaseSensitive( json_exit, "exit_door" )->valuedouble;

            if ( door < 0 || door > 5 )
            {
                bug( "Fread_rooms: vnum %d has bad door number.", vnum );
                exit( 1 );
            }

            pexit			= (EXIT_DATA *)alloc_perm( sizeof(*pexit) );
            pexit->description	= str_dup( cJSON_GetObjectItemCaseSensitive( json_exit, "exit_description" )->valuestring );
            pexit->keyword		= str_dup( cJSON_GetObjectItemCaseSensitive( json_exit, "exit_keyword" )->valuestring );
            pexit->exit_info	= 0;
            locks			= cJSON_GetObjectItemCaseSensitive( json_exit, "exit_locks" )->valuedouble;
            pexit->key		= cJSON_GetObjectItemCaseSensitive( json_exit, "exit_key" )->valuedouble;
            pexit->u1.vnum		= cJSON_GetObjectItemCaseSensitive( json_exit, "exit_u1_vnum" )->valuedouble;

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
            top_exit++;
        }

        json_extra_descriptions = cJSON_GetObjectItemCaseSensitive(json_room, "extra_descriptions");

        cJSON_ArrayForEach(json_extra_description, json_extra_descriptions)
        {
            EXTRA_DESCR_DATA *ed;

            ed			= (EXTRA_DESCR_DATA *)alloc_perm( sizeof(*ed) );
            ed->keyword		= str_dup( cJSON_GetObjectItemCaseSensitive( json_extra_description, "keyword" )->valuestring );
            ed->description		= str_dup( cJSON_GetObjectItemCaseSensitive( json_extra_description, "description" )->valuestring );
            ed->next		= pRoomIndex->extra_descr;
            pRoomIndex->extra_descr	= ed;
            top_ed++;
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
void load_shops (cJSON *json_data)
{
	cJSON *json_shops = NULL;
	cJSON *json_shop = NULL;

    json_shops = cJSON_GetObjectItemCaseSensitive(json_data, "shops");

    if(!json_shops)
        return;

    SHOP_DATA *pShop;
    int keeper = 0;

    cJSON_ArrayForEach(json_shop, json_shops)
    {
        MOB_INDEX_DATA *pMobIndex;

        // ROM mem leak fix, check the keeper before allocating the memory
        // to the SHOP_DATA variable.  -Rhien
        keeper = cJSON_GetObjectItemCaseSensitive( json_shop, "keeper" )->valuedouble;

        if (keeper == 0)
        {
            break;
        }

        // Now that we have a non zero keeper number we can allocate
        pShop = alloc_perm(sizeof(*pShop));
        pShop->keeper = keeper;

		pShop->buy_type[0] = cJSON_GetObjectItemCaseSensitive( json_shop, "buy_type_0" )->valuedouble;
		pShop->buy_type[1] = cJSON_GetObjectItemCaseSensitive( json_shop, "buy_type_1" )->valuedouble;
		pShop->buy_type[2] = cJSON_GetObjectItemCaseSensitive( json_shop, "buy_type_2" )->valuedouble;
		pShop->buy_type[3] = cJSON_GetObjectItemCaseSensitive( json_shop, "buy_type_3" )->valuedouble;
		pShop->buy_type[4] = cJSON_GetObjectItemCaseSensitive( json_shop, "buy_type_4" )->valuedouble;

        pShop->profit_buy = cJSON_GetObjectItemCaseSensitive( json_shop, "profit_buy" )->valuedouble;
        pShop->profit_sell = cJSON_GetObjectItemCaseSensitive( json_shop, "profit_sell" )->valuedouble;
        pShop->open_hour = cJSON_GetObjectItemCaseSensitive( json_shop, "open_hour" )->valuedouble;
        pShop->close_hour = cJSON_GetObjectItemCaseSensitive( json_shop, "close_hour" )->valuedouble;

        pMobIndex = get_mob_index(pShop->keeper);
        pMobIndex->pShop = pShop;

        if (shop_first == NULL)
        {
            shop_first = pShop;
        }

        if (shop_last != NULL)
        {
            shop_last->next = pShop;
        }

        shop_last = pShop;
        pShop->next = NULL;
        top_shop++;
    }

    return;
}


/*
 * Snarf spec proc declarations.
 */
void load_specials( cJSON *json_data )
{
	cJSON *json_specials = NULL;
	cJSON *json_special = NULL;

    json_specials = cJSON_GetObjectItemCaseSensitive(json_data, "specials");

    if(!json_specials)
        return;

    cJSON_ArrayForEach(json_special, json_specials)
    {
		MOB_INDEX_DATA *pMobIndex;

		pMobIndex		= get_mob_index	( cJSON_GetObjectItemCaseSensitive( json_special, "vnum" )->valuedouble );
		pMobIndex->spec_fun	= spec_lookup	( cJSON_GetObjectItemCaseSensitive( json_special, "spec_fun" )->valuestring );
		if ( pMobIndex->spec_fun == 0 )
		{
			bug( "Load_specials: 'M': vnum %d.", pMobIndex->vnum );
			exit( 1 );
		}
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
		    bugf( buf);
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

int random_skill_by_rarity(int rarity) {
    int matching_skills[MAX_SKILL];
    int count = 0;

    for (int sn = 0; sn < MAX_SKILL; sn++) {
        if (skill_table[sn].name == NULL) // skip empty
            continue;

        if (skill_table[sn].book_rarity == rarity)
            matching_skills[count++] = sn;
    }

    if (count == 0) {
        bugf("No skill found with rarity %d", rarity);
        return -1; // not found
    }

    int chosen_index = number_range(0, count - 1);
    return matching_skills[chosen_index];
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
	    bugf("Reset_area: bad command %c.", pReset->command );
	    break;

	case 'M':
	    if ( ( pMobIndex = get_mob_index( pReset->arg1 ) ) == NULL )
	    {
		bugf("Reset_area: 'M': bad vnum %d.", pReset->arg1 );
		continue;
	    }

	    if ( ( pRoomIndex = get_room_index( pReset->arg3 ) ) == NULL )
	    {
		bugf("Reset_area: 'R': bad vnum %d.", pReset->arg3 );
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
         * Create random book
         */
        if (IS_NPC(mob) && number_percent() < 2)
        {
            int skill_chance = number_range(1,100);
            int tier_chance = number_range(1,100);
            int sn = 0;
            int tier = 1;
            
            if(skill_chance < 90)
                sn = random_skill_by_rarity(1);
            else if(skill_chance <98)
                sn = random_skill_by_rarity(2);
            else
                sn = random_skill_by_rarity(3);
            
            if (tier_chance <= 88)
                tier = 1;
            else if (tier_chance <= 98)
                tier = 2;
            else
                tier = 3;


            OBJ_DATA *book;
            OBJ_INDEX_DATA *template = get_obj_index(26);

            book = create_object(template, 0, FALSE);

            book->value[0] = tier;
            book->value[1] = sn;

            char buf_book_name[MAX_STRING_LENGTH];
            char buf_book_short[MAX_STRING_LENGTH];

            snprintf(buf_book_name, sizeof(buf_book_name), "book %s", skill_table[sn].name);
            switch (tier)
            {
                case 1:
                    snprintf(buf_book_short, sizeof(buf_book_short), "Book of %s: Introduction", capitalize(skill_table[sn].name));
                    break;
                case 2:
                    snprintf(buf_book_short, sizeof(buf_book_short), "Book of %s: Intermediate", capitalize(skill_table[sn].name));
                    break;
                case 3:
                    snprintf(buf_book_short, sizeof(buf_book_short), "Book of %s: Mastering", capitalize(skill_table[sn].name));
                    break;
                default:
                    book->value[1] = 0;
                    snprintf(buf_book_short, sizeof(buf_book_short), "Book of Unknown");
                    break;
            }

            free_string(book->name);
            free_string(book->short_descr);
            book->name = str_dup(buf_book_name);
            book->short_descr = str_dup(buf_book_short);

            obj_to_char(book, mob);
        }

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
		bugf("Reset_area: 'O': bad vnum %d.", pReset->arg1 );
		continue;
	    }

	    if ( ( pRoomIndex = get_room_index( pReset->arg3 ) ) == NULL )
	    {
		bugf("Reset_area: 'R': bad vnum %d.", pReset->arg3 );
		continue;
	    }

	    if ( pArea->nplayer > 0
	    ||   count_obj_list( pObjIndex, pRoomIndex->contents ) > 0 )
	    {
		last = FALSE;
		break;
	    }

	    obj       = create_object( pObjIndex, UMIN(number_fuzzy(level),LEVEL_HERO - 1), TRUE );
	    obj->cost = 0;
	    obj_to_room( obj, pRoomIndex );
	    last = TRUE;
	    break;

	case 'P':
	    if ( ( pObjIndex = get_obj_index( pReset->arg1 ) ) == NULL )
	    {
		bugf("Reset_area: 'P': bad vnum %d.", pReset->arg1 );
		continue;
	    }

	    if ( ( pObjToIndex = get_obj_index( pReset->arg3 ) ) == NULL )
	    {
		bugf("Reset_area: 'P': bad vnum %d.", pReset->arg3 );
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
	        obj = create_object( pObjIndex, number_fuzzy(obj_to->level), TRUE );
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
		bugf("Reset_area: 'E' or 'G': bad vnum %d.", pReset->arg1 );
		continue;
	    }

	    if ( !last )
		break;

	    if ( mob == NULL )
	    {
		bugf( "Reset_area: 'E' or 'G': null mob for vnum %d.",pReset->arg1 );
		last = FALSE;
		break;
	    }

	    if ( mob->pIndexData->pShop != NULL )
	    {
		int olevel = 0;

		obj = create_object( pObjIndex, olevel, TRUE );
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
		    obj=create_object(pObjIndex,UMIN(number_fuzzy(level), LEVEL_HERO - 1), TRUE);
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
		bugf("Reset_area: 'D': bad vnum %d.", pReset->arg1 );
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
		bugf("Reset_area: 'R': bad vnum %d.", pReset->arg1 );
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
	bugf("Create_mobile: NULL pMobIndex.");
	exit( 1 );
    }

    mob = new_char();

    mob->pIndexData	= pMobIndex;

    mob->name          = str_dup(pMobIndex->player_name);
    mob->id		= get_mob_id();
    mob->short_descr   = str_dup(pMobIndex->short_descr);
    mob->long_descr    = str_dup(pMobIndex->long_descr);
    mob->description   = str_dup(pMobIndex->description);
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
    if (mob->sex < 0 || mob->sex > 5)
    {
        mob->sex = number_range(1,5);
    }
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
OBJ_DATA *create_object( OBJ_INDEX_DATA *pObjIndex, int level, bool randomize )
{
    AFFECT_DATA *paf;
    OBJ_DATA *obj;
    int i;

    if ( pObjIndex == NULL )
    {
	bugf("Create_object: NULL pObjIndex.");
	exit( 1 );
    }

    obj = new_obj();

    obj->pIndexData	= pObjIndex;
    obj->in_room	= NULL;
    obj->enchanted	= FALSE;

	obj->level = pObjIndex->level;

    obj->wear_loc	= -1;

    obj->name          = str_dup(pObjIndex->name);
    obj->short_descr   = str_dup(pObjIndex->short_descr);
    obj->description   = str_dup(pObjIndex->description);
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

	obj->cost	= pObjIndex->cost;

    /*
     * Mess with object properties.
     */
    switch ( obj->item_type )
    {
    default:
	bugf("Read_object: vnum %d bad type.", pObjIndex->vnum );
	break;

    case ITEM_LIGHT:
        if (obj->value[2] == 999)
            obj->value[2] = -1;
        if(randomize == TRUE)
        {
            obj->value[0]	= 0;								// unused
            obj->value[1]	= 0;								// unused
            obj->value[2]	= (number_percent()<20)?-1:number_range(5,500); // light duration in game hours (-1 unlimited)
            obj->value[3]	= 0;								// unused
            obj->value[4]	= 0;								// unused
        }
	break;

    case ITEM_FOOD:
        if(randomize == TRUE)
        {
            obj->value[0]	= number_range(5,40);				// number of game hours the food will keep the person who eats it full
            obj->value[1]	= number_range(5,70);				// number of hours it will keep the person from getting hungry
            obj->value[2]	= 0;								// unused
            obj->value[3]	= (number_percent()<95)?0:1;		// nonpoisoned:0,poisoned:1
            obj->value[4]	= 0;								// unused
        }
    break;

    case ITEM_FURNITURE:
    case ITEM_TRASH:
    case ITEM_CONTAINER:
    case ITEM_DRINK_CON:
    case ITEM_KEY:
    case ITEM_BOAT:
    case ITEM_CORPSE_NPC:
    case ITEM_CORPSE_PC:
    case ITEM_FOUNTAIN:
    case ITEM_MAP:
    case ITEM_CLOTHING:
    case ITEM_PORTAL:
	break;

    case ITEM_TREASURE:
    case ITEM_WARP_STONE:
    case ITEM_ROOM_KEY:
    case ITEM_GEM:
    case ITEM_JEWELRY:
    case ITEM_BOOK:
	break;

    case ITEM_JUKEBOX:
	for (i = 0; i < 5; i++)
	   obj->value[i] = -1;
	break;

    case ITEM_WEAPON:
        if(randomize == TRUE)
        {
            char *new_name;
            int old_weapon_type = obj->value[0];

            /* change weapon type randomly */
            obj->value[0]	= number_range(0,weapon_table_count()-1);

            /* change weapon name according to weapon type */
            new_name = strrep(obj->name, weapon_name(old_weapon_type), weapon_name(obj->value[0]));
            free_string(obj->name);
            obj->name = str_dup(new_name);
            free(new_name);

            /* change weapon short_descr according to weapon type */
            new_name = strrep(obj->short_descr, weapon_name(old_weapon_type), weapon_name(obj->value[0]));
            free_string(obj->short_descr);
            obj->short_descr = str_dup(new_name);
            free(new_name);

            /* change weapon description according to weapon type */
            new_name = strrep(obj->description, weapon_name(old_weapon_type), weapon_name(obj->value[0]));
            free_string(obj->description);
            obj->description = str_dup(new_name);
            free(new_name);

            /* change dice count */
            obj->value[1] = UMAX(1,number_range(level/11,level/9)+3);

            /* change dice type */
            obj->value[2] = UMAX(1,number_range(level/8,level/6));

            /* change dice extra */
            obj->value[3] = random_damage_type_for_weapon(obj->value[0]);

            /* add random flags to weapon */
            obj->value[4] = obj_random_weapon_flag();
        }
	break;

    case ITEM_ARMOR:
        if(randomize == TRUE)
        {
            int ac_min = UMAX(1, (obj->level + 4) / 5);
            int ac_max = UMAX(1, (obj->level + 3) / 2);

            obj->value[0] = number_range(ac_min, ac_max);
            obj->value[1] = number_range(ac_min, ac_max);
            obj->value[2] = number_range(ac_min, ac_max);
            obj->value[3] = number_range(ac_min, ac_max);
            obj->value[4] = 0;
        }
	break;

    case ITEM_WAND:
    case ITEM_STAFF:
        if(randomize == TRUE)
        {
            obj->value[0]	= (number_percent()<95)?(obj->level):(number_range(5,LEVEL_IMMORTAL)); // spell level
            obj->value[1]	= number_range( UMAX(1,(int)((obj->level+4)/5)) , UMAX(1,(int)((obj->level+3)/2)) );	// maximum number of charges
            obj->value[2]	= obj->value[1];	// current number of charges
            int valid_spells[1000];
            int count = 0;

            for (int i = 0; skill_table[i].name != NULL; i++) {
                if (skill_table[i].spell_fun != spell_null)
                    valid_spells[count++] = i;
            }
            if (count > 0)
            {
                obj->value[3] = valid_spells[number_range(0, count - 1)];
            }
            else
            {
                obj->value[3] = 0;
            }
            obj->value[4]	= 0;					// unused
        }
	break;

    case ITEM_POTION:
    case ITEM_PILL:
    case ITEM_SCROLL:
        if(randomize == TRUE)
        {
            int valid_spells[1000];
            int count = 0;

            for (int i = 0; skill_table[i].name != NULL; i++) {
                if (skill_table[i].spell_fun != spell_null)
                    valid_spells[count++] = i;
            }

            if (count > 0)
            {
                if(obj->value[2] == 0)
                {
                    if(number_range(1,100) == 1)
                    {
                        obj->value[2] = valid_spells[number_range(0, count - 1)];
                    }
                }
                if(obj->value[2] == 0)
                {
                    if(number_range(1,100) == 1)
                    {
                        obj->value[3] = valid_spells[number_range(0, count - 1)];
                    }
                }
                if(obj->value[2] == 0)
                {
                    if(number_range(1,100) == 1)
                    {
                        obj->value[4] = valid_spells[number_range(0, count - 1)];
                    }
                }
            }
            obj->cost = number_range( UMAX(1,obj->level) , UMAX(2,obj->level*3) );
        }
	break;

    case ITEM_MONEY:
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
	bugf("Get_mob_index: bad vnum %d.", vnum );
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
	bugf("Get_obj_index: bad vnum %d.", vnum );
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
	bugf("Get_room_index: bad vnum %d.", vnum );
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
	bugf("Fread_number: bad format.");
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

long char_flag( char *flagtext, int i)
{
    int number;
    char c;
    bool negative = FALSE;

	if(flagtext == 0)
		return 0;

    do
    {
        c = flagtext[i];
        i++;

        if (c == '-')
        {
        negative = TRUE;
        c = flagtext[i];
        i++;
        }

        number = 0;

        if (!isdigit(c))
        {
        while (('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z'))
        {
            number += flag_convert(c);
            c = flagtext[i];
            i++;
        }
        }

        while (isdigit(c))
        {
        number = number * 10 + c - '0';
        c = flagtext[i];
        i++;
        }

        if (c == '|')
            number += char_flag(flagtext,++i);

        else if  ( c != ' ')
            i--;

    }
    while ( c != '\0');

    if (negative)
	return -1 * number;

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
	bugf("Fread_string: MAX_STRING %d exceeded.", MAX_STRING );
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
            bugf("Fread_string: EOF");
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
		int ic;
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
        bugf("Fread_string: MAX_STRING %d exceeded.", MAX_STRING );
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
            bugf("Fread_string_eol  EOF");
            exit( 1 );
            break;
 
        case '\n':  case '\r':
            {
                union
                {
                    char *      pc;
                    char        rgc[sizeof(char *)];
                } u1;
                int ic;
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

    bugf("Fread_word: word too long.");
    exit( 1 );
    return NULL;
}

/*
 * Allocate some ordinary memory,
 *   with the expectation of freeing it someday.
 */
void *alloc_mem( int sMem )
{
    void *pMem;
    int *magic;
    int iList;

    sMem += sizeof(*magic);

    for ( iList = 0; iList < MAX_MEM_LIST; iList++ )
    {
        if ( sMem <= rgSizeList[iList] )
            break;
    }

    if ( iList == MAX_MEM_LIST )
    {
        bugf("Alloc_mem: size %d too large.", sMem );
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

    magic = (int *) pMem;
    *magic = MAGIC_NUM;
    pMem += sizeof(*magic);

    return pMem;
}



/*
 * Free some memory.
 * Recycle it back onto the free list for blocks of that size.
 */
void free_mem( void *pMem, int sMem )
{
    int iList;
    int *magic;

    pMem -= sizeof(*magic);
    magic = (int *) pMem;

    if (*magic != MAGIC_NUM)
    {
        bugf("Attempt to recyle invalid memory of size %d.", sMem);
        bugf((char*) pMem + sizeof(*magic));
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
        bugf("Free_mem: size %d too large.", sMem );
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
	bugf("Alloc_perm: %d too large.", sMem );
	exit( 1 );
    }

    if ( pMemPerm == NULL || iMemPerm + sMem > MAX_PERM_BLOCK )
    {
	iMemPerm = 0;
	if ( ( pMemPerm = calloc( 1, MAX_PERM_BLOCK ) ) == NULL )
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

    str_new = alloc_mem( strlen(str) + 1 );
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
    AREA_DATA *pArea;

    if (argument[0] != '\0')
    {
        printf_to_char(ch, "No argument is used with this command.\n\r");
        return;
    }

    printf_to_char(ch,"Bölgeler:\n\r\n\r");
    for ( pArea = area_first; pArea != NULL; pArea = pArea->next )
    {
        printf_to_char(ch,"[{W%2d %3d{x] {c%35s{x\n\r",pArea->low_range,pArea->high_range,pArea->name);
    }

    return;
}



void do_memory( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];

    sprintf( buf, "Affects %5d\n\r", top_affect    ); printf_to_char(ch, buf);
    sprintf( buf, "Areas   %5d\n\r", top_area      ); printf_to_char(ch, buf);
    sprintf( buf, "ExDes   %5d\n\r", top_ed        ); printf_to_char(ch, buf);
    sprintf( buf, "Exits   %5d\n\r", top_exit      ); printf_to_char(ch, buf);
    sprintf( buf, "Helps   %5d\n\r", top_help      ); printf_to_char(ch, buf);
    sprintf( buf, "Socials %5d\n\r", social_count  ); printf_to_char(ch, buf);
    sprintf( buf, "Mobs    %5d(%d new format)\n\r", top_mob_index,newmobs ); 
    printf_to_char(ch, buf);
    sprintf( buf, "(in use)%5d\n\r", mobile_count  ); printf_to_char(ch, buf);
    sprintf( buf, "Objs    %5d(%d new format)\n\r", top_obj_index,newobjs ); 
    printf_to_char(ch, buf);
    sprintf( buf, "Resets  %5d\n\r", top_reset     ); printf_to_char(ch, buf);
    sprintf( buf, "Rooms   %5d\n\r", top_room      ); printf_to_char(ch, buf);
    sprintf( buf, "Shops   %5d\n\r", top_shop      ); printf_to_char(ch, buf);

    sprintf( buf, "Strings %5d strings of %7d bytes (max %d).\n\r",
	nAllocString, sAllocString, MAX_STRING );
    printf_to_char(ch, buf);

    sprintf( buf, "Perms   %5d blocks  of %7d bytes.\n\r",
	nAllocPerm, sAllocPerm );
    printf_to_char(ch, buf);

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
    fprintf(fp,"MobProt %4d (%8zu bytes)\n",
        top_mob_index, (size_t)(top_mob_index * sizeof(*pMobIndex)));

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

    fprintf(fp,"Mobs    %4d (%8zu bytes), %2d free (%zu bytes)\n",
        count, (size_t)(count * sizeof(*fch)), count2, (size_t)(count2 * sizeof(*fch)));

    /* pcdata */
    count = 0;
    for (pc = pcdata_free; pc != NULL; pc = pc->next)
	count++; 

    fprintf(fp,"Pcdata  %4d (%8zu bytes), %2d free (%zu bytes)\n",
        num_pcs, (size_t)(num_pcs * sizeof(*pc)), count, (size_t)(count * sizeof(*pc)));

    /* descriptors */
    count = 0; count2 = 0;
    for (d = descriptor_list; d != NULL; d = d->next)
	count++;
    for (d= descriptor_free; d != NULL; d = d->next)
	count2++;

    fprintf(fp, "Descs  %4d (%8zu bytes), %2d free (%zu bytes)\n",
        count, (size_t)(count * sizeof(*d)), count2, (size_t)(count2 * sizeof(*d)));

    /* object prototypes */
	for (vnum = 0; nMatch < top_obj_index; vnum++) {
		if ((pObjIndex = get_obj_index(vnum)) != NULL) {
			for (af = pObjIndex->affected; af != NULL; af = af->next) {
				aff_count++;
			}
			nMatch++;
		}
	}

	fprintf(fp,"ObjProt %4d (%8zu bytes)\n",
			top_obj_index, (size_t)(top_obj_index * sizeof(*pObjIndex)));


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

    fprintf(fp,"Objs    %4d (%8zu bytes), %2d free (%zu bytes)\n",
        count, (size_t)(count * sizeof(*obj)), count2, (size_t)(count2 * sizeof(*obj)));

    /* affects */
    count = 0;
    for (af = affect_free; af != NULL; af = af->next)
	count++;

    fprintf(fp,"Affects %4d (%8zu bytes), %2d free (%zu bytes)\n",
        aff_count, (size_t)(aff_count * sizeof(*af)), count, (size_t)(count * sizeof(*af)));

    /* rooms */
    fprintf(fp,"Rooms   %4d (%8zu bytes)\n",
        top_room, (size_t)(top_room * sizeof(*room)));

     /* exits */
	fprintf(fp,"Exits   %4d (%8zu bytes)\n",
        top_exit, (size_t)(top_exit * sizeof(*exit)));

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
    switch ( number_range(0, 3) )
    {
    case 0:  number -= 1; break;
    case 3:  number += 1; break;
    }

    return UMAX( 1, number );
}



/*
 * New number_range() function.
 * Resolves modula problem.
 */
int number_range( int from, int to )
{

  int x;
  int difference;

  difference = to - from;
  if( difference == 0 ) {
    return from;
  }

  do {
    x = random();
  } while (x >= (RAND_MAX - (RAND_MAX % (difference + 1))));

  x %= (difference + 1);

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

/*
 * I've gotten too many bad reports on OS-supplied random number generators.
 * This is the Mitchell-Moore algorithm from Knuth Volume II.
 * Best to leave the constants alone unless you've read Knuth.
 * -- Furey
 */

/* I noticed streaking with this random number generator, so I switched
   back to the system srandom call.  If this doesn't work for you, 
   define OLD_RAND to use the old system -- Alander */

#if defined (OLD_RAND)
static  int     rgiState[2+55];
#endif
 
void init_mm( )
{
#if defined (OLD_RAND)
    int *piState;
    int iState;
 
    piState     = &rgiState[2];
 
    piState[-2] = 55 - 55;
    piState[-1] = 55 - 24;
 
    piState[0]  = ((int) current_time) & ((1 << 30) - 1);
    piState[1]  = 1;
    for ( iState = 2; iState < 55; iState++ )
    {
        piState[iState] = (piState[iState-1] + piState[iState-2])
                        & ((1 << 30) - 1);
    }
#else
    srandom(time(NULL)^getpid());
#endif
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
	bugf("Str_cmp: null astr.");
	return TRUE;
    }

    if ( bstr == NULL )
    {
	bugf("Str_cmp: null bstr.");
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
	bugf("Strn_cmp: null astr.");
	return TRUE;
    }

    if ( bstr == NULL )
    {
	bugf("Strn_cmp: null bstr.");
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
void append_file( CHAR_DATA *ch, char *file, char *str )
{
    FILE *fp;

    if ( IS_NPC(ch) || str[0] == '\0' )
	return;

    fclose( fpReserve );
    if ( ( fp = fopen( file, "a" ) ) == NULL )
    {
	perror( file );
	printf_to_char(ch, "Could not open the file!\n\r");
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
