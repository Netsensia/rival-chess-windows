#include "defines.h"

#define LEVEL 0
#define TIMEPERMOVE 1
#define CHAMPIONSHIP 2
#define BASEINCREMENT 3
#define INFINITETIME 4

#define CHESS 1
#define KINGLET 2
#define SHATRANJ 3
#define SELFTAKE 4

#undef DYNAMICFEN

#ifndef _newrival.h
#define _newrival.h

struct TOpeningPosition
{
#ifdef DYNAMICFEN
	char* Fen;
#else
	char Fen[MAXFEN];
#endif
	int WhiteWins;
	int BlackWins;
	int Draws;
   int FenLen;
};

struct OptionStruct {
	 int Flag;
	 int InvertBoard;
	 int RandomMoveOrdering;
	 int HashReadReduce, HashWriteReduce;
	 int PieceSlide;
	 int ComputerWhite;
	 int ComputerBlack;
	 int Messages;
    int History;
	 int ShowAnalysis;
	 int SearchMethod;
	 int Level;
	 int BaseMinutes, Increment;
	 int Openings;
	 int Time;
	 int NullMoveStopMaterial;
	 int OpeningLearning;
	 int ChampMoves;
	 int ChampTime;
	 int Quiesce, Contempt, NullMoveReduce;
	 int Recapture, PawnPush, Check, SingleReply;
	 int TransRef, NullMove, MinimalWindow, FailHighReductions;
	 int AutoSave;
	 int Variant;
    int MaxExtend, Aspire;
	 int WhiteOffset, BlackOffset, BackOffset, BackGround;
	 int ChessBoard, Pieces, AnalysisBoxStyle, MoveBoxStyle;
	 int AnalysisTextOffset, MoveBoxTextOffset;
	 int MinimumBookResults;
	 int WhitePercent, BlackPercent;
	 int Ponder;
    int HashTableSize;
};
#endif
