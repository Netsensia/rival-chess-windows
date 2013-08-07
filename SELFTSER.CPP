#include "defines.h"
#include "selftser.h"
#include "debug.h"
#include "newrival.h"
#include <windows.h>

extern int HashSuccesses, HashCalls, MakeMoveCalls, GMLCalls, GQMLCalls, GMLCMCalls, GQMLCMCalls, StoreHashMoveCalls;
extern int GetMoveListTimer, GetQuickMoveListTimer, MakeMoveTimer, EvaluateTimer, GetHashMoveTimer, StoreHashMoveTimer;
extern int Case1Calls, Case2Calls, Case3Calls, Case4Calls;
extern int Case1Timer, Case2Timer, Case3Timer, Case4Timer;
int timeticker2, timeticker;

TSelfTakeSearcher::TSelfTakeSearcher(int HashTableSize)
  : TSearcher(HashTableSize)
{
	int i, j;
	VariantID=SELFTAKE;
	GeneratePieceValues();
	SetupTropism();
}


int _pascal
TSelfTakeSearcher::GetMoveList(TPosition InPosition, TMoveArray MoveArray)
{
	GMLCalls++;
	timeticker=GetTickCount();
	Amount=0;
	register POSITIONELEMENT* Position=InPosition;
	register int x, y, SquareIndex, Square;
	int i, u, v;
	POSITIONELEMENT *SquarePointer;
	POSITIONELEMENT tt;
	int LastMovePiece=Position[LASTMOVESQUARE];
	int Score;

	if (Position[MOVER]==WHITE) {
	for (x=1, SquareIndex=10, SquarePointer=Position+10; x<=8; x++, SquareIndex=x*10, SquarePointer=Position+SquareIndex)
	  for (y=1, SquarePointer++, SquareIndex++; y<=8; y++, SquarePointer++, SquareIndex++) {
		 if (*SquarePointer<EMPTY)
		 switch (*SquarePointer) {
			 case WP : if (*(SquarePointer+1)==EMPTY) {
							 if (y==7) {
								CreateMove3(SquareIndex, SquareIndex+1, QUEEN);
								CreateMove3(SquareIndex, SquareIndex+1, BISHOP);
								CreateMove3(SquareIndex, SquareIndex+1, ROOK);
								CreateMove3(SquareIndex, SquareIndex+1, KNIGHT);
							 } else {
								CreateMove2(SquareIndex, SquareIndex+1);
							 }
							 if (y==2 && *(SquarePointer+2)==EMPTY) CreateMove2(SquareIndex, SquareIndex+2);
						  }
						  if (x<8 && *(SquarePointer+11)>EMPTY) {
							 if (y==7) {
								CreateMove3(SquareIndex, SquareIndex+11, QUEEN);
								CreateMove3(SquareIndex, SquareIndex+11, BISHOP);
								CreateMove3(SquareIndex, SquareIndex+11, ROOK);
								CreateMove3(SquareIndex, SquareIndex+11, KNIGHT);
							 } else {
								CreateMove2(SquareIndex, SquareIndex+11);
							 }
						  }
						  if (x>1 && *(SquarePointer-9)>EMPTY) {
							 if (y==7) {
								CreateMove3(SquareIndex, SquareIndex-9, QUEEN);
								CreateMove3(SquareIndex, SquareIndex-9, BISHOP);
								CreateMove3(SquareIndex, SquareIndex-9, ROOK);
								CreateMove3(SquareIndex, SquareIndex-9, KNIGHT);
							 } else {
								CreateMove2(SquareIndex, SquareIndex-9);
							 }
						  }
						  if (y==5 && x>1 && x-1==Position[ENPAWN]) CreateMove2(SquareIndex, SquareIndex-9);
						  if (y==5 && x<8 && x+1==Position[ENPAWN]) CreateMove2(SquareIndex, SquareIndex+11);
						  break;
			 case WK : if (x>1) CreateMove2(SquareIndex, SquareIndex-10);
						  if (x<8) CreateMove2(SquareIndex, SquareIndex+10);
						  if (y>1) CreateMove2(SquareIndex, SquareIndex-1);
						  if (y<8) CreateMove2(SquareIndex, SquareIndex+1);
						  if (x>1 && y>1) CreateMove2(SquareIndex, SquareIndex-11);
						  if (x<8 && y<8) CreateMove2(SquareIndex, SquareIndex+11);
						  if (x>1 && y<8) CreateMove2(SquareIndex, SquareIndex-9);
						  if (x<8 && y>1) CreateMove2(SquareIndex, SquareIndex+9);
						  if (!Position[WKINGMOVED]) {
									if (!Position[WROOK8MOVED]) {
									  if (Position[61]==EMPTY && Position[71]==EMPTY) {
										  CreateMove2(51, 71);
									  }
									}
									if (!Position[WROOK1MOVED]) {
									  if (Position[41]==EMPTY && Position[31]==EMPTY && Position[21]==EMPTY) {
										  CreateMove2(51, 31);
									  }
									}
						  }
						  break;
			 case WN : if (x<8 && y>2) CreateMove2(SquareIndex, SquareIndex+8);
						  if (x>1 && y<7) CreateMove2(SquareIndex, SquareIndex-8);
						  if (x<8 && y<7) CreateMove2(SquareIndex, SquareIndex+12);
						  if (x>1 && y>2) CreateMove2(SquareIndex, SquareIndex-12);
						  if (x<7 && y>1) CreateMove2(SquareIndex, SquareIndex+19);
						  if (x>2 && y<8) CreateMove2(SquareIndex, SquareIndex-19);
						  if (x<7 && y<8) CreateMove2(SquareIndex, SquareIndex+21);
						  if (x>2 && y>1) CreateMove2(SquareIndex, SquareIndex-21);
						  break;
			 case WR : for (i=SquareIndex+10; i<90 ; i+=10)
								  if (Position[i]!=WK) {
									  CreateMove2(SquareIndex, i);
									  if (Position[i]!=EMPTY) break;
								  } else break;
						  for (i=SquareIndex-10; i>9 ; i-=10)
								  if (Position[i]!=WK) {
									  CreateMove2(SquareIndex, i);
									  if (Position[i]!=EMPTY) break;
								  } else break;
						  for (i=SquareIndex+1; Valid[i] ; i++)
								  if (Position[i]!=WK) {
									  CreateMove2(SquareIndex, i);
									  if (Position[i]!=EMPTY) break;
								  } else break;
						  for (i=SquareIndex-1; Valid[i] ; i--)
								  if (Position[i]!=WK) {
									 CreateMove2(SquareIndex, i);
									 if (Position[i]!=EMPTY) break;
								  } else break;
						  break;
			 case WB : for (i=SquareIndex+11; Valid[i] ; i+=11)
								  if (Position[i]!=WK) {
									  CreateMove2(SquareIndex, i);
									  if (Position[i]!=EMPTY) break;
								  } else break;
						  for (i=SquareIndex-11; Valid[i] ; i-=11)
								  if (Position[i]!=WK) {
									  CreateMove2(SquareIndex, i);
									  if (Position[i]!=EMPTY) break;
								  } else break;
						  for (i=SquareIndex-9; Valid[i] ; i-=9)
								  if (Position[i]!=WK) {
									  CreateMove2(SquareIndex, i);
									  if (Position[i]!=EMPTY) break;
								  } else break;
						  for (i=SquareIndex+9; Valid[i] ; i+=9)
								  if (Position[i]!=WK) {
									 CreateMove2(SquareIndex, i);
									 if (Position[i]!=EMPTY) break;
								  } else break;
						  break;
			 case WQ : for (i=SquareIndex+10; i<90 ; i+=10)
								  if (Position[i]!=WK) {
									  CreateMove2(SquareIndex, i);
									  if (Position[i]!=EMPTY) break;
								  } else break;
						  for (i=SquareIndex-10; i>9 ; i-=10)
								  if (Position[i]!=WK) {
									  CreateMove2(SquareIndex, i);
									  if (Position[i]!=EMPTY) break;
								  } else break;
						  for (i=SquareIndex+1; Valid[i] ; i++)
								  if (Position[i]!=WK) {
									  CreateMove2(SquareIndex, i);
									  if (Position[i]!=EMPTY) break;
								  } else break;
						  for (i=SquareIndex-1; Valid[i] ; i--)
								  if (Position[i]!=WK) {
									 CreateMove2(SquareIndex, i);
									 if (Position[i]!=EMPTY) break;
								  } else break;
						  for (i=SquareIndex+11; Valid[i] ; i+=11)
								  if (Position[i]!=WK) {
									  CreateMove2(SquareIndex, i);
									  if (Position[i]!=EMPTY) break;
								  } else break;
						  for (i=SquareIndex-11; Valid[i] ; i-=11)
								  if (Position[i]!=WK) {
									  CreateMove2(SquareIndex, i);
									  if (Position[i]!=EMPTY) break;
								  } else break;
						  for (i=SquareIndex-9; Valid[i] ; i-=9)
								  if (Position[i]!=WK) {
									  CreateMove2(SquareIndex, i);
									  if (Position[i]!=EMPTY) break;
								  } else break;
						  for (i=SquareIndex+9; Valid[i] ; i+=9)
								  if (Position[i]!=WK) {
									 CreateMove2(SquareIndex, i);
									 if (Position[i]!=EMPTY) break;
								  } else break;
						  break;
		 }
	  }
	} else
	{
	for (x=1, SquareIndex=10, SquarePointer=Position+10; x<=8; x++, SquareIndex=x*10, SquarePointer=Position+SquareIndex)
	  for (y=1, SquarePointer++, SquareIndex++; y<=8; y++, SquarePointer++, SquareIndex++) {
		 if (*SquarePointer>EMPTY)
		 switch (*SquarePointer) {
			 case BP : if (*(SquarePointer-1)==EMPTY) {
							  if (y==2)	{
									CreateMove3(SquareIndex, SquareIndex-1, QUEEN);
									CreateMove3(SquareIndex, SquareIndex-1, BISHOP);
									CreateMove3(SquareIndex, SquareIndex-1, ROOK);
									CreateMove3(SquareIndex, SquareIndex-1, KNIGHT);
							  } else {
									CreateMove2(SquareIndex, SquareIndex-1);
							  }
							  if (y==7 && *(SquarePointer-2)==EMPTY)
								  CreateMove2(SquareIndex, SquareIndex-2);
						  }
						  if (x<8 && *(SquarePointer+9)<EMPTY) {
							  if (y==2) {
									CreateMove3(SquareIndex, SquareIndex+9, QUEEN);
									CreateMove3(SquareIndex, SquareIndex+9, BISHOP);
									CreateMove3(SquareIndex, SquareIndex+9, ROOK);
									CreateMove3(SquareIndex, SquareIndex+9, KNIGHT);
							  } else {
									CreateMove2(SquareIndex, SquareIndex+9);
							  }
						  }
						  if (x>1 && *(SquarePointer-11)<EMPTY) {
							  if (y==2) {
									CreateMove3(SquareIndex, SquareIndex-11, QUEEN);
									CreateMove3(SquareIndex, SquareIndex-11, BISHOP);
									CreateMove3(SquareIndex, SquareIndex-11, ROOK);
									CreateMove3(SquareIndex, SquareIndex-11, KNIGHT);
							  } else {
									CreateMove2(SquareIndex, SquareIndex-11);
							  }
						  }
						  if (y==4 && x>1 && x-1==Position[ENPAWN]) CreateMove2(SquareIndex, SquareIndex-11);
						  if (y==4 && x<8 && x+1==Position[ENPAWN]) CreateMove2(SquareIndex, SquareIndex+9);
						  break;
			 case BK : if (x>1) CreateMove2(SquareIndex, SquareIndex-10);
						  if (x<8) CreateMove2(SquareIndex, SquareIndex+10);
						  if (y>1) CreateMove2(SquareIndex, SquareIndex-1);
						  if (y<8) CreateMove2(SquareIndex, SquareIndex+1);
						  if (x>1 && y>1) CreateMove2(SquareIndex, SquareIndex-11);
						  if (x<8 && y<8) CreateMove2(SquareIndex, SquareIndex+11);
						  if (x>1 && y<8) CreateMove2(SquareIndex, SquareIndex-9);
						  if (x<8 && y>1) CreateMove2(SquareIndex, SquareIndex+9);
						  if (!Position[BKINGMOVED]) {
									if (!Position[BROOK8MOVED]) {
									  if (Position[68]==EMPTY && Position[78]==EMPTY) {
										  CreateMove2(58, 78);
									  }
									}
									if (!Position[BROOK1MOVED]) {
									  if (Position[48]==EMPTY && Position[38]==EMPTY && Position[28]==EMPTY) {
										  CreateMove2(58, 38);
									  }
									}
						  }
						  break;
			 case BN : if (x<8 && y>2) CreateMove2(SquareIndex, SquareIndex+8);
						  if (x>1 && y<7) CreateMove2(SquareIndex, SquareIndex-8);
						  if (x<8 && y<7) CreateMove2(SquareIndex, SquareIndex+12);
						  if (x>1 && y>2) CreateMove2(SquareIndex, SquareIndex-12);
						  if (x<7 && y>1) CreateMove2(SquareIndex, SquareIndex+19);
						  if (x>2 && y<8) CreateMove2(SquareIndex, SquareIndex-19);
						  if (x<7 && y<8) CreateMove2(SquareIndex, SquareIndex+21);
						  if (x>2 && y>1) CreateMove2(SquareIndex, SquareIndex-21);
						  break;
			 case BR : for (i=SquareIndex+10; i<90 ; i+=10)
								  if (Position[i]!=BK) {
									  CreateMove2(SquareIndex, i);
									  if (Position[i]!=EMPTY) break;
								  } else break;
						  for (i=SquareIndex-10; i>9 ; i-=10)
								  if (Position[i]!=BK) {
									  CreateMove2(SquareIndex, i);
									  if (Position[i]!=EMPTY) break;
								  } else break;
						  for (i=SquareIndex+1; Valid[i] ; i++)
								  if (Position[i]!=BK) {
									  CreateMove2(SquareIndex, i);
									  if (Position[i]!=EMPTY) break;
								  } else break;
						  for (i=SquareIndex-1; Valid[i] ; i--)
								  if (Position[i]!=BK) {
									 CreateMove2(SquareIndex, i);
									 if (Position[i]!=EMPTY) break;
								  } else break;
						  break;
			 case BB : for (i=SquareIndex+11; Valid[i] ; i+=11)
								  if (Position[i]!=BK) {
									  CreateMove2(SquareIndex, i);
									  if (Position[i]!=EMPTY) break;
								  } else break;
						  for (i=SquareIndex-11; Valid[i] ; i-=11)
								  if (Position[i]!=BK) {
									  CreateMove2(SquareIndex, i);
									  if (Position[i]!=EMPTY) break;
								  } else break;
						  for (i=SquareIndex-9; Valid[i] ; i-=9)
								  if (Position[i]!=BK) {
									  CreateMove2(SquareIndex, i);
									  if (Position[i]!=EMPTY) break;
								  } else break;
						  for (i=SquareIndex+9; Valid[i] ; i+=9)
								  if (Position[i]!=BK) {
									 CreateMove2(SquareIndex, i);
									 if (Position[i]!=EMPTY) break;
								  } else break;
						  break;
			 case BQ : for (i=SquareIndex+10; i<90 ; i+=10)
								  if (Position[i]!=BK) {
									  CreateMove2(SquareIndex, i);
									  if (Position[i]!=EMPTY) break;
								  } else break;
						  for (i=SquareIndex-10; i>9 ; i-=10)
								  if (Position[i]!=BK) {
									  CreateMove2(SquareIndex, i);
									  if (Position[i]!=EMPTY) break;
								  } else break;
						  for (i=SquareIndex+1; Valid[i] ; i++)
								  if (Position[i]!=BK) {
									  CreateMove2(SquareIndex, i);
									  if (Position[i]!=EMPTY) break;
								  } else break;
						  for (i=SquareIndex-1; Valid[i] ; i--)
								  if (Position[i]!=BK) {
									 CreateMove2(SquareIndex, i);
									 if (Position[i]!=EMPTY) break;
								  } else break;
						  for (i=SquareIndex+11; Valid[i] ; i+=11)
								  if (Position[i]!=BK) {
									  CreateMove2(SquareIndex, i);
									  if (Position[i]!=EMPTY) break;
								  } else break;
						  for (i=SquareIndex-11; Valid[i] ; i-=11)
								  if (Position[i]!=BK) {
									  CreateMove2(SquareIndex, i);
									  if (Position[i]!=EMPTY) break;
								  } else break;
						  for (i=SquareIndex-9; Valid[i] ; i-=9)
								  if (Position[i]!=BK) {
									  CreateMove2(SquareIndex, i);
									  if (Position[i]!=EMPTY) break;
								  } else break;
						  for (i=SquareIndex+9; Valid[i] ; i+=9)
								  if (Position[i]!=BK) {
									 CreateMove2(SquareIndex, i);
									 if (Position[i]!=EMPTY) break;
								  } else break;
						  break;
		 }
	  }
	}
	GMLCMCalls+=Amount;
	GetMoveListTimer+=(GetTickCount()-timeticker);
	return Amount;
}

int
TSelfTakeSearcher::UseAllMovesOnQuiesce(TPosition Position)
{
	return IsCheck(Position);
}


