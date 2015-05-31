#ifndef __textin_h__
#define __textin_h__

#define FONTNUM_IS_BASIC_LATIN(num) ((num >= 1000  && num <= 1013) || (num >= 502 && num <= 513) || (num >=32 && num <=43) || (num >= 0 && num <= 19 && num != 11) ? 1 : 0)
#define FONTNUM_IS_GREEK(num)       ((num >= 1014 && num <= 1018) || num == 514 || num == 44 || num == 11 ? 1 : 0)
#define FONTNUM_IS_JAPANESE(num)    ((num >= 1019 && num <= 1021) ? 1 : 0)
#define FONTNUM_IS_BAKOMA(num)    ((num >= 1022 && num <= 1027) ? 1 : 0)

#define UNICODE_IS_BASIC_LATIN(code) ((code >= 0x21 && code <= 0x7E) ? 1 : 0) 
#define UNICODE_IS_SPACE(code) (code == 0x20 ? 1 : 0) 
#define UNICODE_IS_GREEK(code) ((code >= 0x370 && code <= 0x03FF) ? 1 : 0) 
#define UNICODE_IS_HIRAGANA(code) ((code >= 0x3040 && code <= 0x309F) ? 1 : 0) 
#define UNICODE_IS_KATAKANA(code) ((code >= 0x30A0 && code <= 0x30FF) ? 1 : 0) 

#define LATIN_DEFAULT    1000
#define MATH_DEFAULT     1001
#define GREEK_DEFAULT    1014
#define JAPANESE_DEFAULT 1019

#define NONENC  -1
#define UNICODE 0
#define SJIS    1
#define SYMBOL  2
#define APPLE_ROMAN    3
#define ADOBE_CUSTOM   4
#define ADOBE_STANDARD 5

#define SPACE     0x20
#define EXCLAMATION 0x21
#define SHARP     0x23
#define DOLLAR    0x24
#define LEFT_PARENTHESIS 0x28 /* ( */
#define RIGHT_PARENTHESIS 0x29 /* ) */
#define ASTERISK  0x2A
#define PLUS      0x2B
#define COMMA     0x2C
#define MINUS     0x2D
#define PERIOD    0x2E
#define SLASH     0x2F
#define ZERO      0x30
#define ONE       0x31
#define TWO       0x32
#define THREE     0x33
#define FOUR      0x34
#define FIVE      0x35
#define SIX       0x36
#define SEVEN     0x37
#define EIGHT     0x38
#define NINE      0x39
#define SEMICOLON 0x3B
#define EQUAL     0x3D
#define QUESTION  0x3F
#define AT        0x40
#define CAPITAL_A 0x41
#define CAPITAL_B 0x42
#define CAPITAL_C 0x43
#define CAPITAL_D 0x44
#define CAPITAL_E 0x45
#define CAPITAL_F 0x46
#define CAPITAL_G 0x47
#define CAPITAL_H 0x48
#define CAPITAL_I 0x49
#define CAPITAL_J 0x4A
#define CAPITAL_K 0x4B
#define CAPITAL_L 0x4C
#define CAPITAL_M 0x4D
#define CAPITAL_N 0x4E
#define CAPITAL_O 0x4F
#define CAPITAL_P 0x50
#define CAPITAL_Q 0x51
#define CAPITAL_R 0x52
#define CAPITAL_S 0x53
#define CAPITAL_T 0x54
#define CAPITAL_U 0x55
#define CAPITAL_V 0x56
#define CAPITAL_W 0x57
#define CAPITAL_X 0x58
#define CAPITAL_Y 0x59
#define CAPITAL_Z 0x5A
#define LEFT_SQUARE_BRACKET 0x5B /* [ */
#define BACKSLASH 0x5C
#define RIGHT_SQUARE_BRACKET 0x5D /* [ */
#define SMALL_A   0x61
#define SMALL_B   0x62
#define SMALL_C   0x63
#define SMALL_D   0x64
#define SMALL_E   0x65
#define SMALL_F   0x66
#define SMALL_G   0x67
#define SMALL_H   0x68
#define SMALL_I   0x69
#define SMALL_J   0x6A
#define SMALL_K   0x6B
#define SMALL_L   0x6C
#define SMALL_M   0x6D
#define SMALL_N   0x6E
#define SMALL_O   0x6F
#define SMALL_P   0x70
#define SMALL_Q   0x71
#define SMALL_R   0x72
#define SMALL_S   0x73
#define SMALL_T   0x74
#define SMALL_U   0x75
#define SMALL_V   0x76
#define SMALL_W   0x77
#define SMALL_X   0x78
#define SMALL_Y   0x79
#define SMALL_Z   0x7A
#define CURLY_BRA 0x7B
#define VERTICAL_LINE 0x7C
#define CURLY_KET 0x7D

#define SAVE 1
#define GET  2

#define NONENC  -1
#define UNICODE 0
#define SJIS    1
#define SYMBOL  2
#define APPLE_ROMAN    3
#define ADOBE_CUSTOM   4
#define ADOBE_STANDARD 5

#define NULLCHAR  -1
#define LATIN     0
#define GREEK     1
#define JAPANESE  2

#define SUBSIZE      0.7
#define JAPANESESIZE 0.961 /* 10pt vs 13Q */

#endif
