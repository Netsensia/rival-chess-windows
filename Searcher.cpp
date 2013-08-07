#include <stdlib.h>
#include <alloc.h>
#pragma hdrstop
#include "searcher.h"
#include "debug.h"
#include "newrival.h"
#include "chessbrd.h"
#include "utility.h"
#include "shatranb.h"
#include "kingletb.h"
#include "selftbrd.h"

#define POSITIONALMAX 1800

#undef TESTWHITEPASSEDPAWNS
#undef TESTBLACKPASSEDPAWNS
#undef NOZORBRIST
#undef NORANDOM
#undef FILTERHASH
#undef TESTING
#undef TESTVERIFY
#undef TESTKINGSAFETY
#undef HASHCHECK
#undef POSITIONALEVAL
#undef QPOSITIONALEVAL
#undef TESTLOCATIONS
#undef WRITEMOVES
#undef FIRSTEVALONLY

#ifdef POSITIONALEVAL
	 int MaxPositional=-100000;
	 int MinPositional=100000;
	 int Segments[70];
	 int TPCount=1;
	 int TPTotal=0;
#endif

static int oldtime = GetTickCount();

//extern OptionStruct Options;

extern int HashSuccesses, EvaluateCalls, HashCalls, MakeMoveCalls, GMLCalls, GQMLCalls, GMLCMCalls, GQMLCMCalls, StoreHashMoveCalls;
extern int GetMoveListTimer, GetQuickMoveListTimer, MakeMoveTimer, EvaluateTimer, GetHashMoveTimer, StoreHashMoveTimer;
extern int Case1Calls, Case2Calls, Case3Calls, Case4Calls;
extern int Case1Timer, Case2Timer, Case3Timer, Case4Timer;
extern int Verbose;
extern char VERBOSE[MAXFILENAMELENGTH];
extern char PV[MAXFILENAMELENGTH];
extern char PV2[MAXFILENAMELENGTH];
int timeticker2, timeticker;

extern char command[100];
extern long globalstarttime;
extern int extendmaxmovetime, normalmovetime, searchmethod;
extern int globalmeansearchtime, globalsearchtimeleft;
extern int WinBoard, Pondering, showAnalysis;

unsigned int i, y, a;
unsigned int Square;
unsigned int wnp, bnp;
POSITIONELEMENT *p;
POSITIONELEMENT *Wpf;
POSITIONELEMENT *Bpf;
POSITIONELEMENT *Wpfi, *Bpfi;
int WhiteScore, BlackScore;

TSearcher::TSearcher(int CommandLineHashSize)
{
	PieceValues[PAWN+100]=PieceValues[PAWN]=PAWNVALUE/10;
	PieceValues[KNIGHT+100]=PieceValues[KNIGHT]=KNIGHTVALUE/10;
	PieceValues[BISHOP+100]=PieceValues[BISHOP]=BISHOPVALUE/10;
	PieceValues[ROOK+100]=PieceValues[ROOK]=ROOKVALUE/10;
	PieceValues[QUEEN+100]=PieceValues[QUEEN]=QUEENVALUE/10;
	PieceValuesDiv10[PAWN+100]=PieceValuesDiv10[PAWN]=PieceValues[PAWN]/10;
	PieceValuesDiv10[KNIGHT+100]=PieceValuesDiv10[KNIGHT]=PieceValues[KNIGHT]/10;
	PieceValuesDiv10[BISHOP+100]=PieceValuesDiv10[BISHOP]=PieceValues[BISHOP]/10;
	PieceValuesDiv10[ROOK+100]=PieceValuesDiv10[ROOK]=PieceValues[ROOK]/10;
	PieceValuesDiv10[QUEEN+100]=PieceValuesDiv10[QUEEN]=PieceValues[QUEEN]/10;

	StopMove.From=0;
   StopScore=-32100;
	MaxDepth=50;
	MaxQuiesceDepth=50;
	LowEval=LOWEVAL;
	HighEval=HIGHEVAL;
	UseRecaptureExtensions=TRUE;
	UseCheckExtensions=TRUE;
	UsePawnPushExtensions=TRUE;
	UseSingleReplyExtensions=TRUE;
	PrintPV=FALSE;
	NullMoveReduceDepth=1;
   PrincipalPath.Move[0].From=0;
	MinimumMate=9000;
	LastMoveNumber=0;
	// Why do I need this line?  I really don't know!  But it produces BIZARRE results otherwise
	long HashMoveSize=HASHMOVESIZE;
	long ByteSize=CommandLineHashSize*1024;
	long j=1;
	for (long i=1; i<31; i++) {
		j*=2;
		if (j*HashMoveSize>=ByteSize) {
			HashPower=i;
			break;
		}
	}
	NumberOfHashEntries=ByteSize/HashMoveSize;
	if (NumberOfHashEntries<1) {
		NumberOfHashEntries=1;
		HashPower=1;
	}
	HashTable = new THashMove[NumberOfHashEntries];
	if (Verbose) {
		FILE* f = fopen( VERBOSE, "a" );
		fprintf( f, "Requested Hash Size: %li\n", ByteSize);
		fprintf( f, "Hash Power: %li 2^%li=%li (*%i)=%li\n", HashPower, HashPower, j, (long)HashMoveSize, (long)j*(long)HashMoveSize);
		fprintf( f, "Number of Hash Entries: %li\n", NumberOfHashEntries);
		fprintf( f, "Hash Size: %li*%li=%li\n", NumberOfHashEntries, HashMoveSize, NumberOfHashEntries*(long)HashMoveSize);
		fclose(f);
	}
}

TSearcher::~TSearcher()
{
	delete[] HashTable;
}

void
TSearcher::GeneratePieceValues()
{
#ifdef NORANDOM
	srand(1);
#endif
#ifdef HASHTABLE
	int HashPieceSquareValueMax=CalculateHashSize(HashPower);
	int i, j;
	for (i=0; i<89; i++)
		for (j=0; j<12; j++) {
			PieceSquareValues[i][j]=random(HashPieceSquareValueMax);
		}
	ClearHashTable();
#endif
	// These values are used to generate the lock key for a hash table entry
	for (i=0; i<89; i++)
		for (j=0; j<12; j++) {
			LockPieceSquareValues[i][j]=random(2147483647L);
		}
}

void
TSearcher::SetMaxDepth(int NewMaxDepth)
{
	MaxDepth=NewMaxDepth;
}

int
TSearcher::CalculateHashSize(int hashpower) {
	int rv = 1;
	for (int i=0; i<hashpower; i++) {
		rv*=2;
	}
	return rv;
}

void _pascal
TSearcher::StoreHashMoveDetails(TPosition Position, int Height, int Flag, TMove Move, int Value)
{
	StoreHashMoveCalls++;
	timeticker=GetTickCount();
#ifdef NOZORBRIST
	int HashKey=ComputeHashKey(Position);
#else
#ifdef CHARPOSITION
	int HashKey=Position[HASHKEYFIELD1]*HASHDIV3+Position[HASHKEYFIELD2]*HASHDIV2+Position[HASHKEYFIELD3]*HASHDIV1+Position[HASHKEYFIELD4];
#else
	int HashKey=Position[HASHKEYFIELD1];
#endif
#endif
	if (HashKey>=NumberOfHashEntries) HashKey=HashKey-NumberOfHashEntries;
	 THashMove* HashLocation=HashTable+HashKey;
#ifdef FILTERHASH
	 if (HashLocation->Depth<Height || HashLocation->Stale ||
			(HashLocation->Depth==Height && HashLocation->Flag!=VALID &&
				(Flag!=UBOUND || HashLocation->Depth==UBOUND))
#else
	 if (HashLocation->Depth<=Height || HashLocation->Stale
#endif
		 ) {
		 HashLocation->From=Move.From;
		 HashLocation->To=Move.To;
		 HashLocation->Lock=EncodePosition(Position);
		 HashLocation->Depth=Height;
		 HashLocation->Value=Value;
		 HashLocation->Flag=Flag;
		 HashLocation->Stale=FALSE;
	 }
	 StoreHashMoveTimer+=(GetTickCount()-timeticker);

}

void _pascal
TSearcher::GetHashMoveDetails(TPosition Position, int& Height, int& Flag, TMove& Move, int& Value)
{
	HashCalls++;
	timeticker=GetTickCount();
#ifdef NOZORBRIST
	long HashKey=ComputeHashKey(Position);
#else
#ifdef CHARPOSITION
	long HashKey=Position[HASHKEYFIELD1]*HASHDIV3+Position[HASHKEYFIELD2]*HASHDIV2+Position[HASHKEYFIELD3]*HASHDIV1+Position[HASHKEYFIELD4];
#else
	long HashKey=Position[HASHKEYFIELD1];
#endif
#endif
	if (HashKey>=NumberOfHashEntries) HashKey=HashKey-NumberOfHashEntries;
	LockStruct TempLock = EncodePosition(Position);
	if (HashTable[HashKey].Lock.Lock==TempLock.Lock) {
//	if (memcmp((const void *)&HashTable[HashKey].Lock, (const void *)&TempLock, SIZEOFLOCKSTRUCT)==0) {
	  Height=HashTable[HashKey].Depth;
	  Flag=HashTable[HashKey].Flag;
	  Value=HashTable[HashKey].Value;
	  Move.From=HashTable[HashKey].From;
	  Move.To=HashTable[HashKey].To;
	  HashTable[HashKey].Stale=FALSE;
	} else {
	  Height=0;
	}
	GetHashMoveTimer+=(GetTickCount()-timeticker);
}

int
TSearcher::ComputeHashKey(TPosition Position)
{
Case1Start;
	long HashKey=0;
	int x, y, SqIndex, Piece;
	for (x=9; --x; )
	  for (y=9; --y; ) {
		  if (Position[x*10+y]!=EMPTY) {
			 SqIndex=x*10+y;
			 Piece=Position[SqIndex];
			 if (Piece>100) Piece-=95; else Piece-=1;
			 HashKey^=(PieceSquareValues[SqIndex][Piece]);
		  }
	  }
Case1Stop;
	if (Position[MOVER]==WHITE)
		HashKey^=(PieceSquareValues[MOVER][WHITE]);
	else
		HashKey^=(PieceSquareValues[MOVER][BLACK]);
	return HashKey;
}

LockStruct
TSearcher::EncodePosition(TPosition Position)
{
	LockStruct RetLock;
	unsigned long HashKey=0;
	int x, y, SqIndex, Piece;
	for (x=1; x<=8; x++)
	  for (y=1; y<=8; y++) {
		  SqIndex=x*10+y;
		  Piece=Position[SqIndex];
		  if (Piece!=EMPTY) {
			 if (Piece>100) Piece-=95; else Piece-=1;
				HashKey^=(LockPieceSquareValues[SqIndex][Piece]);
		  }
	  }
	HashKey^=(LockPieceSquareValues[MOVER][Position[MOVER]]);
	HashKey^=(LockPieceSquareValues[WROOK1MOVED][Position[WROOK1MOVED]]);
	HashKey^=(LockPieceSquareValues[WROOK8MOVED][Position[WROOK8MOVED]]);
	HashKey^=(LockPieceSquareValues[BROOK1MOVED][Position[BROOK1MOVED]]);
	HashKey^=(LockPieceSquareValues[BROOK8MOVED][Position[BROOK8MOVED]]);
	HashKey^=(LockPieceSquareValues[WKINGMOVED][Position[WKINGMOVED]]);
	HashKey^=(LockPieceSquareValues[BKINGMOVED][Position[BKINGMOVED]]);
	HashKey^=(LockPieceSquareValues[ENPAWN][Position[ENPAWN]]);
	RetLock.Lock=HashKey;

	return RetLock;
}

void
TSearcher::ClearHashTable()
{
	int i;
	for (i=NumberOfHashEntries; i--;) {
		HashTable[i].Depth=0;
		HashTable[i].From=0;
		HashTable[i].To=0;
		HashTable[i].Value=0;
		HashTable[i].Flag=0;
		HashTable[i].Stale=FALSE;
		HashTable[i].Lock.Lock=0;
	}
}

void
TSearcher::StaleHashTable()
{
	int i;
	for (i=NumberOfHashEntries; i--; ) {
		HashTable[i].Stale=TRUE;
	}
}

void
TSearcher::ClearHistory()
{
	for (int i=89; i--; ) {
		for (int j=89; j--; ) {
			History[i][j]=0;
		}
	}
}

void
TSearcher::SetMaxQuiesceDepth(int NewQuiesceDepth)
{
	MaxQuiesceDepth=NewQuiesceDepth;
	if (MaxQuiesceDepth+MaxDepth>(MAXDEPTH-1))
      MaxQuiesceDepth=(MAXDEPTH-1)-MaxDepth;
}

int
TSearcher::GetCurrentPathNodes()
{
	return CurrentPathNodes;
}

TPath
TSearcher::GetPrincipalPath()
{
	PrincipalPath.Nodes=CurrentPathNodes;
	return PrincipalPath;
}

void
TSearcher::ExitWithoutMove()
{
	ExitCode=1;
}

void
TSearcher::ExitWithMove()
{
/*************************************************************************
Only exit with move if a move has been found.
**************************************************************************/
	if (FinalDepth>2 && PrincipalPath.Move[0].From!=0) ExitCode=2;
}

TPath
TSearcher::Search()
{
	if (PrintPV) {
		TChessBoard* PathGame;
		switch (VariantID) {
			case CHESS : PathGame=new TChessBoard(PathBoard); break;
			case SHATRANJ : PathGame=new TShatranjBoard(PathBoard); break;
			case KINGLET : PathGame=new TKingletBoard(PathBoard); break;
			case SELFTAKE : PathGame=new TSelfTakeBoard(PathBoard); break;
		}
		char Fen[MAXFEN];
		PathGame->GetFEN(Fen);
		if (Pondering) {
			writeMessage(PV, "********* Starting ponder *********");
			writeMessage(PV2, "<BR>********* Staring ponder *********");
		} else {
			writeMessage(PV, "********* Starting search *********");
			writeMessage(PV, Fen);
			writeMessage(PV, "**********************************");
			char s[200];
			sprintf(s, "<P STYLE=\"color:FFFF00\">FEN: %s</P>", Fen);
			writeMessage(PV2, s);
		}
		delete PathGame;
	}
	TPath NewPath;
	int i;
	ExitCode=0;
	ClearHistory();
	PrincipalPath.Move[0].From=0; // In case of interuption when move is required, signals no move available
	CurrentPathNodes=1;
	GlobalHashMove.From=0;

	RootMaterial=
		  StartPosition[WHITEQUEENS]*QUEENVALUE+
		  StartPosition[WHITEBISHOPS]*BISHOPVALUE+
		  StartPosition[WHITEKNIGHTS]*KNIGHTVALUE+
		  StartPosition[WHITEROOKS]*ROOKVALUE+
		  StartPosition[WHITEPAWNS]*PAWNVALUE-
		  StartPosition[BLACKQUEENS]*QUEENVALUE-
		  StartPosition[BLACKBISHOPS]*BISHOPVALUE-
		  StartPosition[BLACKKNIGHTS]*KNIGHTVALUE-
		  StartPosition[BLACKROOKS]*ROOKVALUE-
		  StartPosition[BLACKPAWNS]*PAWNVALUE;

	ValueGuess=Evaluate(StartPosition, LOWEVAL, HIGHEVAL);
#ifdef FIRSTEVALONLY
	ExitWithoutMove();
#endif
	CurrentDepth=0;
	Depth0Amount=GetMoveList(StartPosition, Moves[0]);

	for (i=1; i<=Depth0Amount; i++) {
		Moves[0][i].Score=LowEval;
	}
#ifdef WRITEMOVES
	TMove TempMove;
	i=1; int exchanges=1;
	int j;
	while (i<Depth0Amount && exchanges) {
		  TempMove=Moves[0][Depth0Amount];
		  exchanges=0;
		  for (j=Depth0Amount; j>i; j--) {
			 if (StartPosition[Moves[0][j-1].From]>StartPosition[TempMove.From]) {
				 Moves[0][j]=TempMove;
				 TempMove=Moves[0][j-1];
			 } else {
				 Moves[0][j]=Moves[0][j-1];
				 exchanges=1;
			 }
		  }
		  Moves[0][i]=TempMove;
		  i++;
	}
	FILE*f=fopen("c:\\log.txt", "w");
	fprintf( f, "%i: ", Depth0Amount );
	char fromPiece;

	for (i=1; i<=Depth0Amount; i++) {
		switch (StartPosition[Moves[0][i].From]%100) {
			case KING  : fromPiece = 'K'; break;
			case QUEEN : fromPiece = 'Q'; break;
			case BISHOP : fromPiece = 'B'; break;
			case KNIGHT : fromPiece = 'N'; break;
			case ROOK : fromPiece = 'R'; break;
			case PAWN : fromPiece = 'P'; break;
		}
		fprintf( f, "\n%c%c%c%c%c",
			fromPiece,
			(char)((Moves[0][i].From/10)+'a'-1),
			(char)((Moves[0][i].From%10)+'0'),
			(char)((Moves[0][i].To/10)+'a'-1),
			(char)((Moves[0][i].To%10)+'0'));
	}
	fclose(f);
#endif

	PIBest=LowEval;
	Satisfied=TRUE;

	int c=0;
	TPosition NewPosition;
	for (i=1; i<=Depth0Amount; i++) {
	  if (MakeMove(StartPosition, Moves[0][i], NewPosition)) {
		 c++;
	  }
	}

	CurrentDepthZeroMove.From=0;
	FinalQuiesceDepth=MaxQuiesceDepth;
	int LowAspire, HighAspire;
	FinalDepth=1;
	MinimaxZero(StartPosition, 0, LOWEVAL, HIGHEVAL, &NewPath, 0);
	ReOrderMoves(Moves[0], Depth0Amount);
	LowAspire=-NewPath.Value-(AspireWindow/2);
	HighAspire=-NewPath.Value+(AspireWindow/2);

	CurrentPathNodes=1;
	if (c>1)
	for (FinalDepth=1; FinalDepth<=MaxDepth; FinalDepth++) {
#ifdef POSITIONALEVAL
FILE* f=fopen( "c:\\log.txt", "a" );
fprintf( f, "-----------------------------------------------------------------------------\n" );
fprintf( f, "TP %i, TPCount %i, TPAverage %i Max Positional: %i, Min Positional: %i\n", TPTotal, TPCount, TPTotal/TPCount, MaxPositional, MinPositional);
fprintf( f, "-----------------------------------------------------------------------------\n" );
for (i=0; i<=60; i++) {
	fprintf( f, "%i\t", (i-30)*100 );
}
fprintf( f, "\n" );
for (i=0; i<=60; i++) {
	fprintf( f, "%i\t", Segments[i] );
}
fprintf( f, "\n-----------------------------------------------------------------------------\n" );
fclose(f);
#endif
		CurrentDepthZeroMove.From=0;
		followPV=(FinalDepth>1);
		if (MinimumMate>9000 && (10000-MinimumMate-FinalDepth-2)<MaxExtensions) {
			MaxExtensions=(10000-MinimumMate-FinalDepth-2);
			if (MaxExtensions<0) MaxExtensions=0;
			if (Verbose) writeMessage(VERBOSE, "Setting MaxExtensions due to minimum mate", MaxExtensions);
		}
		MinimaxZero(StartPosition, 0, LowAspire, HighAspire, &NewPath, 0);
		if (NewPath.Value<=LowAspire) {
			CurrentDepthZeroMove.From=0;
			followPV=(FinalDepth>1);
			MinimaxZero(StartPosition, 0, LowEval, HighAspire, &NewPath, 0);
		} else
		if (NewPath.Value>=HighAspire) {
			CurrentDepthZeroMove.From=0;
			followPV=(FinalDepth>1);
			MinimaxZero(StartPosition, 0, LowAspire, HighEval, &NewPath, 0);
		}
		HighAspire = NewPath.Value + (AspireWindow/2);
		LowAspire = NewPath.Value - (AspireWindow/2);
		NewPath.Nodes=CurrentPathNodes;
		if (ExitCode==1) NewPath.Move[0].From=0; // Exit with no move required
		if (ExitCode==2) {
			// interupted but move required
			if (PrincipalPath.Value>MinimumMate) {
				if (Verbose) writeMessage(VERBOSE, "Setting Minimum Mate and exiting", PrincipalPath.Value);
				MinimumMate=PrincipalPath.Value;
			}
			return PrincipalPath;
		}
		if (NewPath.Move[0].From==0) return NewPath; // no move available
		memcpy(&PrincipalPath, &NewPath, SIZEOFPATH);
		PrincipalPath.Depth=FinalDepth;
		PIBest=PrincipalPath.Value;
		Satisfied=(PIBest>ValueGuess-SATISFACTION);
		ReOrderMoves(Moves[0], Depth0Amount);
	}
	if (c==0) PrincipalPath.Nodes=0;
	return PrincipalPath;
}

TPath
TSearcher::SolveSearch()
{
}

int
TSearcher::UseAllMovesOnQuiesce(TPosition Position)
{
	return FALSE;
}

void _pascal
TSearcher::Quiesce(TPosition Position, unsigned int Depth, int Lowest, int Highest, TPath* Path, unsigned int ExtendDepth)
{
#ifdef QPOSITIONALEVAL
	  int Quiet=TRUE;
	  static int QuietCount=0;
	  int Flag=0;
	  int OrigLowest=Lowest, OrigHighest=Highest;
#endif
	  int All=FALSE;
	  int Legal=TRUE;
	  TMove* move;
	  TPath NewPath;
	  TPath BestPath;
	  if (Depth<(FinalDepth+ExtendDepth)+FinalQuiesceDepth) {
			All=((FinalDepth+ExtendDepth)!=Depth) && UseAllMovesOnQuiesce(Position);
	  }
	  if (!All) {
			Path->Value=Evaluate(Position, Lowest, Highest);
			Path->Move[Depth].From=0;
			if (Path->Value>=Highest || Depth>=FinalDepth+ExtendDepth+FinalQuiesceDepth) {
#ifdef QPOSITIONALEVAL
			  Flag = 1;
			  BestPath.Value=Path->Value;
			  goto done;
#else
			  return;
#endif
			}
	  } else {
			Path->Value=-10000;
			Legal=FALSE;
	  }
	  int NumberOfMoves;
	  if (All) {
			CurrentDepth=Depth;
			NumberOfMoves=GetMoveList(Position, Moves[Depth]);
	  } else {
			NumberOfMoves=GetQuickMoveList(Position, Moves[Depth]);
	  }
/*************************************************************************
Effectively make a null-move.  If we imagine that all we do is swap the
side to move, then we would value this move as -Evaluate(Position).
To achieve the same result, we just take the value Evaluate(Position) as
the value of the null-move.  If our capture tree cannot find a move that
beats this value then we keep it as the value of the node.  This is
because the capture tree does not consider all moves and so may make us
believe that a position is bad when infact we may have achieved a better
score for the position by making a move other than those considered in
the capture tree.  This 'other move' is represented here as a null-move.
We then generate our set of capture moves.

If merit is not good enough to cause a cutoff then the null move is
represented by BestPath.Move[Depth].  From being set to 0 before the
consideration of all the generated moves.  If this is still 0 at the
end then this null move is assigned to the path at this depth and a
cutoff will occur at the next level up because no move will have been
good enough to increase Lowest (alpha).
**************************************************************************/
	  BestPath.Value=Path->Value;
	  BestPath.Move[Depth].From=0;
	  if (BestPath.Value>Lowest) Lowest=BestPath.Value;
	  //int InitialLowest=Lowest;
	  for (move=&Moves[Depth][1]; move<=&Moves[Depth][NumberOfMoves] && !ExitCode; move++) {
			//if (!All && Path->Value+(GetFile[PieceValues[Position[move->To]]]-BASEPIECEVALUE)+POSITIONALMAX>=InitialLowest)
			if (MakeMove(Position, *move, NewPosition[Depth])) {
			  Legal=TRUE;
			  Quiesce(NewPosition[Depth], Depth+1, -Highest, -Lowest, &NewPath, ExtendDepth);
			  ReverseNewPathValue;
			  if (NewPath.Value>=Highest) {
				  memcpy(Path, &NewPath, SIZEOFPATH);
				  Path->Move[Depth]=*move;
#ifdef QPOSITIONALEVAL
				  Flag = 2;
				  BestPath.Value=NewPath.Value;
				  goto done;
#else
				  return;
#endif
			  }
			  if (NewPath.Value>BestPath.Value) {
#ifdef QPOSITIONALEVAL
				  Quiet=FALSE;
#endif
				  memcpy(&BestPath, &NewPath, SIZEOFPATH);
				  BestPath.Move[Depth]=*move;
				  if (NewPath.Value>Lowest) Lowest=NewPath.Value;
			  }
			} /* MakeMove */
	  } /* for */
	  if (!Legal && !ExitCode)
			if (All)
				Path->Value=-10000;
			else
				Path->Value=0;
	  else
	  memcpy(Path, &BestPath, SIZEOFPATH);
done:
#ifdef QPOSITIONALEVAL
	QuietCount++;
	if (/*!Quiet &&*/ QuietCount>0) {
		QuietCount=0;
		char s[500];
		int p[89];
		for (int x=0; x<89; x++) {
			p[x]=Position[x];
		}
		TChessBoard* a = new TChessBoard(p);
		a->GetFEN(s);
		FILE* f=fopen("c:\\log.txt", "a");
		fprintf(f, "%s S %i\t L %i\t H %i\t D %i\t F %i\t E %i\t Q %i\t T %i\t FEN: %s\n",
			(Flag==0 ? "Straight" : (Flag==1 ? "Eval Cut" : "Move Cut")), BestPath.Value,
			OrigLowest, OrigHighest,
			Depth, FinalDepth, ExtendDepth, FinalQuiesceDepth, Depth+ExtendDepth+FinalQuiesceDepth, s );
		fclose(f);
		delete a;
	}
#endif
}

int TSearcher::VerifyMove(TPosition Position, TMove Move)
{
	int Legal=TRUE;
	int Temp;
	TPosition NewPosition;
	memcpy(NewPosition, Position, SIZEOFPOSITION);
	if (NewPosition[MOVER]==WHITE && NewPosition[Move.From]>=EMPTY) Legal=FALSE;
	if (NewPosition[MOVER]==BLACK && NewPosition[Move.From]<=EMPTY) Legal=FALSE;
	if (Legal) {
		switch (NewPosition[Move.From]) {
			case WP : Temp=Move.To-Move.From;
						 if (Temp==1 && Temp==-9 && Temp==11) Legal=FALSE;
						 if (GetY(Move.To)==8 && Move.PromotionPiece==EMPTY) Legal=FALSE;
						 break;
			case BP : Temp=Move.To-Move.From;
						 if (Temp==-1 && Temp==9 && Temp==-11) Legal=FALSE;
						 if (GetY(Move.To)==1 && Move.PromotionPiece==EMPTY) Legal=FALSE;
						 break;
			case WR : Legal=FALSE;
						 for (Temp=Move.From+10; Valid[Temp] && Position[Temp]>=EMPTY; Temp+=10) if (Temp==Move.To) Legal=TRUE;
						 for (Temp=Move.From-10; Valid[Temp] && Position[Temp]>=EMPTY; Temp-=10) if (Temp==Move.To) Legal=TRUE;
						 for (Temp=Move.From+1; Valid[Temp] && Position[Temp]>=EMPTY; Temp++) if (Temp==Move.To) Legal=TRUE;
						 for (Temp=Move.From-1; Valid[Temp] && Position[Temp]>=EMPTY; Temp--) if (Temp==Move.To) Legal=TRUE;
						 break;
			case BR : Legal=FALSE;
						 for (Temp=Move.From+10; Valid[Temp] && Position[Temp]<=EMPTY; Temp+=10) if (Temp==Move.To) Legal=TRUE;
						 for (Temp=Move.From-10; Valid[Temp] && Position[Temp]<=EMPTY; Temp-=10) if (Temp==Move.To) Legal=TRUE;
						 for (Temp=Move.From+1; Valid[Temp] && Position[Temp]<=EMPTY; Temp++) if (Temp==Move.To) Legal=TRUE;
						 for (Temp=Move.From-1; Valid[Temp] && Position[Temp]<=EMPTY; Temp--) if (Temp==Move.To) Legal=TRUE;
						 break;
			case WB : Legal=FALSE;
						 for (Temp=Move.From+9; Valid[Temp] && Position[Temp]>=EMPTY; Temp+=9) if (Temp==Move.To) Legal=TRUE;
						 for (Temp=Move.From-9; Valid[Temp] && Position[Temp]>=EMPTY; Temp-=9) if (Temp==Move.To) Legal=TRUE;
						 for (Temp=Move.From+11; Valid[Temp] && Position[Temp]>=EMPTY; Temp+=11) if (Temp==Move.To) Legal=TRUE;
						 for (Temp=Move.From-11; Valid[Temp] && Position[Temp]>=EMPTY; Temp-=11) if (Temp==Move.To) Legal=TRUE;
						 break;
			case BB : Legal=FALSE;
						 for (Temp=Move.From+9; Valid[Temp] && Position[Temp]<=EMPTY; Temp+=9) if (Temp==Move.To) Legal=TRUE;
						 for (Temp=Move.From-9; Valid[Temp] && Position[Temp]<=EMPTY; Temp-=9) if (Temp==Move.To) Legal=TRUE;
						 for (Temp=Move.From+11; Valid[Temp] && Position[Temp]<=EMPTY; Temp+=11) if (Temp==Move.To) Legal=TRUE;
						 for (Temp=Move.From-11; Valid[Temp] && Position[Temp]<=EMPTY; Temp-=11) if (Temp==Move.To) Legal=TRUE;
						 break;
			case WQ : Legal=FALSE;
						 for (Temp=Move.From+10; Valid[Temp] && Position[Temp]>=EMPTY; Temp+=10) if (Temp==Move.To) Legal=TRUE;
						 for (Temp=Move.From-10; Valid[Temp] && Position[Temp]>=EMPTY; Temp-=10) if (Temp==Move.To) Legal=TRUE;
						 for (Temp=Move.From+1; Valid[Temp] && Position[Temp]>=EMPTY; Temp++) if (Temp==Move.To) Legal=TRUE;
						 for (Temp=Move.From-1; Valid[Temp] && Position[Temp]>=EMPTY; Temp--) if (Temp==Move.To) Legal=TRUE;
						 for (Temp=Move.From+9; Valid[Temp] && Position[Temp]>=EMPTY; Temp+=9) if (Temp==Move.To) Legal=TRUE;
						 for (Temp=Move.From-9; Valid[Temp] && Position[Temp]>=EMPTY; Temp-=9) if (Temp==Move.To) Legal=TRUE;
						 for (Temp=Move.From+11; Valid[Temp] && Position[Temp]>=EMPTY; Temp+=11) if (Temp==Move.To) Legal=TRUE;
						 for (Temp=Move.From-11; Valid[Temp] && Position[Temp]>=EMPTY; Temp-=11) if (Temp==Move.To) Legal=TRUE;
						 break;
			case BQ : Legal=FALSE;
						 for (Temp=Move.From+10; Valid[Temp] && Position[Temp]<=EMPTY; Temp+=10) if (Temp==Move.To) Legal=TRUE;
						 for (Temp=Move.From-10; Valid[Temp] && Position[Temp]<=EMPTY; Temp-=10) if (Temp==Move.To) Legal=TRUE;
						 for (Temp=Move.From+1; Valid[Temp] && Position[Temp]<=EMPTY; Temp++) if (Temp==Move.To) Legal=TRUE;
						 for (Temp=Move.From-1; Valid[Temp] && Position[Temp]<=EMPTY; Temp--) if (Temp==Move.To) Legal=TRUE;
						 for (Temp=Move.From+9; Valid[Temp] && Position[Temp]<=EMPTY; Temp+=9) if (Temp==Move.To) Legal=TRUE;
						 for (Temp=Move.From-9; Valid[Temp] && Position[Temp]<=EMPTY; Temp-=9) if (Temp==Move.To) Legal=TRUE;
						 for (Temp=Move.From+11; Valid[Temp] && Position[Temp]<=EMPTY; Temp+=11) if (Temp==Move.To) Legal=TRUE;
						 for (Temp=Move.From-11; Valid[Temp] && Position[Temp]<=EMPTY; Temp-=11) if (Temp==Move.To) Legal=TRUE;
						 break;
			case WN : Temp=abs(Move.From-Move.To);
						 if (Temp!=12 && Temp!=21 && Temp!=8 && Temp!=19) Legal=FALSE;
						 break;
			case BN : Temp=abs(Move.From-Move.To);
						 if (Temp!=12 && Temp!=21 && Temp!=8 && Temp!=19) Legal=FALSE;
						 break;
			case WK : Temp=abs(Move.From-Move.To);
						 if (Temp!=1 && Temp!=11 && Temp!=10 && Temp!=9 && !(Move.From==51 && (Move.To==31 || Move.To==71))) Legal=FALSE;
						 break;
			case BK : Temp=abs(Move.From-Move.To);
						 if (Temp!=1 && Temp!=11 && Temp!=10 && Temp!=9 && !(Move.From==58 && (Move.To==38 || Move.To==78))) Legal=FALSE;
						 break;
		}
	}
/*************************************************************************
Make move and check for check
**************************************************************************/
	NewPosition[Move.To]=NewPosition[Move.From];
	if (NewPosition[Move.To]==WP) {
		if (GetY(Move.To)==6 && GetX(Move.To)==NewPosition[ENPAWN]) NewPosition[Move.To-1]=EMPTY;
		if (GetY(Move.To)==8) NewPosition[Move.To]=Move.PromotionPiece;
	}
	if (NewPosition[Move.To]==BP) {
		if (GetY(Move.To)==3 && GetX(Move.To)==NewPosition[ENPAWN]) NewPosition[Move.To+1]=EMPTY;
		if (GetY(Move.To)==1) NewPosition[Move.To]=Move.PromotionPiece+100;
	}
	if (NewPosition[Move.From]==WK) {
		NewPosition[WKINGPOSITION]=Move.To;
		if (Move.From==51 && Move.To==71) {
			NewPosition[71]=WK;
			NewPosition[61]=WR;
			NewPosition[81]=EMPTY;
		}
		if (Move.From==51 && Move.To==31) {
			NewPosition[31]=WK;
			NewPosition[41]=WR;
			NewPosition[11]=EMPTY;
		}
	}
	if (NewPosition[Move.From]==BK) {
		NewPosition[BKINGPOSITION]=Move.To;
		if (Move.From==58 && Move.To==78) {
			NewPosition[78]=BK;
			NewPosition[68]=BR;
			NewPosition[88]=EMPTY;
		}
		if (Move.From==58 && Move.To==38) {
			NewPosition[38]=BK;
			NewPosition[48]=BR;
			NewPosition[18]=EMPTY;
		}
	}
	NewPosition[Move.From]=EMPTY;
	if (IsCheck(NewPosition)) Legal=FALSE;
#ifdef TESTVERIFY
	if (!Legal) {
		FILE* f=fopen("c:\\log.txt", "a");
		char s[500];
		int p[89];
		for (int x=0; x<89; x++) {
			p[x]=Position[x];
		}
		TChessBoard* a = new TChessBoard(p);
		a->GetFEN(s);
		fprintf(f, "%s Move: %i-%i, Temp %i Check %i Promote %i\n", s, Move.From, Move.To, Temp, IsCheck(Position), Move.PromotionPiece);
		for (x=0; x<89; x++) {
			p[x]=NewPosition[x];
		}
		delete a;
		a = new TChessBoard(p);
		a->GetFEN(s);
		fprintf(f, "%s Check %i\n", s, IsCheck(NewPosition));
		delete a;
		fclose(f);
	}
#endif
	return Legal;
}

void
TSearcher::TranslateMove(TMove mv, char* s)
{
			sprintf(s, "%c%c%c%c ",
					(char)(GetFile[mv.From]+'a'-1),
					(char)(GetRank[mv.From]+'0'),
					(char)(GetFile[mv.To]+'a'-1),
					(char)(GetRank[mv.To]+'0'));
}

void _pascal
TSearcher::Minimax(TPosition Position, unsigned int Depth, int Lowest, int Highest, TPath* Path, unsigned int LocalFinalDepth)
{
	if (ExitCode!=0) return;
	int ExtendDepth = (LocalFinalDepth-FinalDepth);
/*************************************************************************
Do we want to extend?
**************************************************************************/
	if (ExtendDepth<MaxExtensions) {
		if (UsePawnPushExtensions &&
				((	Position[LastMove[Depth-1].To]==WP &&
					GetY(LastMove[Depth-1].To)>=6 &&
					GetY(LastMove[Depth-1].To)<8
				 ) ||
				 ( Position[LastMove[Depth-1].To]==BP &&
					GetY(LastMove[Depth-1].To)<=3 &&
					GetY(LastMove[Depth-1].To)>1
				 )
				)
			 ) ExtendDepth++; else
		if (UseRecaptureExtensions && (LastMove[Depth-1].From==LastMove[Depth-2].From && LastMove[Depth-1].To==LastMove[Depth-2].To &&
					 (Position[WHITEQUEENS]*QUEENVALUE+Position[WHITEBISHOPS]*BISHOPVALUE+
					  Position[WHITEKNIGHTS]*KNIGHTVALUE+Position[WHITEROOKS]*ROOKVALUE+
					  Position[WHITEPAWNS]*PAWNVALUE-Position[BLACKQUEENS]*QUEENVALUE-
					  Position[BLACKBISHOPS]*BISHOPVALUE-Position[BLACKKNIGHTS]*KNIGHTVALUE-
					  Position[BLACKROOKS]*ROOKVALUE-Position[BLACKPAWNS]*PAWNVALUE)==RootMaterial)) ExtendDepth++; else
		if (UseCheckExtensions && IsCheck(Position)) ExtendDepth++;
	}
	LastMove[Depth].From=0;
/*************************************************************************
If this is a terminal node then evaluate using the static-evaluation
function.
**************************************************************************/
	if (Depth>=FinalDepth+ExtendDepth) {
		Quiesce(Position, Depth, Lowest, Highest, Path, ExtendDepth);
		Path->Move[Depth].From=0;
		return;
	}
	CurrentPathNodes++;
	TMove* move;
	int MoveCount=0;
	int NumberOfMoves, Flag;
	TPath BestPath;
	TMove* BestPathMove=&BestPath.Move[Depth];
	BestPathMove->From=0;
	POSITIONELEMENT* NewPositionPointer=NewPosition[Depth];
	TPath NewPath;
	int Height=0;
	GlobalHashMove.From=BestPathMove->From=0;
	GetHashMoveDetails(Position, Height, Flag, *BestPathMove, BestPath.Value);
	if (Height>0 && BestPathMove->From!=0) {
		if (!VerifyMove(Position, *BestPathMove)) Height=0;
		if (Height>0) GlobalHashMove=*BestPathMove;
		if (Height>=(FinalDepth+ExtendDepth-Depth)/* || (Flag==VALID && BestPath.Value>=MinimumMate)*/) {
			HashSuccesses++;
			if (Flag==VALID) {
				Path->Move[Depth]=*BestPathMove;
				Path->Move[Depth].Capture=Position[BestPathMove->To];
				Path->Move[Depth+1].From=0;
				Path->Value=BestPath.Value;
				return;
			}
			if (Flag==UBOUND && BestPath.Value<Highest) Highest=BestPath.Value;
			if (Flag==LBOUND && BestPath.Value>Lowest) Lowest=BestPath.Value;
			if (Lowest>=Highest) {
				Path->Move[Depth]=*BestPathMove;
				Path->Move[Depth].Capture=Position[BestPathMove->To];
				Path->Move[Depth+1].From=0;
				Path->Value=BestPath.Value;
				UPDATEHISTORY(Path->Move[Depth].From, Path->Move[Depth].To);
				return;
			}
		}
	}

	// Fail-High Reductions
	// If this is during a minimal window search...
	BestPath.Value=LowEval;
	int FailHigh=0;
	if (UseFailHighReductions && !followPV) {
		if (Evaluate(Position, Lowest, Highest) >= Highest + FAILHIGHERROR) {
			FailHigh=1;
		}
	}
	// Null Move?
	// Not when searching Principal Variation
	// Not when in check
	// Not if nearly at the leaf nodes
	if (!followPV && UseNullMove && ((Depth+NullMoveReduceDepth+1)<(FinalDepth+ExtendDepth)) && !IsCheck(Position)) {
		// Not if previous move was a null move
		if (LastMove[Depth-1].From!=0) {
			int Pieces;
			if (Position[MOVER]==WHITE) {
				Pieces=Position[WHITEQUEENS]+Position[WHITEKNIGHTS]+Position[WHITEBISHOPS]+Position[WHITEROOKS];
			} else {
				Pieces=Position[BLACKQUEENS]+Position[BLACKKNIGHTS]+Position[BLACKBISHOPS]+Position[BLACKROOKS];
			}
			// Not if not many pieces left for the side to move
			if (Pieces>NullMoveStopMaterial) {
#ifndef NOZORBRIST
#ifdef CHARPOSITION
				int HashKey=Position[HASHKEYFIELD1]*HASHDIV3+Position[HASHKEYFIELD2]*HASHDIV2+Position[HASHKEYFIELD3]*HASHDIV1+Position[HASHKEYFIELD4];
#else
				int HashKey=Position[HASHKEYFIELD1];
#endif
				int OrigHashKey=HashKey;
#endif
				int EnPawn=Position[ENPAWN];
#ifndef NOZORBRIST
				HashKey^=PieceSquareValues[0][1];
				HashKey^=PieceSquareValues[0][0];
#ifdef CHARPOSITION
				Position[HASHKEYFIELD1]=HashKey/HASHDIV3;
				Position[HASHKEYFIELD2]=HashKey%HASHDIV3/HASHDIV2;
				Position[HASHKEYFIELD3]=HashKey%HASHDIV2/HASHDIV1;
				Position[HASHKEYFIELD4]=HashKey%HASHDIV1;
#else
				Position[HASHKEYFIELD1]=HashKey;
#endif
#endif
				// Make the null move
				Position[MOVER]=(Position[MOVER]==WHITE ? BLACK : WHITE);
				Position[ENPAWN]=0;
				Path->Move[Depth].From=0;
				Path->Move[Depth+1].From=0;
				// Pass down a minimal window, if the next search can't get
				// a value at least of at least -Highest, then it has failed high
				// here because the value will be >Highest, which means the
				// null move is good enough to cause a cut off.
				// With this, we assume that the best real move in this position
				// must be better than the null move and would therefore also
				// cause a cut-off.
				Minimax(Position, Depth+NullMoveReduceDepth+1, -Highest, -Highest+1, &NewPath, FinalDepth+ExtendDepth-FailHigh);
#ifndef NOZORBRIST
#ifdef CHARPOSITION
				Position[HASHKEYFIELD1]=OrigHashKey/HASHDIV3;
				Position[HASHKEYFIELD2]=OrigHashKey%HASHDIV3/HASHDIV2;
				Position[HASHKEYFIELD3]=OrigHashKey%HASHDIV2/HASHDIV1;
				Position[HASHKEYFIELD4]=OrigHashKey%HASHDIV1;
#else
				Position[HASHKEYFIELD1]=OrigHashKey;
#endif
#endif
				Position[MOVER]=(Position[MOVER]==WHITE ? BLACK : WHITE);
				Position[ENPAWN]=EnPawn;
				ReverseNewPathValue;
				if (NewPath.Value>=Highest) {
					Flag=LBOUND;
					Path->Value=NewPath.Value;
					BestPath.Value=NewPath.Value;
					BestPathMove->From=0;
					StoreHashMoveDetails(Position, FinalDepth+ExtendDepth-Depth-NullMoveReduceDepth, LBOUND, *BestPathMove, BestPath.Value);
					return;
				} //else {
				  //	if (Evaluate(Position, Lowest, Highest) >= Highest + FAILHIGHERROR) {
				  //		FailHigh=1;
				  //	}
				//}
			}
		}
	} // Null Move
	Flag=UBOUND;

	CurrentDepth=Depth;
	NumberOfMoves=GetMoveList(Position, Moves[Depth]);

	for (move=&Moves[Depth][1]; move<=&Moves[Depth][NumberOfMoves] && !ExitCode; move++) {
		if (MakeMove(Position, *move, NewPositionPointer)) {
			MoveCount++;
/*************************************************************************
Draw?
**************************************************************************/
			if (MoveCount>1) followPV=FALSE;
			if (Depth==1) {
			 CurrentDepth=Depth;
			 if (PreviousPosition(*move)) {
				NewPath.Value=-Contempt;
				NewPath.Move[Depth+1].From=0;
				goto DrawJump;
			 }
			}
			LastMove[Depth]=*move;
			if (MoveCount>1) {
				// Minimal Window Search
				Minimax(NewPositionPointer, Depth+1, -Lowest-1, -Lowest, &NewPath, FinalDepth+ExtendDepth-FailHigh);
				ReverseNewPathValue;
				if (NewPath.Value>Lowest && NewPath.Value<Highest) {
					  Minimax(NewPositionPointer, Depth+1, -Highest, -Lowest, &NewPath, FinalDepth+ExtendDepth-FailHigh);
					  ReverseNewPathValue;
				}
			} else {
				Minimax(NewPositionPointer, Depth+1, -Highest, -Lowest, &NewPath, FinalDepth+ExtendDepth-FailHigh);
				ReverseNewPathValue;
			}
DrawJump:
			if (NewPath.Value>=Highest) {
				if (NewPath.Value>9000) Flag=VALID; else
				Flag=LBOUND;
				memcpy(Path, &NewPath, SIZEOFPATH);
				Path->Move[Depth]=*BestPathMove=*move;
				BestPath.Value=NewPath.Value;
				UPDATEHISTORY(Path->Move[Depth].From, Path->Move[Depth].To);
				goto done;
			}
			if (NewPath.Value>BestPath.Value) {
				memcpy(&BestPath, &NewPath, SIZEOFPATH);
				*BestPathMove=*move;
				if (NewPath.Value>Lowest) {
				  Flag=VALID;
				  Lowest=NewPath.Value;
				}
			}
		} /* MakeMove */
	}

	if (MoveCount==0) {
		switch (WinnerWhenNoMoves(Position)) {
				case 1 : Path->Value=10000; break;
				case 0 : Path->Value=0; break;
				case -1 : Path->Value=-10000; break;
				default : exit(0);
		}
		Path->Move[Depth].From=0;
		BestPathMove->From=0;
		BestPath.Value=Path->Value;
		Flag=VALID;
		goto done;
	}

	memcpy(Path, &BestPath, SIZEOFPATH);
done:
	if (ExitCode==0) {
		  StoreHashMoveDetails(Position, FinalDepth+ExtendDepth-Depth, Flag, *BestPathMove, BestPath.Value);
	}
}

void _pascal
TSearcher::MinimaxZero(TPosition Position, unsigned int Depth, int Lowest, int Highest, TPath* Path, unsigned int ExtendDepth)
{
	if (ExitCode!=0) return;
	LastMove[Depth].From=0;
	CurrentPathNodes++;
	TMove* move;
	int MoveCount=0;
	int NumberOfMoves, Flag;
	TPath BestPath;
	TMove* BestPathMove=&BestPath.Move[Depth];
	BestPathMove->From=0;
	POSITIONELEMENT* NewPositionPointer=NewPosition[Depth];
	TPath NewPath;

	BestPath.Value=LowEval;

	Flag=UBOUND;
	for (move=&Moves[Depth][1]; move<=&Moves[Depth][Depth0Amount] && !ExitCode; move++) {
#ifdef TESTING
		writeLog(move->From);
		writeLog(move->To);
#endif
		if (MakeMove(Position, *move, NewPositionPointer)) {
			CurrentDepthZeroMove=*move;
/*************************************************************************
Draw?
**************************************************************************/
			if (++MoveCount>1) followPV=FALSE;
			CurrentDepth=Depth;
			if (PreviousPosition(*move)) {
				 NewPath.Value=Contempt;
				 NewPath.Move[Depth+1].From=0;
				 goto DrawJump;
			}
			LastMove[Depth]=*move;
			if (MoveCount>1 && UseMinimalWindow) {
				Minimax(NewPositionPointer, Depth+1, -Lowest-1, -Lowest, &NewPath, FinalDepth);
				ReverseNewPathValue;
				if (NewPath.Value>Lowest && NewPath.Value<Highest) {
					  Minimax(NewPositionPointer, Depth+1, -Highest, -Lowest, &NewPath, FinalDepth);
					  ReverseNewPathValue;
				}
			} else {
				Minimax(NewPositionPointer, Depth+1, -Highest, -Lowest, &NewPath, FinalDepth);
				ReverseNewPathValue;
			}
DrawJump:
			move->Score=NewPath.Value;
			if (NewPath.Value>=Highest) {
				Flag=LBOUND;
				memcpy(Path, &NewPath, SIZEOFPATH);
				Path->Move[Depth]=*BestPathMove=*move;
				BestPath.Value=NewPath.Value;
				UPDATEHISTORY(Path->Move[Depth].From, Path->Move[Depth].To);
				goto done;
			}
			if (NewPath.Value>BestPath.Value) {
				memcpy(&BestPath, &NewPath, SIZEOFPATH);
				*BestPathMove=*move;
				if (NewPath.Value>Lowest) {
				  Flag=VALID;
				  Lowest=NewPath.Value;
				  if (ExitCode==0) {
					  memcpy(&PrincipalPath, &BestPath, SIZEOFPATH);
					  PrincipalPath.Depth=FinalDepth+ExtendDepth;
					  Satisfied=(PrincipalPath.Value>=(PIBest-SATISFACTION))
						&& PrincipalPath.Value>ValueGuess-SATISFACTION;
					  if (PrintPV) WritePV(PV, PV2);
					  if (PrincipalPath.Value>MinimumMate) ExitWithMove();
				  }
				}
			}
		} /* MakeMove */
	}
	memcpy(Path, &BestPath, SIZEOFPATH);
done:
	if (PrincipalPath.Value>=StopScore &&
			PrincipalPath.Move[0].From==StopMove.From &&
			PrincipalPath.Move[0].To==StopMove.To) ExitWithMove();
#ifdef HASHTABLE
	if (/*BestPath.Value<9000 && BestPath.Value>-9000 && */ExitCode==0) {
		  StoreHashMoveDetails(Position, FinalDepth+ExtendDepth-Depth, Flag, *BestPathMove, BestPath.Value);
	}
#endif
}

void _pascal
TSearcher::SolveForMate(TPosition Position, unsigned int Depth, int Lowest, int Highest, TPath* Path)
{
}

void
TSearcher::ReOrderMoves(TMoveArray Moves, int Amount)
{
	TMove TempMove;
	int i, x;
	for (i=1; i<=Amount-1; i++)
		for (x=i+1; x<=Amount; x++)
			if (Moves[x].Score>Moves[i].Score) {
				TempMove=Moves[i];
				Moves[i]=Moves[x];
				Moves[x]=TempMove;
			}
}

void
TSearcher::SetUseHistory(int use)
{
	UseHistory=use;
}

void
TSearcher::SetUseNullMove(int use)
{
	UseNullMove=use;
}

void
TSearcher::SetUseFailHighReductions(int use)
{
	UseFailHighReductions=use;
}

void
TSearcher::SetContempt(int contempt)
{
	Contempt=contempt;
}

void
TSearcher::SetUseMinimalWindow(int useminwin)
{
	UseMinimalWindow=useminwin;
}

void
TSearcher::SetInitialPosition(TChessBoard NewPosition)
{
	int i, x, y;
	POSITIONELEMENT Square;
	if (MinimumMate>9000) {
		if ((NewPosition.LastMoveMade()-LastMoveNumber)!=2) {
			if (Verbose) {
				writeMessage(VERBOSE, "Not correct move.  Last move was", LastMoveNumber);
				writeMessage(VERBOSE, "Resetting minimum mate to 9000.  This move is", NewPosition.LastMoveMade());
			}
			MinimumMate=9000;
		} else {
			if (Verbose) writeMessage(VERBOSE, "Accepted Minimum mate requirement", MinimumMate);
		}
	}
	LastMoveNumber=NewPosition.LastMoveMade();
	if (Verbose) writeMessage(VERBOSE, "Set last move made as", LastMoveNumber);
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
	for (x=1; x<=8; x++) {
	  for (y=1; y<=8; y++) {
		 Square=x*10+y;
		 if (StartPosition[Square]==WK) {
			  StartPosition[WKINGPOSITION]=Square;
			  StartPosition[WHITEKINGS]++;
			  StartPosition[WHITEKINGS+StartPosition[WHITEKINGS]]=Square;
		 }
		 if (StartPosition[Square]==BK) {
			  StartPosition[BKINGPOSITION]=Square;
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
	for (i=1; i<=MoveList.Amount; i++) {
		if ((MoveList.Moves[i].From==51 && MoveList.Moves[i].To==71) ||
			(MoveList.Moves[i].From==51 && MoveList.Moves[i].To==31))
				StartPosition[WHITECASTLED]=TRUE;
		if ((MoveList.Moves[i].From==58 && MoveList.Moves[i].To==78) ||
			(MoveList.Moves[i].From==58 && MoveList.Moves[i].To==38))
				StartPosition[BLACKCASTLED]=TRUE;
	}
#ifndef NOZORBRIST
	int HashKey = ComputeHashKey(StartPosition);
#ifdef CHARPOSITION
	StartPosition[HASHKEYFIELD1]=HashKey/HASHDIV3;
	StartPosition[HASHKEYFIELD2]=HashKey%HASHDIV3/HASHDIV2;
	StartPosition[HASHKEYFIELD3]=HashKey%HASHDIV2/HASHDIV1;
	StartPosition[HASHKEYFIELD4]=HashKey%HASHDIV1;
#else
	StartPosition[HASHKEYFIELD1]=HashKey;
#endif
#endif
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
	DrawMoves[DrawCount].From=0;
	Ply1DrawMovesPly0[Ply1DrawCount].From=0;
	for (i=0; i<89; i++)
   	PathBoard[i]=StartPosition[i];
}

int _pascal
TSearcher::MakeMove(TPosition Position, TMove& Move, TPosition NewPosition)
{
	MakeMoveCalls++;
	timeticker=GetTickCount();
	int TypeIndex, PieceIndex;
	memcpy(NewPosition, Position, SIZEOFPOSITION);
/*************************************************************************
First we make the move.  In the rest of the function we will modify the
new position if required, (castling, promotion, en-passant, for example).
**************************************************************************/
	POSITIONELEMENT *FromPiece=Position+Move.From;
	POSITIONELEMENT *ToPiece=Position+Move.To;
#ifndef NOZORBRIST
#ifdef CHARPOSITION
	int HashKey=Position[HASHKEYFIELD1]*HASHDIV3+Position[HASHKEYFIELD2]*HASHDIV2+Position[HASHKEYFIELD3]*HASHDIV1+Position[HASHKEYFIELD4];
#else
	int HashKey=Position[HASHKEYFIELD1];
#endif
#ifdef HASHCHECK
	int OrigKey=HashKey;
#endif
	int White=*FromPiece<EMPTY;
#endif
	NewPosition[Move.To]=*FromPiece;
#ifndef NOZORBRIST
	HashKey^=PieceSquareValues[Move.From][*FromPiece-(White ? 1 : 95)];
#endif
/*************************************************************************
For display purposes (when showing the path), we update the CapturePiece
filed of the move structure.  Also we update the Piece Value fields
of the position.
**************************************************************************/
	if (*ToPiece!=EMPTY) {
#ifndef NOZORBRIST
		if (*ToPiece>EMPTY) {
			HashKey^=PieceSquareValues[Move.To][*ToPiece-95];
		} else {
			HashKey^=PieceSquareValues[Move.To][*ToPiece-1];
		}
#endif
		Move.Capture=*ToPiece;
	} else
		Move.Capture=EMPTY;
#ifndef NOZORBRIST
	HashKey^=PieceSquareValues[Move.To][*FromPiece-(White ? 1 : 95)];
#endif
	NewPosition[Move.From]=EMPTY;
/*************************************************************************
For kings, we need to make alterations for castling moves and also we need
to update the NewPosition[xKINGPOSITION] values.  Also we flag that the
king has moved to avoid future castling.
**************************************************************************/
	if (*FromPiece==WK) {
		NewPosition[WKINGPOSITION]=Move.To;
		NewPosition[WKINGMOVED]=TRUE;
		if (Move.From==51 && Move.To==31) {
			NewPosition[41]=WR;
			NewPosition[11]=EMPTY;
			NewPosition[WHITECASTLED]=TRUE;
#ifndef NOZORBRIST
			HashKey^=PieceSquareValues[11][WR-1];
			HashKey^=PieceSquareValues[41][WR-1];
#endif
		}
		if (Move.From==51 && Move.To==71) {
			NewPosition[61]=WR;
			NewPosition[81]=EMPTY;
			NewPosition[WHITECASTLED]=TRUE;
#ifndef NOZORBRIST
			HashKey^=PieceSquareValues[81][WR-1];
			HashKey^=PieceSquareValues[61][WR-1];
#endif
		}
	}
	if (*FromPiece==BK) {
		NewPosition[BKINGPOSITION]=Move.To;
		NewPosition[BKINGMOVED]=TRUE;
		if (Move.From==58 && Move.To==38) {
			NewPosition[48]=BR;
			NewPosition[18]=EMPTY;
			NewPosition[BLACKCASTLED]=TRUE;
#ifndef NOZORBRIST
			HashKey^=PieceSquareValues[18][BR-95];
			HashKey^=PieceSquareValues[48][BR-95];
#endif
		}
		if (Move.From==58 && Move.To==78) {
			NewPosition[68]=BR;
			NewPosition[88]=EMPTY;
			NewPosition[BLACKCASTLED]=TRUE;
#ifndef NOZORBRIST
			HashKey^=PieceSquareValues[88][BR-95];
			HashKey^=PieceSquareValues[68][BR-95];
#endif
		}
	}
/*************************************************************************
For pawns, we need to make alterations for promotions and en-passant moves.
**************************************************************************/
	if (*FromPiece==WP) {
	  if (GetRank[Move.To]==8) {
		  NewPosition[Move.To]=Move.PromotionPiece;
#ifndef NOZORBRIST
		  HashKey^=PieceSquareValues[Move.To][WP-1];
		  HashKey^=PieceSquareValues[Move.To][Move.PromotionPiece-1];
#endif
	  }
	  if ((Move.From-Move.To==-11 || Move.From-Move.To==9) && *ToPiece==EMPTY) {
		  NewPosition[Move.To-1]=EMPTY;
#ifndef NOZORBRIST
		  HashKey^=PieceSquareValues[Move.To-1][BP-95];
#endif
		  Move.Capture=BP;
	  }
	}
	if (*FromPiece==BP) {
	  if (GetRank[Move.To]==1) {
		  NewPosition[Move.To]=Move.PromotionPiece+100;
#ifndef NOZORBRIST
		  HashKey^=PieceSquareValues[Move.To][BP-95];
		  HashKey^=PieceSquareValues[Move.To][Move.PromotionPiece+5];
#endif
	  }
	  if ((Move.From-Move.To==11 || Move.From-Move.To==-9) && *ToPiece==EMPTY) {
		  NewPosition[Move.To+1]=EMPTY;
#ifndef NOZORBRIST
		  HashKey^=PieceSquareValues[Move.To+1][WP-1];
#endif
		  Move.Capture=WP;
	  }
	}
/*************************************************************************
For rooks, we need to flag their movements for castling privelege purposes.
**************************************************************************/
	if (Move.From==11 || Move.To==11) NewPosition[WROOK1MOVED]=TRUE;
	if (Move.From==81 || Move.To==81) NewPosition[WROOK8MOVED]=TRUE;
	if (Move.From==18 || Move.To==18) NewPosition[BROOK1MOVED]=TRUE;
	if (Move.From==88 || Move.To==88) NewPosition[BROOK8MOVED]=TRUE;
/*************************************************************************
We need to set the [ENPAWN] file.
**************************************************************************/
	NewPosition[ENPAWN]=0;
	if (*FromPiece==WP && GetRank[Move.From]==2 && GetRank[Move.To]==4) NewPosition[ENPAWN]=GetFile[Move.To];
	if (*FromPiece==BP && GetRank[Move.From]==7 && GetRank[Move.To]==5) NewPosition[ENPAWN]=GetFile[Move.To];
// Change the side to move
#ifndef NOZORBRIST
	HashKey^=PieceSquareValues[0][1];
	HashKey^=PieceSquareValues[0][0];
#ifdef CHARPOSITION
	NewPosition[HASHKEYFIELD1]=HashKey/HASHDIV3;
	NewPosition[HASHKEYFIELD2]=HashKey%HASHDIV3/HASHDIV2;
	NewPosition[HASHKEYFIELD3]=HashKey%HASHDIV2/HASHDIV1;
	NewPosition[HASHKEYFIELD4]=HashKey%HASHDIV1;
#else
	NewPosition[HASHKEYFIELD1]=HashKey;
#endif
#endif
	AlterLocationInformation;
/*************************************************************************
Before we change the side to move, let's check to see if the move would
leave the mover in check.  If so, we must return 0.
**************************************************************************/
	if (IsCheck(NewPosition)) {
		 MakeMoveTimer+=(GetTickCount()-timeticker);
		 return 0;
	}
	if (NewPosition[MOVER]==WHITE) NewPosition[MOVER]=BLACK; else NewPosition[MOVER]=WHITE;
/*************************************************************************
Turn this on to test whether the hash value is calculated
correctly on the fly
**************************************************************************/
#ifdef HASHCHECK
	if (HashKey!=ComputeHashKey(NewPosition)) {
		FILE* f;
		f=fopen("c:\\log.txt", "a");
		fprintf( f, "Original Calculated: %i\n", ComputeHashKey(Position) );
		fprintf( f, "Final Calculated: %i\n", ComputeHashKey(NewPosition) );
		char s1[500]; char s2[500]; int p[89];
		for (int x=0; x<89; x++) p[x]=Position[x];
		TChessBoard* a = new TChessBoard(p); a->GetFEN(s1);
		delete a;
		for (x=0; x<89; x++) p[x]=NewPosition[x];
		a = new TChessBoard(p); a->GetFEN(s2);
		delete a;
		fprintf(f, "Mover: %s\n%s1\n%s2\nMove: %i-%i=%i\nOrig: %i HashKey %i\n", (Position[MOVER]==WHITE ? "White" : "Black"), s1, s2, Move.From, Move.To, Move.PromotionPiece, OrigKey, HashKey);
		fprintf(f, "[81][WR] %i [11][BN] %i [11][WR] %i\n", PieceSquareValues[81][WR-1], PieceSquareValues[11][BN-95], PieceSquareValues[11][WR-1]);
		ExitWithMove();
		fclose(f);
	}
#endif
	NewPosition[LASTMOVESQUARE]=Move.To;
	MakeMoveTimer+=(GetTickCount()-timeticker);
	return 1;
}

int
TSearcher::KingSafety(TPosition Position)
{
/**************************************************************************
Makeshift King safety function.  Prevents Rival neglecting the king but
doesn't contain much knowledge.
Evaluates from White King Side Castle point of view, position is modified
prior to calling.
**************************************************************************/
	int Surround=(Position[62]==PAWN)+(Position[63]==PAWN)+
					 (Position[72]==PAWN)+(Position[73]==PAWN)+
					 (Position[82]==PAWN)+(Position[83]==PAWN)+
					 (Position[63]==KNIGHT)+(Position[72]==BISHOP);
	int Score=Surround*MINORKINGSAFETYUNIT;
	if (Position[62]==PAWN && Position[72]==PAWN && Position[82]==PAWN) {
		// A.  Three straight pawns.  Bonus if knight protects H-Pawn
		Score+=KINGSAFETYSTRONG;
		if (Position[63]==KNIGHT) Score+=KINGSAFETYUNIT;
	} else
	if (Position[62]==PAWN && Position[73]==PAWN && Position[82]==PAWN && Position[72]==BISHOP) {
		// B. Even stronger if opponent doesn't have same colour bishop.
		Score+=KINGSAFETYSTRONG;
	} else
	if (Position[62]==PAWN && Position[73]==PAWN && Position[83]==PAWN) {
		// C.
		Score+=KINGSAFETYQUITESTRONG;
	} else
	if (Position[62]==PAWN && Position[72]==PAWN && Position[83]==PAWN) {
		// D.  H pawn on rank 3.  Should also check the danger that the
		// opponent has the bishop that can attack the h-pawn.
		Score+=KINGSAFETYQUITESTRONG;
		if (Position[63]==KNIGHT) Score+=KINGSAFETYUNIT;
	} else
	if (Position[64]==PAWN && Position[72]==PAWN && Position[82]==PAWN) {
		// E.
		Score+=KINGSAFETYSTRONG;
		if (Position[63]==KNIGHT) Score+=KINGSAFETYUNIT;
	} else
	if (Position[63]==PAWN && Position[72]==PAWN && Position[82]==PAWN) {
		// F.
		Score+=KINGSAFETYQUITEWEAK;
	} else
	if (Position[62]==PAWN && Position[73]==PAWN && Position[82]==PAWN) {
		// G. Only score if no opposite queen
		if (Position[MOVER]==WHITE && !Position[BLACKQUEENS])	Score+=KINGSAFETYVERYWEAK; else
		if (Position[MOVER]==BLACK && !Position[WHITEQUEENS])	Score+=KINGSAFETYVERYWEAK;
	} else
	if (Position[63]==PAWN && Position[72]==PAWN && Position[83]==PAWN) {
		// H.
		Score+=KINGSAFETYVERYWEAK;
	} else
	if (Position[64]==PAWN && Position[73]==PAWN && Position[82]==PAWN) {
		// I. Award points only if knight and bishop well placed
		if (Position[63]==KNIGHT && Position[72]==BISHOP) {
			Score+=KINGSAFETYQUITEWEAK;
		}
	}

	return Score;
}

int pascal
TSearcher::Evaluate(TPosition Position, int Lowest, int Highest)
{
	 timeticker=GetTickCount();
/**************************************************************************
Yield to the operating system occasionally.
**************************************************************************/
	 TimeAndYieldCode;
#ifdef WRITEPOSITIONBEFOREEVAL
	 char st[500];
	 int po[89];
	 for (int x=0; x<89; x++) po[x]=Position[x];
	 TChessBoard* b = new TChessBoard(po);
	 b->GetFEN(st);
	 writeLog(st);
	 delete b;
#endif
	 CurrentPathNodes++;
	 int i;
	 BOOL OpeningPhase;
	 BOOL EndgamePhase=FALSE;

/**************************************************************************
Populate some variables that may be useful later on.
**************************************************************************/
	 int WhiteMajorPieces=Position[WHITEROOKS]*ROOKVALUE+Position[WHITEQUEENS]*QUEENVALUE;
	 int BlackMajorPieces=Position[BLACKROOKS]*ROOKVALUE+Position[BLACKQUEENS]*QUEENVALUE;
	 int WhiteMinorPieces=Position[WHITEKNIGHTS]*KNIGHTVALUE+Position[WHITEBISHOPS]*BISHOPVALUE;
	 int BlackMinorPieces=Position[BLACKKNIGHTS]*KNIGHTVALUE+Position[BLACKBISHOPS]*BISHOPVALUE;
	 int WhitePieces=WhiteMajorPieces+WhiteMinorPieces;
	 int BlackPieces=BlackMajorPieces+BlackMinorPieces;
/**************************************************************************
If one side has a lone king, evaluate for mate.
**************************************************************************/
	 if ((WhitePieces==0 || BlackPieces==0) && Position[WHITEPAWNS]==0 && Position[BLACKPAWNS]==0) return LoneKing(Position);

	 int WhiteScore=WhitePieces+Position[WHITEPAWNS]*PAWNVALUE;
	 int BlackScore=BlackPieces+Position[BLACKPAWNS]*PAWNVALUE;
	 int WhiteKing=Position[WKINGPOSITION];
	 int BlackKing=Position[BKINGPOSITION];
	 int PawnRank, PawnFileIndex, PotentialPawnRank, FirstOppositionRank, SecondOppositionRank;
	 POSITIONELEMENT* v=Position+WHITEPAWNS;
/**************************************************************************
If the evaluation cannot be within the search bounds, exit now.
**************************************************************************/
#ifndef POSITIONALEVAL
	 if (BlackPieces!=0 && WhitePieces!=0) {
		 if (Position[MOVER]==WHITE) {
			if (WhiteScore-BlackScore > Highest*10+POSITIONALMAX) return Highest;
			if (WhiteScore-BlackScore < Lowest*10-POSITIONALMAX) return Lowest;
		 } else {
			if (BlackScore-WhiteScore > Highest*10+POSITIONALMAX) return Highest;
			if (BlackScore-WhiteScore < Lowest*10-POSITIONALMAX) return Lowest;
		}
	 }
#endif
/**************************************************************************
Is this the opening, middlegame or the endgame?
**************************************************************************/
	 if (Position[MOVER]==WHITE) {
		OpeningPhase=
		(!Position[WHITECASTLED] && !Position[WKINGMOVED] && !(Position[WROOK1MOVED] && Position[WROOK8MOVED])) ||
		(Position[21]==WN || Position[31]==WB || Position[61]==WB || Position[71]==WN);
	 } else {
		OpeningPhase=
		(!Position[BLACKCASTLED] && !Position[BKINGMOVED] && !(Position[BROOK1MOVED] && Position[BROOK8MOVED])) ||
		(Position[28]==BN || Position[38]==BB || Position[68]==BB || Position[78]==BN);
	 }
	 if (!OpeningPhase) {
		EndgamePhase=(WhitePieces<ENDGAMEPIECES && BlackPieces<ENDGAMEPIECES);
	 } else {
/**************************************************************************
Evaluate piece development in the opening.
**************************************************************************/
		int WhiteUnmovedMinors=((Position[21]==WN)+(Position[31]==WB)+(Position[61]==WB)+(Position[71]==WN));
		int BlackUnmovedMinors=((Position[28]==BN)+(Position[38]==BB)+(Position[68]==BB)+(Position[78]==BN));
		WhiteScore-=WhiteUnmovedMinors*UNDEVELOPEDMINOR;
		BlackScore-=BlackUnmovedMinors*UNDEVELOPEDMINOR;
		if (WhiteUnmovedMinors==4 && Position[41]!=WQ) WhiteScore-=QUEENOUTEARLY;
		if (BlackUnmovedMinors==4 && Position[48]!=BQ) BlackScore-=QUEENOUTEARLY;
		WhiteScore+=CENTRALOCCUPATION*((Position[44]<EMPTY) + (Position[45]<EMPTY) + (Position[54]<EMPTY) + (Position[55]<EMPTY));
		BlackScore+=CENTRALOCCUPATION*((Position[44]>EMPTY) + (Position[45]>EMPTY) + (Position[54]>EMPTY) + (Position[55]>EMPTY));
	 }
/**************************************************************************
Evaluate each piece in turn.
First Pawns which get a bonus for advancing.
**************************************************************************/
	 int MostAdvancedWhitePawn[9]={0, 0, 0, 0, 0, 0, 0, 0, 0};
	 int LeastAdvancedWhitePawn[9]={9, 9, 9, 9, 9, 9, 9, 9, 9};
	 for (v+=*v; v!=Position+WHITEPAWNS; v--) {
		WhiteScore+=(EndgamePhase ? WhitePawnAdvance[*v]*PAWNADVANCEENDGAME : WhitePawnAdvance[*v]);
		PawnRank=GetRank[*v];
		PawnFileIndex=GetFile[*v];
		if (PawnRank>MostAdvancedWhitePawn[PawnFileIndex]) MostAdvancedWhitePawn[PawnFileIndex]=PawnRank;
		if (PawnRank<LeastAdvancedWhitePawn[PawnFileIndex]) LeastAdvancedWhitePawn[PawnFileIndex]=PawnRank;
	 }
	 int MostAdvancedBlackPawn[9]={9, 9, 9, 9, 9, 9, 9, 9, 9};
	 int LeastAdvancedBlackPawn[9]={0, 0, 0, 0, 0, 0, 0, 0, 0};
	 for (v+=(BLACKPAWNS-WHITEPAWNS), v+=*v; v!=Position+BLACKPAWNS; v--) {
		BlackScore+=(EndgamePhase ? BlackPawnAdvance[*v]*PAWNADVANCEENDGAME : BlackPawnAdvance[*v]);
		PawnRank=GetRank[*v];
		PawnFileIndex=GetFile[*v];
		if (PawnRank<MostAdvancedBlackPawn[PawnFileIndex]) MostAdvancedBlackPawn[PawnFileIndex]=PawnRank;
		if (PawnRank>LeastAdvancedBlackPawn[PawnFileIndex]) LeastAdvancedBlackPawn[PawnFileIndex]=PawnRank;
	 }
//goto PositionalEvalSkip;
/**************************************************************************
Evaluate Rooks for Mobility, King Tropism (take distance from score),
Seventh Rank, Open/Semi-Open Files and Doubled on same Rank or File.
**************************************************************************/
	 for (v+=(WHITEROOKS-BLACKPAWNS), v+=*v; v!=Position+WHITEROOKS; v--) {
		for (i=*v+10; i<90; i+=10) if (Position[i]==EMPTY) WhiteScore+=ROOKRANKMOBILITY; else {if (Position[i]==WR) WhiteScore+=DOUBLEDFILEROOKS; break;}
		for (i=*v-10; i>9; i-=10) if (Position[i]==EMPTY) WhiteScore+=ROOKRANKMOBILITY;	else {if (Position[i]==WR) WhiteScore+=DOUBLEDFILEROOKS; break;}
		for (i=*v-1; Valid[i]; i--) if (Position[i]==EMPTY) WhiteScore+=ROOKFILEMOBILITY; else {if (Position[i]==WR) WhiteScore+=DOUBLEDRANKROOKS; break;}
		for (i=*v+1; Valid[i]; i++) if (Position[i]==EMPTY) WhiteScore+=ROOKFILEMOBILITY; else {if (Position[i]==WR) WhiteScore+=DOUBLEDRANKROOKS; break;}
		WhiteScore+=(-RookKingTropism[*v][BlackKing])+(WhiteSeventhRank[*v] ? ROOKSEVENTHRANK : 0)+
		(MostAdvancedWhitePawn[GetFile[*v]]==0 ? MostAdvancedBlackPawn[GetFile[*v]]==9 ? ROOKONOPENFILE : ROOKONSEMIOPENFILE : 0);
	 }
	 for (v+=(BLACKROOKS-WHITEROOKS), v+=*v; v!=Position+BLACKROOKS; v--) {
		for (i=*v+10; i<90; i+=10) if (Position[i]==EMPTY) BlackScore+=ROOKRANKMOBILITY; else {if (Position[i]==BR) BlackScore+=DOUBLEDFILEROOKS; break;}
		for (i=*v-10; i>9; i-=10) if (Position[i]==EMPTY) BlackScore+=ROOKRANKMOBILITY;	else {if (Position[i]==BR) BlackScore+=DOUBLEDFILEROOKS; break;}
		for (i=*v-1; Valid[i]; i--) if (Position[i]==EMPTY) BlackScore+=ROOKFILEMOBILITY; else {if (Position[i]==BR) BlackScore+=DOUBLEDRANKROOKS; break;}
		for (i=*v+1; Valid[i]; i++) if (Position[i]==EMPTY) BlackScore+=ROOKFILEMOBILITY; else {if (Position[i]==BR) BlackScore+=DOUBLEDRANKROOKS; break;}
		BlackScore+=(-RookKingTropism[*v][WhiteKing])+(BlackSeventhRank[*v] ? ROOKSEVENTHRANK : 0)+
		(MostAdvancedBlackPawn[GetFile[*v]]==9 ? MostAdvancedWhitePawn[GetFile[*v]]==0 ? ROOKONOPENFILE : ROOKONSEMIOPENFILE : 0);
	 }
/**************************************************************************
Evaluate Knights for piece square bonus and King Tropism (added this time)
**************************************************************************/
	 for (v+=(WHITEKNIGHTS-BLACKROOKS), v+=*v; v!=Position+WHITEKNIGHTS; WhiteScore+=(knightcontrol[*v]+KnightKingTropism[*v][BlackKing]), v--);
	 for (v+=(BLACKKNIGHTS-WHITEKNIGHTS), v+=*v; v!=Position+BLACKKNIGHTS; BlackScore+=(knightcontrol[*v]+KnightKingTropism[*v][WhiteKing]), v--);
/**************************************************************************
Evaluate Bishops for Mobility and King Tropism (take distance from score)
**************************************************************************/
	 for (v+=(WHITEBISHOPS-BLACKKNIGHTS), v+=*v; v!=Position+WHITEBISHOPS; v--) {
		WhiteScore+=(knightcontrol[*v]-BishopKingTropism[*v][BlackKing]);
		for (i=*v+11; Valid[i]; i+=11) if (Position[i]==EMPTY) WhiteScore+=BISHOPMOBILITY; else break;
		for (i=*v+9; Valid[i]; i+=9) if (Position[i]==EMPTY) WhiteScore+=BISHOPMOBILITY; else break;
		for (i=*v-11; Valid[i]; i-=11) if (Position[i]==EMPTY) WhiteScore+=BISHOPMOBILITY; else break;
		for (i=*v-9; Valid[i]; i-=9) if (Position[i]==EMPTY) WhiteScore+=BISHOPMOBILITY; else break;
	 }
	 for (v+=(BLACKBISHOPS-WHITEBISHOPS), v+=*v; v!=Position+BLACKBISHOPS; v--) {
		BlackScore+=(knightcontrol[*v]-BishopKingTropism[*v][WhiteKing]);
		for (i=*v+11; Valid[i]; i+=11) if (Position[i]==EMPTY) BlackScore+=BISHOPMOBILITY; else break;
		for (i=*v+9; Valid[i]; i+=9) if (Position[i]==EMPTY) BlackScore+=BISHOPMOBILITY; else break;
		for (i=*v-11; Valid[i]; i-=11) if (Position[i]==EMPTY) BlackScore+=BISHOPMOBILITY; else break;
		for (i=*v-9; Valid[i]; i-=9) if (Position[i]==EMPTY) BlackScore+=BISHOPMOBILITY; else break;
	 }
/**************************************************************************
Evaluate Queens for Mobility and King Tropism (take distance from score)
**************************************************************************/
	 for (v+=(WHITEQUEENS-BLACKBISHOPS), v+=*v; v!=Position+WHITEQUEENS; v--) WhiteScore-=QueenKingTropism[*v][BlackKing];
	 for (v+=(BLACKQUEENS-WHITEQUEENS), v+=*v; v!=Position+BLACKQUEENS; v--) BlackScore-=QueenKingTropism[*v][WhiteKing];

/**************************************************************************
King safety.
Award points for safe king positions.
Before calling the king safety method, the bottom right side of the board
is populated with the castled position of the side to evaluate.  That is,
if black has castled queen side, the pieces around the black king are
transposed to the bottom right hand side of the board as if it were a
white king side castle.  This slows the routine slightly but ensures that
the position is evaluated the same for all four castle positions.
**************************************************************************/
	 int WhiteKingSafety=0;
	 int BlackKingSafety=0;
	 if (!EndgamePhase) {
		 int x, y;
		 TPosition NewPosition;
		 NewPosition[MOVER]=Position[MOVER];
		 NewPosition[WHITEQUEENS]=Position[WHITEQUEENS];
		 NewPosition[BLACKQUEENS]=Position[BLACKQUEENS];
		 if (GetRank[WhiteKing]<3 && (GetFile[WhiteKing]<4 || GetFile[WhiteKing]>5)) {
			 if (WhiteKing==11 || WhiteKing==21 || WhiteKing==31) {
				for (x=5; x<=8; x++)
					for (y=1; y<=4; y++) {
						NewPosition[x*10+y]=Position[(9-x)*10+y];
						if (NewPosition[x*10+y]>EMPTY) WhiteKingSafety-=KINGSAFETYUNIT;
						if (NewPosition[x*10+y]==BQ) WhiteKingSafety-=KINGSAFETYUNIT;
					}
			 } else {
				for (x=5; x<=8; x++)
					for (y=1; y<=4; y++) {
						NewPosition[x*10+y]=Position[x*10+y];
						if (NewPosition[x*10+y]>EMPTY) WhiteKingSafety-=KINGSAFETYUNIT;
						if (NewPosition[x*10+y]==BQ) WhiteKingSafety-=KINGSAFETYUNIT;
					}
			 }
			 WhiteKingSafety+=KingSafety(NewPosition);
			 WhiteScore+=WhiteKingSafety;
		 }
		 if (GetRank[BlackKing]>6 && (GetFile[BlackKing]<4 || GetFile[BlackKing]>5)) {
			 if (BlackKing==18 || BlackKing==28 || BlackKing==38) {
				for (x=5; x<=8; x++)
					for (y=1; y<=4; y++) {
						NewPosition[x*10+y]=Position[(9-x)*10+(9-y)];
						if (NewPosition[x*10+y]<EMPTY) NewPosition[x*10+y]+=100; else
						if (NewPosition[x*10+y]>EMPTY) NewPosition[x*10+y]-=100;
						if (NewPosition[x*10+y]>EMPTY) BlackKingSafety-=KINGSAFETYUNIT;
						if (NewPosition[x*10+y]==BQ) BlackKingSafety-=KINGSAFETYUNIT;
					}
			 } else {
				for (x=5; x<=8; x++)
					for (y=1; y<=4; y++) {
						NewPosition[x*10+y]=Position[x*10+(9-y)];
						if (NewPosition[x*10+y]<EMPTY) NewPosition[x*10+y]+=100; else
						if (NewPosition[x*10+y]>EMPTY) NewPosition[x*10+y]-=100;
						if (NewPosition[x*10+y]>EMPTY) BlackKingSafety-=KINGSAFETYUNIT;
						if (NewPosition[x*10+y]==BQ) BlackKingSafety-=KINGSAFETYUNIT;
					}
			 }
			 BlackKingSafety+=KingSafety(NewPosition);
			 BlackScore+=BlackKingSafety;
		 }
#ifdef TESTKINGSAFETY
		static int Count=0;
		Count++;
		if (Count>0) {
			Count=0;
			char s[500];
			int p[89];
			for (int x=0; x<89; x++) {
				p[x]=Position[x];
			}
			TChessBoard* a = new TChessBoard(p);
			a->GetFEN(s);
			FILE* f=fopen("c:\\log.txt", "a");
			fprintf(f, "%s W %i B %i\n", s, WhiteKingSafety, BlackKingSafety);
			fclose(f);

			delete a;
			for (x=0; x<89; x++) {
				p[x]=NewPosition[x];
			}
			a = new TChessBoard(p);
			a->GetFEN(s);
			f=fopen("c:\\log.txt", "a");
			fprintf(f, "%s W %i B %i\n", s, WhiteKingSafety, BlackKingSafety);
			fclose(f);

			delete a;
		}
#endif
	 }
/**************************************************************************
Evaluate Pawn Structure.  Penalise doubled/isolated pawns a fixed value.
Award a fixed bonus for passed pawns.
**************************************************************************/
	 // for each pawn file
	 for (i=9; --i; ) {
		/* any pawns on this file? */
		if (MostAdvancedWhitePawn[i]>0) {
			/* Most advanced friendly pawn */
			PawnRank=MostAdvancedWhitePawn[i];
			/* more than one? */
			if (LeastAdvancedWhitePawn[i]!=PawnRank) WhiteScore-=DOUBLEDPAWNS;
			/* no friendly pawn on left or right of this file? */
			if ((i==1 || MostAdvancedWhitePawn[i-1]==0) && (i==8 || MostAdvancedWhitePawn[i+1]==0)) WhiteScore-=ISOLATEDPAWN;
			/* no opposition pawn blocking on left or right of this file? */
			PotentialPawnRank=PawnRank;
			if (Position[MOVER]==WHITE) {
				if (PawnRank==2 && Position[i*10+3]==EMPTY && Position[i*10+4]==EMPTY)
					PotentialPawnRank+=2; else
				if (Position[i*10+(PawnRank+1)]==EMPTY)
					PotentialPawnRank++;
			}
			if (LeastAdvancedBlackPawn[i]<=PotentialPawnRank && (i==1 || LeastAdvancedBlackPawn[i-1]<=PotentialPawnRank) && (i==8 || LeastAdvancedBlackPawn[i+1]<=PotentialPawnRank)) {
#ifdef TESTWHITEPASSEDPAWNS
		char s[500];
		int p[89];
		for (int x=0; x<89; x++) {
			p[x]=Position[x];
		}
		TChessBoard* a = new TChessBoard(p);
		a->GetFEN(s);
		FILE* f=fopen("c:\\log.txt", "a");
		fprintf(f, "\nFEN: %s\n",	s);
		fprintf(f, "I %i\nMAWPI %i\nLABPI %i\nLABPI-1 %i\nLABPI+1 %i",
				i,
				MostAdvancedWhitePawn[i],
				LeastAdvancedBlackPawn[i],
				(i>1 ? LeastAdvancedBlackPawn[i-1] : -2),
				(i<8 ? LeastAdvancedBlackPawn[i+1] : -3));
		fclose(f);
		delete a;
				exit(0);
#endif
				WhiteScore+=PASSEDPAWN*PawnRank;
				if (BlackPieces==0) {
					// Opponent's king too far behind
					if (PawnRank-GetRank[BlackKing] > 1) WhiteScore+=(PASSEDPAWNHOME*PawnRank); else
					// On sixth rank supported by own king
					//if (abs(GetRank[WhiteKing]-PotentialPawnRank)<2 && PotentialPawnRank>=6 && abs(GetFile[WhiteKing]-i)==1) WhiteScore+=(PASSEDPAWNHOME*PawnRank); else
					// Opponent king can't get into the magic square!
					if (abs(GetFile[BlackKing]-i)>(9-PotentialPawnRank)) WhiteScore+=(PASSEDPAWNHOME*PawnRank);
				}
			}
		}
		if (MostAdvancedBlackPawn[i]<9) {
			PawnRank=MostAdvancedBlackPawn[i];
			if (LeastAdvancedBlackPawn[i]!=PawnRank) BlackScore-=DOUBLEDPAWNS;
			if ((i==1 || MostAdvancedBlackPawn[i-1]==9) && (i==8 || MostAdvancedBlackPawn[i+1]==9)) BlackScore-=ISOLATEDPAWN;
			PotentialPawnRank=PawnRank;
			if (Position[MOVER]==BLACK) {
				if (PawnRank==7 && Position[i*10+6]==EMPTY && Position[i*10+5]==EMPTY)
					PotentialPawnRank-=2; else
				if (Position[i*10+(PawnRank-1)]==EMPTY)
					PotentialPawnRank--;
			}
			if (LeastAdvancedWhitePawn[i]>=PotentialPawnRank && (i==1 || LeastAdvancedWhitePawn[i-1]>=PotentialPawnRank) && (i==8 || LeastAdvancedWhitePawn[i+1]>=PotentialPawnRank)) {
#ifdef TESTBLACKPASSEDPAWNS
		char s[500];
		int p[89];
		for (int x=0; x<89; x++) {
			p[x]=Position[x];
		}
		TChessBoard* a = new TChessBoard(p);
		a->GetFEN(s);
		FILE* f=fopen("c:\\log.txt", "a");
		fprintf(f, "\nFEN: %s\n",	s);
		fprintf(f, "I %i\nMABPI %i\nLAWPI %i\nLAWPI-1 %i\nLAWPI+1 %i",
				i,
				MostAdvancedBlackPawn[i],
				LeastAdvancedWhitePawn[i],
				(i>1 ? LeastAdvancedWhitePawn[i-1] : -2),
				(i<8 ? LeastAdvancedWhitePawn[i+1] : -3));
		fclose(f);
		delete a;
				exit(0);
#endif
				BlackScore+=PASSEDPAWN*(9-PawnRank);
				if (WhitePieces==0) {
					if (GetRank[WhiteKing]-PotentialPawnRank > 1) BlackScore+=(PASSEDPAWNHOME*(9-PawnRank)); else
					//if (abs(GetRank[BlackKing]-PotentialPawnRank)<2 && PotentialPawnRank<=3 && abs(GetFile[BlackKing]-i)==1) BlackScore+=(PASSEDPAWNHOME*(9-PawnRank)); else
					if (abs(GetFile[WhiteKing]-i)>PotentialPawnRank) BlackScore+=(PASSEDPAWNHOME*(9-PawnRank));
				}
			}
		}
	 }
/**************************************************************************
If this is the endgame then encourage the king towards the centre.  Also
give a bonus for closeness to enemy king.  Also award points for having
the opposition.
**************************************************************************/
	 if (EndgamePhase) {
			//WhiteScore+=(KINGCENTRE*CentreKing[WhiteKing]);
			//BlackScore+=(KINGCENTRE*CentreKing[BlackKing]);
			if (WhitePieces>BlackPieces) WhiteScore+=TropismNear[WhiteKing][BlackKing]*MOVEKINGNEAR;
			if (BlackPieces>WhitePieces) BlackScore+=TropismNear[WhiteKing][BlackKing]*MOVEKINGNEAR;
			/*if (WhitePieces==0 && BlackPieces==0) {
				int WFile=GetFile[WhiteKing];
				int BFile=GetFile[BlackKing];
				int WRank=GetRank[WhiteKing];
				int BRank=GetRank[BlackKing];
				int Distance=0;
				if (WFile==BFile) Distance=abs(WRank-BRank); else
				if (WRank==BRank) Distance=abs(WFile-BFile); else
				if (abs(WRank-BRank)==abs(WFile-BFile)) Distance=abs(WRank-BRank);
				if (Distance==2) if (Position[MOVER]==BLACK) WhiteScore+=OPPOSITION2; else BlackScore+=OPPOSITION2;
				if (Distance==3) if (Position[MOVER]==WHITE) WhiteScore+=OPPOSITION2; else BlackScore+=OPPOSITION2;
				if (Distance==4) if (Position[MOVER]==BLACK) WhiteScore+=OPPOSITION4; else BlackScore+=OPPOSITION4;
				if (Distance==5) if (Position[MOVER]==WHITE) WhiteScore+=OPPOSITION4; else BlackScore+=OPPOSITION4;
				if (Distance==6) if (Position[MOVER]==BLACK) WhiteScore+=OPPOSITION6; else BlackScore+=OPPOSITION6;
				if (Distance==7) if (Position[MOVER]==WHITE) WhiteScore+=OPPOSITION6; else BlackScore+=OPPOSITION6;
			}*/
	 }
/**************************************************************************
Award the side with the greater piece material a bonus that relates to the
piece material difference and the number of friendly pawns.
**************************************************************************/
/*	 if (WhitePieces>BlackPieces)
		WhiteScore+=((WhitePieces-BlackPieces)*(Position[WHITEPAWNS]>3 ? 3 : Position[WHITEPAWNS])*RATIOBONUS)/1000;
	 if (BlackPieces>WhitePieces)
		BlackScore+=((BlackPieces-WhitePieces)*(Position[BLACKPAWNS]>3 ? 3 : Position[BLACKPAWNS])*RATIOBONUS)/1000;*/
/**************************************************************************
Penalise for giving up the right to castle.
**************************************************************************/
	 if (!Position[WHITECASTLED]) {
		if (Position[WKINGMOVED]) WhiteScore-=NOCASTLEPOSSIBLE; else {
			if (Position[WROOK8MOVED]) WhiteScore-=KINGCASTLENOPOSSIBLE;
			if (Position[WROOK1MOVED])	WhiteScore-=QUEENCASTLENOPOSSIBLE;
		}
	 }
	 if (!Position[BLACKCASTLED]) {
		if (Position[BKINGMOVED]) BlackScore-=NOCASTLEPOSSIBLE; else {
			if (Position[BROOK8MOVED]) BlackScore-=KINGCASTLENOPOSSIBLE;
			if (Position[BROOK1MOVED])	BlackScore-=QUEENCASTLENOPOSSIBLE;
		}
	 }

/**************************************************************************
Record the function duration and return the score from the point of view
of the side on the move in this position.
**************************************************************************/
	 EvaluateTimer+=(GetTickCount()-timeticker);
	 EvaluateCalls++;
#ifdef POSITIONALEVAL
//PositionalEvalSkip:
	 int Positional;
	 if (Position[MOVER]==WHITE) {
		int Score=WhiteScore-BlackScore;
		int Orig=(Position[WHITEPAWNS]*PAWNVALUE+WhitePieces)-(Position[BLACKPAWNS]*PAWNVALUE+BlackPieces);
		Positional=Score-Orig;
		if (Positional>MaxPositional) MaxPositional=Positional;
		if (Positional<MinPositional) MinPositional=Positional;
	 } else {
		int Score=BlackScore-WhiteScore;
		int Orig=(Position[BLACKPAWNS]*PAWNVALUE+BlackPieces)-(Position[WHITEPAWNS]*PAWNVALUE+WhitePieces);
		Positional=Score-Orig;
		if (Positional>MaxPositional) MaxPositional=Positional;
		if (Positional<MinPositional) MinPositional=Positional;
	 }
	 int Segment=(Positional+3000)/100;
	 if (Segment<0) Segment=0;
    if (Segment>60) Segment=60;
	 Segments[Segment]++;
	 if (Positional>1000) {
		TPCount++;
		TPTotal+=Positional;
	 }
	 if (abs(Positional-MaxPositional)<50 || abs(Positional-MinPositional)<50) {
		char s[500];
		int p[89];
		for (int x=0; x<89; x++) {
			p[x]=Position[x];
		}
		TChessBoard* a = new TChessBoard(p);
		a->GetFEN(s);
		FILE* f=fopen("c:\\log.txt", "a");
		fprintf(f, "Positional %i, FEN: %s\n",	Positional, s);
		fclose(f);
		delete a;
	 }
#endif
	 if (Position[MOVER]==WHITE) {
		return (int)((WhiteScore-BlackScore)*0.1);
	 } else {
		return (int)((BlackScore-WhiteScore)*0.1);
	 }
}

int pascal
TSearcher::IsCheck(TPosition Position)
{
	int i;
	POSITIONELEMENT* KingPointer;
	int KingSquare;
	int KingX, KingY;
	if (Position[MOVER]==WHITE)
	{
		KingSquare=Position[WKINGPOSITION];
		KingPointer=Position+KingSquare;
		  KingX=GetFile[KingSquare];
		  KingY=GetRank[KingSquare];

/* Attacking Black Knight? */
		if (Position[BLACKKNIGHTS]) {
		  if (KingX>2 && KingY>1 && *(KingPointer-21)==BN) goto InCheck;
		  if (KingX>2 && KingY<8 && *(KingPointer-19)==BN) goto InCheck;
		  if (KingX>1 && KingY>2 && *(KingPointer-12)==BN) goto InCheck;
		  if (KingX>1 && KingY<7 && *(KingPointer-8)==BN) goto InCheck;
		  if (KingX<7 && KingY<8 && *(KingPointer+21)==BN) goto InCheck;
		  if (KingX<7 && KingY>1 && *(KingPointer+19)==BN) goto InCheck;
		  if (KingX<8 && KingY<7 && *(KingPointer+12)==BN) goto InCheck;
		  if (KingX<8 && KingY>2 && *(KingPointer+8)==BN) goto InCheck;
		}
/* Attacking Black Pawn? */
		if (KingX>1 && KingY<8 && *(KingPointer-9)==BP) goto InCheck;
		if (KingX<8 && KingY<8 && *(KingPointer+11)==BP) goto InCheck;
/* Attacking Black Rook or Queen? */
		if (Position[BLACKROOKS] || Position[BLACKQUEENS]) {
		 for (i=KingSquare+1; Valid[i]; i++) if (Position[i]!=EMPTY) if (Position[i]==BQ || Position[i]==BR) goto InCheck; else break;
		 for (i=KingSquare-1; Valid[i]; i--) if (Position[i]!=EMPTY) if (Position[i]==BQ || Position[i]==BR) goto InCheck; else break;
		 for (i=KingSquare+10; i<90; i+=10) if (Position[i]!=EMPTY) if (Position[i]==BQ || Position[i]==BR) goto InCheck; else break;
		 for (i=KingSquare-10; i>9; i-=10) if (Position[i]!=EMPTY) if (Position[i]==BQ || Position[i]==BR) goto InCheck; else break;
		}
/* Attacking Black Bishop or Queen? */
		if (Position[BLACKBISHOPS] || Position[BLACKQUEENS]) {
		 for (i=KingSquare+11; Valid[i]; i+=11) if (Position[i]!=EMPTY) if (Position[i]==BQ || Position[i]==BB) goto InCheck; else break;
		 for (i=KingSquare-11; Valid[i]; i-=11) if (Position[i]!=EMPTY) if (Position[i]==BQ || Position[i]==BB) goto InCheck; else break;
		 for (i=KingSquare+9; Valid[i]; i+=9) if (Position[i]!=EMPTY) if (Position[i]==BQ || Position[i]==BB) goto InCheck; else break;
		 for (i=KingSquare-9; Valid[i]; i-=9) if (Position[i]!=EMPTY) if (Position[i]==BQ || Position[i]==BB) goto InCheck; else break;
		}
/* Attacking Black King */
		i=KingSquare-Position[BKINGPOSITION];
		if (i==-11 || i==-9 || i==+9 || i==+11 || i==-10 || i==-1 || i==+10 || i==+1)
		  goto InCheck;
	}
	if (Position[MOVER]==BLACK)
	{
		KingSquare=Position[BKINGPOSITION];
		KingPointer=Position+KingSquare;
		KingX=GetFile[KingSquare];
		KingY=GetRank[KingSquare];

/* Attacking White Knight? */
		if (Position[WHITEKNIGHTS]) {
		  if (KingX>2 && KingY>1 && *(KingPointer-21)==WN) goto InCheck;
		  if (KingX>2 && KingY<8 && *(KingPointer-19)==WN) goto InCheck;
		  if (KingX>1 && KingY>2 && *(KingPointer-12)==WN) goto InCheck;
		  if (KingX>1 && KingY<7 && *(KingPointer-8)==WN) goto InCheck;
		  if (KingX<7 && KingY<8 && *(KingPointer+21)==WN) goto InCheck;
		  if (KingX<7 && KingY>1 && *(KingPointer+19)==WN) goto InCheck;
		  if (KingX<8 && KingY<7 && *(KingPointer+12)==WN) goto InCheck;
		  if (KingX<8 && KingY>2 && *(KingPointer+8)==WN) goto InCheck;
		}
/* Attacking White Pawn? */
		if (KingX<8 && KingY>1 && *(KingPointer+9)==WP) goto InCheck;
		if (KingX>1 && KingY>1 &&  *(KingPointer-11)==WP) goto InCheck;
/* Attacking White Rook or Queen? */
		if (Position[WHITEROOKS] || Position[WHITEQUEENS]) {
		 for (i=KingSquare+1; Valid[i]; i++) if (Position[i]!=EMPTY) if (Position[i]==WQ || Position[i]==WR) goto InCheck; else break;
		 for (i=KingSquare-1; Valid[i]; i--) if (Position[i]!=EMPTY) if (Position[i]==WQ || Position[i]==WR) goto InCheck; else break;
		 for (i=KingSquare+10; i<90; i+=10) if (Position[i]!=EMPTY) if (Position[i]==WQ || Position[i]==WR) goto InCheck; else break;
		 for (i=KingSquare-10; i>9; i-=10) if (Position[i]!=EMPTY) if (Position[i]==WQ || Position[i]==WR) goto InCheck; else break;
		}
/* Attacking White Bishop or Queen? */
		if (Position[WHITEBISHOPS] || Position[WHITEQUEENS]) {
		 for (i=KingSquare+11; Valid[i]; i+=11) if (Position[i]!=EMPTY) if (Position[i]==WQ || Position[i]==WB) goto InCheck; else break;
		 for (i=KingSquare-11; Valid[i]; i-=11) if (Position[i]!=EMPTY) if (Position[i]==WQ || Position[i]==WB) goto InCheck; else break;
		 for (i=KingSquare+9; Valid[i]; i+=9) if (Position[i]!=EMPTY) if (Position[i]==WQ || Position[i]==WB) goto InCheck; else break;
		 for (i=KingSquare-9; Valid[i]; i-=9) if (Position[i]!=EMPTY) if (Position[i]==WQ || Position[i]==WB) goto InCheck; else break;
		}
/* Attacking White King */
		i=KingSquare-Position[WKINGPOSITION];
		if (i==-11 || i==-9 || i==+9 || i==+11 || i==-10 || i==-1 || i==+10 || i==+1)
		  goto InCheck;
	}
	return FALSE;
InCheck:
	return TRUE;
}

int
TSearcher::WinnerWhenNoMoves(TPosition Position)
{
	return (IsCheck(Position) ? -1 : 0);
}

int
TSearcher::PreviousPosition(TMove Move)
{
	if (CurrentDepth==0) {
	  for (int i=0; DrawMoves[i].From!=0; i++)
		  if (Move.From==DrawMoves[i].From && Move.To==DrawMoves[i].To)
			  return TRUE;
	  return FALSE;
	}
	if (CurrentDepth==1) {
	  for (int i=0; Ply1DrawMovesPly0[i].From!=0; i++)
		  if (CurrentDepthZeroMove.From==Ply1DrawMovesPly0[i].From &&
				CurrentDepthZeroMove.To==Ply1DrawMovesPly0[i].To &&
				Move.From==Ply1DrawMovesPly1[i].From &&
				Move.To==Ply1DrawMovesPly1[i].To)
				  return TRUE;
	  return FALSE;
	}
	exit(0);
}

void
TSearcher::SetEvaluationParameters(int* PawnRatings)
{
	for (int i=0; i<8; i++)
		PawnValues[i]=PawnRatings[i];
}

void
TSearcher::SetupTropism()
{
	int i, j;
	for (i=0; i<89; i++) {
		GetFile[i]=i/10;
		GetRank[i]=i%10;
		for (j=0; j<89; j++) {
			QueenKingTropism[i][j]=
				(8 * (min(abs(GetX(i)-GetX(j)), abs(GetY(i)-GetY(j)))) );
			BishopKingTropism[i][j]=
				(8 * (min(abs(GetX(i)-GetX(j)), abs(GetY(i)-GetY(j)))) );
			KnightKingTropism[i][j]=
				(12*(5-(abs(GetX(j)-GetX(i))+abs(GetY(j)-GetY(i)))));
			RookKingTropism[i][j]=
				(8 * (min(abs(GetX(i)-GetX(j)), abs(GetY(i)-GetY(j)))) );
			KingKingTropism[i][j]=
				(16*(5-(abs(GetX(j)-GetX(i))+abs(GetY(j)-GetY(i)))));
			TropismNear[i][j]=
				7-(min(abs(GetX(j)-GetX(i)),abs(GetY(j)-GetY(i))));
			TropismFar[i][j]=
				(min(abs(GetX(j)-GetX(i)),abs(GetY(j)-GetY(i))));
			SameDiagonal[i][j]=(((abs(i-j)%9)==0) || ((abs(i-j)%11)==0));
			KnightAttacks[i][j]=
			  (	abs(GetX(i)-GetX(j)==2) || abs(GetY(i)-GetY(j)==1) ||
					abs(GetX(i)-GetX(j)==1) || abs(GetY(i)-GetY(j)==2));
		}
	}
	for (i=1; i<8; i++) {
		for (j=1; j<8; j++) {
			BlackPawnAdvance[i*10+j]=WhitePawnAdvance[(9-i)*10+(9-j)];
		}
	}
#ifdef POSITIONALEVAL
	for (i=0; i<=60; i++) Segments[i]=0;
#endif
}

int _pascal
TSearcher::GetQuickMoveList(TPosition InPosition, TMoveArray MoveArray)
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
		for (j=1; j<=Position[WHITEPAWNS]; j++) {
				Square=Position[WHITEPAWNS+j];
				SquarePointer=Position+Square;
				x=GetFile[Square];
				y=GetRank[Square];
				if (y==7) {
					if (*(SquarePointer+1)==EMPTY) CreateMove4(Square, Square+1, QUEEN);
					if (x<8 && *(SquarePointer+11)>EMPTY) CreateMove4(Square, Square+11, QUEEN);
					if (x>1 && *(SquarePointer-9)>EMPTY) CreateMove4(Square, Square-9, QUEEN);
				} else {
					if (x<8 && *(SquarePointer+11)>EMPTY) CreateMove(Square, Square+11);
					if (x>1 && *(SquarePointer-9)>EMPTY) CreateMove(Square, Square-9);
					if (y==5 && x>1 && x-1==Position[ENPAWN]) CreateMove(Square, Square-9);
					if (y==5 && x<8 && x+1==Position[ENPAWN]) CreateMove(Square, Square+11);
					//if (y>6 && *(SquarePointer+1)==EMPTY) CreateMove(Square, Square+1);
				}
		}
		for (j=1; j<=Position[WHITEKNIGHTS]; j++) {
				Square=Position[WHITEKNIGHTS+j];
				SquarePointer=Position+Square;
				x=GetFile[Square];
				y=GetRank[Square];
				if (x<8 && y>2 && *(SquarePointer+8)>EMPTY) CreateMove(Square, Square+8);
				if (x>1 && y<7 && *(SquarePointer-8)>EMPTY) CreateMove(Square, Square-8);
				if (x<8 && y<7 && *(SquarePointer+12)>EMPTY) CreateMove(Square, Square+12);
				if (x>1 && y>2 && *(SquarePointer-12)>EMPTY) CreateMove(Square, Square-12);
				if (x<7 && y>1 && *(SquarePointer+19)>EMPTY) CreateMove(Square, Square+19);
				if (x>2 && y<8 && *(SquarePointer-19)>EMPTY) CreateMove(Square, Square-19);
				if (x<7 && y<8 && *(SquarePointer+21)>EMPTY) CreateMove(Square, Square+21);
				if (x>2 && y>1 && *(SquarePointer-21)>EMPTY) CreateMove(Square, Square-21);
		}
		Square=Position[WKINGPOSITION];
		SquarePointer=Position+Square;
		x=GetFile[Square];
		y=GetRank[Square];
		if (x>1 && *(SquarePointer-10)>EMPTY) CreateMove(Square, Square-10);
		if (x<8 && *(SquarePointer+10)>EMPTY) CreateMove(Square, Square+10);
		if (y>1 && *(SquarePointer-1)>EMPTY) CreateMove(Square, Square-1);
		if (y<8 && *(SquarePointer+1)>EMPTY) CreateMove(Square, Square+1);
		if (x>1 && y>1 && *(SquarePointer-11)>EMPTY) CreateMove(Square, Square-11);
		if (x<8 && y<8 && *(SquarePointer+11)>EMPTY) CreateMove(Square, Square+11);
		if (x>1 && y<8 && *(SquarePointer-9)>EMPTY) CreateMove(Square, Square-9);
		if (x<8 && y>1 && *(SquarePointer+9)>EMPTY) CreateMove(Square, Square+9);
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
		for (j=1; j<=Position[BLACKPAWNS]; j++) {
				Square=Position[BLACKPAWNS+j];
				SquarePointer=Position+Square;
				x=GetFile[Square];
				y=GetRank[Square];
				if (y==2) {
					if (*(SquarePointer-1)==EMPTY) CreateMove4(Square, Square-1, QUEEN);
					if (x<8 && *(SquarePointer+9)<EMPTY) CreateMove4(Square, Square+9, QUEEN);
					if (x>1 && *(SquarePointer-11)<EMPTY) CreateMove4(Square, Square-11, QUEEN);
				} else {
					if (x<8 && *(SquarePointer+9)<EMPTY) CreateMove(Square, Square+9);
					if (x>1 && *(SquarePointer-11)<EMPTY) CreateMove(Square, Square-11);
					if (y==4 && x>1 && x-1==Position[ENPAWN]) CreateMove(Square, Square-11);
					if (y==4 && x<8 && x+1==Position[ENPAWN]) CreateMove(Square, Square+9);
					//if (y<3 && *(SquarePointer-1)==EMPTY) CreateMove4(Square, Square-1);
				}
		}
		for (j=1; j<=Position[BLACKKNIGHTS]; j++) {
				Square=Position[BLACKKNIGHTS+j];
				SquarePointer=Position+Square;
				x=GetFile[Square];
				y=GetRank[Square];
				if (x<8 && y>2 && *(SquarePointer+8)<EMPTY) CreateMove(Square, Square+8);
				if (x>1 && y<7 && *(SquarePointer-8)<EMPTY) CreateMove(Square, Square-8);
				if (x<8 && y<7 && *(SquarePointer+12)<EMPTY) CreateMove(Square, Square+12);
				if (x>1 && y>2 && *(SquarePointer-12)<EMPTY) CreateMove(Square, Square-12);
				if (x<7 && y>1 && *(SquarePointer+19)<EMPTY) CreateMove(Square, Square+19);
				if (x>2 && y<8 && *(SquarePointer-19)<EMPTY) CreateMove(Square, Square-19);
				if (x<7 && y<8 && *(SquarePointer+21)<EMPTY) CreateMove(Square, Square+21);
				if (x>2 && y>1 && *(SquarePointer-21)<EMPTY) CreateMove(Square, Square-21);
		}
		Square=Position[BKINGPOSITION];
		SquarePointer=Position+Square;
		x=GetFile[Square];
		y=GetRank[Square];
		if (x>1 && *(SquarePointer-10)<EMPTY) CreateMove(Square, Square-10);
		if (x<8 && *(SquarePointer+10)<EMPTY) CreateMove(Square, Square+10);
		if (y>1 && *(SquarePointer-1)<EMPTY) CreateMove(Square, Square-1);
		if (y<8 && *(SquarePointer+1)<EMPTY) CreateMove(Square, Square+1);
		if (x>1 && y>1 && *(SquarePointer-11)<EMPTY) CreateMove(Square, Square-11);
		if (x<8 && y<8 && *(SquarePointer+11)<EMPTY) CreateMove(Square, Square+11);
		if (x>1 && y<8 && *(SquarePointer-9)<EMPTY) CreateMove(Square, Square-9);
		if (x<8 && y>1 && *(SquarePointer+9)<EMPTY) CreateMove(Square, Square+9);
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
int _pascal
TSearcher::GetMoveList(TPosition InPosition, TMoveArray MoveArray)
{
	GMLCalls++;
	unsigned long Score;
	timeticker=GetTickCount();
	register POSITIONELEMENT* Position=InPosition;
	int i, u, v;
	POSITIONELEMENT* SquarePointer;
	int x, y;
	Amount=0;
	int LastMovePiece=Position[LASTMOVESQUARE];
	if (Position[MOVER]==WHITE) {
		POSITIONELEMENT* q=Position+WHITEPAWNS;
		for (q+=*q; q!=Position+WHITEPAWNS; q--) {
				SquarePointer=Position+*q;
				x=GetFile[*q];
				y=GetRank[*q];
				if (*(SquarePointer+1)==EMPTY) {
							 if (y==7) {
								CreateMove3(*q, *q+1, QUEEN);
								CreateMove3(*q, *q+1, BISHOP);
								CreateMove3(*q, *q+1, ROOK);
								CreateMove3(*q, *q+1, KNIGHT);
							 } else {
								CreateMove2(*q, *q+1);
							 }
							 if (y==2 && *(SquarePointer+2)==EMPTY) CreateMove2(*q, *q+2);
						  }
						  if (x<8 && *(SquarePointer+11)>EMPTY) {
							 if (y==7) {
								CreateMove3(*q, *q+11, QUEEN);
								CreateMove3(*q, *q+11, BISHOP);
								CreateMove3(*q, *q+11, ROOK);
								CreateMove3(*q, *q+11, KNIGHT);
							 } else {
								CreateMove2(*q, *q+11);
							 }
						  }
						  if (x>1 && *(SquarePointer-9)>EMPTY) {
							 if (y==7) {
								CreateMove3(*q, *q-9, QUEEN);
								CreateMove3(*q, *q-9, BISHOP);
								CreateMove3(*q, *q-9, ROOK);
								CreateMove3(*q, *q-9, KNIGHT);
							 } else {
								CreateMove2(*q, *q-9);
							 }
						  }
						  if (y==5 && x>1 && x-1==Position[ENPAWN]) CreateMove2(*q, *q-9);
						  if (y==5 && x<8 && x+1==Position[ENPAWN]) CreateMove2(*q, *q+11);
		}
		for (q+=(WHITEKNIGHTS-WHITEPAWNS), q+=*q; q!=Position+WHITEKNIGHTS; q--) {
				SquarePointer=Position+*q;
				x=GetFile[*q];
				y=GetRank[*q];
				if (x<8 && y>2 && *(SquarePointer+8)>=EMPTY) CreateMove2(*q, *q+8);
				if (x>1 && y<7 && *(SquarePointer-8)>=EMPTY) CreateMove2(*q, *q-8);
				if (x<8 && y<7 && *(SquarePointer+12)>=EMPTY) CreateMove2(*q, *q+12);
				if (x>1 && y>2 && *(SquarePointer-12)>=EMPTY) CreateMove2(*q, *q-12);
				if (x<7 && y>1 && *(SquarePointer+19)>=EMPTY) CreateMove2(*q, *q+19);
				if (x>2 && y<8 && *(SquarePointer-19)>=EMPTY) CreateMove2(*q, *q-19);
				if (x<7 && y<8 && *(SquarePointer+21)>=EMPTY) CreateMove2(*q, *q+21);
				if (x>2 && y>1 && *(SquarePointer-21)>=EMPTY) CreateMove2(*q, *q-21);
		}
		for (q+=(WHITEBISHOPS-WHITEKNIGHTS), q+=*q; q!=Position+WHITEBISHOPS; q--) {
				for (i=*q+11; Valid[i] ; i+=11)
				  if (Position[i]>=EMPTY) {
					  CreateMove2(*q, i);
					  if (Position[i]!=EMPTY) break;
				  } else break;
				for (i=*q-11; Valid[i] ; i-=11)
				  if (Position[i]>=EMPTY) {
					  CreateMove2(*q, i);
					  if (Position[i]!=EMPTY) break;
				  } else break;
				for (i=*q-9; Valid[i] ; i-=9)
				  if (Position[i]>=EMPTY) {
					  CreateMove2(*q, i);
					  if (Position[i]!=EMPTY) break;
				  } else break;
				for (i=*q+9; Valid[i] ; i+=9)
				  if (Position[i]>=EMPTY) {
					 CreateMove2(*q, i);
					 if (Position[i]!=EMPTY) break;
				  } else break;
				}
		for (q+=(WHITEROOKS-WHITEBISHOPS), q+=*q; q!=Position+WHITEROOKS; q--) {
				for (i=*q+10; i<90 ; i+=10)
				  if (Position[i]>=EMPTY) {
					  CreateMove2(*q, i);
					  if (Position[i]!=EMPTY) break;
				  } else break;
				for (i=*q-10; i>9 ; i-=10)
				  if (Position[i]>=EMPTY) {
					  CreateMove2(*q, i);
					  if (Position[i]!=EMPTY) break;
				  } else break;
				for (i=*q+1; Valid[i] ; i++)
					 if (Position[i]>=EMPTY) {
					  CreateMove2(*q, i);
					  if (Position[i]!=EMPTY) break;
				  } else break;
				for (i=*q-1; Valid[i] ; i--)
					  if (Position[i]>=EMPTY) {
						CreateMove2(*q, i);
						if (Position[i]!=EMPTY) break;
					  } else break;
		}
		for (q+=(WHITEQUEENS-WHITEROOKS), q+=*q; q!=Position+WHITEQUEENS; q--) {
				for (i=*q+10; i<90 ; i+=10)
								  if (Position[i]>=EMPTY) {
									  CreateMove2(*q, i);
									  if (Position[i]!=EMPTY) break;
								  } else break;
						  for (i=*q-10; i>9 ; i-=10)
								  if (Position[i]>=EMPTY) {
									  CreateMove2(*q, i);
									  if (Position[i]!=EMPTY) break;
								  } else break;
						  for (i=*q+1; Valid[i] ; i++)
								  if (Position[i]>=EMPTY) {
									  CreateMove2(*q, i);
									  if (Position[i]!=EMPTY) break;
								  } else break;
						  for (i=*q-1; Valid[i] ; i--)
								  if (Position[i]>=EMPTY) {
									 CreateMove2(*q, i);
									 if (Position[i]!=EMPTY) break;
								  } else break;
						  for (i=*q+11; Valid[i] ; i+=11)
								  if (Position[i]>=EMPTY) {
									  CreateMove2(*q, i);
									  if (Position[i]!=EMPTY) break;
								  } else break;
						  for (i=*q-11; Valid[i] ; i-=11)
								  if (Position[i]>=EMPTY) {
									  CreateMove2(*q, i);
									  if (Position[i]!=EMPTY) break;
								  } else break;
						  for (i=*q-9; Valid[i] ; i-=9)
								  if (Position[i]>=EMPTY) {
									  CreateMove2(*q, i);
									  if (Position[i]!=EMPTY) break;
								  } else break;
						  for (i=*q+9; Valid[i] ; i+=9)
								  if (Position[i]>=EMPTY) {
									 CreateMove2(*q, i);
									 if (Position[i]!=EMPTY) break;
								  } else break;
		}
		q+=(WKINGPOSITION-WHITEQUEENS);
		SquarePointer=Position+*q;
		x=GetFile[*q];
		y=GetRank[*q];
		if (x>1 && *(SquarePointer-10)>=EMPTY) CreateMove2(*q, *q-10);
		if (x<8 && *(SquarePointer+10)>=EMPTY) CreateMove2(*q, *q+10);
		if (y>1 && *(SquarePointer-1)>=EMPTY) CreateMove2(*q, *q-1);
		if (y<8 && *(SquarePointer+1)>=EMPTY) CreateMove2(*q, *q+1);
		if (x>1 && y>1 && *(SquarePointer-11)>=EMPTY) CreateMove2(*q, *q-11);
		if (x<8 && y<8 && *(SquarePointer+11)>=EMPTY) CreateMove2(*q, *q+11);
		if (x>1 && y<8 && *(SquarePointer-9)>=EMPTY) CreateMove2(*q, *q-9);
		if (x<8 && y>1 && *(SquarePointer+9)>=EMPTY) CreateMove2(*q, *q+9);
		if (!Position[WKINGMOVED]) {
			 if (!IsCheck(Position)) {
				if (!Position[WROOK8MOVED]) {
				  if (Position[61]==EMPTY && Position[71]==EMPTY) {
					  Position[61]=WK;
					  Position[51]=EMPTY;
					  Position[WKINGPOSITION]=61;
					  int NoThroughCheck=!IsCheck(Position);
					  Position[51]=WK;
					  Position[61]=EMPTY;
					  Position[WKINGPOSITION]=51;
					  if (NoThroughCheck) CreateMove2(51, 71);
				  }
				}
				if (!Position[WROOK1MOVED]) {
				  if (Position[41]==EMPTY && Position[31]==EMPTY && Position[21]==EMPTY) {
					  Position[41]=WK;
					  Position[51]=EMPTY;
					  Position[WKINGPOSITION]=41;
					  int NoThroughCheck=!IsCheck(Position);
					  Position[51]=WK;
					  Position[41]=EMPTY;
					  Position[WKINGPOSITION]=51;
					  if (NoThroughCheck) CreateMove2(51, 31);
				  }
				}
			 }
		}
	} else {
		POSITIONELEMENT* q=Position+BLACKPAWNS;
		for (q+=*q; q!=Position+BLACKPAWNS; q--) {
				SquarePointer=Position+*q;
				x=GetFile[*q];
				y=GetRank[*q];
				if (*(SquarePointer-1)==EMPTY) {
							  if (y==2)	{
									CreateMove3(*q, *q-1, QUEEN);
									CreateMove3(*q, *q-1, BISHOP);
									CreateMove3(*q, *q-1, ROOK);
									CreateMove3(*q, *q-1, KNIGHT);
							  } else {
									CreateMove2(*q, *q-1);
							  }
							  if (y==7 && *(SquarePointer-2)==EMPTY)
								  CreateMove2(*q, *q-2);
						  }
						  if (x<8 && *(SquarePointer+9)<EMPTY) {
							  if (y==2) {
									CreateMove3(*q, *q+9, QUEEN);
									CreateMove3(*q, *q+9, BISHOP);
									CreateMove3(*q, *q+9, ROOK);
									CreateMove3(*q, *q+9, KNIGHT);
							  } else {
									CreateMove2(*q, *q+9);
							  }
						  }
						  if (x>1 && *(SquarePointer-11)<EMPTY) {
							  if (y==2) {
									CreateMove3(*q, *q-11, QUEEN);
									CreateMove3(*q, *q-11, BISHOP);
									CreateMove3(*q, *q-11, ROOK);
									CreateMove3(*q, *q-11, KNIGHT);
							  } else {
									CreateMove2(*q, *q-11);
							  }
						  }
						  if (y==4 && x>1 && x-1==Position[ENPAWN]) CreateMove2(*q, *q-11);
						  if (y==4 && x<8 && x+1==Position[ENPAWN]) CreateMove2(*q, *q+9);
		}
		for (q+=(BLACKKNIGHTS-BLACKPAWNS), q+=*q; q!=Position+BLACKKNIGHTS; q--) {
				SquarePointer=Position+*q;
				x=GetFile[*q];
				y=GetRank[*q];
				if (x<8 && y>2 && *(SquarePointer+8)<=EMPTY) CreateMove2(*q, *q+8);
				if (x>1 && y<7 && *(SquarePointer-8)<=EMPTY) CreateMove2(*q, *q-8);
				if (x<8 && y<7 && *(SquarePointer+12)<=EMPTY) CreateMove2(*q, *q+12);
				if (x>1 && y>2 && *(SquarePointer-12)<=EMPTY) CreateMove2(*q, *q-12);
				if (x<7 && y>1 && *(SquarePointer+19)<=EMPTY) CreateMove2(*q, *q+19);
				if (x>2 && y<8 && *(SquarePointer-19)<=EMPTY) CreateMove2(*q, *q-19);
				if (x<7 && y<8 && *(SquarePointer+21)<=EMPTY) CreateMove2(*q, *q+21);
				if (x>2 && y>1 && *(SquarePointer-21)<=EMPTY) CreateMove2(*q, *q-21);
		}
		for (q+=(BLACKBISHOPS-BLACKKNIGHTS), q+=*q; q!=Position+BLACKBISHOPS; q--) {
				for (i=*q+11; Valid[i] ; i+=11)
								  if (Position[i]<=EMPTY) {
									  CreateMove2(*q, i);
									  if (Position[i]!=EMPTY) break;
								  } else break;
						  for (i=*q-11; Valid[i] ; i-=11)
								  if (Position[i]<=EMPTY) {
									  CreateMove2(*q, i);
									  if (Position[i]!=EMPTY) break;
								  } else break;
						  for (i=*q-9; Valid[i] ; i-=9)
								  if (Position[i]<=EMPTY) {
									  CreateMove2(*q, i);
									  if (Position[i]!=EMPTY) break;
								  } else break;
						  for (i=*q+9; Valid[i] ; i+=9)
								  if (Position[i]<=EMPTY) {
									 CreateMove2(*q, i);
									 if (Position[i]!=EMPTY) break;
								  } else break;
		}
		for (q+=(BLACKROOKS-BLACKBISHOPS), q+=*q; q!=Position+BLACKROOKS; q--) {
				for (i=*q+10; i<90 ; i+=10)
								  if (Position[i]<=EMPTY) {
									  CreateMove2(*q, i);
									  if (Position[i]!=EMPTY) break;
								  } else break;
						  for (i=*q-10; i>9 ; i-=10)
								  if (Position[i]<=EMPTY) {
									  CreateMove2(*q, i);
									  if (Position[i]!=EMPTY) break;
								  } else break;
						  for (i=*q+1; Valid[i] ; i++)
								  if (Position[i]<=EMPTY) {
									  CreateMove2(*q, i);
									  if (Position[i]!=EMPTY) break;
								  } else break;
						  for (i=*q-1; Valid[i] ; i--)
								  if (Position[i]<=EMPTY) {
									 CreateMove2(*q, i);
									 if (Position[i]!=EMPTY) break;
								  } else break;
		}
		for (q+=(BLACKQUEENS-BLACKROOKS), q+=*q; q!=Position+BLACKQUEENS; q--) {
				for (i=*q+10; i<90 ; i+=10)
								  if (Position[i]<=EMPTY) {
									  CreateMove2(*q, i);
									  if (Position[i]!=EMPTY) break;
								  } else break;
						  for (i=*q-10; i>9 ; i-=10)
								  if (Position[i]<=EMPTY) {
									  CreateMove2(*q, i);
									  if (Position[i]!=EMPTY) break;
								  } else break;
						  for (i=*q+1; Valid[i] ; i++)
								  if (Position[i]<=EMPTY) {
									  CreateMove2(*q, i);
									  if (Position[i]!=EMPTY) break;
								  } else break;
						  for (i=*q-1; Valid[i] ; i--)
								  if (Position[i]<=EMPTY) {
									 CreateMove2(*q, i);
									 if (Position[i]!=EMPTY) break;
								  } else break;
						  for (i=*q+11; Valid[i] ; i+=11)
								  if (Position[i]<=EMPTY) {
									  CreateMove2(*q, i);
									  if (Position[i]!=EMPTY) break;
								  } else break;
						  for (i=*q-11; Valid[i] ; i-=11)
								  if (Position[i]<=EMPTY) {
									  CreateMove2(*q, i);
									  if (Position[i]!=EMPTY) break;
								  } else break;
						  for (i=*q-9; Valid[i] ; i-=9)
								  if (Position[i]<=EMPTY) {
									  CreateMove2(*q, i);
									  if (Position[i]!=EMPTY) break;
								  } else break;
						  for (i=*q+9; Valid[i] ; i+=9)
								  if (Position[i]<=EMPTY) {
									 CreateMove2(*q, i);
									 if (Position[i]!=EMPTY) break;
								  } else break;
		}
		q+=(BKINGPOSITION-BLACKQUEENS);
		SquarePointer=Position+*q;
		x=GetFile[*q];
		y=GetRank[*q];
		if (x>1 && *(SquarePointer-10)<=EMPTY) CreateMove2(*q, *q-10);
		if (x<8 && *(SquarePointer+10)<=EMPTY) CreateMove2(*q, *q+10);
		if (y>1 && *(SquarePointer-1)<=EMPTY) CreateMove2(*q, *q-1);
		if (y<8 && *(SquarePointer+1)<=EMPTY) CreateMove2(*q, *q+1);
		if (x>1 && y>1 && *(SquarePointer-11)<=EMPTY) CreateMove2(*q, *q-11);
		if (x<8 && y<8 && *(SquarePointer+11)<=EMPTY) CreateMove2(*q, *q+11);
		if (x>1 && y<8 && *(SquarePointer-9)<=EMPTY) CreateMove2(*q, *q-9);
		if (x<8 && y>1 && *(SquarePointer+9)<=EMPTY) CreateMove2(*q, *q+9);
		if (!Position[BKINGMOVED]) {
			if (!IsCheck(Position)) {
				if (!Position[BROOK8MOVED]) {
					if (Position[68]==EMPTY && Position[78]==EMPTY) {
						Position[68]=BK;
						Position[58]=EMPTY;
						Position[BKINGPOSITION]=68;
						int NoThroughCheck=!IsCheck(Position);
						Position[58]=BK;
						Position[68]=EMPTY;
						Position[BKINGPOSITION]=58;
						if (NoThroughCheck) CreateMove2(58, 78);
					}
				}
				if (!Position[BROOK1MOVED]) {
					if (Position[48]==EMPTY && Position[38]==EMPTY && Position[28]==EMPTY) {
						Position[48]=BK;
						Position[58]=EMPTY;
						Position[BKINGPOSITION]=48;
						int NoThroughCheck=!IsCheck(Position);
						Position[58]=BK;
						Position[48]=EMPTY;
						Position[BKINGPOSITION]=58;
						if (NoThroughCheck) CreateMove2(58, 38);
					}
				}
			}
		}
	}
	if (followPV) {
		for (i=1; i<=Amount; i++) {
			if (MoveArray[i].From==PrincipalPath.Move[CurrentDepth].From &&
					MoveArray[i].To==PrincipalPath.Move[CurrentDepth].To)  {
					TMove TempMove=MoveArray[i];
					for (int j=i-1; j>=1; j--) {
						MoveArray[j+1]=MoveArray[j];
					}
					MoveArray[1]=TempMove;
			}
		}
	}
	GMLCMCalls+=Amount;
	GetMoveListTimer+=(GetTickCount()-timeticker);
	return Amount;
}

#define KINGDISTANCE 300
#define KINGKINGTROPISM 250
#define KINGPAWNTROPISM 250
#define TWOKNIGHTS 100
#define CORRECTBISHOPCORNER 2000
#define EDGEPAWN 250
#define ENEMYHASOPPOSITION 1500
#define PUSHPAWN 200

int TSearcher::LoneKing(TPosition Position)
{
	int WinningSide;
	int WinningKing;
	int LosingKing;
	int BishopColour=NOCOLOUR;
	int Score=
		Position[WHITEQUEENS]*QUEENVALUE+
		Position[WHITEBISHOPS]*BISHOPVALUE+
		Position[WHITEKNIGHTS]*KNIGHTVALUE+
		Position[WHITEROOKS]*ROOKVALUE+
		Position[WHITEPAWNS]*PAWNVALUE -
		Position[BLACKQUEENS]*QUEENVALUE -
		Position[BLACKBISHOPS]*BISHOPVALUE -
		Position[BLACKKNIGHTS]*KNIGHTVALUE -
		Position[BLACKROOKS]*ROOKVALUE -
		Position[BLACKPAWNS]*PAWNVALUE;

	if (Score==0) {
		return 0;
	} else
	if (Score>0) {
		WinningSide=WHITE;
		WinningKing=Position[WKINGPOSITION];
		LosingKing=Position[BKINGPOSITION];
		if (Score==BISHOPVALUE && Position[WHITEBISHOPS]==1) return 0;
		if (Score==KNIGHTVALUE && Position[WHITEKNIGHTS]==1) return 0;
		if (Score==KNIGHTVALUE*2 && Position[WHITEKNIGHTS]==2) return TWOKNIGHTS;
		if (Score==KNIGHTVALUE+BISHOPVALUE &&
			Position[WHITEKNIGHTS]==1 &&
			Position[WHITEBISHOPS]==1) {
			BishopColour=(
				(GetX(Position[WHITEBISHOPS+1])+GetY(Position[WHITEBISHOPS+1]))%2==0
					? BLACK : WHITE);
		}
	} else {
		WinningSide=BLACK;
		WinningKing=Position[BKINGPOSITION];
		LosingKing=Position[WKINGPOSITION];
		Score=-Score;
		if (Score==BISHOPVALUE && Position[BLACKBISHOPS]==1) return 0;
		if (Score==KNIGHTVALUE && Position[BLACKKNIGHTS]==1) return 0;
		if (Score==KNIGHTVALUE*2 && Position[BLACKKNIGHTS]==2) return TWOKNIGHTS;
		if (Score==KNIGHTVALUE+BISHOPVALUE &&
			Position[BLACKKNIGHTS]==1 &&
			Position[BLACKBISHOPS]==1) {
			BishopColour=(
				(GetX(Position[BLACKBISHOPS+1])+GetY(Position[BLACKBISHOPS+1]))%2==0
					? BLACK : WHITE);
		}
	}

	// force enemy king to edge of board
	Score+=(min(abs(GetX(LosingKing)-4.5), abs(GetY(LosingKing)-4.5))*KINGDISTANCE);
	//Score+=((abs(GetX(LosingKing)-4.5)+
	//			abs(GetY(LosingKing)-4.5))*
	//			KINGDISTANCE);

	// If needed, send enemy king to correct bishop corner
	if ((BishopColour==WHITE && Quadrant[LosingKing]%2==0) ||
		 (BishopColour==BLACK && Quadrant[LosingKing]%2==1)) {
			Score+=((abs(GetX(LosingKing)-4.5)+
						abs(GetY(LosingKing)-4.5))*
						KINGDISTANCE);
	}
	// send friendly king close to enemy king
	Score+=(TropismNear[WinningKing][LosingKing]*KINGKINGTROPISM);

	Score/=10.0;
	EvaluateTimer+=(GetTickCount()-timeticker);
	EvaluateCalls++;
	if (WinningSide==Position[MOVER])
		return Score;
	else
		return -Score;
};

void
TSearcher::SetUseRecaptureExtensions(int use)
{
	UseRecaptureExtensions=use;
}

void
TSearcher::SetUseCheckExtensions(int use)
{
	UseCheckExtensions=use;
}

void
TSearcher::SetUsePawnPushExtensions(int use)
{
	UsePawnPushExtensions=use;
}

void
TSearcher::SetNullMoveReduceDepth(int depth)
{
	NullMoveReduceDepth=depth;
}

void
TSearcher::SetMaxExtensions(int max)
{
	MaxExtensions=max;
}

void
TSearcher::SetAspireWindow(int window)
{
	AspireWindow=window;
}

void
TSearcher::SetNullMoveStopMaterial(int mat)
{
	NullMoveStopMaterial=mat;
}

void
TSearcher::SetHashReadReduce(int red)
{
	HashReadReduce=red;
}

void
TSearcher::SetHashWriteReduce(int red)
{
	HashWriteReduce=red;
}

void
TSearcher::SetPrintPV(int InPrintPV)
{
	PrintPV=InPrintPV;
}

void
TSearcher::SetUseSingleReplyExtensions(int use)
{
	UseSingleReplyExtensions=use;
}

void
TSearcher::WritePV(char* PV, char* PV2)
{
	TMove PathMove;
	char pv[4000];
	strcpy(pv, "");
	int i,j;
	TChessBoard *PathGame;
	switch (VariantID) {
		case CHESS : PathGame=new TChessBoard(PathBoard); break;
		case SHATRANJ : PathGame=new TShatranjBoard(PathBoard); break;
		case KINGLET : PathGame=new TKingletBoard(PathBoard); break;
		case SELFTAKE : PathGame=new TSelfTakeBoard(PathBoard); break;
	}
	for (i=0; PrincipalPath.Move[i].From!=0; i++) {
		PathMove=PrincipalPath.Move[i];
		char s[10];
		PathGame->TranslateMoveToAlgebraic(s, PathMove);
		strcat(pv, s);
		strcat(pv, " ");
		if (strlen(s)>6) s[6]='\0';
		PathGame->MakeMove(PathMove, TRUE);
	}
	delete PathGame;
	FILE *f;
	long t=GetTickCount()-globalstarttime;
	if ((f=fopen( PV, "a" ))!=NULL) {
		fprintf( f, "Sec:%i.%i%i Dpt:%i Val:%i.%i%i Nds:%i PV: %s\n",
					t/1000,
					t%1000/100,
					t%1000%100/10,
					PrincipalPath.Depth,
					PrincipalPath.Value/100,
					abs(PrincipalPath.Value%100/10),
					abs(PrincipalPath.Value%10),
					CurrentPathNodes,
					pv );
		fclose(f);
	}
	if (PV2!=NULL) {
		f = fopen( PV2, "a" );
		fprintf( f, "<BR><font color=FF0000>Sec:%i.%i%i Dpt:%i Val:%i.%i%i Nds:%i<br></font><font color=00FF00>PV: %s</font>\n",
					t/1000,
					t%1000/100,
					t%1000%100/10,
					PrincipalPath.Depth,
					PrincipalPath.Value/100,
					abs(PrincipalPath.Value%100/10),
					abs(PrincipalPath.Value%10),
					CurrentPathNodes,
					pv );
		fclose(f);
	}
}

void
TSearcher::SetStopScore(int Score) {
	StopScore=Score;
}

void
TSearcher::SetStopMove(TMove Move) {
	StopMove=Move;
}

void
TSearcher::SetRandomMoveOrder(BOOL order) {
	RandomMoveOrder=order;
}

