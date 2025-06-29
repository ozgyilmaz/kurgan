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