#include "newrival.h"
#include "kinglser.h"
#include "debug.h"

extern int HashSuccesses, HashCalls, MakeMoveCalls, GMLCalls, GQMLCalls, GMLCMCalls, GQMLCMCalls, StoreHashMoveCalls;
extern int GetMoveListTimer, GetQuickMoveListTimer, MakeMoveTimer, EvaluateTimer, GetHashMoveTimer, StoreHashMoveTimer;
extern int Case1Calls, Case2Calls, Case3Calls, Case4Calls;
extern int Case1Timer, Case2Timer, Case3Timer, Case4Timer;
int timeticker2, timeticker;

TKingletSearcher::TKingletSearcher(int HashTableSize)
  : TSearcher(HashTableSize)
{
	int i, j;
	VariantID=KINGLET;
	GeneratePieceValues();

	PieceValues[PAWN]=100;
	PieceValues[KNIGHT]=325;
	PieceValues[BISHOP]=335;
	PieceValues[ROOK]=500;
	PieceValues[QUEEN]=900;
	PieceValues[KING]=250;
	PieceValues[PAWN+100]=100;
	PieceValues[KNIGHT+100]=325;
	PieceValues[BISHOP+100]=335;
	PieceValues[ROOK+100]=500;
	PieceValues[QUEEN+100]=900;
	PieceValues[KING+100]=250;
	PieceValuesDiv10[PAWN]=10;
	PieceValuesDiv10[KNIGHT]=32;
	PieceValuesDiv10[BISHOP]=33;
	PieceValuesDiv10[ROOK]=50;
	PieceValuesDiv10[QUEEN]=90;
	PieceValuesDiv10[KING]=25;
	PieceValuesDiv10[PAWN+100]=10;
	PieceValuesDiv10[KNIGHT+100]=32;
	PieceValuesDiv10[BISHOP+100]=33;
	PieceValuesDiv10[ROOK+100]=50;
	PieceValuesDiv10[QUEEN+100]=90;
	PieceValuesDiv10[KING+100]=25;

	SetupTropism();
}

// I tracked down a vicious little bug by using this define in the
// next function.  It may come in useful again.

void
TKingletSearcher::SetInitialPosition(TChessBoard NewPosition)
{
	int i, x, y;
	POSITIONELEMENT Square;
	for (i=0; i<89; i++)
		StartPosition[i]=NewPosition.GetSquare(i);
	StartPosition[WHITEPAWNS]=
	StartPosition[WHITEKNIGHTS]=
	StartPosition[WHITEBISHOPS]=
	StartPosition[WHITEQUEENS]=
	StartPosition[WHITEROOKS]=
	StartPosition[WHITEKINGS]=
	StartPosition[BLACKPAWNS]=
	StartPosition[BLACKKNIGHTS]=
	StartPosition[BLACKBISHOPS]=
	StartPosition[BLACKQUEENS]=
	StartPosition[BLACKROOKS]=
	StartPosition[BLACKKINGS]=0;
	Evalues[7]=5000;
	Evalues[8]=3250;
	Evalues[9]=3400;
	Evalues[10]=9000;
	Evalues[11]=2500;
	for (i=0; i<7; i++)
		Evalues[i]=1000;
	for (x=1; x<=8; x++) {
	  for (y=1; y<=8; y++) {
		 Square=x*10+y;
		 if (StartPosition[Square]==WK) {
			  StartPosition[WHITEKINGS]++;
			  StartPosition[WHITEKINGS+StartPosition[WHITEKINGS]]=Square;
		 }
		 if (StartPosition[Square]==BK) {
			  StartPosition[BLACKKINGS]++;
			  StartPosition[BLACKKINGS+StartPosition[BLACKKINGS]]=Square;
		 }
		 if (StartPosition[Square]==WP) {
			  StartPosition[WHITEPAWNS]++;
			  StartPosition[WHITEPAWNS+StartPosition[WHITEPAWNS]]=Square;
		 }
		 if (StartPosition[Square]==BP) {
			  StartPosition[BLACKPAWNS]++;
			  StartPosition[BLACKPAWNS+StartPosition[BLACKPAWNS]]=Square;
		 }
		 if (StartPosition[Square]==WB) {
			  StartPosition[WHITEBISHOPS]++;
			  StartPosition[WHITEBISHOPS+StartPosition[WHITEBISHOPS]]=Square;
		 }
		 if (StartPosition[Square]==BB) {
			  StartPosition[BLACKBISHOPS]++;
			  StartPosition[BLACKBISHOPS+StartPosition[BLACKBISHOPS]]=Square;
		 }
		 if (StartPosition[Square]==WN) {
			  StartPosition[WHITEKNIGHTS]++;
			  StartPosition[WHITEKNIGHTS+StartPosition[WHITEKNIGHTS]]=Square;
		 }
		 if (StartPosition[Square]==BN) {
			  StartPosition[BLACKKNIGHTS]++;
			  StartPosition[BLACKKNIGHTS+StartPosition[BLACKKNIGHTS]]=Square;
		 }
		 if (StartPosition[Square]==WQ) {
			  StartPosition[WHITEQUEENS]++;
			  StartPosition[WHITEQUEENS+StartPosition[WHITEQUEENS]]=Square;
		 }
		 if (StartPosition[Square]==BQ) {
			  StartPosition[BLACKQUEENS]++;
			  StartPosition[BLACKQUEENS+StartPosition[BLACKQUEENS]]=Square;
		 }
		 if (StartPosition[Square]==WR) {
			  StartPosition[WHITEROOKS]++;
			  StartPosition[WHITEROOKS+StartPosition[WHITEROOKS]]=Square;
		 }
		 if (StartPosition[Square]==BR) {
			  StartPosition[BLACKROOKS]++;
			  StartPosition[BLACKROOKS+StartPosition[BLACKROOKS]]=Square;
		 }
	  }
	}
	StartPosition[LASTMOVESQUARE]=0;
	TMoveList MoveList=NewPosition.GetPreviousMoves();
	StartPosition[WHITECASTLED]=FALSE;
	StartPosition[BLACKCASTLED]=FALSE;
#ifdef HELP
	f=fopen("tttt", "w"); fputc('4', f); fclose(f);
#endif
	for (i=1; i<=MoveList.Amount; i++) {
		if ((MoveList.Moves[i].From==51 && MoveList.Moves[i].To==71) ||
			(MoveList.Moves[i].From==51 && MoveList.Moves[i].To==31))
				StartPosition[WHITECASTLED]=TRUE;
		if ((MoveList.Moves[i].From==58 && MoveList.Moves[i].To==78) ||
			(MoveList.Moves[i].From==58 && MoveList.Moves[i].To==38))
				StartPosition[BLACKCASTLED]=TRUE;
	}
#ifdef CHARPOSITION
	int HashKey=ComputeHashKey(StartPosition);
	StartPosition[HASHKEYFIELD1]=HashKey/HASHDIV3;
	StartPosition[HASHKEYFIELD2]=HashKey%HASHDIV3/HASHDIV2;
	StartPosition[HASHKEYFIELD3]=HashKey%HASHDIV2/HASHDIV1;
	StartPosition[HASHKEYFIELD4]=HashKey%HASHDIV1;
#else
	StartPosition[HASHKEYFIELD1]=ComputeHashKey(StartPosition);
#endif
	DrawMoves[0].From=0;
	TMoveArray Moves;
	int Identical;
	int DrawCount=0;
	int Ply1DrawCount=0;
	TPosition TempPosition;
	int MovesMade=NewPosition.LastMoveMade();
	CurrentDepth=0;
	int Amount=GetMoveList(StartPosition, Moves);
	int j;
	int LastChange=0;
	DrawMoves[0].From=0;
	Ply1DrawMovesPly0[0].From=0;
/* Set LastChange=Number of last capturing move */
	for (j=1; j<=MovesMade; j++) {
		if (NewPosition.MovesMade[j].Capture!=EMPTY || NewPosition.MovesMade[j].Score==TRUE)
			LastChange=j;
	}

	for (i=1; i<=Amount; i++) {
		if (MakeMove(StartPosition, Moves[i], TempPosition)) {
			NewPosition.TakeBackAllMoves();
/* Replay all moves up to the Last Change */
			for (j=1; j<=LastChange; j++) NewPosition.ReplayMove();
			for (j=LastChange; j<MovesMade; j++) {
				Identical=TRUE;
				for (x=0; x<=6; x++)
					if (NewPosition.GetSquare(x)!=TempPosition[x]) Identical=FALSE;
				if (Identical)
				for (x=1; x<=8 && Identical; x++)
					for (y=1; y<=8 && Identical; y++)
					  if (NewPosition.GetSquare(x*10+y)!=TempPosition[x*10+y]) Identical=FALSE;
				if (Identical) {
				  DrawMoves[DrawCount]=Moves[i];
				  DrawCount++;
				  if (DrawCount>24) DrawCount=24;
				  break;
				}
/* Test TempPosition2 which represents what position opponent can bring about */
				TMoveArray Moves2;
				TPosition TempPosition2;
				int Amount2=GetMoveList(TempPosition, Moves2);
				for (int k=1; k<=Amount2; k++) {
				  if (MakeMove(TempPosition, Moves2[k], TempPosition2)) {
					 Identical=TRUE;
					 for (x=0; x<=6; x++)
						if (NewPosition.GetSquare(x)!=TempPosition2[x]) Identical=FALSE;
					 if (Identical)
					 for (x=1; x<=8 && Identical; x++)
						for (y=1; y<=8 && Identical; y++)
						  if (NewPosition.GetSquare(x*10+y)!=TempPosition2[x*10+y]) Identical=FALSE;
					 if (Identical) {
						Ply1DrawMovesPly0[Ply1DrawCount]=Moves[i];
						Ply1DrawMovesPly1[Ply1DrawCount]=Moves2[k];
						Ply1DrawCount++;
						if (Ply1DrawCount>24) Ply1DrawCount=24;
						break;
					 }
				  }
				}
				NewPosition.ReplayMove();
			}
		}
	}
#ifdef HELP
	f=fopen("tttt", "w"); fputc('7', f); fclose(f);
#endif
	DrawMoves[DrawCount].From=0;
	Ply1DrawMovesPly0[Ply1DrawCount].From=0;
	for (i=0; i<89; i++)
		PathBoard[i]=StartPosition[i];
}

/*#define CreateMove(f, t) \
{ \
  MoveArray[++Amount].From=(f); \
  MoveArray[Amount].To=(t); \
}

#define CreateMove2(f, t) \
{ \
	tt=(t); \
	if ((f)==pmf && (tt)==pmt) Score=30000-CurrentDepth; else {\
	  Score=0; \
	  if (Position[(tt)]!=EMPTY) { \
		  if ((tt)==LastMovePiece) Score+=1001; \
		  Score+=(PieceValues[(Position[(tt)])]);     \
		  Score-=(PieceValuesDiv10[(Position[(f)])]); \
	  } \
	  if ((f)==GlobalHashMove.From && (tt)==GlobalHashMove.To) \
			Score+=1000; \
	  if ((f)==k1f && (tt)==k1t) Score+=50; \
	  if ((f)==k2f && (tt)==k2t) Score+=45;  \
	} \
	if (Score==0) u=Amount+1; \
	else { \
	  u=1; \
	  while (u<=Amount && Score<=MoveArray[u].Score) u++;\
	  for (v=Amount; v>=u; v--) \
		  MoveArray[v+1]=MoveArray[v];  \
	} ; \
	Amount++; \
	MoveArray[u].From=(f); \
	MoveArray[u].To=(tt); \
	MoveArray[u].Score=Score; \
} */


int sortfunc(const void *a, const void *b)
{
	if (((TMove*)a)->Score==((TMove*)b)->Score) return 0;
	if (((TMove*)a)->Score<((TMove*)b)->Score) return 1;
	return -1;
}

int _pascal
TKingletSearcher::GetMoveList(TPosition InPosition, TMoveArray MoveArray)
{
	GMLCalls++;
	timeticker=GetTickCount();
	Amount=0;
	register POSITIONELEMENT* Position=InPosition;
	register int x, y, SquareIndex, Square;
	int i, u, v;
//	POSITIONELEMENT *p;
	POSITIONELEMENT *SquarePointer;
	POSITIONELEMENT tt;
	int LastMovePiece=Position[LASTMOVESQUARE];
//	int k1f=Killers[0][CurrentDepth].From;
//	int k1t=Killers[0][CurrentDepth].To;
//	int k2f=Killers[1][CurrentDepth].From;
//	int k2t=Killers[1][CurrentDepth].To;
//	int pmf=(PrincipalPath.Move[CurrentDepth].From);
//	int pmt=(PrincipalPath.Move[CurrentDepth].To);
	int Score;

	if (Position[MOVER]==WHITE) {
	if (Position[WHITEPAWNS]==0) return 0;
	for (x=1, SquareIndex=10, SquarePointer=Position+10; x<=8; x++, SquareIndex=x*10, SquarePointer=Position+SquareIndex)
	  for (y=1, SquarePointer++, SquareIndex++; y<=8; y++, SquarePointer++, SquareIndex++) {
		 if (*SquarePointer<EMPTY)
		 switch (*SquarePointer) {
			 case WP : if (*(SquarePointer+1)==EMPTY) {
							 CreateMove3(SquareIndex, SquareIndex+1, KING);
							 if (y==2 && *(SquarePointer+2)==EMPTY)
								CreateMove2(SquareIndex, SquareIndex+2);
						  }
						  if (x<8 && *(SquarePointer+11)>EMPTY) CreateMove3(SquareIndex, SquareIndex+11, KING);
						  if (x>1 && *(SquarePointer-9)>EMPTY) CreateMove3(SquareIndex, SquareIndex-9, KING);
						  if (y==5 && x>1 && x-1==Position[ENPAWN]) CreateMove2(SquareIndex, SquareIndex-9);
						  if (y==5 && x<8 && x+1==Position[ENPAWN]) CreateMove2(SquareIndex, SquareIndex+11);
						  break;
			 case WK : if (x>1 && *(SquarePointer-10)>=EMPTY) CreateMove2(SquareIndex, SquareIndex-10);
						  if (x<8 && *(SquarePointer+10)>=EMPTY) CreateMove2(SquareIndex, SquareIndex+10);
						  if (y>1 && *(SquarePointer-1)>=EMPTY) CreateMove2(SquareIndex, SquareIndex-1);
						  if (y<8 && *(SquarePointer+1)>=EMPTY) CreateMove2(SquareIndex, SquareIndex+1);
						  if (x>1 && y>1 && *(SquarePointer-11)>=EMPTY) CreateMove2(SquareIndex, SquareIndex-11);
						  if (x<8 && y<8 && *(SquarePointer+11)>=EMPTY) CreateMove2(SquareIndex, SquareIndex+11);
						  if (x>1 && y<8 && *(SquarePointer-9)>=EMPTY) CreateMove2(SquareIndex, SquareIndex-9);
						  if (x<8 && y>1 && *(SquarePointer+9)>=EMPTY) CreateMove2(SquareIndex, SquareIndex+9);
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
			 case WN : if (x<8 && y>2 && *(SquarePointer+8)>=EMPTY) CreateMove2(SquareIndex, SquareIndex+8);
						  if (x>1 && y<7 && *(SquarePointer-8)>=EMPTY) CreateMove2(SquareIndex, SquareIndex-8);
						  if (x<8 && y<7 && *(SquarePointer+12)>=EMPTY) CreateMove2(SquareIndex, SquareIndex+12);
						  if (x>1 && y>2 && *(SquarePointer-12)>=EMPTY) CreateMove2(SquareIndex, SquareIndex-12);
						  if (x<7 && y>1 && *(SquarePointer+19)>=EMPTY) CreateMove2(SquareIndex, SquareIndex+19);
						  if (x>2 && y<8 && *(SquarePointer-19)>=EMPTY) CreateMove2(SquareIndex, SquareIndex-19);
						  if (x<7 && y<8 && *(SquarePointer+21)>=EMPTY) CreateMove2(SquareIndex, SquareIndex+21);
						  if (x>2 && y>1 && *(SquarePointer-21)>=EMPTY) CreateMove2(SquareIndex, SquareIndex-21);
						  break;
			 case WR : for (i=SquareIndex+10; i<90 ; i+=10)
								  if (Position[i]>=EMPTY) {
									  CreateMove2(SquareIndex, i);
									  if (Position[i]!=EMPTY) break;
								  } else break;
						  for (i=SquareIndex-10; i>9 ; i-=10)
								  if (Position[i]>=EMPTY) {
									  CreateMove2(SquareIndex, i);
									  if (Position[i]!=EMPTY) break;
								  } else break;
						  for (i=SquareIndex+1; Valid[i] ; i++)
								  if (Position[i]>=EMPTY) {
									  CreateMove2(SquareIndex, i);
									  if (Position[i]!=EMPTY) break;
								  } else break;
						  for (i=SquareIndex-1; Valid[i] ; i--)
								  if (Position[i]>=EMPTY) {
									 CreateMove2(SquareIndex, i);
									 if (Position[i]!=EMPTY) break;
								  } else break;
						  break;
			 case WB : for (i=SquareIndex+11; Valid[i] ; i+=11)
								  if (Position[i]>=EMPTY) {
									  CreateMove2(SquareIndex, i);
									  if (Position[i]!=EMPTY) break;
								  } else break;
						  for (i=SquareIndex-11; Valid[i] ; i-=11)
								  if (Position[i]>=EMPTY) {
									  CreateMove2(SquareIndex, i);
									  if (Position[i]!=EMPTY) break;
								  } else break;
						  for (i=SquareIndex-9; Valid[i] ; i-=9)
								  if (Position[i]>=EMPTY) {
									  CreateMove2(SquareIndex, i);
									  if (Position[i]!=EMPTY) break;
								  } else break;
						  for (i=SquareIndex+9; Valid[i] ; i+=9)
								  if (Position[i]>=EMPTY) {
									 CreateMove2(SquareIndex, i);
									 if (Position[i]!=EMPTY) break;
								  } else break;
						  break;
			 case WQ : for (i=SquareIndex+10; i<90 ; i+=10)
								  if (Position[i]>=EMPTY) {
									  CreateMove2(SquareIndex, i);
									  if (Position[i]!=EMPTY) break;
								  } else break;
						  for (i=SquareIndex-10; i>9 ; i-=10)
								  if (Position[i]>=EMPTY) {
									  CreateMove2(SquareIndex, i);
									  if (Position[i]!=EMPTY) break;
								  } else break;
						  for (i=SquareIndex+1; Valid[i] ; i++)
								  if (Position[i]>=EMPTY) {
									  CreateMove2(SquareIndex, i);
									  if (Position[i]!=EMPTY) break;
								  } else break;
						  for (i=SquareIndex-1; Valid[i] ; i--)
								  if (Position[i]>=EMPTY) {
									 CreateMove2(SquareIndex, i);
									 if (Position[i]!=EMPTY) break;
								  } else break;
						  for (i=SquareIndex+11; Valid[i] ; i+=11)
								  if (Position[i]>=EMPTY) {
									  CreateMove2(SquareIndex, i);
									  if (Position[i]!=EMPTY) break;
								  } else break;
						  for (i=SquareIndex-11; Valid[i] ; i-=11)
								  if (Position[i]>=EMPTY) {
									  CreateMove2(SquareIndex, i);
									  if (Position[i]!=EMPTY) break;
								  } else break;
						  for (i=SquareIndex-9; Valid[i] ; i-=9)
								  if (Position[i]>=EMPTY) {
									  CreateMove2(SquareIndex, i);
									  if (Position[i]!=EMPTY) break;
								  } else break;
						  for (i=SquareIndex+9; Valid[i] ; i+=9)
								  if (Position[i]>=EMPTY) {
									 CreateMove2(SquareIndex, i);
									 if (Position[i]!=EMPTY) break;
								  } else break;
						  break;
		 }
	  }
	} else
	{
   if (Position[BLACKPAWNS]==0) return 0;
	for (x=1, SquareIndex=10, SquarePointer=Position+10; x<=8; x++, SquareIndex=x*10, SquarePointer=Position+SquareIndex)
	  for (y=1, SquarePointer++, SquareIndex++; y<=8; y++, SquarePointer++, SquareIndex++) {
		 if (*SquarePointer>EMPTY)
		 switch (*SquarePointer) {
			 case BP : if (*(SquarePointer-1)==EMPTY) {
							  CreateMove3(SquareIndex, SquareIndex-1, KING);
							  if (y==7 && *(SquarePointer-2)==EMPTY)
								  CreateMove2(SquareIndex, SquareIndex-2);
						  }
						  if (x<8 && *(SquarePointer+9)<EMPTY) CreateMove3(SquareIndex, SquareIndex+9, KING);
						  if (x>1 && *(SquarePointer-11)<EMPTY) CreateMove3(SquareIndex, SquareIndex-11, KING);
						  if (y==4 && x>1 && x-1==Position[ENPAWN]) CreateMove2(SquareIndex, SquareIndex-11);
						  if (y==4 && x<8 && x+1==Position[ENPAWN]) CreateMove2(SquareIndex, SquareIndex+9);
						  break;
			 case BK : if (x>1 && *(SquarePointer-10)<=EMPTY) CreateMove2(SquareIndex, SquareIndex-10);
						  if (x<8 && *(SquarePointer+10)<=EMPTY) CreateMove2(SquareIndex, SquareIndex+10);
						  if (y>1 && *(SquarePointer-1)<=EMPTY) CreateMove2(SquareIndex, SquareIndex-1);
						  if (y<8 && *(SquarePointer+1)<=EMPTY) CreateMove2(SquareIndex, SquareIndex+1);
						  if (x>1 && y>1 && *(SquarePointer-11)<=EMPTY) CreateMove2(SquareIndex, SquareIndex-11);
						  if (x<8 && y<8 && *(SquarePointer+11)<=EMPTY) CreateMove2(SquareIndex, SquareIndex+11);
						  if (x>1 && y<8 && *(SquarePointer-9)<=EMPTY) CreateMove2(SquareIndex, SquareIndex-9);
						  if (x<8 && y>1 && *(SquarePointer+9)<=EMPTY) CreateMove2(SquareIndex, SquareIndex+9);
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
			 case BN : if (x<8 && y>2 && *(SquarePointer+8)<=EMPTY) CreateMove2(SquareIndex, SquareIndex+8);
						  if (x>1 && y<7 && *(SquarePointer-8)<=EMPTY) CreateMove2(SquareIndex, SquareIndex-8);
						  if (x<8 && y<7 && *(SquarePointer+12)<=EMPTY) CreateMove2(SquareIndex, SquareIndex+12);
						  if (x>1 && y>2 && *(SquarePointer-12)<=EMPTY) CreateMove2(SquareIndex, SquareIndex-12);
						  if (x<7 && y>1 && *(SquarePointer+19)<=EMPTY) CreateMove2(SquareIndex, SquareIndex+19);
						  if (x>2 && y<8 && *(SquarePointer-19)<=EMPTY) CreateMove2(SquareIndex, SquareIndex-19);
						  if (x<7 && y<8 && *(SquarePointer+21)<=EMPTY) CreateMove2(SquareIndex, SquareIndex+21);
						  if (x>2 && y>1 && *(SquarePointer-21)<=EMPTY) CreateMove2(SquareIndex, SquareIndex-21);
						  break;
			 case BR : for (i=SquareIndex+10; i<90 ; i+=10)
								  if (Position[i]<=EMPTY) {
									  CreateMove2(SquareIndex, i);
									  if (Position[i]!=EMPTY) break;
								  } else break;
						  for (i=SquareIndex-10; i>9 ; i-=10)
								  if (Position[i]<=EMPTY) {
									  CreateMove2(SquareIndex, i);
									  if (Position[i]!=EMPTY) break;
								  } else break;
						  for (i=SquareIndex+1; Valid[i] ; i++)
								  if (Position[i]<=EMPTY) {
									  CreateMove2(SquareIndex, i);
									  if (Position[i]!=EMPTY) break;
								  } else break;
						  for (i=SquareIndex-1; Valid[i] ; i--)
								  if (Position[i]<=EMPTY) {
									 CreateMove2(SquareIndex, i);
									 if (Position[i]!=EMPTY) break;
								  } else break;
						  break;
			 case BB : for (i=SquareIndex+11; Valid[i] ; i+=11)
								  if (Position[i]<=EMPTY) {
									  CreateMove2(SquareIndex, i);
									  if (Position[i]!=EMPTY) break;
								  } else break;
						  for (i=SquareIndex-11; Valid[i] ; i-=11)
								  if (Position[i]<=EMPTY) {
									  CreateMove2(SquareIndex, i);
									  if (Position[i]!=EMPTY) break;
								  } else break;
						  for (i=SquareIndex-9; Valid[i] ; i-=9)
								  if (Position[i]<=EMPTY) {
									  CreateMove2(SquareIndex, i);
									  if (Position[i]!=EMPTY) break;
								  } else break;
						  for (i=SquareIndex+9; Valid[i] ; i+=9)
								  if (Position[i]<=EMPTY) {
									 CreateMove2(SquareIndex, i);
									 if (Position[i]!=EMPTY) break;
								  } else break;
						  break;
			 case BQ : for (i=SquareIndex+10; i<90 ; i+=10)
								  if (Position[i]<=EMPTY) {
									  CreateMove2(SquareIndex, i);
									  if (Position[i]!=EMPTY) break;
								  } else break;
						  for (i=SquareIndex-10; i>9 ; i-=10)
								  if (Position[i]<=EMPTY) {
									  CreateMove2(SquareIndex, i);
									  if (Position[i]!=EMPTY) break;
								  } else break;
						  for (i=SquareIndex+1; Valid[i] ; i++)
								  if (Position[i]<=EMPTY) {
									  CreateMove2(SquareIndex, i);
									  if (Position[i]!=EMPTY) break;
								  } else break;
						  for (i=SquareIndex-1; Valid[i] ; i--)
								  if (Position[i]<=EMPTY) {
									 CreateMove2(SquareIndex, i);
									 if (Position[i]!=EMPTY) break;
								  } else break;
						  for (i=SquareIndex+11; Valid[i] ; i+=11)
								  if (Position[i]<=EMPTY) {
									  CreateMove2(SquareIndex, i);
									  if (Position[i]!=EMPTY) break;
								  } else break;
						  for (i=SquareIndex-11; Valid[i] ; i-=11)
								  if (Position[i]<=EMPTY) {
									  CreateMove2(SquareIndex, i);
									  if (Position[i]!=EMPTY) break;
								  } else break;
						  for (i=SquareIndex-9; Valid[i] ; i-=9)
								  if (Position[i]<=EMPTY) {
									  CreateMove2(SquareIndex, i);
									  if (Position[i]!=EMPTY) break;
								  } else break;
						  for (i=SquareIndex+9; Valid[i] ; i+=9)
								  if (Position[i]<=EMPTY) {
									 CreateMove2(SquareIndex, i);
									 if (Position[i]!=EMPTY) break;
								  } else break;
						  break;
		 }
	  }
	}
/*************************************************************************
Sort the moves.
**************************************************************************/
//	if (Amount>1) {
//Case1Start;
/* QUICKSORT USING LIBRARY FUNCTION and user defined comparison function
	  MoveArray[0].Score=32100;
	  qsort((void *)MoveArray, Amount+1, 6, sortfunc); */

/* SELECTION SORT
	  for (i=1; i<Amount; i++) {
		  x=i;
		  for (tt=i+1; tt<=Amount; tt++) {
			  if (MoveArray[tt].Score>MoveArray[x].Score)
				  x=tt;
			  if (i!=x) {
				  TempMove=MoveArray[i];
				  MoveArray[i]=MoveArray[x];
				  MoveArray[x]=TempMove;
			  }
		  }
	  }            */

/* BUBBLE SORT, 0.2+ miliseconds on DX400
	  i=1; int exchanges=1;
	  while (i<Amount && exchanges) {
		  TempMove=MoveArray[Amount];
		  exchanges=0;
		  for (j=Amount; j>i; j--) {
			 if (MoveArray[j-1].Score>TempMove.Score) {
				 MoveArray[j]=TempMove;
				 TempMove=MoveArray[j-1];
			 } else {
				 MoveArray[j]=MoveArray[j-1];
				 exchanges=1;
			 }
		  }
		  MoveArray[i]=TempMove;
		  i++;
	  } */

/* BINARY INSERTION SORT 
	  // Top=i, Bottom=j, Middle=tt
	  for (x=2; x<=Amount; x++) {
		  TempMove=MoveArray[x];
		  j=1;
		  i=x-1;
		  while (j<=i) {
			  tt=(j+i)/2;
			  if (TempMove.Score>MoveArray[tt].Score)
					i=tt-1; else
						j=tt+1;
		  }
		  for (y=x-1; y>=j; y--)
			  MoveArray[y+1]=MoveArray[y];
//		  _fmovmem(&MoveArray[j], &MoveArray[j+1], 6*(x-j));
		  MoveArray[j]=TempMove;
	  } */
//Case1Stop;
//	}
	GMLCMCalls+=Amount;
	GetMoveListTimer+=(GetTickCount()-timeticker);
	return Amount;
}

int _pascal
TKingletSearcher::GetQuickMoveList(TPosition InPosition, TMoveArray MoveArray)
{
	GQMLCalls++;
	timeticker=GetTickCount();
	register POSITIONELEMENT* Position=InPosition;
	int i, j;
	int Square, SquareIndex;
	POSITIONELEMENT* SquarePointer;
	int x, y;
	Amount=0;
	int LastMovePiece=Position[LASTMOVESQUARE];

	if (Position[MOVER]==WHITE) {
		if (Position[WHITEPAWNS]==0) return 0;
		for (j=1; j<=Position[WHITEPAWNS]; j++) {
				Square=Position[WHITEPAWNS+j];
				SquarePointer=Position+Square;
				x=GetX(Square);
				y=GetY(Square);
				if (x<8 && *(SquarePointer+11)>EMPTY) CreateMove4(Square, Square+11, KING);
				if (x>1 && *(SquarePointer-9)>EMPTY) CreateMove4(Square, Square-9, KING);
				if (y==5 && x>1 && x-1==Position[ENPAWN]) CreateMove(Square, Square-9);
				if (y==5 && x<8 && x+1==Position[ENPAWN]) CreateMove(Square, Square+11);
				if (y>5 && *(SquarePointer+1)==EMPTY) CreateMove(Square, Square+1);
		}
		for (j=1; j<=Position[WHITEKNIGHTS]; j++) {
				Square=Position[WHITEKNIGHTS+j];
				SquarePointer=Position+Square;
				x=GetX(Square);
				y=GetY(Square);
				if (x<8 && y>2 && *(SquarePointer+8)>EMPTY) CreateMove(Square, Square+8);
				if (x>1 && y<7 && *(SquarePointer-8)>EMPTY) CreateMove(Square, Square-8);
				if (x<8 && y<7 && *(SquarePointer+12)>EMPTY) CreateMove(Square, Square+12);
				if (x>1 && y>2 && *(SquarePointer-12)>EMPTY) CreateMove(Square, Square-12);
				if (x<7 && y>1 && *(SquarePointer+19)>EMPTY) CreateMove(Square, Square+19);
				if (x>2 && y<8 && *(SquarePointer-19)>EMPTY) CreateMove(Square, Square-19);
				if (x<7 && y<8 && *(SquarePointer+21)>EMPTY) CreateMove(Square, Square+21);
				if (x>2 && y>1 && *(SquarePointer-21)>EMPTY) CreateMove(Square, Square-21);
		}
		for (j=1; j<=Position[WHITEKINGS]; j++) {
				Square=Position[WHITEKINGS+j];
				SquarePointer=Position+Square;
				x=GetX(Square);
				y=GetY(Square);
				if (x>1 && *(SquarePointer-10)>EMPTY) CreateMove(Square, Square-10);
				if (x<8 && *(SquarePointer+10)>EMPTY) CreateMove(Square, Square+10);
				if (y>1 && *(SquarePointer-1)>EMPTY) CreateMove(Square, Square-1);
				if (y<8 && *(SquarePointer+1)>EMPTY) CreateMove(Square, Square+1);
				if (x>1 && y>1 && *(SquarePointer-11)>EMPTY) CreateMove(Square, Square-11);
				if (x<8 && y<8 && *(SquarePointer+11)>EMPTY) CreateMove(Square, Square+11);
				if (x>1 && y<8 && *(SquarePointer-9)>EMPTY) CreateMove(Square, Square-9);
				if (x<8 && y>1 && *(SquarePointer+9)>EMPTY) CreateMove(Square, Square+9);
		}
		for (j=1; j<=Position[WHITEROOKS]; j++) {
				  Square=Position[WHITEROOKS+j];
						  for (i=Square+10; i<90 ; i+=10)
								  if (Position[i]>EMPTY) {
									  CreateMove(Square, i);
									  break;
								  } else if (Position[i]<EMPTY) break;
						  for (i=Square-10; i>9 ; i-=10)
								  if (Position[i]>EMPTY) {
									  CreateMove(Square, i);
									  break;
								  } else if (Position[i]<EMPTY) break;
						  for (i=Square+1; Valid[i] ; i++)
								  if (Position[i]>EMPTY) {
									  CreateMove(Square, i);
									  break;
								  } else if (Position[i]<EMPTY) break;
						  for (i=Square-1; Valid[i] ; i--)
								  if (Position[i]>EMPTY) {
									 CreateMove(Square, i);
									 break;
								  } else if (Position[i]<EMPTY) break;
		}
		for (j=1; j<=Position[WHITEBISHOPS]; j++) {
				Square=Position[WHITEBISHOPS+j];
						  for (i=Square+11; Valid[i] ; i+=11)
								  if (Position[i]>EMPTY) {
									  CreateMove(Square, i);
									  break;
								  } else if (Position[i]<EMPTY) break;
						  for (i=Square-11; Valid[i] ; i-=11)
								  if (Position[i]>EMPTY) {
									  CreateMove(Square, i);
									  break;
								  } else if (Position[i]<EMPTY) break;
						  for (i=Square-9; Valid[i] ; i-=9)
								  if (Position[i]>EMPTY) {
									  CreateMove(Square, i);
									  break;
								  } else if (Position[i]<EMPTY) break;
						  for (i=Square+9; Valid[i] ; i+=9)
								  if (Position[i]>EMPTY) {
									 CreateMove(Square, i);
									 break;
								  } else if (Position[i]<EMPTY) break;
		}
		for (j=1; j<=Position[WHITEQUEENS]; j++) {
				Square=Position[WHITEQUEENS+j];
						  for (i=Square+10; i<90 ; i+=10)
								  if (Position[i]>EMPTY) {
									  CreateMove(Square, i);
									  break;
								  } else if (Position[i]<EMPTY) break;
						  for (i=Square-10; i>9 ; i-=10)
								  if (Position[i]>EMPTY) {
									  CreateMove(Square, i);
									  break;
								  } else if (Position[i]<EMPTY) break;
						  for (i=Square+1; Valid[i] ; i++)
								  if (Position[i]>EMPTY) {
									  CreateMove(Square, i);
									  break;
								  } else if (Position[i]<EMPTY) break;
						  for (i=Square-1; Valid[i] ; i--)
								  if (Position[i]>EMPTY) {
									 CreateMove(Square, i);
									 break;
								  } else if (Position[i]<EMPTY) break;
						  for (i=Square+11; Valid[i] ; i+=11)
								  if (Position[i]>EMPTY) {
									  CreateMove(Square, i);
									  break;
								  } else if (Position[i]<EMPTY) break;
						  for (i=Square-11; Valid[i] ; i-=11)
								  if (Position[i]>EMPTY) {
									  CreateMove(Square, i);
									  break;
								  } else if (Position[i]<EMPTY) break;
						  for (i=Square-9; Valid[i] ; i-=9)
								  if (Position[i]>EMPTY) {
									  CreateMove(Square, i);
									  break;
								  } else if (Position[i]<EMPTY) break;
						  for (i=Square+9; Valid[i] ; i+=9)
								  if (Position[i]>EMPTY) {
									 CreateMove(Square, i);
									 break;
								  } else if (Position[i]<EMPTY) break;
		}
	} else {
      if (Position[BLACKPAWNS]==0) return 0;
		for (j=1; j<=Position[BLACKPAWNS]; j++) {
				Square=Position[BLACKPAWNS+j];
				SquarePointer=Position+Square;
				x=GetX(Square);
				y=GetY(Square);
				if (x<8 && *(SquarePointer+9)<EMPTY) CreateMove4(Square, Square+9, KING);
				if (x>1 && *(SquarePointer-11)<EMPTY) CreateMove4(Square, Square-11, KING);
				if (y==4 && x>1 && x-1==Position[ENPAWN]) CreateMove(Square, Square-11);
				if (y==4 && x<8 && x+1==Position[ENPAWN]) CreateMove(Square, Square+9);
				if (y<6 && *(SquarePointer-1)==EMPTY) CreateMove(Square, Square-1);
		}
		for (j=1; j<=Position[BLACKKNIGHTS]; j++) {
				Square=Position[BLACKKNIGHTS+j];
				SquarePointer=Position+Square;
				x=GetX(Square);
				y=GetY(Square);
				if (x<8 && y>2 && *(SquarePointer+8)<EMPTY) CreateMove(Square, Square+8);
				if (x>1 && y<7 && *(SquarePointer-8)<EMPTY) CreateMove(Square, Square-8);
				if (x<8 && y<7 && *(SquarePointer+12)<EMPTY) CreateMove(Square, Square+12);
				if (x>1 && y>2 && *(SquarePointer-12)<EMPTY) CreateMove(Square, Square-12);
				if (x<7 && y>1 && *(SquarePointer+19)<EMPTY) CreateMove(Square, Square+19);
				if (x>2 && y<8 && *(SquarePointer-19)<EMPTY) CreateMove(Square, Square-19);
				if (x<7 && y<8 && *(SquarePointer+21)<EMPTY) CreateMove(Square, Square+21);
				if (x>2 && y>1 && *(SquarePointer-21)<EMPTY) CreateMove(Square, Square-21);
		}
		for (j=1; j<=Position[BLACKKINGS]; j++) {
				Square=Position[BLACKKINGS+j];
				SquarePointer=Position+Square;
				x=GetX(Square);
				y=GetY(Square);
				if (x>1 && *(SquarePointer-10)<EMPTY) CreateMove(Square, Square-10);
				if (x<8 && *(SquarePointer+10)<EMPTY) CreateMove(Square, Square+10);
				if (y>1 && *(SquarePointer-1)<EMPTY) CreateMove(Square, Square-1);
				if (y<8 && *(SquarePointer+1)<EMPTY) CreateMove(Square, Square+1);
				if (x>1 && y>1 && *(SquarePointer-11)<EMPTY) CreateMove(Square, Square-11);
				if (x<8 && y<8 && *(SquarePointer+11)<EMPTY) CreateMove(Square, Square+11);
				if (x>1 && y<8 && *(SquarePointer-9)<EMPTY) CreateMove(Square, Square-9);
				if (x<8 && y>1 && *(SquarePointer+9)<EMPTY) CreateMove(Square, Square+9);
		}
		for (j=1; j<=Position[BLACKROOKS]; j++) {
			  Square=Position[BLACKROOKS+j];
			  for (i=Square+10; i<90 ; i+=10)
					if (Position[i]<EMPTY) {
						 CreateMove(Square, i);
						 break;
					} else if (Position[i]>EMPTY) break;
			  for (i=Square-10; i>9 ; i-=10)
					 if (Position[i]<EMPTY) {
						  CreateMove(Square, i);
						  break;
					 } else if (Position[i]>EMPTY) break;
			  for (i=Square+1; Valid[i] ; i++)
					 if (Position[i]<EMPTY) {
						  CreateMove(Square, i);
						  break;
					 } else if (Position[i]>EMPTY) break;
			  for (i=Square-1; Valid[i] ; i--)
					 if (Position[i]<EMPTY) {
						  CreateMove(Square, i);
						  break;
					 } else if (Position[i]>EMPTY) break;
		}
		for (j=1; j<=Position[BLACKBISHOPS]; j++) {
				Square=Position[BLACKBISHOPS+j];
						  for (i=Square+11; Valid[i] ; i+=11)
								  if (Position[i]<EMPTY) {
									  CreateMove(Square, i);
									  break;
								  } else if (Position[i]>EMPTY) break;
						  for (i=Square-11; Valid[i] ; i-=11)
								  if (Position[i]<EMPTY) {
									  CreateMove(Square, i);
									  break;
								  } else if (Position[i]>EMPTY) break;
						  for (i=Square-9; Valid[i] ; i-=9)
								  if (Position[i]<EMPTY) {
									  CreateMove(Square, i);
									  break;
								  } else if (Position[i]>EMPTY) break;
						  for (i=Square+9; Valid[i] ; i+=9)
								  if (Position[i]<EMPTY) {
									 CreateMove(Square, i);
									 break;
								  } else if (Position[i]>EMPTY) break;
		}
		for (j=1; j<=Position[BLACKQUEENS]; j++) {
				Square=Position[BLACKQUEENS+j];
						  for (i=Square+10; i<90 ; i+=10)
								  if (Position[i]<EMPTY) {
									  CreateMove(Square, i);
									  break;
								  } else if (Position[i]>EMPTY) break;
						  for (i=Square-10; i>9 ; i-=10)
								  if (Position[i]<EMPTY) {
									  CreateMove(Square, i);
									  break;
								  } else if (Position[i]>EMPTY) break;
						  for (i=Square+1; Valid[i] ; i++)
								  if (Position[i]<EMPTY) {
									  CreateMove(Square, i);
									  break;
								  } else if (Position[i]>EMPTY) break;
						  for (i=Square-1; Valid[i] ; i--)
								  if (Position[i]<EMPTY) {
									 CreateMove(Square, i);
									 break;
								  } else if (Position[i]>EMPTY) break;
						  for (i=Square+11; Valid[i] ; i+=11)
								  if (Position[i]<EMPTY) {
									  CreateMove(Square, i);
									  break;
								  } else if (Position[i]>EMPTY) break;
						  for (i=Square-11; Valid[i] ; i-=11)
								  if (Position[i]<EMPTY) {
									  CreateMove(Square, i);
									  break;
								  } else if (Position[i]>EMPTY) break;
						  for (i=Square-9; Valid[i] ; i-=9)
								  if (Position[i]<EMPTY) {
									  CreateMove(Square, i);
									  break;
								  } else if (Position[i]>EMPTY) break;
						  for (i=Square+9; Valid[i] ; i+=9)
								  if (Position[i]<EMPTY) {
									 CreateMove(Square, i);
									 break;
								  } else if (Position[i]>EMPTY) break;
		}
	}

/*************************************************************************
Assign scores to each move using the Score filed of the TMove class.
If we are at depth zero (CurrentPath.Move[0].From==0) and at least one
iteration has already been carried out (PrincipalPath.Move[0].From!=0)
then we order the moves according to the Depth0Moves scores.
**************************************************************************/
	  if (Amount>1)
	  for (i=1; i<=Amount; i++) {
		 MoveArray[i].Score=0;
		 if (MoveArray[i].To==LastMovePiece)
			MoveArray[i].Score+=10000;
		 if (Position[MoveArray[i].To]!=EMPTY) {
			MoveArray[i].Score+=(PieceValues[(Position[MoveArray[i].To])]-PieceValuesDiv10[(Position[MoveArray[i].From])]);
		 }
	  }

	if (Amount>3) {
	  int Top, Bottom, Middle;
	  for (i=2; i<=Amount; i++) {
		  TempMove=MoveArray[i];
		  Bottom=1;
		  Top=i-1;
		  while (Bottom<=Top) {
			  Middle=(Bottom+Top)/2;
			  if (TempMove.Score>MoveArray[Middle].Score)
					Top=Middle-1; else
						Bottom=Middle+1;
		  }
		  for (y=i-1; y>=Bottom; y--)
			  MoveArray[y+1]=MoveArray[y];
		  MoveArray[Bottom]=TempMove;
	  }
	} else
	if (Amount>1) {
	  if (Amount>2) {
		 if (MoveArray[3].Score>MoveArray[2].Score) {
			TempMove=MoveArray[2];
			MoveArray[2]=MoveArray[3];
			MoveArray[3]=TempMove;
		 }
	  }
	  if (MoveArray[2].Score>MoveArray[1].Score) {
			TempMove=MoveArray[1];
			MoveArray[1]=MoveArray[2];
			MoveArray[2]=TempMove;
	  }
	  if (Amount>2) {
		 if (MoveArray[3].Score>MoveArray[2].Score) {
			TempMove=MoveArray[2];
			MoveArray[2]=MoveArray[3];
			MoveArray[3]=TempMove;
		 }
	  }
	}
	GQMLCMCalls+=Amount;
	GetQuickMoveListTimer+=(GetTickCount()-timeticker);
	return Amount;
}

int
TKingletSearcher::WinnerWhenNoMoves(TPosition Position)
{
	if (Position[WHITEPAWNS]==0 || Position[BLACKPAWNS]==0) return -1;
	return 0;
}

void
TKingletSearcher::SetEvaluationParameters(int* Evals)
{
	for (int i=0; i<7; i++) {
		Evalues[i]=Evals[i];
		if (Evalues[i]>10000)
			exit(0);
	}
}

inline int pascal
TKingletSearcher::IsCheck(TPosition)
{
	return FALSE;
}

int pascal
TKingletSearcher::Evaluate(TPosition Position, int Lowest, int Highest)
{
	 static int oldtime = GetTickCount();
	 static PollCount=0;
	  if (++PollCount==500) {
		  MSG msg;
		  if (ExitCode==0 && PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE)){
					TranslateMessage(&msg);
					DispatchMessage(&msg);
		  }
		  PollCount=0;
		}
	 int white=0, black=0;

	 white+=Position[WHITEKINGS]*250;
	 black+=Position[BLACKKINGS]*250;
	 white+=Position[WHITEPAWNS]*PAWNVALUE;
    int i;
	 for (i=Position[WHITEPAWNS]+1; --i; ) {
			 white-=WhitePawnAdvance[Position[WHITEPAWNS+i]];
	 }
	 black+=Position[BLACKPAWNS]*PAWNVALUE;
	 for (i=Position[BLACKPAWNS]+1; --i; ) {
			 black-=BlackPawnAdvance[Position[BLACKPAWNS+i]];
	 }
	 for (i=Position[WHITEROOKS]+1; --i; ) {
			 white+=ROOKVALUE;
	 }
	 for (i=Position[BLACKROOKS]+1; --i; ) {
			 black+=ROOKVALUE;
	 }
	 for (i=Position[WHITEKNIGHTS]+1; --i; ) {
			 white+=KNIGHTVALUE;
	 }
	 for (i=Position[BLACKKNIGHTS]+1; --i; ) {
			 black+=KNIGHTVALUE;
			 black+=knightcontrol[Position[i]];
	 }
	 for (i=Position[WHITEBISHOPS]+1; --i; ) {
			 white+=BISHOPVALUE;
          white+=knightcontrol[Position[i]];
	 }
	 for (i=Position[BLACKBISHOPS]+1; --i; ) {
			 black+=BISHOPVALUE;
	 }
	 for (i=Position[WHITEQUEENS]+1; --i; ) {
			 white+=QUEENVALUE;
	 }
	 for (i=Position[BLACKQUEENS]+1; --i; ) {
			 black+=QUEENVALUE;
	 }

	 if (Position[MOVER]==WHITE)
		 return (int)((white-black)*0.1); else
			 return (int)((black-white)*0.1);
}


