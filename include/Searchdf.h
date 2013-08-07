#include "newadd.h"

//#define CHARPOSITION

#ifndef SEARCHDF_H
#define SEARCHDF_H
// Define constants that represent indices into the TPosition array.
// We use the position of the white king and black king very often in
// the search and so it is very advantageous to be able to access their
// positions quickly.  TPosition[19]=White King Square.
#define WKINGPOSITION 19
#define BKINGPOSITION 20
#define LASTMOVESQUARE 29
#define WHITECASTLED 39
#define BLACKCASTLED 40
#define HASHKEYFIELD1 49
#ifdef CHARPOSITION
	#define HASHKEYFIELD2 50
	#define HASHKEYFIELD3 59
	#define HASHKEYFIELD4 60
#endif

#define WHITEPAWNS 90
#define WHITEKNIGHTS 99
#define WHITEBISHOPS 110
#define WHITEROOKS 121
#define WHITEQUEENS 132
#define WHITEKINGS 142
#define BLACKPAWNS 152
#define BLACKKNIGHTS 161
#define BLACKBISHOPS 172
#define BLACKROOKS 183
#define BLACKQUEENS 194
#define BLACKKINGS 204

#define NUMBEROFPOSITIONELEMENTS (BLACKKINGS+10)

#ifdef CHARPOSITION
	#define POSITIONELEMENT unsigned char
	#define SIZEOFPOSITIONELEMENT 1
#else
	#define POSITIONELEMENT unsigned int
	#define SIZEOFPOSITIONELEMENT 4
#endif

#define SIZEOFPOSITION NUMBEROFPOSITIONELEMENTS*SIZEOFPOSITIONELEMENT

typedef POSITIONELEMENT TPosition[NUMBEROFPOSITIONELEMENTS];

#define HASHDIV3 (128*128*128)
#define HASHDIV2 (128*128)
#define HASHDIV1 128

struct LockStruct
{
	int Lock;
};

#define SIZEOFLOCKSTRUCT 4

struct THashMove
{
	char From;
	char To;
	LockStruct Lock;
	int Value;
	char Depth;
	char Flag;
	char Stale;
};

#define HASHMOVESIZE 9+SIZEOFLOCKSTRUCT

#endif
