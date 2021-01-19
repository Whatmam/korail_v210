#ifndef _FND_PARSER_H_
#define _FND_PARSER_H_


typedef	enum
{
	PARSER_INT_01 = 5,		// | 	// 30
	PARSER_INT_02,			// _	// 40
	PARSER_INT_03 = 30,		// _	// 08
	PARSER_INT_04 = 33,		// !	// 02	// + DP
	PARSER_INT_05,			// "	// 22
	PARSER_INT_06 = 39,		// '	// 20
	PARSER_INT_07 = 45,		// -	// 40
	PARSER_INT_08,			// .	// + DP

	PARSER_INT_09 = 48,		// 0	// 3F		
	PARSER_INT_10,			// 1	// 30
	PARSER_INT_11,			// 2	// 5B
	PARSER_INT_12,			// 3	// 40
	PARSER_INT_13,			// 4	// 66
	PARSER_INT_14,			// 5	// 6D
	PARSER_INT_15,			// 6	// 7D
	PARSER_INT_16,			// 7	// 03
	PARSER_INT_17,			// 8	// 7F
	PARSER_INT_18,			// 9	// 67

	PARSER_INT_19 = 65,		// A	// 77
	PARSER_INT_20,			// B	// 7F
	PARSER_INT_21,			// C	// 39
	PARSER_INT_22,			// D	// 3F
	PARSER_INT_23,			// E	// 79
	PARSER_INT_24,			// F	// 71
	PARSER_INT_25,			// G	// 7E
	PARSER_INT_26,			// H	// 76
	PARSER_INT_27,			// I	// 06
	PARSER_INT_28,			// J	// 1E
	PARSER_INT_29,			// K	// 7E
	PARSER_INT_30,			// L	// 38
	PARSER_INT_31,			// M	// XXXXXX
	PARSER_INT_32,			// N	// 54 // n
	PARSER_INT_33,			// O	// 3F
	PARSER_INT_34,			// P	// 73
	PARSER_INT_35,			// Q	// XXXXXX
	PARSER_INT_36,			// R	// 77
	PARSER_INT_37,			// S	// 6D
	PARSER_INT_38,			// T    // XXXXXX
	PARSER_INT_39,			// U	// 3D
	PARSER_INT_40,			// V	// 3D
	PARSER_INT_41,			// W	// XXXXXX
	PARSER_INT_42,			// X	// XXXXXX
	PARSER_INT_43,			// Y    // 6E
	PARSER_INT_44,			// Z	// 5B

	PARSER_INT_45 = 91,		// [	// 39
	PARSER_INT_46 = 93,		// ]	// 0F

	PARSER_INT_47 = 95,		// _	// 08
	PARSER_INT_48,			// '	// 20
	PARSER_INT_49 = 124		// |	// 30
	
} SampleParserNum;


#endif _FND_PARSER_H_
