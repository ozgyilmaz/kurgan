#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "magic.h"
#include "interp.h"
#include "recycle.h"

DECLARE_MPROG_FUN_GREET( greet_prog_baker	);

void mprog_set(MOB_INDEX_DATA *mobindex,const char *progtype, const char *name)
{
   if (!str_cmp(progtype, "greet_prog"))
     {
        if (!str_cmp(name, "greet_prog_baker"))
	 mobindex->mprogs->greet_prog = greet_prog_baker;
       else
	 {
	   bug("Load_mprogs: 'M': Function not found for vnum %d",
	       mobindex->vnum);
	   exit(1);
	 }

       SET_BIT(mobindex->progtypes, MPROG_GREET);
       return;
     }
   bug( "Load_mprogs: 'M': invalid program type for vnum %d",mobindex->vnum);
   exit(1);
 }

void greet_prog_baker(CHAR_DATA *mob, CHAR_DATA *ch)
{
    char buf[MAX_STRING_LENGTH];

    if (!can_see(mob,ch) || IS_NPC(ch) || IS_IMMORTAL(ch))
    {
        return;
    }

    interpret(mob,"smile");
    sprintf(buf,"Welcome to my Bakery, %s", ch->name );
    do_say(mob,buf);
}