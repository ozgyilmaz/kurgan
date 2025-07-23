# Changelog

## 07.06.2025
- First commit: Original ROM2.4b6
- Changelog added.

## 08.06.2025
- Correction of compilation errors and warnings.

## 09.06.2025
- number_mm() removed.
- number_range() modernised. Bias problem resolved.
- number_bits() removed and it's usage replaced with number_range()
- UTF8 character support for argument and command input (by Prool http://mud.kharkov.org, proolix@gmail.com)
- RGB 8-bit color codes are implemented. Check do_who() for "wch->level" coloring. Color names are taken from: https://www.w3schools.com/colors/
- Immortal characters can see Room, Mob, Object VNUMs when Holylight is active.
- Autoexit setting removed. From now on exits will always be shown.
- Compact mode (COMM_COMPACT) removed.
- printf_to_char() implemented.
- bugf() implemented.

## 18.06.2025
- pc_race_table struct has been removed. The data moved to race_table struct.
- Errors corrected: printf_to_char() and bugf()
- "dev" branch created.

## 19.06.2025
- Level range info in #AREA section corrected: limbo, midgaard, ofcol, pyramid
- Socials modified to contain 8 lines for every single social in social.are
- proto.are removed.
- area-to-json.py script implemented to convert ROM areas to json.
- area-to-json.sh for batch operations for all area files /area directory.
- Area json converter improved. Json parsing code implemented.
- Json areas created. *.are files archived. area.lst edited.
- UTF8 code removed.
- Character customization removed.
- Kurgan headers and greeting.

## 20.06.2025
- Greeting screen edited. Startup script implemented.

# 29.06.2025
- Prompt configuration and ROOM colors (room name and exits)
- pc_race_table struct is back.
- These values moved from class_table to pc_race_table: sh_int attr_prime, sh_int weapon, sh_int skill_adept, sh_int	thac0_00, sh_int thac0_32, sh_int hp_min, sh_int hp_max, bool fMana
- Skill groups and related functions removed: do_groups, group_add, group_remove, group_lookup, gn_add, gn_remove
- class_table's guild, base_group and default_group has been removed.
- class_table and related things removed.
- group_table removed. list_group_chosen() removed.
- skill_level and rating in skill_table removed.
- pose_table and do_pose modified for a classless system.
- new character's weapon selection removed. racial weapons implemented.

# 30.06.2025
- new title_table implemented.
- pc_race_type's "class_mult" is removed.
- player class system is removed.
- "class" clean-up
- Lope's Color 2.0 implemented.

# 06.07.2025
- book system added.
- function backtrace for bugf().
- strdup() added strings in create_mobile(), clone_mobile() and create_object().

# 13.07.2025
- book_rarity added to skill_table. it's used during book creation in db.c file.
- act_color() implemented, now act_new() calls act_color().
- fight messages colored and dam count is given for each hit.
- sector and area info added to room name. (A Room in Mud School [City] [mud school])

# 14.07.2025
- "n" added to prompt. shop keeper info were string, converted to int with area-to-json.py script.

# 17.07.2025
- "new_format" variable removed. all objects and mobs are already new format.

# 19.07.2025
- new genders added: nonbinary, androgynous and agender

# 20.07.2025
- strrep() added. Weapon objects are now randomized.
- Armor objects are randomized.
- Light and food objects are randomized.
- Wands, staffs, potions, pills and scrolls are randomized.
- BUGFIX: Welcome screen's unreadable chars corrected.
- Area chess2 added.
- automated quest system implemented. (Vassago and Anatolia mud)
- char creation screen: alignment selection removed

# 22.07.2025
- do_scan() added to available commands.
- MAX_ALIAS increase from 5 to 25.
- BUGFIX: do_areas() was not working properly, repaired.
- BUGFIX: in json areas, area low_range and high_range were string. converter script repaired, now the values are int.
- send_to_char() remaining instances replaced with printf_to_char().
- xp compute for levels lower than 10 edited. also xp randomization is reconfigured.
- bool fMana (Race gains mana on level) has been removed from game.
- "giant" is not a pc race anymore.