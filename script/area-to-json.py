########################################################################################
#                                                                                      #
#  JSON converter for Anatolia Mud area files                                          #
#  github.com/ozgyilmaz/Anatolia-MUD-area-to-JSON-converter                            #
#                                                                                      #
#  Inspired by maplechori's "parse ewar" (github.com/maplechori/parse_ewar)            #
#  Written by Ozgur Yilmaz (github.com/ozgyilmaz)                                      #
#                                                                                      #
#  The project consists of a python file and highly using pyparsing module.            #
#  The python script easily converts a standard Anatolia MUD area file to a JSON file. #
#                                                                                      #
#  There are some exceptions and failures:                                             #
#  social.are is beyond the scope. At least for now...                                 #
#                                                                                      #
#  Usage:                                                                              #
#  python are-to-json.py <area_name_without_extension>                                 #
#                                                                                      #
#  Example:                                                                            #
#  python are-to-json.py under2                                                        #
#  python are-to-json.py help                                                          #
#  python are-to-json.py midgaard                                                      #
#                                                                                      #
########################################################################################

from pyparsing import *

import argparse
import json
import os
import re

space               = White(' ',exact=1)
# read everything till a tilde.
#tilde_string        = Combine(Regex("[^~]*") + Suppress(Literal("~")))
tilde_string = Combine(SkipTo("~") + Suppress("~"))
vnum                = Suppress(Literal("#")) + Word(nums).setParseAction(lambda s, l, t: [int(t[0])]).setResultsName("vnum") + Suppress(restOfLine)
asterisk_comment    = Regex("[\*].*")
social_one_line     = Combine(LineStart() + NotAny(Literal("#")) + restOfLine)

social_grammar      =   Suppress(Literal("#SOCIALS")) +\
                        ZeroOrMore(
                            Group(
                                social_one_line.setResultsName('name') +\
                                social_one_line.setResultsName('char_no_arg') +\
                                social_one_line.setResultsName('others_no_arg') +\
                                social_one_line.setResultsName('char_found') +\
                                social_one_line.setResultsName('others_found') +\
                                social_one_line.setResultsName('vict_found') +\
                                social_one_line.setResultsName('char_not_found') +\
                                social_one_line.setResultsName('char_auto') +\
                                social_one_line.setResultsName('others_auto')
                            )
                        ) + Suppress(Literal("#0"))

area_grammar = Suppress(Literal("#AREA")) +\
    tilde_string.setResultsName('file_name') + Suppress(restOfLine) +\
    tilde_string.setResultsName('name') + Suppress(restOfLine) +\
    Suppress("{") +\
    ZeroOrMore(Suppress(space)) +\
    Word(nums).setParseAction(lambda s, l, t: [int(t[0])]).setResultsName('low_range') +\
    Word(nums).setParseAction(lambda s, l, t: [int(t[0])]).setResultsName('high_range') +\
    ZeroOrMore(Suppress(space)) +\
    Suppress("}") +\
    Word(alphanums).setResultsName('writer') +\
    tilde_string.setResultsName('credits') + Suppress(restOfLine) +\
    Word(nums).setParseAction(lambda s, l, t: [int(t[0])]).setResultsName('min_vnum') +\
    Word(nums).setParseAction(lambda s, l, t: [int(t[0])]).setResultsName('max_vnum') + Suppress(restOfLine)

room_grammar        =   Suppress(Literal("#ROOMS")) +\
                        ZeroOrMore(
                            Group(
                                vnum +\
                                tilde_string.setResultsName('name') + Suppress(restOfLine) +\
                                tilde_string.setResultsName('description') + Suppress(restOfLine) +\
                                Suppress(Word(nums)) +\
                                Word(alphanums+"|"+"-").setResultsName('flags') +\
                                Word("-"+nums).setParseAction(lambda s, l, t: [int(t[0])]).setResultsName('sector') + Suppress(restOfLine) &\
                                ZeroOrMore(
                                    (Suppress(Literal("H")) + Word(nums).setParseAction(lambda s, l, t: [int(t[0])])).setResultsName('heal_rate') |
                                    (Suppress(Literal("M")) + Word(nums).setParseAction(lambda s, l, t: [int(t[0])])).setResultsName('mana_rate')|
                                    (Suppress(Literal("O")) + tilde_string).setResultsName('owner') |
                                    Group(
                                        Suppress(Literal("E")) +\
                                        tilde_string.setResultsName('keyword') +\
                                        tilde_string.setResultsName('description')
                                    ).setResultsName('extra_descriptions*') |
                                    Group(
                                        Suppress(Literal("D")) + Word(nums).setParseAction(lambda s, l, t: [int(t[0])]).setResultsName('exit_door') +\
                                        tilde_string.setResultsName('exit_description') +\
                                        tilde_string.setResultsName('exit_keyword') +\
                                        Word(nums).setParseAction(lambda s, l, t: [int(t[0])]).setResultsName('exit_locks') +\
                                        Word("-" + nums).setParseAction(lambda s, l, t: [int(t[0])]).setResultsName('exit_key') +\
                                        Word("-" +nums).setParseAction(lambda s, l, t: [int(t[0])]).setResultsName('exit_u1_vnum')
                                    ).setResultsName('exits*')
                                ) +\
                                Suppress(Literal("S"))
                            )
                        ) + Suppress(Literal("#0"))

object_grammar      =   Suppress(Literal("#OBJECTS")) +\
                        ZeroOrMore(
                            Group(
                                vnum +\
                                tilde_string.setResultsName('name') + Suppress(restOfLine) +\
                                tilde_string.setResultsName('short_description') + Suppress(restOfLine) +\
                                tilde_string.setResultsName('description') + Suppress(restOfLine) +\
                                tilde_string.setResultsName('material') + Suppress(restOfLine) +\
                                Word(alphanums+"_"+"-").setResultsName('type') +\
                                Word(alphanums+"|").setResultsName('extra_flags') +\
                                Word(alphanums+"|").setResultsName('wear_flags') + Suppress(restOfLine) +\
                                Word(alphanums+" "+"'"+"-").setResultsName('values') +\
                                Word("-"+nums).setParseAction(lambda s, l, t: [int(t[0])]).setResultsName('level') +\
                                Word("-"+nums).setParseAction(lambda s, l, t: [int(t[0])]).setResultsName('weight') +\
                                Word("-"+nums).setParseAction(lambda s, l, t: [int(t[0])]).setResultsName('cost') +\
                                Word(alphanums).setResultsName('condition') + Suppress(restOfLine) +\
                                ZeroOrMore(
                                    Group(
                                        Suppress(Literal("A")) +\
                                        Word("-"+nums).setParseAction(lambda s, l, t: [int(t[0])]).setResultsName('location') +\
                                        Word("-"+nums).setParseAction(lambda s, l, t: [int(t[0])]).setResultsName('modifier')
                                    ).setResultsName('affects_a*') |
                                    Group(
                                        Suppress(Literal("F")) +\
                                        Word(alphas).setResultsName('where') +\
                                        Word(nums).setParseAction(lambda s, l, t: [int(t[0])]).setResultsName('location') +\
                                        Word("-"+nums).setParseAction(lambda s, l, t: [int(t[0])]).setResultsName('modifier') +\
                                        Word(alphanums).setResultsName('bitvector')
                                    ).setResultsName('affects_f*') |
                                    Group(
                                        Suppress(Literal("E")) +\
                                        tilde_string.setResultsName('keyword') +\
                                        tilde_string.setResultsName('description')
                                    ).setResultsName('extra_descriptions*')
                                )
                            )
                        ) + Suppress(Literal("#0"))

mobile_grammar      =   Suppress(Literal("#MOBILES")) +\
                        ZeroOrMore(
                            Group(
                                vnum +\
                                tilde_string.setResultsName('name') + Suppress(restOfLine) +\
                                tilde_string.setResultsName('short_description') + Suppress(restOfLine) +\
                                tilde_string.setResultsName('long_description') + Suppress(restOfLine) +\
                                tilde_string.setResultsName('description') + Suppress(restOfLine) +\
                                tilde_string.setResultsName('race') + Suppress(restOfLine) +\
                                Word(alphanums+"|").setResultsName('act') +\
                                Word(alphanums+"|").setResultsName('affected_by') +\
                                Word(nums+"-").setParseAction(lambda s, l, t: [int(t[0])]).setResultsName('alignment') +\
                                Word(nums).setParseAction(lambda s, l, t: [int(t[0])]).setResultsName('group') + Suppress(restOfLine) +\
                                Word(nums).setParseAction(lambda s, l, t: [int(t[0])]).setResultsName('level') +\
                                Word(nums+"-").setParseAction(lambda s, l, t: [int(t[0])]).setResultsName('hitroll') +\
                                Word(alphanums+"+").setParseAction(lambda s, l, t: [list(map(int,re.split('d|\+',t[0])))]).setResultsName('hit_dice') +\
                                Word(alphanums+"+").setParseAction(lambda s, l, t: [list(map(int,re.split('d|\+',t[0])))]).setResultsName('mana_dice') +\
                                Word(alphanums+"+").setParseAction(lambda s, l, t: [list(map(int,re.split('d|\+',t[0])))]).setResultsName('dam_dice') +\
                                Word(alphanums).setResultsName('dam_type') + Suppress(restOfLine) +\
                                Word(nums+"-").setParseAction(lambda s, l, t: [int(t[0])]).setResultsName('ac_pierce') +\
                                Word(nums+"-").setParseAction(lambda s, l, t: [int(t[0])]).setResultsName('ac_bash') +\
                                Word(nums+"-").setParseAction(lambda s, l, t: [int(t[0])]).setResultsName('ac_slash') +\
                                Word(nums+"-").setParseAction(lambda s, l, t: [int(t[0])]).setResultsName('ac_exotic') + Suppress(restOfLine) +\
                                Word(alphanums).setResultsName('off_flags') +\
                                Word(alphanums).setResultsName('imm_flags') +\
                                Word(alphanums).setResultsName('res_flags') +\
                                Word(alphanums).setResultsName('vuln_flags') + Suppress(restOfLine) +\
                                Word(alphanums).setResultsName('start_pos') +\
                                Word(alphanums).setResultsName('default_pos') +\
                                Word(alphanums).setResultsName('sex') +\
                                Word(nums).setParseAction(lambda s, l, t: [int(t[0])]).setResultsName('wealth') + Suppress(restOfLine) +\
                                Word(alphanums).setResultsName('form') +\
                                Word(alphanums).setResultsName('parts') +\
                                Word(alphanums).setResultsName('size') +\
                                Word(alphanums).setResultsName('material') + Suppress(restOfLine) +\
                                ZeroOrMore(
                                    Group(
                                        Suppress(Literal("F")) +\
                                        Word(alphas).setResultsName('word') +\
                                        Word(alphanums).setResultsName('flag')
                                    ).setResultsName('affects_f*')
                                )
                            )
                        ) + Suppress(Literal("#0"))

reset_grammar       =   Suppress(Literal("#RESETS")) +\
                        ZeroOrMore(
                            Suppress(asterisk_comment) |
                            Group(Literal("G").setResultsName('command') + Suppress(Word("-" + nums)) + Word("-" + nums).setParseAction(lambda s, l, t: [int(t[0])]).setResultsName('arg1') + Word("-" + nums).setParseAction(lambda s, l, t: [int(t[0])]).setResultsName('arg2') + Suppress(restOfLine)) |
                            Group(Literal("R").setResultsName('command') + Suppress(Word("-" + nums)) + Word("-" + nums).setParseAction(lambda s, l, t: [int(t[0])]).setResultsName('arg1') + Word("-" + nums).setParseAction(lambda s, l, t: [int(t[0])]).setResultsName('arg2') + Suppress(restOfLine)) |
                            Group(Literal("O").setResultsName('command') + Suppress(Word("-" + nums)) + Word("-" + nums).setParseAction(lambda s, l, t: [int(t[0])]).setResultsName('arg1') + Word("-" + nums).setParseAction(lambda s, l, t: [int(t[0])]).setResultsName('arg2') + Word("-" + nums).setParseAction(lambda s, l, t: [int(t[0])]).setResultsName('arg3') + Suppress(restOfLine)) |
                            Group(Literal("E").setResultsName('command') + Suppress(Word("-" + nums)) + Word("-" + nums).setParseAction(lambda s, l, t: [int(t[0])]).setResultsName('arg1') + Word("-" + nums).setParseAction(lambda s, l, t: [int(t[0])]).setResultsName('arg2') + Word("-" + nums).setParseAction(lambda s, l, t: [int(t[0])]).setResultsName('arg3') + Suppress(restOfLine)) |
                            Group(Literal("D").setResultsName('command') + Suppress(Word("-" + nums)) + Word("-" + nums).setParseAction(lambda s, l, t: [int(t[0])]).setResultsName('arg1') + Word("-" + nums).setParseAction(lambda s, l, t: [int(t[0])]).setResultsName('arg2') + Word("-" + nums).setParseAction(lambda s, l, t: [int(t[0])]).setResultsName('arg3') + Suppress(restOfLine)) |
                            Group(Literal("P").setResultsName('command') + Suppress(Word("-" + nums)) + Word("-" + nums).setParseAction(lambda s, l, t: [int(t[0])]).setResultsName('arg1') + Word("-" + nums).setParseAction(lambda s, l, t: [int(t[0])]).setResultsName('arg2') + Word("-" + nums).setParseAction(lambda s, l, t: [int(t[0])]).setResultsName('arg3') + Word("-" + nums).setParseAction(lambda s, l, t: [int(t[0])]).setResultsName('arg4') + Suppress(restOfLine)) |
                            Group(Literal("M").setResultsName('command') + Suppress(Word("-" + nums)) + Word("-" + nums).setParseAction(lambda s, l, t: [int(t[0])]).setResultsName('arg1') + Word("-" + nums).setParseAction(lambda s, l, t: [int(t[0])]).setResultsName('arg2') + Word("-" + nums).setParseAction(lambda s, l, t: [int(t[0])]).setResultsName('arg3') + Word("-" + nums).setParseAction(lambda s, l, t: [int(t[0])]).setResultsName('arg4') + Suppress(restOfLine))
                        ) + Suppress(Literal("S"))

shop_grammar        =   Suppress(Literal("#SHOPS")) +\
                        ZeroOrMore(
                            Group(
                                Word(nums).setParseAction(lambda s, l, t: [int(t[0])]).setResultsName('keeper') +\
                                Word("-" + nums).setParseAction(lambda s, l, t: [int(t[0])]).setResultsName('buy_type_0') +\
                                Word("-" + nums).setParseAction(lambda s, l, t: [int(t[0])]).setResultsName('buy_type_1') +\
                                Word("-" + nums).setParseAction(lambda s, l, t: [int(t[0])]).setResultsName('buy_type_2') +\
                                Word("-" + nums).setParseAction(lambda s, l, t: [int(t[0])]).setResultsName('buy_type_3') +\
                                Word("-" + nums).setParseAction(lambda s, l, t: [int(t[0])]).setResultsName('buy_type_4') +\
                                Word("-" + nums).setParseAction(lambda s, l, t: [int(t[0])]).setResultsName('profit_buy') +\
                                Word("-" + nums).setParseAction(lambda s, l, t: [int(t[0])]).setResultsName('profit_sell') +\
                                Word("-" + nums).setParseAction(lambda s, l, t: [int(t[0])]).setResultsName('open_hour') +\
                                Word("-" + nums).setParseAction(lambda s, l, t: [int(t[0])]).setResultsName('close_hour') +\
                                Suppress(restOfLine)
                            )
                        ) + Suppress(Literal("0"))

practicer_grammar   =   Suppress(Literal("#PRACTICERS")) +\
                        ZeroOrMore(
                            Suppress(asterisk_comment) |
                            Group(
                                Suppress(Literal("M")) + Word(nums).setParseAction(lambda s, l, t: [int(t[0])]).setResultsName('vnum') + Word(alphanums+"_").setResultsName('practicer') + Suppress(restOfLine)
                            )
                        ) + Suppress(Literal("S"))

special_grammar     =   Suppress(Literal("#SPECIALS")) +\
                        ZeroOrMore(
                            Suppress(asterisk_comment) |
                            Group(
                                Suppress(Literal("M")) + Word(nums).setParseAction(lambda s, l, t: [int(t[0])]).setResultsName('vnum') + Word(alphanums+"_").setResultsName('spec_fun') + Suppress(restOfLine)
                            )
                        ) + Suppress(Literal("S"))

omprog_grammar      =   Suppress(Literal("#OMPROGS")) +\
                        ZeroOrMore(
                            Suppress(asterisk_comment) |
                            Group(
                                oneOf("M O").setResultsName('command') + Word(nums).setParseAction(lambda s, l, t: [int(t[0])]).setResultsName('vnum') + Word(alphanums+"_").setResultsName('progtype') + Word(alphanums+"_").setResultsName('progname') + Suppress(restOfLine)
                            )
                        ) + Suppress(Literal("S"))

help_grammar        =   Suppress(Literal("#HELPS")) +\
                        ZeroOrMore(
                            Suppress(Literal("0 $~")) |
                            Group(
                                Word("-"+nums).setParseAction(lambda s, l, t: [int(t[0])]).setResultsName('level') + tilde_string.setResultsName('keyword') + tilde_string.setResultsName('text')
                            )
                        )

areaflag_grammar    =   Suppress(Literal("#FLAG")) +\
                        Word(alphas).setResultsName('area_flag')

music_entry = Group(
    tilde_string("artist") +
    tilde_string("title") +
    tilde_string("lyrics")
)

music_grammar = ZeroOrMore(music_entry) + Suppress(Literal("#")) + StringEnd()

def parse_file(filemem):

    pattern         =   (Group(area_grammar).setResultsName("area") |\
                            Group(room_grammar).setResultsName("rooms") |\
                            Group(object_grammar).setResultsName("objects") |\
                            Group(mobile_grammar).setResultsName("mobiles") |\
                            Group(reset_grammar).setResultsName("resets") |\
                            Group(shop_grammar).setResultsName("shops") |\
                            Group(practicer_grammar).setResultsName("practicers") |\
                            Group(special_grammar).setResultsName("specials") |\
                            Group(omprog_grammar).setResultsName("omprogs") |\
                            Group(help_grammar).setResultsName("helps") |\
                            Group(social_grammar).setResultsName("socials") |\
                            Group(areaflag_grammar).setResultsName("area_flag")
                        )

    content         = OneOrMore(pattern)

    area_parser     = content + Suppress("#$")

    try:
        result      = area_parser.parseString(filemem, parseAll=False)
        rr          = result.asDict()
        return rr
    except ParseException as pe:
       #print(pe.markInputline())
       #print(pe)
       print(f"Hata:\n{pe.line}\n{' ' * (pe.column - 1)}^\nHata: {pe}")
    return

def main():
    parser          = argparse.ArgumentParser(description='process ROM area file')
    parser.add_argument('area', help="Area file or list of areas", type=str)
    
    args            = parser.parse_args()

    filename_with_ext = args.area
    basename = os.path.splitext(filename_with_ext)[0]

    if filename_with_ext.endswith("music.txt"):
        with open(filename_with_ext, encoding="utf-8") as f:
            data = f.read()
    
        result = music_grammar.parseString(data)
        
        music_list = []
        for song in result:
            music_list.append({
                "artist": song["artist"],
                "title": song["title"],
                "lyrics": song["lyrics"]
            })

        with open(basename + ".json", "w", encoding="utf-8") as out:
            json.dump({"songs": music_list}, out, indent=4, ensure_ascii=False)
        
        return

    if not filename_with_ext.endswith(".are"):
        print("Error: Filename should end with '.are' extension.")
        exit(1)
    
    with open(filename_with_ext, "r", encoding="utf-8") as f:
        data = parse_file(f.read())

    with open(basename + ".json", "w", encoding="utf-8") as f:
        json.dump(data, f, indent=4, ensure_ascii=False)

    
if __name__ == "__main__":
    main()