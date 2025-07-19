#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include "merc.h"

int random_damage_type_for_weapon(int weapon_type)
{
    int matches[100]; // maximum 100 uygun kayıt (fazlası yok zaten)
    int count = 0;

    for (int i = 0; attack_table[i].name != NULL; i++)
    {
        int dam = attack_table[i].damage;

        switch (weapon_type)
        {
            case WEAPON_DAGGER:
            case WEAPON_SPEAR:
            case WEAPON_POLEARM:
                if (dam == DAM_PIERCE)
                    matches[count++] = dam;
                break;

            case WEAPON_SWORD:
            case WEAPON_AXE:
                if (dam == DAM_SLASH)
                    matches[count++] = dam;
                break;

            case WEAPON_MACE:
            case WEAPON_FLAIL:
            case WEAPON_WHIP:
                if (dam == DAM_BASH)
                    matches[count++] = dam;
                break;

            default:
                // tüm hasar türlerine izin ver (özel silahlar için)
                matches[count++] = dam;
                break;
        }
    }

    if (count == 0)
        return DAM_BASH; // fallback

    return matches[number_range(0, count - 1)];
}

int obj_random_weapon_flag()
{
	int newflag = 0;
	
	if(number_percent()<=2)
		SET_BIT(newflag,WEAPON_FLAMING);
	if(number_percent()<=2)
		SET_BIT(newflag,WEAPON_FROST);
	if(number_percent()<=2)
		SET_BIT(newflag,WEAPON_VAMPIRIC);
	if(number_percent()<=2)
		SET_BIT(newflag,WEAPON_SHARP);
	if(number_percent()<=2)
		SET_BIT(newflag,WEAPON_VORPAL);
	if(number_percent()<=2)
		SET_BIT(newflag,WEAPON_TWO_HANDS);
	if(number_percent()<=2)
		SET_BIT(newflag,WEAPON_SHOCKING);
	if(number_percent()<=2)
		SET_BIT(newflag,WEAPON_POISON);

    return newflag;
}
