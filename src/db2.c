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
#endif

#include "merc.h"
#include "db.h"
#include "lookup.h"


/* values for db2.c */
struct		social_type	social_table		[MAX_SOCIALS];
int		social_count;

/* snarf a socials file */
void load_socials( cJSON *json_data )
{
    cJSON *json_socials = NULL;
    cJSON *json_social = NULL;

    json_socials = cJSON_GetObjectItemCaseSensitive(json_data, "socials");

    if(!json_socials)
        return;

    cJSON_ArrayForEach(json_social, json_socials)
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

        strcpy(social.name, cJSON_GetObjectItemCaseSensitive( json_social, "name" )->valuestring);
        social.char_no_arg = str_dup(cJSON_GetObjectItemCaseSensitive( json_social, "char_no_arg" )->valuestring);
        social.others_no_arg = str_dup(cJSON_GetObjectItemCaseSensitive( json_social, "others_no_arg" )->valuestring);
        social.char_found = str_dup(cJSON_GetObjectItemCaseSensitive( json_social, "char_found" )->valuestring);
        social.others_found = str_dup(cJSON_GetObjectItemCaseSensitive( json_social, "others_found" )->valuestring);
        social.vict_found = str_dup(cJSON_GetObjectItemCaseSensitive( json_social, "vict_found" )->valuestring);
        social.char_not_found = str_dup(cJSON_GetObjectItemCaseSensitive( json_social, "char_not_found" )->valuestring);
        social.char_auto = str_dup(cJSON_GetObjectItemCaseSensitive( json_social, "char_auto" )->valuestring);
        social.others_auto = str_dup(cJSON_GetObjectItemCaseSensitive( json_social, "others_auto" )->valuestring);
        
        social_table[social_count] = social;
    	social_count++;
    }
}
    





/*
 * Snarf a mob section.  new style
 */
void load_mobiles( cJSON *json_data )
{
    cJSON *json_mobiles = NULL;
    cJSON *json_mobile = NULL;
    cJSON *json_mobile_affs = NULL;
    cJSON *json_mobile_aff = NULL;
    cJSON *value_array = NULL;

    json_mobiles = cJSON_GetObjectItemCaseSensitive(json_data, "mobiles");

    if(!json_mobiles)
        return;

    MOB_INDEX_DATA *pMobIndex;
 
    cJSON_ArrayForEach(json_mobile, json_mobiles)
    {
        sh_int vnum;
        int iHash;
 
        vnum                            = cJSON_GetObjectItemCaseSensitive( json_mobile, "vnum" )->valuedouble;
        if ( vnum == 0 )
            break;
 
        fBootDb = FALSE;
        if ( get_mob_index( vnum ) != NULL )
        {
            bug( "Load_mobiles: vnum %d duplicated.", vnum );
            exit( 1 );
        }
        fBootDb = TRUE;
 
        pMobIndex                       = alloc_perm( sizeof(*pMobIndex) );
        pMobIndex->vnum                 = vnum;
	    newmobs++;
        pMobIndex->player_name          = str_dup( cJSON_GetObjectItemCaseSensitive( json_mobile, "name" )->valuestring );
        pMobIndex->short_descr          = str_dup( cJSON_GetObjectItemCaseSensitive( json_mobile, "short_description" )->valuestring );
        pMobIndex->long_descr           = str_dup( cJSON_GetObjectItemCaseSensitive( json_mobile, "long_description" )->valuestring );
        pMobIndex->description          = str_dup( cJSON_GetObjectItemCaseSensitive( json_mobile, "description" )->valuestring );
	    pMobIndex->race		 	= race_lookup(cJSON_GetObjectItemCaseSensitive( json_mobile, "race" )->valuestring);
 
        pMobIndex->long_descr[0]        = UPPER(pMobIndex->long_descr[0]);
        pMobIndex->description[0]       = UPPER(pMobIndex->description[0]);
 
        pMobIndex->act                  = char_flag( (char *)cJSON_GetObjectItemCaseSensitive( json_mobile, "act" )->valuestring, 0 ) | ACT_IS_NPC
					| race_table[pMobIndex->race].act;
        pMobIndex->affected_by          = char_flag( (char *)cJSON_GetObjectItemCaseSensitive( json_mobile, "affected_by" )->valuestring, 0 )
					| race_table[pMobIndex->race].aff;
        pMobIndex->pShop                = NULL;
        pMobIndex->alignment            = cJSON_GetObjectItemCaseSensitive( json_mobile, "alignment" )->valuedouble;
        pMobIndex->group                = cJSON_GetObjectItemCaseSensitive( json_mobile, "group" )->valuedouble;

        pMobIndex->level                = cJSON_GetObjectItemCaseSensitive( json_mobile, "level" )->valuedouble;
        pMobIndex->hitroll              = cJSON_GetObjectItemCaseSensitive( json_mobile, "hitroll" )->valuedouble;

        value_array = cJSON_GetObjectItem(json_mobile, "hit_dice");
	    /* read hit dice */
        pMobIndex->hit[DICE_NUMBER]     = cJSON_GetArrayItem(value_array, 0)->valuedouble;
        pMobIndex->hit[DICE_TYPE]   	= cJSON_GetArrayItem(value_array, 1)->valuedouble;
        pMobIndex->hit[DICE_BONUS]      = cJSON_GetArrayItem(value_array, 2)->valuedouble;

        value_array = cJSON_GetObjectItem(json_mobile, "mana_dice");
        /* read mana dice */
        pMobIndex->mana[DICE_NUMBER]	= cJSON_GetArrayItem(value_array, 0)->valuedouble;
        pMobIndex->mana[DICE_TYPE]	    = cJSON_GetArrayItem(value_array, 1)->valuedouble;
        pMobIndex->mana[DICE_BONUS]	    = cJSON_GetArrayItem(value_array, 2)->valuedouble;

        value_array = cJSON_GetObjectItem(json_mobile, "dam_dice");
        /* read damage dice */
        pMobIndex->damage[DICE_NUMBER]	= cJSON_GetArrayItem(value_array, 0)->valuedouble;
        pMobIndex->damage[DICE_TYPE]	= cJSON_GetArrayItem(value_array, 1)->valuedouble;
        pMobIndex->damage[DICE_BONUS]	= cJSON_GetArrayItem(value_array, 2)->valuedouble;

        pMobIndex->dam_type		= attack_lookup(cJSON_GetObjectItemCaseSensitive( json_mobile, "dam_type" )->valuestring);

        /* read armor class */
        pMobIndex->ac[AC_PIERCE]	= cJSON_GetObjectItemCaseSensitive( json_mobile, "ac_pierce" )->valuedouble * 10;
        pMobIndex->ac[AC_BASH]		= cJSON_GetObjectItemCaseSensitive( json_mobile, "ac_bash" )->valuedouble * 10;
        pMobIndex->ac[AC_SLASH]		= cJSON_GetObjectItemCaseSensitive( json_mobile, "ac_slash" )->valuedouble * 10;
        pMobIndex->ac[AC_EXOTIC]	= cJSON_GetObjectItemCaseSensitive( json_mobile, "ac_exotic" )->valuedouble * 10;

        /* read flags and add in data from the race table */
        pMobIndex->off_flags		= char_flag( (char *)cJSON_GetObjectItemCaseSensitive( json_mobile, "off_flags" )->valuestring, 0 )
                        | race_table[pMobIndex->race].off;
        pMobIndex->imm_flags		= char_flag( (char *)cJSON_GetObjectItemCaseSensitive( json_mobile, "imm_flags" )->valuestring, 0 )
                        | race_table[pMobIndex->race].imm;
        pMobIndex->res_flags		= char_flag( (char *)cJSON_GetObjectItemCaseSensitive( json_mobile, "res_flags" )->valuestring, 0 )
                        | race_table[pMobIndex->race].res;
        pMobIndex->vuln_flags		= char_flag( (char *)cJSON_GetObjectItemCaseSensitive( json_mobile, "vuln_flags" )->valuestring, 0 )
                        | race_table[pMobIndex->race].vuln;

        /* vital statistics */
        pMobIndex->start_pos		= position_lookup(cJSON_GetObjectItemCaseSensitive( json_mobile, "start_pos" )->valuestring);
        pMobIndex->default_pos		= position_lookup(cJSON_GetObjectItemCaseSensitive( json_mobile, "default_pos" )->valuestring);
        pMobIndex->sex			= sex_lookup(cJSON_GetObjectItemCaseSensitive( json_mobile, "sex" )->valuestring);

        pMobIndex->wealth		= cJSON_GetObjectItemCaseSensitive( json_mobile, "wealth" )->valuedouble;

        pMobIndex->form			= char_flag( (char *)cJSON_GetObjectItemCaseSensitive( json_mobile, "form" )->valuestring, 0 )
                        | race_table[pMobIndex->race].form;
        pMobIndex->parts		= char_flag( (char *)cJSON_GetObjectItemCaseSensitive( json_mobile, "parts" )->valuestring, 0 )
                        | race_table[pMobIndex->race].parts;
        /* size */
        pMobIndex->size			= size_lookup(cJSON_GetObjectItemCaseSensitive( json_mobile, "size" )->valuestring);
        pMobIndex->material		= str_dup(cJSON_GetObjectItemCaseSensitive( json_mobile, "material" )->valuestring);
    
        json_mobile_affs = cJSON_GetObjectItemCaseSensitive(json_mobile, "affects_f");

        cJSON_ArrayForEach(json_mobile_aff, json_mobile_affs)
        {
            char *word;
            long vector;

            word                    = str_dup(cJSON_GetObjectItemCaseSensitive( json_mobile_aff, "word" )->valuestring);
            vector                  = char_flag( (char *)cJSON_GetObjectItemCaseSensitive( json_mobile_aff, "flag" )->valuestring, 0 );

            if (!str_prefix(word,"act"))
                REMOVE_BIT(pMobIndex->act,vector);
            else if (!str_prefix(word,"aff"))
                REMOVE_BIT(pMobIndex->affected_by,vector);
            else if (!str_prefix(word,"off"))
                REMOVE_BIT(pMobIndex->off_flags,vector);
            else if (!str_prefix(word,"imm"))
                REMOVE_BIT(pMobIndex->imm_flags,vector);
            else if (!str_prefix(word,"res"))
                REMOVE_BIT(pMobIndex->res_flags,vector);
            else if (!str_prefix(word,"vul"))
                REMOVE_BIT(pMobIndex->vuln_flags,vector);
            else if (!str_prefix(word,"for"))
                REMOVE_BIT(pMobIndex->form,vector);
            else if (!str_prefix(word,"par"))
                REMOVE_BIT(pMobIndex->parts,vector);
            else
            {
                bug("Flag remove: flag not found.",0);
                exit(1);
            }
        }

        iHash                   = vnum % MAX_KEY_HASH;
        pMobIndex->next         = mob_index_hash[iHash];
        mob_index_hash[iHash]   = pMobIndex;
        top_mob_index++;
        kill_table[URANGE(0, pMobIndex->level, MAX_LEVEL-1)].number++;
    }
 
    return;
}

/*
 * Snarf an obj section. new style
 */
void load_objects( cJSON *json_data )
{
    cJSON *json_objects = NULL;
    cJSON *json_object = NULL;
    char value_array[MAX_STRING_LENGTH];

    json_objects = cJSON_GetObjectItemCaseSensitive(json_data, "objects");

    if(!json_objects)
        return;

    OBJ_INDEX_DATA *pObjIndex;
 
    cJSON_ArrayForEach(json_object, json_objects)
    {
        sh_int vnum;
        int iHash;
 
        vnum                            = cJSON_GetObjectItemCaseSensitive( json_object, "vnum" )->valuedouble;

        if ( vnum == 0 )
            break;
 
        fBootDb = FALSE;
        if ( get_obj_index( vnum ) != NULL )
        {
            bug( "Load_objects: vnum %d duplicated.", vnum );
            exit( 1 );
        }
        fBootDb = TRUE;
 
        pObjIndex                       = alloc_perm( sizeof(*pObjIndex) );
        pObjIndex->vnum                 = vnum;
	    pObjIndex->reset_num		= 0;
	    newobjs++;
        pObjIndex->name                 = str_dup( cJSON_GetObjectItemCaseSensitive( json_object, "name" )->valuestring );
        pObjIndex->short_descr          = str_dup( cJSON_GetObjectItemCaseSensitive( json_object, "short_description" )->valuestring );
        pObjIndex->description          = str_dup( cJSON_GetObjectItemCaseSensitive( json_object, "description" )->valuestring );
        pObjIndex->material		        = str_dup( cJSON_GetObjectItemCaseSensitive( json_object, "material" )->valuestring );
 
        pObjIndex->item_type            = item_lookup(cJSON_GetObjectItemCaseSensitive( json_object, "type" )->valuestring);
        pObjIndex->extra_flags          = char_flag( (char *)cJSON_GetObjectItemCaseSensitive( json_object, "extra_flags" )->valuestring, 0 );
        pObjIndex->wear_flags           = char_flag( (char *)cJSON_GetObjectItemCaseSensitive( json_object, "wear_flags" )->valuestring, 0 );

        strcpy(value_array, (char *)cJSON_GetObjectItemCaseSensitive(json_object, "values")->valuestring);

        switch(pObjIndex->item_type)
        {
        case ITEM_WEAPON:
            pObjIndex->value[0]		= weapon_type(strtok(value_array," "));
            pObjIndex->value[1]		= atoi(strtok(NULL," "));
            pObjIndex->value[2]		= atoi(strtok(NULL," "));
            pObjIndex->value[3]		= attack_lookup(strtok(NULL," "));
            pObjIndex->value[4]		= char_flag( strtok(NULL," "), 0 );
            break;
        case ITEM_CONTAINER:
            pObjIndex->value[0]		= atoi(strtok(value_array," "));
            pObjIndex->value[1]		= char_flag( strtok(NULL," "), 0 );
            pObjIndex->value[2]		= atoi(strtok(NULL," "));
            pObjIndex->value[3]		= atoi(strtok(NULL," "));
            pObjIndex->value[4]		= atoi(strtok(NULL," "));
            break;
        case ITEM_DRINK_CON:
        case ITEM_FOUNTAIN:
                pObjIndex->value[0]         = atoi(strtok(value_array," "));
                pObjIndex->value[1]         = atoi(strtok(NULL," "));
                pObjIndex->value[2]         = liq_lookup(strtok(NULL," "));
                pObjIndex->value[3]         = atoi(strtok(NULL," "));
                pObjIndex->value[4]         = atoi(strtok(NULL," "));
                break;
        case ITEM_WAND:
        case ITEM_STAFF:
            pObjIndex->value[0]		= atoi(strtok(value_array," "));
            pObjIndex->value[1]		= atoi(strtok(NULL," "));
            pObjIndex->value[2]		= atoi(strtok(NULL," "));
            pObjIndex->value[3]		= skill_lookup(strtok(NULL," "));
            pObjIndex->value[4]		= atoi(strtok(NULL," "));
            break;
        case ITEM_POTION:
        case ITEM_PILL:
        case ITEM_SCROLL:
        case ITEM_BOOK:
            pObjIndex->value[0]		= atoi(strtok(value_array," "));
            pObjIndex->value[1]		= skill_lookup(strtok(NULL," "));
            pObjIndex->value[2]		= skill_lookup(strtok(NULL," "));
            pObjIndex->value[3]		= skill_lookup(strtok(NULL," "));
            pObjIndex->value[4]		= skill_lookup(strtok(NULL," "));
            break;
        default:
            pObjIndex->value[0]             = char_flag( strtok(value_array," "), 0 );
            pObjIndex->value[1]             = char_flag( strtok(NULL," "), 0 );
            pObjIndex->value[2]             = char_flag( strtok(NULL," "), 0 );
            pObjIndex->value[3]             = char_flag( strtok(NULL," "), 0 );
            pObjIndex->value[4]		        = char_flag( strtok(NULL," "), 0 );
            break;
        }
	    pObjIndex->level		            = cJSON_GetObjectItemCaseSensitive( json_object, "level" )->valuedouble;
        pObjIndex->weight               = cJSON_GetObjectItemCaseSensitive( json_object, "weight" )->valuedouble;
        pObjIndex->cost                 = cJSON_GetObjectItemCaseSensitive( json_object, "cost" )->valuedouble;

        switch (cJSON_GetObjectItemCaseSensitive( json_object, "condition" )->valuestring[0])
        {
            case ('P') :		pObjIndex->condition = 100; break;
            case ('G') :		pObjIndex->condition =  90; break;
            case ('A') :		pObjIndex->condition =  75; break;
            case ('W') :		pObjIndex->condition =  50; break;
            case ('D') :		pObjIndex->condition =  25; break;
            case ('B') :		pObjIndex->condition =  10; break;
            case ('R') :		pObjIndex->condition =   0; break;
            default:			pObjIndex->condition = 100; break;
        }

        cJSON *json_object_a_affs = NULL;
        cJSON *json_object_a_aff = NULL;
        cJSON *json_object_f_affs = NULL;
        cJSON *json_object_f_aff = NULL;
        cJSON *json_object_extra_descriptions = NULL;
        cJSON *json_object_extra_description = NULL;

        json_object_a_affs = cJSON_GetObjectItemCaseSensitive(json_object, "affects_a");
        json_object_f_affs = cJSON_GetObjectItemCaseSensitive(json_object, "affects_f");
        json_object_extra_descriptions = cJSON_GetObjectItemCaseSensitive(json_object, "extra_descriptions");
 
        cJSON_ArrayForEach(json_object_a_aff, json_object_a_affs)
        {
            AFFECT_DATA *paf;
    
            paf = alloc_perm( sizeof(*paf) );
            paf->where		= TO_OBJECT;
            paf->type               = -1;
            paf->level              = pObjIndex->level;
            paf->duration           = -1;
            paf->location           = cJSON_GetObjectItemCaseSensitive( json_object_a_aff, "location" )->valuedouble;
            paf->modifier           = cJSON_GetObjectItemCaseSensitive( json_object_a_aff, "modifier" )->valuedouble;
            paf->bitvector          = 0;
            paf->next               = pObjIndex->affected;
            pObjIndex->affected     = paf;
            top_affect++;
        }
        cJSON_ArrayForEach(json_object_f_aff, json_object_f_affs)
        {
            AFFECT_DATA *paf;
    
            paf = alloc_perm( sizeof(*paf) );
            switch (cJSON_GetObjectItemCaseSensitive( json_object_f_aff, "where" )->valuestring[0])
            {
                case 'A':
                    paf->where          = TO_AFFECTS;
                    break;
                case 'I':
                    paf->where		= TO_IMMUNE;
                    break;
                case 'R':
                    paf->where		= TO_RESIST;
                    break;
                case 'V':
                    paf->where		= TO_VULN;
                    break;
                default:
                    bug( "Load_objects: Bad where on flag set.", 0 );
                    exit( 1 );
            }
            paf->type               = -1;
            paf->level              = pObjIndex->level;
            paf->duration           = -1;
            paf->location           = cJSON_GetObjectItemCaseSensitive( json_object_f_aff, "location" )->valuedouble;
            paf->modifier           = cJSON_GetObjectItemCaseSensitive( json_object_f_aff, "modifier" )->valuedouble;
            paf->bitvector          = char_flag( (char *)cJSON_GetObjectItemCaseSensitive( json_object_f_aff, "bitvector" )->valuestring, 0 );
            paf->next               = pObjIndex->affected;
            pObjIndex->affected     = paf;
            top_affect++;
            
        }
        cJSON_ArrayForEach(json_object_extra_description, json_object_extra_descriptions)
        {
            EXTRA_DESCR_DATA *ed;

            ed			= (EXTRA_DESCR_DATA *)alloc_perm( sizeof(*ed) );
            ed->keyword		= str_dup( cJSON_GetObjectItemCaseSensitive( json_object_extra_description, "keyword" )->valuestring );
            ed->description		= str_dup( cJSON_GetObjectItemCaseSensitive( json_object_extra_description, "description" )->valuestring );
            ed->next		= pObjIndex->extra_descr;
            pObjIndex->extra_descr  = ed;
            top_ed++;
        }
 
        iHash                   = vnum % MAX_KEY_HASH;
        pObjIndex->next         = obj_index_hash[iHash];
        obj_index_hash[iHash]   = pObjIndex;
        top_obj_index++;
    }
 
    return;
}

