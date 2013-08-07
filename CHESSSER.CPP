#include <time.h>
#include <windows.h>
#include "defines.h"
#include "chessser.h"
#include "newrival.h"
#include "debug.h"

extern int HashSuccesses, EvaluateCalls, HashCalls, MakeMoveCalls, GMLCalls, GQMLCalls, GMLCMCalls, GQMLCMCalls, StoreHashMoveCalls;
extern int GetMoveListTimer, GetQuickMoveListTimer, MakeMoveTimer, EvaluateTimer, GetHashMoveTimer, StoreHashMoveTimer;
extern int Case1Calls, Case2Calls, Case3Calls, Case4Calls;
extern int Case1Timer, Case2Timer, Case3Timer, Case4Timer;
int timeticker2, timeticker;

TChessSearcher::TChessSearcher(int HashTableSize)
  : TSearcher(HashTableSize)
{
	int i, j;
	VariantID=CHESS;
	GeneratePieceValues();
	SetupTropism();
}

int
TChessSearcher::UseAllMovesOnQuiesce(TPosition Position)
{
	return IsCheck(Position);
}

