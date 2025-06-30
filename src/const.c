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

#include <sys/types.h>
#include <stdio.h>
#include <time.h>
#include "merc.h"
#include "magic.h"
#include "interp.h"


/* item type list */
const struct item_type		item_table	[]	=
{
    {	ITEM_LIGHT,	"light"		},
    {	ITEM_SCROLL,	"scroll"	},
    {	ITEM_WAND,	"wand"		},
    {   ITEM_STAFF,	"staff"		},
    {   ITEM_WEAPON,	"weapon"	},
    {   ITEM_TREASURE,	"treasure"	},
    {   ITEM_ARMOR,	"armor"		},
    {	ITEM_POTION,	"potion"	},
    {	ITEM_CLOTHING,	"clothing"	},
    {   ITEM_FURNITURE,	"furniture"	},
    {	ITEM_TRASH,	"trash"		},
    {	ITEM_CONTAINER,	"container"	},
    {	ITEM_DRINK_CON, "drink"		},
    {	ITEM_KEY,	"key"		},
    {	ITEM_FOOD,	"food"		},
    {	ITEM_MONEY,	"money"		},
    {	ITEM_BOAT,	"boat"		},
    {	ITEM_CORPSE_NPC,"npc_corpse"	},
    {	ITEM_CORPSE_PC,	"pc_corpse"	},
    {   ITEM_FOUNTAIN,	"fountain"	},
    {	ITEM_PILL,	"pill"		},
    {	ITEM_PROTECT,	"protect"	},
    {	ITEM_MAP,	"map"		},
    {	ITEM_PORTAL,	"portal"	},
    {	ITEM_WARP_STONE,"warp_stone"	},
    {	ITEM_ROOM_KEY,	"room_key"	},
    {	ITEM_GEM,	"gem"		},
    {	ITEM_JEWELRY,	"jewelry"	},
    {   ITEM_JUKEBOX,	"jukebox"	},
    {   0,		NULL		}
};


/* weapon selection table */
const	struct	weapon_type	weapon_table	[]	=
{
   { "sword",	OBJ_VNUM_SCHOOL_SWORD,	WEAPON_SWORD,	&gsn_sword	},
   { "mace",	OBJ_VNUM_SCHOOL_MACE,	WEAPON_MACE,	&gsn_mace 	},
   { "dagger",	OBJ_VNUM_SCHOOL_DAGGER,	WEAPON_DAGGER,	&gsn_dagger	},
   { "axe",	OBJ_VNUM_SCHOOL_AXE,	WEAPON_AXE,	&gsn_axe	},
   { "staff",	OBJ_VNUM_SCHOOL_STAFF,	WEAPON_SPEAR,	&gsn_spear	},
   { "flail",	OBJ_VNUM_SCHOOL_FLAIL,	WEAPON_FLAIL,	&gsn_flail	},
   { "whip",	OBJ_VNUM_SCHOOL_WHIP,	WEAPON_WHIP,	&gsn_whip	},
   { "polearm",	OBJ_VNUM_SCHOOL_POLEARM,WEAPON_POLEARM,	&gsn_polearm	},
   { NULL,	0,				0,	NULL		}
};


 
/* wiznet table and prototype for future flag setting */
const   struct wiznet_type      wiznet_table    []              =
{
   {    "on",           WIZ_ON,         IM },
   {    "prefix",	WIZ_PREFIX,	IM },
   {    "ticks",        WIZ_TICKS,      IM },
   {    "logins",       WIZ_LOGINS,     IM },
   {    "sites",        WIZ_SITES,      L4 },
   {    "links",        WIZ_LINKS,      L7 },
   {	"newbies",	WIZ_NEWBIE,	IM },
   {	"spam",		WIZ_SPAM,	L5 },
   {    "deaths",       WIZ_DEATHS,     IM },
   {    "resets",       WIZ_RESETS,     L4 },
   {    "mobdeaths",    WIZ_MOBDEATHS,  L4 },
   {    "flags",	WIZ_FLAGS,	L5 },
   {	"penalties",	WIZ_PENALTIES,	L5 },
   {	"saccing",	WIZ_SACCING,	L5 },
   {	"levels",	WIZ_LEVELS,	IM },
   {	"load",		WIZ_LOAD,	L2 },
   {	"restore",	WIZ_RESTORE,	L2 },
   {	"snoops",	WIZ_SNOOPS,	L2 },
   {	"switches",	WIZ_SWITCHES,	L2 },
   {	"secure",	WIZ_SECURE,	L1 },
   {	NULL,		0,		0  }
};

/* attack table  -- not very organized :( */
const 	struct attack_type	attack_table	[MAX_DAMAGE_MESSAGE]	=
{
    { 	"none",		"hit",		-1		},  /*  0 */
    {	"slice",	"slice", 	DAM_SLASH	},	
    {   "stab",		"stab",		DAM_PIERCE	},
    {	"slash",	"slash",	DAM_SLASH	},
    {	"whip",		"whip",		DAM_SLASH	},
    {   "claw",		"claw",		DAM_SLASH	},  /*  5 */
    {	"blast",	"blast",	DAM_BASH	},
    {   "pound",	"pound",	DAM_BASH	},
    {	"crush",	"crush",	DAM_BASH	},
    {   "grep",		"grep",		DAM_SLASH	},
    {	"bite",		"bite",		DAM_PIERCE	},  /* 10 */
    {   "pierce",	"pierce",	DAM_PIERCE	},
    {   "suction",	"suction",	DAM_BASH	},
    {	"beating",	"beating",	DAM_BASH	},
    {   "digestion",	"digestion",	DAM_ACID	},
    {	"charge",	"charge",	DAM_BASH	},  /* 15 */
    { 	"slap",		"slap",		DAM_BASH	},
    {	"punch",	"punch",	DAM_BASH	},
    {	"wrath",	"wrath",	DAM_ENERGY	},
    {	"magic",	"magic",	DAM_ENERGY	},
    {   "divine",	"divine power",	DAM_HOLY	},  /* 20 */
    {	"cleave",	"cleave",	DAM_SLASH	},
    {	"scratch",	"scratch",	DAM_PIERCE	},
    {   "peck",		"peck",		DAM_PIERCE	},
    {   "peckb",	"peck",		DAM_BASH	},
    {   "chop",		"chop",		DAM_SLASH	},  /* 25 */
    {   "sting",	"sting",	DAM_PIERCE	},
    {   "smash",	 "smash",	DAM_BASH	},
    {   "shbite",	"shocking bite",DAM_LIGHTNING	},
    {	"flbite",	"flaming bite", DAM_FIRE	},
    {	"frbite",	"freezing bite", DAM_COLD	},  /* 30 */
    {	"acbite",	"acidic bite", 	DAM_ACID	},
    {	"chomp",	"chomp",	DAM_PIERCE	},
    {  	"drain",	"life drain",	DAM_NEGATIVE	},
    {   "thrust",	"thrust",	DAM_PIERCE	},
    {   "slime",	"slime",	DAM_ACID	},
    {	"shock",	"shock",	DAM_LIGHTNING	},
    {   "thwack",	"thwack",	DAM_BASH	},
    {   "flame",	"flame",	DAM_FIRE	},
    {   "chill",	"chill",	DAM_COLD	},
    {   NULL,		NULL,		0		}
};

/* race table */
const 	struct	race_type	race_table	[]		=
{
/*
    {
	name,		pc_race?,
	act bits,	aff_by bits,	off bits,
	imm,		res,		vuln,
	form,		parts 
    },
*/
    { "unique",		FALSE, 0, 0, 0, 0, 0, 0, 0, 0 },

    { 
	"human",		TRUE, 
	0,		0, 		0,
	0, 		0,		0,
	A|H|M|V,	A|B|C|D|E|F|G|H|I|J|K
    },

    {
	"elf",			TRUE,
	0,		AFF_INFRARED,	0,
	0,		RES_CHARM,	VULN_IRON,
	A|H|M|V,	A|B|C|D|E|F|G|H|I|J|K
    },

    {
	"dwarf",		TRUE,
	0,		AFF_INFRARED,	0,
	0,		RES_POISON|RES_DISEASE, VULN_DROWNING,
	A|H|M|V,	A|B|C|D|E|F|G|H|I|J|K
    },

    {
	"giant",		TRUE,
	0,		0,		0,
	0,		RES_FIRE|RES_COLD,	VULN_MENTAL|VULN_LIGHTNING,
	A|H|M|V,	A|B|C|D|E|F|G|H|I|J|K
    },

    {
	"bat",			FALSE,
	0,		AFF_FLYING|AFF_DARK_VISION,	OFF_DODGE|OFF_FAST,
	0,		0,		VULN_LIGHT,
	A|G|V,		A|C|D|E|F|H|J|K|P
    },

    {
	"bear",			FALSE,
	0,		0,		OFF_CRUSH|OFF_DISARM|OFF_BERSERK,
	0,		RES_BASH|RES_COLD,	0,
	A|G|V,		A|B|C|D|E|F|H|J|K|U|V
    },

    {
	"cat",			FALSE,
	0,		AFF_DARK_VISION,	OFF_FAST|OFF_DODGE,
	0,		0,		0,
	A|G|V,		A|C|D|E|F|H|J|K|Q|U|V
    },

    {
	"centipede",		FALSE,
	0,		AFF_DARK_VISION,	0,
	0,		RES_PIERCE|RES_COLD,	VULN_BASH,
 	A|B|G|O,		A|C|K	
    },

    {
	"dog",			FALSE,
	0,		0,		OFF_FAST,
	0,		0,		0,
	A|G|V,		A|C|D|E|F|H|J|K|U|V
    },

    {
	"doll",			FALSE,
	0,		0,		0,
	IMM_COLD|IMM_POISON|IMM_HOLY|IMM_NEGATIVE|IMM_MENTAL|IMM_DISEASE
	|IMM_DROWNING,	RES_BASH|RES_LIGHT,
	VULN_SLASH|VULN_FIRE|VULN_ACID|VULN_LIGHTNING|VULN_ENERGY,
	E|J|M|cc,	A|B|C|G|H|K
    },

    { 	"dragon", 		FALSE, 
	0, 			AFF_INFRARED|AFF_FLYING,	0,
	0,			RES_FIRE|RES_BASH|RES_CHARM, 
	VULN_PIERCE|VULN_COLD,
	A|H|Z,		A|C|D|E|F|G|H|I|J|K|P|Q|U|V|X
    },

    {
	"fido",			FALSE,
	0,		0,		OFF_DODGE|ASSIST_RACE,
	0,		0,			VULN_MAGIC,
	A|B|G|V,	A|C|D|E|F|H|J|K|Q|V
    },		
   
    {
	"fox",			FALSE,
	0,		AFF_DARK_VISION,	OFF_FAST|OFF_DODGE,
	0,		0,		0,
	A|G|V,		A|C|D|E|F|H|J|K|Q|V
    },

    {
	"goblin",		FALSE,
	0,		AFF_INFRARED,	0,
	0,		RES_DISEASE,	VULN_MAGIC,
	A|H|M|V,	A|B|C|D|E|F|G|H|I|J|K
    },

    {
	"hobgoblin",		FALSE,
	0,		AFF_INFRARED,	0,
	0,		RES_DISEASE|RES_POISON,	0,
	A|H|M|V,        A|B|C|D|E|F|G|H|I|J|K|Y
    },

    {
	"kobold",		FALSE,
	0,		AFF_INFRARED,	0,
	0,		RES_POISON,	VULN_MAGIC,
	A|B|H|M|V,	A|B|C|D|E|F|G|H|I|J|K|Q
    },

    {
	"lizard",		FALSE,
	0,		0,		0,
	0,		RES_POISON,	VULN_COLD,
	A|G|X|cc,	A|C|D|E|F|H|K|Q|V
    },

    {
	"modron",		FALSE,
	0,		AFF_INFRARED,		ASSIST_RACE|ASSIST_ALIGN,
	IMM_CHARM|IMM_DISEASE|IMM_MENTAL|IMM_HOLY|IMM_NEGATIVE,
			RES_FIRE|RES_COLD|RES_ACID,	0,
	H,		A|B|C|G|H|J|K
    },

    {
	"orc",			FALSE,
	0,		AFF_INFRARED,	0,
	0,		RES_DISEASE,	VULN_LIGHT,
	A|H|M|V,	A|B|C|D|E|F|G|H|I|J|K
    },

    {
	"pig",			FALSE,
	0,		0,		0,
	0,		0,		0,
	A|G|V,	 	A|C|D|E|F|H|J|K
    },	

    {
	"rabbit",		FALSE,
	0,		0,		OFF_DODGE|OFF_FAST,
	0,		0,		0,
	A|G|V,		A|C|D|E|F|H|J|K
    },
    
    {
	"school monster",	FALSE,
	ACT_NOALIGN,		0,		0,
	IMM_CHARM|IMM_SUMMON,	0,		VULN_MAGIC,
	A|M|V,		A|B|C|D|E|F|H|J|K|Q|U
    },	

    {
	"snake",		FALSE,
	0,		0,		0,
	0,		RES_POISON,	VULN_COLD,
	A|G|X|Y|cc,	A|D|E|F|K|L|Q|V|X
    },
 
    {
	"song bird",		FALSE,
	0,		AFF_FLYING,		OFF_FAST|OFF_DODGE,
	0,		0,		0,
	A|G|W,		A|C|D|E|F|H|K|P
    },

    {
	"troll",		FALSE,
	0,		AFF_REGENERATION|AFF_INFRARED|AFF_DETECT_HIDDEN,
	OFF_BERSERK,
 	0,	RES_CHARM|RES_BASH,	VULN_FIRE|VULN_ACID,
	A|B|H|M|V,		A|B|C|D|E|F|G|H|I|J|K|U|V
    },

    {
	"water fowl",		FALSE,
	0,		AFF_SWIM|AFF_FLYING,	0,
	0,		RES_DROWNING,		0,
	A|G|W,		A|C|D|E|F|H|K|P
    },		
  
    {
	"wolf",			FALSE,
	0,		AFF_DARK_VISION,	OFF_FAST|OFF_DODGE,
	0,		0,		0,	
	A|G|V,		A|C|D|E|F|J|K|Q|V
    },

    {
	"wyvern",		FALSE,
	0,		AFF_FLYING|AFF_DETECT_INVIS|AFF_DETECT_HIDDEN,
	OFF_BASH|OFF_FAST|OFF_DODGE,
	IMM_POISON,	0,	VULN_LIGHT,
	A|B|G|Z,		A|C|D|E|F|H|J|K|Q|V|X
    },

    {
	"unique",		FALSE,
	0,		0,		0,
	0,		0,		0,		
	0,		0
    },


    {
	NULL, 0, 0, 0, 0, 0, 0
    }
};

const	struct	pc_race_type	pc_race_table	[]	=
{
    { "null race", "", 0,
      { "" }, { 13, 13, 13, 13, 13 }, { 18, 18, 18, 18, 18 }, 0,
      0, 0, 75, 20, -4, 10, 15, TRUE },

    {
	"human",	"Human",	0,
	{ "" },
	{ 13, 13, 13, 13, 13 },	{ 18, 18, 18, 18, 18 },	SIZE_MEDIUM,
	STAT_INT, OBJ_VNUM_SCHOOL_SWORD, 75, 20, -4,  10,  15,  TRUE
    },

    { 	
	"elf",		" Elf ",	5,
	{ "sneak", "hide" },
	{ 12, 14, 13, 15, 11 },	{ 16, 20, 18, 21, 15 }, SIZE_SMALL,
	STAT_DEX, OBJ_VNUM_SCHOOL_DAGGER, 75, 22, -6, 8, 12, TRUE
    },

    {
	"dwarf",	"Dwarf",	8,
	{ "berserk" },
	{ 14, 12, 14, 10, 15 },	{ 20, 16, 19, 14, 21 }, SIZE_MEDIUM,
	STAT_CON, OBJ_VNUM_SCHOOL_AXE, 75, 20, -3, 12, 18, FALSE
    },

    {
	"giant",	"Giant",	6,
	{ "bash", "fast healing" },
	{ 16, 11, 13, 11, 14 },	{ 22, 15, 18, 15, 20 }, SIZE_LARGE,
	STAT_STR, OBJ_VNUM_SCHOOL_MACE, 75, 18, -2, 15, 22, FALSE
    }
};

/*
 * Titles.
 */
const struct title_type title_table[] = {
	{ NULL }, // 0
    { "the Initiate" }, // 1
    { "the Learner" },
    { "the Novice" },
    { "the Apprentice" },
    { "the Adept" },
    { "the Seeker" },
    { "the Explorer" },
    { "the Wanderer" },
    { "the Watcher" },
    { "the Student" }, // 10
    { "the Observer" },
    { "the Scribe" },
    { "the Disciple" },
    { "the Acolyte" },
    { "the Follower" },
    { "the Keeper" },
    { "the Guard" },
    { "the Knight Errant" },
    { "the Knight" },
    { "the Crusader" }, // 20
    { "the Warrior" },
    { "the Veteran" },
    { "the Defender" },
    { "the Protector" },
    { "the Hunter" },
    { "the Challenger" },
    { "the Champion" },
    { "the Warden" },
    { "the Tracker" },
    { "the Ranger" }, // 30
    { "the Pathfinder" },
    { "the Outrider" },
    { "the Sentinel" },
    { "the Avenger" },
    { "the Mercenary" },
    { "the Commander" },
    { "the Marshal" },
    { "the Hero" },
    { "the Valiant" },
    { "the Brave" }, // 40
    { "the Wise" },
    { "the Scholar" },
    { "the Sage" },
    { "the Mystic" },
    { "the Adept of Lore" },
    { "the Spellbinder" },
    { "the Enchanter" },
    { "the Sorcerer" },
    { "the Arcanist" },
    { "the Magus" }, // 50
    { "the Archon" },
    { "the Seer" },
    { "the Visionary" },
    { "the Oracle" },
    { "the Lightbearer" },
    { "the Flamekeeper" },
    { "the Stormcaller" },
    { "the Shadowwalker" },
    { "the Dreamer" },
    { "the Awakened" }, // 60
    { "the Ascendant" },
    { "the Paragon" },
    { "the Redeemer" },
    { "the Harbinger" },
    { "the Just" },
    { "the Calm" },
    { "the Fierce" },
    { "the Silent Blade" },
    { "the Iron Will" },
    { "the Flame of Hope" }, // 70
    { "the Stonefist" },
    { "the Windborn" },
    { "the Stormheart" },
    { "the Bloodsworn" },
    { "the Unbroken" },
    { "the Eternal" },
    { "the Vindicator" },
    { "the Immortal" },
    { "the Timeless" },
    { "the Boundless" }, // 80
    { "the Supreme" },
    { "the Peerless" },
    { "the Infallible" },
    { "the Incorruptible" },
    { "the Omniscient" },
    { "the Transcendent" },
    { "the Infinite" },
    { "the Celestial" },
    { "the Cosmic Flame" },
    { "the Voidwalker" }, // 90
    { "the Soulkeeper" },
    { "the Spiritborn" },
    { "the Unity" },
    { "the Echo of Ages" },
    { "the Truthseeker" },
    { "the Final Dawn" },
    { "the Last Hope" },
    { "the Eternal Flame" },
    { "the One Above All" },
    { "the Myth" }, // 100
    { "the Legend" },
    { "the Nameless" },
    { "the Kurgan" }
};



/*
 * Attribute bonus tables.
 */
const	struct	str_app_type	str_app		[26]		=
{
    { -5, -4,   0,  0 },  /* 0  */
    { -5, -4,   3,  1 },  /* 1  */
    { -3, -2,   3,  2 },
    { -3, -1,  10,  3 },  /* 3  */
    { -2, -1,  25,  4 },
    { -2, -1,  55,  5 },  /* 5  */
    { -1,  0,  80,  6 },
    { -1,  0,  90,  7 },
    {  0,  0, 100,  8 },
    {  0,  0, 100,  9 },
    {  0,  0, 115, 10 }, /* 10  */
    {  0,  0, 115, 11 },
    {  0,  0, 130, 12 },
    {  0,  0, 130, 13 }, /* 13  */
    {  0,  1, 140, 14 },
    {  1,  1, 150, 15 }, /* 15  */
    {  1,  2, 165, 16 },
    {  2,  3, 180, 22 },
    {  2,  3, 200, 25 }, /* 18  */
    {  3,  4, 225, 30 },
    {  3,  5, 250, 35 }, /* 20  */
    {  4,  6, 300, 40 },
    {  4,  6, 350, 45 },
    {  5,  7, 400, 50 },
    {  5,  8, 450, 55 },
    {  6,  9, 500, 60 }  /* 25   */
};



const	struct	int_app_type	int_app		[26]		=
{
    {  3 },	/*  0 */
    {  5 },	/*  1 */
    {  7 },
    {  8 },	/*  3 */
    {  9 },
    { 10 },	/*  5 */
    { 11 },
    { 12 },
    { 13 },
    { 15 },
    { 17 },	/* 10 */
    { 19 },
    { 22 },
    { 25 },
    { 28 },
    { 31 },	/* 15 */
    { 34 },
    { 37 },
    { 40 },	/* 18 */
    { 44 },
    { 49 },	/* 20 */
    { 55 },
    { 60 },
    { 70 },
    { 80 },
    { 85 }	/* 25 */
};



const	struct	wis_app_type	wis_app		[26]		=
{
    { 0 },	/*  0 */
    { 0 },	/*  1 */
    { 0 },
    { 0 },	/*  3 */
    { 0 },
    { 1 },	/*  5 */
    { 1 },
    { 1 },
    { 1 },
    { 1 },
    { 1 },	/* 10 */
    { 1 },
    { 1 },
    { 1 },
    { 1 },
    { 2 },	/* 15 */
    { 2 },
    { 2 },
    { 3 },	/* 18 */
    { 3 },
    { 3 },	/* 20 */
    { 3 },
    { 4 },
    { 4 },
    { 4 },
    { 5 }	/* 25 */
};



const	struct	dex_app_type	dex_app		[26]		=
{
    {   60 },   /* 0 */
    {   50 },   /* 1 */
    {   50 },
    {   40 },
    {   30 },
    {   20 },   /* 5 */
    {   10 },
    {    0 },
    {    0 },
    {    0 },
    {    0 },   /* 10 */
    {    0 },
    {    0 },
    {    0 },
    {    0 },
    { - 10 },   /* 15 */
    { - 15 },
    { - 20 },
    { - 30 },
    { - 40 },
    { - 50 },   /* 20 */
    { - 60 },
    { - 75 },
    { - 90 },
    { -105 },
    { -120 }    /* 25 */
};


const	struct	con_app_type	con_app		[26]		=
{
    { -4, 20 },   /*  0 */
    { -3, 25 },   /*  1 */
    { -2, 30 },
    { -2, 35 },	  /*  3 */
    { -1, 40 },
    { -1, 45 },   /*  5 */
    { -1, 50 },
    {  0, 55 },
    {  0, 60 },
    {  0, 65 },
    {  0, 70 },   /* 10 */
    {  0, 75 },
    {  0, 80 },
    {  0, 85 },
    {  0, 88 },
    {  1, 90 },   /* 15 */
    {  2, 95 },
    {  2, 97 },
    {  3, 99 },   /* 18 */
    {  3, 99 },
    {  4, 99 },   /* 20 */
    {  4, 99 },
    {  5, 99 },
    {  6, 99 },
    {  7, 99 },
    {  8, 99 }    /* 25 */
};



/*
 * Liquid properties.
 * Used in world.obj.
 */
const	struct	liq_type	liq_table	[]	=
{
/*    name			color	proof, full, thirst, food, ssize */
    { "water",			"clear",	{   0, 1, 10, 0, 16 }	},
    { "beer",			"amber",	{  12, 1,  8, 1, 12 }	},
    { "red wine",		"burgundy",	{  30, 1,  8, 1,  5 }	},
    { "ale",			"brown",	{  15, 1,  8, 1, 12 }	},
    { "dark ale",		"dark",		{  16, 1,  8, 1, 12 }	},

    { "whisky",			"golden",	{ 120, 1,  5, 0,  2 }	},
    { "lemonade",		"pink",		{   0, 1,  9, 2, 12 }	},
    { "firebreather",		"boiling",	{ 190, 0,  4, 0,  2 }	},
    { "local specialty",	"clear",	{ 151, 1,  3, 0,  2 }	},
    { "slime mold juice",	"green",	{   0, 2, -8, 1,  2 }	},

    { "milk",			"white",	{   0, 2,  9, 3, 12 }	},
    { "tea",			"tan",		{   0, 1,  8, 0,  6 }	},
    { "coffee",			"black",	{   0, 1,  8, 0,  6 }	},
    { "blood",			"red",		{   0, 2, -1, 2,  6 }	},
    { "salt water",		"clear",	{   0, 1, -2, 0,  1 }	},

    { "coke",			"brown",	{   0, 2,  9, 2, 12 }	}, 
    { "root beer",		"brown",	{   0, 2,  9, 2, 12 }   },
    { "elvish wine",		"green",	{  35, 2,  8, 1,  5 }   },
    { "white wine",		"golden",	{  28, 1,  8, 1,  5 }   },
    { "champagne",		"golden",	{  32, 1,  8, 1,  5 }   },

    { "mead",			"honey-colored",{  34, 2,  8, 2, 12 }   },
    { "rose wine",		"pink",		{  26, 1,  8, 1,  5 }	},
    { "benedictine wine",	"burgundy",	{  40, 1,  8, 1,  5 }   },
    { "vodka",			"clear",	{ 130, 1,  5, 0,  2 }   },
    { "cranberry juice",	"red",		{   0, 1,  9, 2, 12 }	},

    { "orange juice",		"orange",	{   0, 2,  9, 3, 12 }   }, 
    { "absinthe",		"green",	{ 200, 1,  4, 0,  2 }	},
    { "brandy",			"golden",	{  80, 1,  5, 0,  4 }	},
    { "aquavit",		"clear",	{ 140, 1,  5, 0,  2 }	},
    { "schnapps",		"clear",	{  90, 1,  5, 0,  2 }   },

    { "icewine",		"purple",	{  50, 2,  6, 1,  5 }	},
    { "amontillado",		"burgundy",	{  35, 2,  8, 1,  5 }	},
    { "sherry",			"red",		{  38, 2,  7, 1,  5 }   },	
    { "framboise",		"red",		{  50, 1,  7, 1,  5 }   },
    { "rum",			"amber",	{ 151, 1,  4, 0,  2 }	},

    { "cordial",		"clear",	{ 100, 1,  5, 0,  2 }   },
    { NULL,			NULL,		{   0, 0,  0, 0,  0 }	}
};



/*
 * The skill and spell table.
 * Slot numbers must never be changed as they appear in #OBJECTS sections.
 */
#define SLOT(n)	n

const	struct	skill_type	skill_table	[MAX_SKILL]	=
{

/*
 * Magic spells.
 */

    {
	"reserved",
	0,			TAR_IGNORE,		POS_STANDING,
	NULL,			SLOT( 0),	 0,	 0,
	"",			"",		""
    },

    {
	"acid blast",
	spell_acid_blast,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(70),	20,	12,
	"acid blast",		"!Acid Blast!"
    },

    {
	"armor",
	spell_armor,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			SLOT( 1),	 5,	12,
	"",			"You feel less armored.",	""
    },

    {
	"bless",
	spell_bless,		TAR_OBJ_CHAR_DEF,	POS_STANDING,
	NULL,			SLOT( 3),	 5,	12,
	"",			"You feel less righteous.", 
	"$p's holy aura fades."
    },

    {
	"blindness",
	spell_blindness,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_blindness,		SLOT( 4),	 5,	12,
	"",			"You can see again.",	""
    },

    {
	"burning hands",
	spell_burning_hands,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT( 5),	15,	12,
	"burning hands",	"!Burning Hands!", 	""
    },

    {
	"call lightning",
	spell_call_lightning,	TAR_IGNORE,		POS_FIGHTING,
	NULL,			SLOT( 6),	15,	12,
	"lightning bolt",	"!Call Lightning!",	""
    },

    {   "calm",
	spell_calm,		TAR_IGNORE,		POS_FIGHTING,
	NULL,			SLOT(509),	30,	12,
	"",			"You have lost your peace of mind.",	""
    },

    {
	"cancellation",
	spell_cancellation,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(507),	20,	12,
	""			"!cancellation!",	""
    },

    {
	"cause critical",
	spell_cause_critical,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(63),	20,	12,
	"spell",		"!Cause Critical!",	""
    },

    {
	"cause light",
	spell_cause_light,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(62),	15,	12,
	"spell",		"!Cause Light!",	""
    },

    {
	"cause serious",
	spell_cause_serious,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(64),	17,	12,
	"spell",		"!Cause Serious!",	""
    },

    {   
	"chain lightning",
	spell_chain_lightning,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(500),	25,	12,
	"lightning",		"!Chain Lightning!",	""
    }, 

    {
	"change sex",
	spell_change_sex,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(82),	15,	12,
	"",			"Your body feels familiar again.",	""
    },

    {
	"charm person",
	spell_charm_person,	TAR_CHAR_OFFENSIVE,	POS_STANDING,
	&gsn_charm_person,	SLOT( 7),	 5,	12,
	"",			"You feel more self-confident.",	""
    },

    {
	"chill touch",
	spell_chill_touch,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT( 8),	15,	12,
	"chilling touch",	"You feel less cold.",	""
    },

    {
	"colour spray",
	spell_colour_spray,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(10),	15,	12,
	"colour spray",		"!Colour Spray!",	""
    },

    {
	"continual light",
	spell_continual_light,	TAR_IGNORE,		POS_STANDING,
	NULL,			SLOT(57),	 7,	12,
	"",			"!Continual Light!",	""
    },

    {
	"control weather",
	spell_control_weather,	TAR_IGNORE,		POS_STANDING,
	NULL,			SLOT(11),	25,	12,
	"",			"!Control Weather!",	""
    },

    {
	"create food",
	spell_create_food,	TAR_IGNORE,		POS_STANDING,
	NULL,			SLOT(12),	 5,	12,
	"",			"!Create Food!",	""
    },

    {
	"create rose",
	spell_create_rose,	TAR_IGNORE,		POS_STANDING,
	NULL,			SLOT(511),	30, 	12,
	"",			"!Create Rose!",	""
    },  

    {
	"create spring",
	spell_create_spring,	TAR_IGNORE,		POS_STANDING,
	NULL,			SLOT(80),	20,	12,
	"",			"!Create Spring!",	""
    },

    {
	"create water",
	spell_create_water,	TAR_OBJ_INV,		POS_STANDING,
	NULL,			SLOT(13),	 5,	12,
	"",			"!Create Water!",	""
    },

    {
	"cure blindness",
	spell_cure_blindness,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(14),	 5,	12,
	"",			"!Cure Blindness!",	""
    },

    {
	"cure critical",
	spell_cure_critical,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(15),	20,	12,
	"",			"!Cure Critical!",	""
    },

    {
	"cure disease",
	spell_cure_disease,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			SLOT(501),	20,	12,
	"",			"!Cure Disease!",	""
    },

    {
	"cure light",
	spell_cure_light,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(16),	10,	12,
	"",			"!Cure Light!",		""
    },

    {
	"cure poison",
	spell_cure_poison,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			SLOT(43),	 5,	12,
	"",			"!Cure Poison!",	""
    },

    {
	"cure serious",
	spell_cure_serious,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(61),	15,	12,
	"",			"!Cure Serious!",	""
    },

    {
	"curse",
	spell_curse,		TAR_OBJ_CHAR_OFF,	POS_FIGHTING,
	&gsn_curse,		SLOT(17),	20,	12,
	"curse",		"The curse wears off.", 
	"$p is no longer impure."
    },

    {
	"demonfire",
	spell_demonfire,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(505),	20,	12,
	"torments",		"!Demonfire!",		""
    },	

    {
	"detect evil",
	spell_detect_evil,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			SLOT(18),	 5,	12,
	"",			"The red in your vision disappears.",	""
    },

    {
        "detect good",
        spell_detect_good,      TAR_CHAR_SELF,          POS_STANDING,
        NULL,                   SLOT(513),        5,     12,
        "",                     "The gold in your vision disappears.",	""
    },

    {
	"detect hidden",
	spell_detect_hidden,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			SLOT(44),	 5,	12,
	"",			"You feel less aware of your surroundings.",	
	""
    },

    {
	"detect invis",
	spell_detect_invis,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			SLOT(19),	 5,	12,
	"",			"You no longer see invisible objects.",
	""
    },

    {
	"detect magic",
	spell_detect_magic,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			SLOT(20),	 5,	12,
	"",			"The detect magic wears off.",	""
    },

    {
	"detect poison",
	spell_detect_poison,	TAR_OBJ_INV,		POS_STANDING,
	NULL,			SLOT(21),	 5,	12,
	"",			"!Detect Poison!",	""
    },

    {
	"dispel evil",
	spell_dispel_evil,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(22),	15,	12,
	"dispel evil",		"!Dispel Evil!",	""
    },

    {
        "dispel good",
        spell_dispel_good,      TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,                   SLOT(512),      15,     12,
        "dispel good",          "!Dispel Good!",	""
    },

    {
	"dispel magic",
	spell_dispel_magic,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(59),	15,	12,
	"",			"!Dispel Magic!",	""
    },

    {
	"earthquake",
	spell_earthquake,	TAR_IGNORE,		POS_FIGHTING,
	NULL,			SLOT(23),	15,	12,
	"earthquake",		"!Earthquake!",		""
    },

    {
	"enchant armor",
	spell_enchant_armor,	TAR_OBJ_INV,		POS_STANDING,
	NULL,			SLOT(510),	100,	24,
	"",			"!Enchant Armor!",	""
    },

    {
	"enchant weapon",
	spell_enchant_weapon,	TAR_OBJ_INV,		POS_STANDING,
	NULL,			SLOT(24),	100,	24,
	"",			"!Enchant Weapon!",	""
    },

    {
	"energy drain",
	spell_energy_drain,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(25),	35,	12,
	"energy drain",		"!Energy Drain!",	""
    },

    {
	"faerie fire",
	spell_faerie_fire,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(72),	 5,	12,
	"faerie fire",		"The pink aura around you fades away.",
	""
    },

    {
	"faerie fog",
	spell_faerie_fog,	TAR_IGNORE,		POS_STANDING,
	NULL,			SLOT(73),	12,	12,
	"faerie fog",		"!Faerie Fog!",		""
    },

    {
	"farsight",
	spell_farsight,		TAR_IGNORE,		POS_STANDING,
	NULL,			SLOT(521),	36,	20,
	"farsight",		"!Farsight!",		""
    },	

    {
	"fireball",
	spell_fireball,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(26),	15,	12,
	"fireball",		"!Fireball!",		""
    },
  
    {
	"fireproof",
	spell_fireproof,	TAR_OBJ_INV,		POS_STANDING,
	NULL,			SLOT(523),	10,	12,
	"",			"",	"$p's protective aura fades."
    },

    {
	"flamestrike",
	spell_flamestrike,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(65),	20,	12,
	"flamestrike",		"!Flamestrike!",		""
    },

    {
	"fly",
	spell_fly,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			SLOT(56),	10,	18,
	"",			"You slowly float to the ground.",	""
    },

    {
	"floating disc",
	spell_floating_disc,	TAR_IGNORE,		POS_STANDING,
	NULL,			SLOT(522),	40,	24,
	"",			"!Floating disc!",	""
    },

    {
        "frenzy",
        spell_frenzy,           TAR_CHAR_DEFENSIVE,     POS_STANDING,
        NULL,                   SLOT(504),      30,     24,
        "",                     "Your rage ebbs.",	""
    },

    {
	"gate",
	spell_gate,		TAR_IGNORE,		POS_FIGHTING,
	NULL,			SLOT(83),	80,	12,
	"",			"!Gate!",		""
    },

    {
	"giant strength",
	spell_giant_strength,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			SLOT(39),	20,	12,
	"",			"You feel weaker.",	""
    },

    {
	"harm",
	spell_harm,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(27),	35,	12,
	"harm spell",		"!Harm!,		"""
    },
  
    {
	"haste",
	spell_haste,		TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(502),	30,	12,
	"",			"You feel yourself slow down.",	""
    },

    {
	"heal",
	spell_heal,		TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(28),	50,	12,
	"",			"!Heal!",		""
    },
  
    {
	"heat metal",
	spell_heat_metal,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(516), 	25,	18,
	"spell",		"!Heat Metal!",		""
    },

    {
	"holy word",
	spell_holy_word,	TAR_IGNORE,	POS_FIGHTING,
	NULL,			SLOT(506), 	200,	24,
	"divine wrath",		"!Holy Word!",		""
    },

    {
	"identify",	
	spell_identify,		TAR_OBJ_INV,		POS_STANDING,
	NULL,			SLOT(53),	12,	24,
	"",			"!Identify!",		""
    },

    {
	"infravision",
	spell_infravision,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			SLOT(77),	 5,	18,
	"",			"You no longer see in the dark.",	""
    },

    {
	"invisibility",
	spell_invis,		TAR_OBJ_CHAR_DEF,	POS_STANDING,
	&gsn_invis,		SLOT(29),	 5,	12,
	"",			"You are no longer invisible.",		
	"$p fades into view."
    },

    {
	"know alignment",
	spell_know_alignment,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(58),	 9,	12,
	"",			"!Know Alignment!",	""
    },

    {
	"lightning bolt",
	spell_lightning_bolt,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(30),	15,	12,
	"lightning bolt",	"!Lightning Bolt!",	""
    },

    {
	"locate object",
	spell_locate_object,	TAR_IGNORE,		POS_STANDING,
	NULL,			SLOT(31),	20,	18,
	"",			"!Locate Object!",	""
    },

    {
	"magic missile",
	spell_magic_missile,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(32),	15,	12,
	"magic missile",	"!Magic Missile!",	""
    },

    {
	"mass healing",
	spell_mass_healing,	TAR_IGNORE,		POS_STANDING,
	NULL,			SLOT(508),	100,	36,
	"",			"!Mass Healing!",	""
    },

    {
	"mass invis",
	spell_mass_invis,	TAR_IGNORE,		POS_STANDING,
	&gsn_mass_invis,	SLOT(69),	20,	24,
	"",			"You are no longer invisible.",		""
    },

    {
        "nexus",
        spell_nexus,            TAR_IGNORE,             POS_STANDING,
        NULL,                   SLOT(520),       150,   36,
        "",                     "!Nexus!",		""
    },

    {
	"pass door",
	spell_pass_door,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			SLOT(74),	20,	12,
	"",			"You feel solid again.",	""
    },

    {
	"plague",
	spell_plague,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_plague,		SLOT(503),	20,	12,
	"sickness",		"Your sores vanish.",	""
    },

    {
	"poison",
	spell_poison,		TAR_OBJ_CHAR_OFF,	POS_FIGHTING,
	&gsn_poison,		SLOT(33),	10,	12,
	"poison",		"You feel less sick.",	
	"The poison on $p dries up."
    },

    {
        "portal",
        spell_portal,           TAR_IGNORE,             POS_STANDING,
        NULL,                   SLOT(519),       100,     24,
        "",                     "!Portal!",		""
    },

    {
	"protection evil",
	spell_protection_evil,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			SLOT(34), 	5,	12,
	"",			"You feel less protected.",	""
    },

    {
        "protection good",
        spell_protection_good,  TAR_CHAR_SELF,          POS_STANDING,
        NULL,                   SLOT(514),       5,     12,
        "",                     "You feel less protected.",	""
    },

    {
        "ray of truth",
        spell_ray_of_truth,     TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,                   SLOT(518),      20,     12,
        "ray of truth",         "!Ray of Truth!",	""
    },

    {
	"recharge",
	spell_recharge,		TAR_OBJ_INV,		POS_STANDING,
	NULL,			SLOT(517),	60,	24,
	"",			"!Recharge!",		""
    },

    {
	"refresh",
	spell_refresh,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			SLOT(81),	12,	18,
	"refresh",		"!Refresh!",		""
    },

    {
	"remove curse",
	spell_remove_curse,	TAR_OBJ_CHAR_DEF,	POS_STANDING,
	NULL,			SLOT(35),	 5,	12,
	"",			"!Remove Curse!",	""
    },

    {
	"sanctuary",
	spell_sanctuary,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	&gsn_sanctuary,		SLOT(36),	75,	12,
	"",			"The white aura around your body fades.",
	""
    },

    {
	"shield",
	spell_shield,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			SLOT(67),	12,	18,
	"",			"Your force shield shimmers then fades away.",
	""
    },

    {
	"shocking grasp",
	spell_shocking_grasp,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(53),	15,	12,
	"shocking grasp",	"!Shocking Grasp!",	""
    },

    {
	"sleep",
	spell_sleep,		TAR_CHAR_OFFENSIVE,	POS_STANDING,
	&gsn_sleep,		SLOT(38),	15,	12,
	"",			"You feel less tired.",	""
    },

    {
        "slow",
        spell_slow,             TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,                   SLOT(515),      30,     12,
        "",                     "You feel yourself speed up.",	""
    },

    {
	"stone skin",
	spell_stone_skin,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			SLOT(66),	12,	18,
	"",			"Your skin feels soft again.",	""
    },

    {
	"summon",
	spell_summon,		TAR_IGNORE,		POS_STANDING,
	NULL,			SLOT(40),	50,	12,
	"",			"!Summon!",		""
    },

    {
	"teleport",
	spell_teleport,		TAR_CHAR_SELF,		POS_FIGHTING,
	NULL,	 		SLOT( 2),	35,	12,
	"",			"!Teleport!",		""
    },

    {
	"ventriloquate",
	spell_ventriloquate,	TAR_IGNORE,		POS_STANDING,
	NULL,			SLOT(41),	 5,	12,
	"",			"!Ventriloquate!",	""
    },

    {
	"weaken",
	spell_weaken,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(68),	20,	12,
	"spell",		"You feel stronger.",	""
    },

    {
	"word of recall",
	spell_word_of_recall,	TAR_CHAR_SELF,		POS_RESTING,
	NULL,			SLOT(42),	 5,	12,
	"",			"!Word of Recall!",	""
    },

/*
 * Dragon breath
 */
    {
	"acid breath",
	spell_acid_breath,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(200),	100,	24,
	"blast of acid",	"!Acid Breath!",	""
    },

    {
	"fire breath",
	spell_fire_breath,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(201),	200,	24,
	"blast of flame",	"The smoke leaves your eyes.",	""
    },

    {
	"frost breath",
	spell_frost_breath,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(202),	125,	24,
	"blast of frost",	"!Frost Breath!",	""
    },

    {
	"gas breath",	
	spell_gas_breath,	TAR_IGNORE,		POS_FIGHTING,
	NULL,			SLOT(203),	175,	24,
	"blast of gas",		"!Gas Breath!",		""
    },

    {
	"lightning breath",
	spell_lightning_breath,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(204),	150,	24,
	"blast of lightning",	"!Lightning Breath!",	""
    },

/*
 * Spells for mega1.are from Glop/Erkenbrand.
 */
    {
        "general purpose",
        spell_general_purpose,  TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,                   SLOT(401),      0,      12,
        "general purpose ammo", "!General Purpose Ammo!",	""
    },
 
    {
        "high explosive", 
        spell_high_explosive,   TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,                   SLOT(402),      0,      12,
        "high explosive ammo",  "!High Explosive Ammo!",	""
    },


/* combat and weapons skills */


    {
	"axe",
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_axe,            	SLOT( 0),       0,      0,
        "",                     "!Axe!",		""
    },

    {
        "dagger",
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_dagger,            SLOT( 0),       0,      0,
        "",                     "!Dagger!",		""
    },
 
    {
	"flail",
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_flail,            	SLOT( 0),       0,      0,
        "",                     "!Flail!",		""
    },

    {
	"mace",
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_mace,            	SLOT( 0),       0,      0,
        "",                     "!Mace!",		""
    },

    {
	"polearm",
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_polearm,           SLOT( 0),       0,      0,
        "",                     "!Polearm!",		""
    },
    
    {
	"shield block",
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_shield_block,	SLOT(0),	0,	0,
	"",			"!Shield!",		""
    },
 
    {
	"spear",
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_spear,            	SLOT( 0),       0,      0,
        "",                     "!Spear!",		""
    },

    {
	"sword",
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_sword,            	SLOT( 0),       0,      0,
        "",                     "!sword!",		""
    },

    {
	"whip",
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_whip,            	SLOT( 0),       0,      0,
        "",                     "!Whip!",	""
    },

    {
        "backstab",
        spell_null,             TAR_IGNORE,             POS_STANDING,
        &gsn_backstab,          SLOT( 0),        0,     24,
        "backstab",             "!Backstab!",		""
    },

    {
	"bash",
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_bash,            	SLOT( 0),       0,      24,
        "bash",                 "!Bash!",		""
    },

    {
	"berserk",
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_berserk,        	SLOT( 0),       0,      24,
        "",                     "You feel your pulse slow down.",	""
    },

    {
	"dirt kicking",
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_dirt,		SLOT( 0),	0,	24,
	"kicked dirt",		"You rub the dirt out of your eyes.",	""
    },

    {
        "disarm",
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_disarm,            SLOT( 0),        0,     24,
        "",                     "!Disarm!",		""
    },
 
    {
        "dodge", 
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_dodge,             SLOT( 0),        0,     0,
        "",                     "!Dodge!",		""
    },
 
    {
        "enhanced damage",
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_enhanced_damage,   SLOT( 0),        0,     0,
        "",                     "!Enhanced Damage!",	""
    },

    {
	"envenom",
	spell_null,		TAR_IGNORE,	  	POS_RESTING,
	&gsn_envenom,		SLOT(0),	0,	36,
	"",			"!Envenom!",		""
    },

    {
	"hand to hand",
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_hand_to_hand,	SLOT( 0),	0,	0,
	"",			"!Hand to Hand!",	""
    },

    {
        "kick",
        spell_null,             TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        &gsn_kick,              SLOT( 0),        0,     12,
        "kick",                 "!Kick!",		""
    },

    {
        "parry",
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_parry,             SLOT( 0),        0,     0,
        "",                     "!Parry!",		""
    },

    {
        "rescue",
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_rescue,            SLOT( 0),        0,     12,
        "",                     "!Rescue!",		""
    },

    {
	"trip",
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_trip,		SLOT( 0),	0,	24,
	"trip",			"!Trip!",		""
    },

    {
        "second attack",
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_second_attack,     SLOT( 0),        0,     0,
        "",                     "!Second Attack!",	""
    },

    {
        "third attack",
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_third_attack,      SLOT( 0),        0,     0,
        "",                     "!Third Attack!",	""
    },

/* non-combat skills */

    { 
	"fast healing",
	spell_null,		TAR_IGNORE,		POS_SLEEPING,
	&gsn_fast_healing,	SLOT( 0),	0,	0,
	"",			"!Fast Healing!",	""
    },

    {
	"haggle",
	spell_null,		TAR_IGNORE,		POS_RESTING,
	&gsn_haggle,		SLOT( 0),	0,	0,
	"",			"!Haggle!",		""
    },

    {
	"hide",	
	spell_null,		TAR_IGNORE,		POS_RESTING,
	&gsn_hide,		SLOT( 0),	 0,	12,
	"",			"!Hide!",		""
    },

    {
	"lore",	
	spell_null,		TAR_IGNORE,		POS_RESTING,
	&gsn_lore,		SLOT( 0),	0,	36,
	"",			"!Lore!",		""
    },

    {
	"meditation",
	spell_null,		TAR_IGNORE,		POS_SLEEPING,
	&gsn_meditation,	SLOT( 0),	0,	0,
	"",			"Meditation",		""
    },

    {
	"peek",		
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_peek,		SLOT( 0),	 0,	 0,
	"",			"!Peek!",		""
    },

    {
	"pick lock",
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_pick_lock,		SLOT( 0),	 0,	12,
	"",			"!Pick!",		""
    },

    {
	"sneak",
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_sneak,		SLOT( 0),	 0,	12,
	"",			"You no longer feel stealthy.",	""
    },

    {
	"steal",
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_steal,		SLOT( 0),	 0,	24,
	"",			"!Steal!",		""
    },

    {
	"scrolls",
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_scrolls,		SLOT( 0),	0,	24,
	"",			"!Scrolls!",		""
    },

    {
	"staves",
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_staves,		SLOT( 0),	0,	12,
	"",			"!Staves!",		""
    },
    
    {
	"wands",
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_wands,		SLOT( 0),	0,	12,
	"",			"!Wands!",		""
    },

    {
	"recall",
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_recall,		SLOT( 0),	0,	12,
	"",			"!Recall!",		""
    }
};
