/***************************************************************************
 *   KURGAN MUD – Based on ROM 2.4, Merc 2.1, and DikuMUD                  *
 *                                                                         *
 *   KURGAN MUD is a customized extension by Özgür Yilmaz                  *
 ***************************************************************************/

#define RGB(a,b,c) "\033[38;2;" #a ";" #b ";" #c "m"

// Lope's colors

#define CLR_LOPES_RED               RGB(170,0,0)
#define CLR_LOPES_B_RED             RGB(255,85,85)
#define CLR_LOPES_GREEN             RGB(0,170,0)
#define CLR_LOPES_B_GREEN             RGB(85,255,85)
#define CLR_LOPES_YELLOW                RGB(170,85,0)
#define CLR_LOPES_B_YELLOW                RGB(255,255,85)
#define CLR_LOPES_BLUE              RGB(0,0,170)
#define CLR_LOPES_B_BLUE              RGB(85,85,255)
#define CLR_LOPES_MAGENTA           RGB(170,0,170)
#define CLR_LOPES_B_MAGENTA           RGB(255,85,255)
#define CLR_LOPES_CYAN              RGB(0,170,170)
#define CLR_LOPES_B_CYAN              RGB(85,255,255)
#define CLR_LOPES_WHITE             RGB(170,170,170)
#define CLR_LOPES_B_WHITE             RGB(255,255,255)
#define CLR_LOPES_GREY              RGB(80, 80, 80)




/*
* HTML Color Groups
* https://www.w3schools.com/colors/colors_groups.asp
*/

#define CLR_RESET                   "\033[0m"

#define CLR_ROOM_NAME               RGB(201, 130, 75)
#define CLR_ROOM_EXITS              RGB(109, 213, 227)

/* Pink Colors */
#define CLR_PINK                    RGB(255, 192, 203)
#define CLR_LIGHT_PINK              RGB(255, 182, 193)
#define CLR_HOT_PINK                RGB(255, 105, 180)
#define CLR_DEEP_PINK               RGB(255, 20, 147)
#define CLR_PALE_VIOLET_RED         RGB(199, 21, 133)
#define CLR_MEDIUM_VIOLET_RED       RGB(219, 112, 147)

/* Purple Colors */
#define CLR_LAVENDER                RGB(230,230,250)
#define CLR_THISTLE                 RGB(216,191,216)
#define CLR_PLUM                    RGB(221,160,221)
#define CLR_ORCHID                  RGB(218,112,214)
#define CLR_VIOLET                  RGB(238,130,238)
#define CLR_FUCHSIA                 RGB(255,0,255)
#define CLR_MAGENTA                 RGB(255,0,255)
#define CLR_MEDIUM_ORCHID           RGB(186,85,211)
#define CLR_DARK_ORCHID             RGB(153,50,204)
#define CLR_DARK_VIOLET             RGB(148,0,211)
#define CLR_BLUE_VIOLET             RGB(138,43,226)
#define CLR_DARK_MAGENTA            RGB(139,0,139)
#define CLR_PURPLE                  RGB(128,0,128)
#define CLR_MEDIUM_PURPLE           RGB(147,112,219)
#define CLR_MEDIUM_SLATE_BLUE       RGB(123,104,238)
#define CLR_SLATE_BLUE              RGB(106,90,205)
#define CLR_DARK_SLATE_BLUE         RGB(72,61,139)
#define CLR_REBECCA_PURPLE          RGB(102,51,153)
#define CLR_INDIGO                  RGB(75,0,130)

/* Red Colors */
#define CLR_LIGHT_SALMON            RGB(255,160,122)
#define CLR_SALMON                  RGB(250,128,114)
#define CLR_DARK_SALMON             RGB(233,150,122)
#define CLR_LIGHT_CORAL             RGB(240,128,128)
#define CLR_INDIAN_RED              RGB(205,92,92)
#define CLR_CRIMSON                 RGB(220,20,60)
#define CLR_RED                     RGB(255,0,0)
#define CLR_FIRE_BRICK              RGB(178,34,34)
#define CLR_DARK_RED                RGB(139,0,0)

/* Orange Colors */
#define CLR_ORANGE                  RGB(255,165,0)
#define CLR_DARK_ORANGE             RGB(255,140,0)
#define CLR_CORAL                   RGB(255,127,80)
#define CLR_TOMATO                  RGB(255,99,71)
#define CLR_ORANGE_RED              RGB(255,69,0)

/* Yellow Colors */
#define CLR_GOLD                    RGB(255,215,0)
#define CLR_YELLOW                  RGB(255,255,0)
#define CLR_LIGHT_YELLOW            RGB(255,255,224)
#define CLR_LEMON_CHIFFON           RGB(255,250,205)
#define CLR_LIGHT_GOLDEN_ROD_YELLOW RGB(250,250,210)
#define CLR_PAPAYA_WHIP             RGB(255,239,213)
#define CLR_MOCCASIN                RGB(255,228,181)
#define CLR_PEACH_PUFF              RGB(255,218,185)
#define CLR_PALE_GOLDEN_ROD         RGB(238,232,170)
#define CLR_KHAKI                   RGB(240,230,140)
#define CLR_DARK_KHAKI              RGB(189,183,107)

/* Green Colors */
#define CLR_GREEN_YELLOW            RGB(173,255,47)
#define CLR_CHARTREUSE              RGB(127,255,0)
#define CLR_LAWN_GREEN              RGB(124,252,0)
#define CLR_LIME                    RGB(0,255,0)
#define CLR_LIME_GREEN              RGB(50,205,50)
#define CLR_PALE_GREEN              RGB(152,251,152)
#define CLR_LIGHT_GREEN             RGB(144,238,144)
#define CLR_MEDIUM_SPRING_GREEN     RGB(0,250,154)
#define CLR_SPRING_GREEN            RGB(0,255,127)
#define CLR_MEDIUM_SEA_GREEN        RGB(60,179,113)
#define CLR_SEA_GREEN               RGB(46,139,87)
#define CLR_FOREST_GREEN            RGB(34,139,34)
#define CLR_GREEN                   RGB(0,128,0)
#define CLR_DARK_GREEN              RGB(0,100,0)
#define CLR_YELLOW_GREEN            RGB(154,205,50)
#define CLR_OLIVE_DRAB              RGB(107,142,35)
#define CLR_DARK_OLIVE_GREEN        RGB(85,107,47)
#define CLR_MEDIUM_AQUA_MARINE      RGB(102,205,170)
#define CLR_DARK_SEA_GREEN          RGB(143,188,143)
#define CLR_LIGHT_SEA_GREEN         RGB(32,178,170)
#define CLR_DARK_CYAN               RGB(0,139,139)
#define CLR_TEAL                    RGB(0,128,128)

/* Cyan Colors */
#define CLR_AQUA                    RGB(0,255,255)
#define CLR_CYAN                    RGB(0,255,255)
#define CLR_LIGHT_CYAN              RGB(224,255,255)
#define CLR_PALE_TURQUOISE          RGB(175,238,238)
#define CLR_AQUAMARINE              RGB(127,255,212)
#define CLR_TURQUOISE               RGB(64,224,208)
#define CLR_MEDIUM_TURQUOISE        RGB(72,209,204)
#define CLR_DARK_TURQUOISE          RGB(0,206,209)

/* Blue Colors */
#define CLR_CADET_BLUE              RGB(95,158,160)
#define CLR_STEEL_BLUE              RGB(70,130,180)
#define CLR_LIGHT_STEEL_BLUE        RGB(176,196,222)
#define CLR_LIGHT_BLUE              RGB(173,216,230)
#define CLR_POWDER_BLUE             RGB(176,224,230)
#define CLR_LIGHT_SKY_BLUE          RGB(135,206,250)
#define CLR_SKY_BLUE                RGB(135,206,235)
#define CLR_CORNFLOWER_BLUE         RGB(100,149,237)
#define CLR_DEEP_SKY_BLUE           RGB(0,191,255)
#define CLR_DODGER_BLUE             RGB(30,144,255)
#define CLR_ROYAL_BLUE              RGB(65,105,225)
#define CLR_BLUE                    RGB(0,0,255)
#define CLR_MEDIUM_BLUE             RGB(0,0,205)
#define CLR_DARK_BLUE               RGB(0,0,139)
#define CLR_NAVY                    RGB(0,0,128)
#define CLR_MIDNIGHT_BLUE           RGB(25,25,112)

/* Brown Colors */
#define CLR_CORNSILK                RGB(255,248,220)
#define CLR_BLANCHED_ALMOND         RGB(255,235,205)
#define CLR_BISQUE                  RGB(255,228,196)
#define CLR_NAVAJO_WHITE            RGB(255,222,173)
#define CLR_WHEAT                   RGB(245,222,179)
#define CLR_BURLY_WOOD              RGB(222,184,135)
#define CLR_TAN                     RGB(210,180,140)
#define CLR_ROSY_BROWN              RGB(188,143,143)
#define CLR_SANDY_BROWN             RGB(244,164,96)
#define CLR_GOLDEN_ROD              RGB(218,165,32)
#define CLR_DARK_GOLDEN_ROD         RGB(184,134,11)
#define CLR_PERU                    RGB(205,133,63)
#define CLR_CHOCOLATE               RGB(210,105,30)
#define CLR_OLIVE                   RGB(128,128,0)
#define CLR_SADDLE_BROWN            RGB(139,69,19)
#define CLR_SIENNA                  RGB(160,82,45)
#define CLR_BROWN                   RGB(165,42,42)
#define CLR_MAROON                  RGB(128,0,0)

/* White Colors */
#define CLR_WHITE                   RGB(255,255,255)
#define CLR_SNOW                    RGB(255,250,250)
#define CLR_HONEY_DEW               RGB(240,255,240)
#define CLR_MINT_CREAM              RGB(245,255,250)
#define CLR_AZURE                   RGB(240,255,255)
#define CLR_ALICE_BLUE              RGB(240,248,255)
#define CLR_GHOST_WHITE             RGB(248,248,255)
#define CLR_WHITE_SMOKE             RGB(245,245,245)
#define CLR_SEA_SHELL               RGB(255,245,238)
#define CLR_BEIGE                   RGB(245,245,220)
#define CLR_OLD_LACE                RGB(253,245,230)
#define CLR_FLORAL_WHITE            RGB(255,250,240)
#define CLR_IVORY                   RGB(255,255,240)
#define CLR_ANTIQUE_WHITE           RGB(250,235,215)
#define CLR_LINEN                   RGB(250,240,230)
#define CLR_LAVENDER_BLUSH          RGB(255,240,245)
#define CLR_MISTY_ROSE              RGB(255,228,225)

/* Grey Colors */
#define CLR_GAINSBORO               RGB(220,220,220)
#define CLR_LIGHT_GRAY              RGB(211,211,211)
#define CLR_SILVER                  RGB(192,192,192)
#define CLR_DARK_GRAY               RGB(169,169,169)
#define CLR_DIM_GRAY                RGB(105,105,105)
#define CLR_GRAY                    RGB(128,128,128)
#define CLR_LIGHT_SLATE_GRAY        RGB(119,136,153)
#define CLR_SLATE_GRAY              RGB(112,128,144)
#define CLR_DARK_SLATE_GRAY         RGB(47,79,79)
#define CLR_BLACK                   RGB(0,0,0)