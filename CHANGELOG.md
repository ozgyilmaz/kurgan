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