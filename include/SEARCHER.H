#ifndef _SEARCHER_H
#define _SEARCHER_H
#include "chessbrd.h"
#include "searchdf.h"
#include "newadd.h"

#define LOWEVAL -32000
#define HIGHEVAL 32000

#define FAILHIGHERROR 200
#define PAWNVALUE 1000
#define KNIGHTVALUE 3000
#define BISHOPVALUE 3000
#define ROOKVALUE 5000
#define QUEENVALUE 9000
#define DOUBLEDPAWNS 75  // Penalty for each extra pawn on file
#define ISOLATEDPAWN 75
#define PASSEDPAWN 250 // per rank
#define PAWNADVANCEMULTIPLIER 1.5 // Multiply regular advance by this when not in end game
#define PAWNADVANCEENDGAME 3 // Multiply regular advance by this when in end game
#define PASSEDPAWNHOME 500 // per rank
#define DOUBLEDFILEROOKS 80 // for the pair
#define DOUBLEDRANKROOKS 50 // for the pair
#define ROOKONOPENFILE 70
#define ROOKONSEMIOPENFILE 45
#define ROOKSEVENTHRANK 25
#define KINGCASTLENOPOSSIBLE 70
#define QUEENCASTLENOPOSSIBLE 40
#define QUEENOUTEARLY 150
#define NOCASTLEPOSSIBLE KINGCASTLENOPOSSIBLE+QUEENCASTLENOPOSSIBLE
#define RATIOBONUS 10 // Multiply by number of friendly pawns (>3 ? 3) and piece difference
#define ROOKFILEMOBILITY 5 // per square
#define ROOKRANKMOBILITY 5 // per square
#define QUEENMOBILITY 10 // per square
#define BISHOPMOBILITY 15 // per square
#define ENDGAMEPIECES ROOKVALUE+ROOKVALUE+KNIGHTVALUE+BISHOPVALUE
#define UNDEVELOPEDMINOR 35
#define CENTRALOCCUPATION 25 // e.g. rook vs knight = 2*MOVEKINGNEAR*(14-(rank distance + file distance))
#define MOVEKINGNEAR 0 // multiplied by [0 to 7]
#define OPPOSITION2 50
#define OPPOSITION4 30
#define OPPOSITION6 10
#define MINORKINGSAFETYUNIT 25
#define KINGCENTRE 10
#define KINGSAFETYUNIT 60
#define KINGSAFETYVERYWEAK KINGSAFETYUNIT
#define KINGSAFETYWEAK KINGSAFETYUNIT*2
#define KINGSAFETYQUITEWEAK KINGSAFETYUNIT*3
#define KINGSAFETYQUITESTRONG KINGSAFETYUNIT*4
#define KINGSAFETYSTRONG KINGSAFETYUNIT*5
#define KINGSAFETYVERYSTRONG KINGSAFETYUNIT*6

#define UNKNOWN 0
#define NULLMOVE 1
#define PAWNPUSH 2
#define ONLYMOVE 4
#define PUTINCHECK 8
#define CAPTURE 16

#define BASECAPTURE 2000L
#define HASHMOVEBONUS 6002L
#define LASTMOVEPIECEBONUS 1000L //5001L
#define HISTORYSCOREMAX 6000L
#define MAXHISTORYDEPTH 10

const int TwoPowers[15]= {
	1,
	2,
	4,
	8,
	16,
	32,
	64,
	128,
	256,
	512,
	1024,
	2048,
	4096,
	8192,
	16384
};

const int Valid[110]=
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	  0, 1, 1, 1, 1, 1, 1, 1, 1, 0,
	  0, 1, 1, 1, 1, 1, 1, 1, 1, 0,
	  0, 1, 1, 1, 1, 1, 1, 1, 1, 0,
	  0, 1, 1, 1, 1, 1, 1, 1, 1, 0,
	  0, 1, 1, 1, 1, 1, 1, 1, 1, 0,
	  0, 1, 1, 1, 1, 1, 1, 1, 1, 0,
	  0, 1, 1, 1, 1, 1, 1, 1, 1, 0,
	  0, 1, 1, 1, 1, 1, 1, 1, 1, 0,
	  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	  0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

const int TypeIndexes[107]=
	{ 0,
	  WHITEPAWNS,
	  WHITEKNIGHTS,
	  WHITEBISHOPS,
	  WHITEROOKS,
	  WHITEQUEENS,
	  WHITEKINGS,
	  0, 0, 0, 0,
	  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	  BLACKPAWNS,
	  BLACKKNIGHTS,
	  BLACKBISHOPS,
	  BLACKROOKS,
	  BLACKQUEENS,
	  BLACKKINGS };


const int A1H8Diagonal[89]=
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	  0, 1, 0, 0, 0, 0, 0, 0, 0, 0,
	  0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
	  0, 0, 0, 1, 0, 0, 0, 0, 0, 0,
	  0, 0, 0, 0, 1, 0, 0, 0, 0, 0,
	  0, 0, 0, 0, 0, 1, 0, 0, 0, 0,
	  0, 0, 0, 0, 0, 0, 1, 0, 0, 0,
	  0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
	  0, 0, 0, 0, 0, 0, 0, 0, 1 };

const int A8H1Diagonal[89]=
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	  0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
	  0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
	  0, 0, 0, 0, 0, 0, 1, 0, 0, 0,
	  0, 0, 0, 0, 0, 1, 0, 0, 0, 0,
	  0, 0, 0, 0, 1, 0, 0, 0, 0, 0,
	  0, 0, 0, 1, 0, 0, 0, 0, 0, 0,
	  0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
	  0, 1, 0, 0, 0, 0, 0, 0, 0 };

const int A2G8Diagonal[89]=
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	  0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
	  0, 0, 0, 1, 0, 0, 0, 0, 0, 0,
	  0, 0, 0, 0, 1, 0, 0, 0, 0, 0,
	  0, 0, 0, 0, 0, 1, 0, 0, 0, 0,
	  0, 0, 0, 0, 0, 0, 1, 0, 0, 0,
	  0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
	  0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
	  0, 0, 0, 0, 0, 0, 0, 0, 0 };

const int A7G1Diagonal[89]=
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	  0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
	  0, 0, 0, 0, 0, 0, 1, 0, 0, 0,
	  0, 0, 0, 0, 0, 1, 0, 0, 0, 0,
	  0, 0, 0, 0, 1, 0, 0, 0, 0, 0,
	  0, 0, 0, 1, 0, 0, 0, 0, 0, 0,
	  0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
	  0, 1, 0, 0, 0, 0, 0, 0, 0, 0,
	  0, 0, 0, 0, 0, 0, 0, 0, 0 };

const int B1H7Diagonal[89]=
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	  0, 1, 0, 0, 0, 0, 0, 0, 0, 0,
	  0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
	  0, 0, 0, 1, 0, 0, 0, 0, 0, 0,
	  0, 0, 0, 0, 1, 0, 0, 0, 0, 0,
	  0, 0, 0, 0, 0, 1, 0, 0, 0, 0,
	  0, 0, 0, 0, 0, 0, 1, 0, 0, 0,
	  0, 0, 0, 0, 0, 0, 0, 1, 0 };

const int B8H2Diagonal[89]=
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	  0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
	  0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
	  0, 0, 0, 0, 0, 0, 1, 0, 0, 0,
	  0, 0, 0, 0, 0, 1, 0, 0, 0, 0,
	  0, 0, 0, 0, 1, 0, 0, 0, 0, 0,
	  0, 0, 0, 1, 0, 0, 0, 0, 0, 0,
	  0, 0, 1, 0, 0, 0, 0, 0, 0 };

const int WhitePawnAdvance[88]=
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	  0, 0, 0, 5*PAWNADVANCEMULTIPLIER, 10*PAWNADVANCEMULTIPLIER, 15*PAWNADVANCEMULTIPLIER, 20*PAWNADVANCEMULTIPLIER, 25*PAWNADVANCEMULTIPLIER, 0, 0,
	  0, 0, 0, 5*PAWNADVANCEMULTIPLIER, 10*PAWNADVANCEMULTIPLIER, 30*PAWNADVANCEMULTIPLIER, 40*PAWNADVANCEMULTIPLIER, 50*PAWNADVANCEMULTIPLIER, 0, 0,
	  0, 0, 0, 20*PAWNADVANCEMULTIPLIER, 30*PAWNADVANCEMULTIPLIER, 90*PAWNADVANCEMULTIPLIER, 110*PAWNADVANCEMULTIPLIER, 135*PAWNADVANCEMULTIPLIER, 0, 0,
	  0, 0, 0, 35*PAWNADVANCEMULTIPLIER, 80*PAWNADVANCEMULTIPLIER, 120*PAWNADVANCEMULTIPLIER, 150*PAWNADVANCEMULTIPLIER, 180*PAWNADVANCEMULTIPLIER, 0, 0,
	  0, 0, 0, 35*PAWNADVANCEMULTIPLIER, 80*PAWNADVANCEMULTIPLIER, 120*PAWNADVANCEMULTIPLIER, 150*PAWNADVANCEMULTIPLIER, 180*PAWNADVANCEMULTIPLIER, 0, 0,
	  0, 0, 0, 20*PAWNADVANCEMULTIPLIER, 30*PAWNADVANCEMULTIPLIER, 90*PAWNADVANCEMULTIPLIER, 110*PAWNADVANCEMULTIPLIER, 135*PAWNADVANCEMULTIPLIER, 0, 0,
	  0, 0, 0, 5*PAWNADVANCEMULTIPLIER, 10*PAWNADVANCEMULTIPLIER, 30*PAWNADVANCEMULTIPLIER, 40*PAWNADVANCEMULTIPLIER, 50*PAWNADVANCEMULTIPLIER, 0, 0,
	  0, 0, 0, 5*PAWNADVANCEMULTIPLIER, 10*PAWNADVANCEMULTIPLIER, 15*PAWNADVANCEMULTIPLIER, 20*PAWNADVANCEMULTIPLIER, 25*PAWNADVANCEMULTIPLIER };

int BlackPawnAdvance[88];

const int WhiteSeventhRank[89]=
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	  0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
	  0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
	  0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
	  0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
	  0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
	  0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
	  0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
	  0, 0, 0, 0, 0, 0, 0, 1, 0 };

const int BlackSeventhRank[89]=
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	  0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
	  0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
	  0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
	  0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
	  0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
	  0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
	  0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
	  0, 0, 1, 0, 0, 0, 0, 0, 0 };

const int WhiteBackRank[89]=
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	  0, 1, 0, 0, 0, 0, 0, 0, 0, 0,
	  0, 1, 0, 0, 0, 0, 0, 0, 0, 0,
	  0, 1, 0, 0, 0, 0, 0, 0, 0, 0,
	  0, 1, 0, 0, 0, 0, 0, 0, 0, 0,
	  0, 1, 0, 0, 0, 0, 0, 0, 0, 0,
	  0, 1, 0, 0, 0, 0, 0, 0, 0, 0,
	  0, 1, 0, 0, 0, 0, 0, 0, 0, 0,
	  0, 1, 0, 0, 0, 0, 0, 0, 0 };

const int BlackBackRank[89]=
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	  0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
	  0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
	  0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
	  0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
	  0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
	  0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
	  0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
	  0, 0, 0, 0, 0, 0, 0, 0, 1 };

const int Quadrant[89]=
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	  0, 1, 1, 1, 1, 2, 2, 2, 2, 0,
	  0, 1, 1, 1, 1, 2, 2, 2, 2, 0,
	  0, 1, 1, 1, 1, 2, 2, 2, 2, 0,
	  0, 1, 1, 1, 1, 2, 2, 2, 2, 0,
	  0, 4, 4, 4, 4, 3, 3, 3, 3, 0,
	  0, 4, 4, 4, 4, 3, 3, 3, 3, 0,
	  0, 4, 4, 4, 4, 3, 3, 3, 3, 0,
	  0, 4, 4, 4, 4, 3, 3, 3, 3 };

const int CornerSquare[89]=
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	  0, 1, 1, 0, 0, 0, 0, 1, 1, 0,
	  0, 1, 1, 0, 0, 0, 0, 1, 1, 0,
	  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	  0, 1, 1, 0, 0, 0, 0, 1, 1, 0,
	  0, 1, 1, 0, 0, 0, 0, 1, 1 };

const int CentreKing[89]=
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	  0, 0, 1, 1, 1, 1, 1, 1, 0, 0,
	  0, 0, 1, 5, 5, 5, 5, 1, 0, 0,
	  0, 0, 1, 5, 10, 10, 5, 1, 0, 0,
	  0, 0, 1, 5, 10, 10, 5, 1, 0, 0,
	  0, 0, 1, 5, 5, 5, 5, 1, 0, 0,
	  0, 0, 1, 1, 1, 1, 1, 1, 0, 0,
	  0, 0, 0, 0, 0, 0, 0, 0, 0 };

const int knightcontrol[89]={
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, -128,-96, -164, -132, -132, -164, -96, -128, 0,
	0, -96, -64, -32,  0,  0, -32, -64, -96, 0,
	0, -64, -32,  32,  32,  32,  32, -32, -64, 0,
	0, -32,  0,  32,  32,  32,  32,  0, -32, 0,
	0, -32,  0,  32,  32,  32,  32,  0, -32, 0,
	0, -64, -32,  32,  32,  32,  32, -32, -64, 0,
	0, -96, -64, -32,  0,  0, -32, -64, -96, 0,
	0, -128,-96, -164, -132, -132, -164, -96, -128
};

#define MOVEARRAYMAX 200
#define SATISFACTION 75
#define MINIMALWINDOW
#define HASHTABLE

#define INCREASECONVERGER 15
#define MAXDEPTH 100

#define VALID 0
#define LBOUND 1
#define UBOUND 2

struct TPath
{
	TMove Move[MAXDEPTH];
	int Value;
	int Nodes;
	char Depth;
};

#define SIZEOFPATH MAXDEPTH*SIZEOFMOVE+10

typedef TMove TMoveArray[MOVEARRAYMAX];


class TSearcher {
	public:
		TSearcher(int);
      ~TSearcher();
		void SetMaxDepth(int);
		void SetMaxQuiesceDepth(int);
		void SetMaxExtensions(int);
		void SetAspireWindow(int);
		void SetUseHistory(int);
      void SetUseSingleReplyExtensions(int);
		void SetNullMoveStopMaterial(int);
		void SetHashReadReduce(int);
		void SetHashWriteReduce(int);
		void SetUseNullMove(int);
		void SetUseFailHighReductions(int);
		void SetUseRecaptureExtensions(int);
		void SetUseCheckExtensions(int);
		void SetUsePawnPushExtensions(int);
		void SetNullMoveReduceDepth(int);
		void SetUseMinimalWindow(int);
		int CalculateHashsize(int);
		virtual void SetEvaluationParameters(int*);
		void SetContempt(int);
      void SetPrintPV(int);
		int Satisfied;
		virtual void SetInitialPosition(TChessBoard);
		void ClearHashTable();
		void StaleHashTable();
		void ClearHistory();
		void ExitWithoutMove();
		void ExitWithMove();
      int CalculateHashSize(int);
		void GeneratePieceValues();
		int GetCurrentPathNodes();
		TPath GetPrincipalPath();
		TPath Search();
		TPath SolveSearch();
		void TranslateMove(TMove, char*);
		void SetStopScore(int);
		void SetStopMove(TMove);
		void SetRandomMoveOrder(BOOL);
		void WritePV(char*, char*);
	protected:
		int VariantID;
		int MaxExtensions;
		int AspireWindow;
		int UseRecaptureExtensions;
		int UseCheckExtensions;
		int UsePawnPushExtensions;
		int UseSingleReplyExtensions;
		int NullMoveReduceDepth;
		int MinimumMate;
		int LastMoveNumber;
		int RootMaterial;
		int KingSafety(TPosition);
		int HashPower;
		int PrintPV;
		unsigned int NumberOfHashEntries;
		int UseNullMove;
      int UseFailHighReductions;
		int Amount;
		int followPV;
		int LoneKing(TPosition Position);
		//int LoneKingAgainstPawns(TPosition Position);
		void SetupTropism();
		int PawnValues[8];
		TMove DrawMoves[25];
		TMove Ply1DrawMovesPly0[25];
		TMove Ply1DrawMovesPly1[25];
		int PieceValues[107];
		int PieceValuesDiv10[107];
		int QueenKingTropism[89][89];
		int BishopKingTropism[89][89];
		int KnightKingTropism[89][89];
		int RookKingTropism[89][89];
		int KingKingTropism[89][89];
		int TropismNear[89][89];
		int TropismFar[89][89];
		int KnightAttacks[89][89];
		BOOL SameDiagonal[89][89];
		virtual int UseAllMovesOnQuiesce(TPosition);
		virtual int VerifyMove(TPosition, TMove);
		int PieceSquareValues[89][12];
		unsigned long LockPieceSquareValues[89][12];
		unsigned long History[89][89];
		THashMove* HashTable;
		virtual inline int pascal IsCheck(TPosition);
		inline int ComputeHashKey(TPosition);
		LockStruct EncodePosition(TPosition Position);
		virtual int _pascal GetMoveList(TPosition, TMoveArray);
		virtual int _pascal GetQuickMoveList(TPosition, TMoveArray);
		virtual int _pascal MakeMove(TPosition, TMove&, TPosition);
		virtual int pascal Evaluate(TPosition, int, int);
		virtual int WinnerWhenNoMoves(TPosition);
		virtual void _pascal GetHashMoveDetails(TPosition, int& Height, int& Flag, TMove&, int& Value);
		virtual void _pascal StoreHashMoveDetails(TPosition, int, int, TMove, int);
		TPath PrincipalPath;
		int CurrentPathNodes;
		TMove CurrentDepthZeroMove;
		virtual int PreviousPosition(TMove);
		int ExitCode;
		TPosition StartPosition;
		TMoveArray Depth0Moves;
		int GetFile[89];
		int GetRank[89];
		TMove GlobalHashMove;
		TMove TempMove;
		int CurrentDepth;
		TMove LastMove[MAXDEPTH];
		int PathBoard[89];
		int UseHistory;
   	BOOL RandomMoveOrder;
	private:
		TMove StopMove;
		int StopScore;
		int ValueGuess;
		int NullMoveStopMaterial;
		int HashReadReduce, HashWriteReduce;
		TPosition NewPosition[MAXDEPTH];
		TMoveArray Moves[MAXDEPTH];
		void _pascal Minimax(TPosition, unsigned int, int, int, TPath*, unsigned int);
		void _pascal MinimaxZero(TPosition, unsigned int, int, int, TPath*, unsigned int);
		void _pascal SolveForMate(TPosition, unsigned int, int, int, TPath*);
		void _pascal Quiesce(TPosition, unsigned int, int, int, TPath*, unsigned int);
		int MaxDepth;
		int MaxQuiesceDepth;
		int FinalDepth;
		int FinalQuiesceDepth;
		int LowEval;
		int HighEval;
		int Depth0Amount;
		int Contempt, UseMinimalWindow;
		void ReOrderMoves(TMoveArray, int);
		int PIBest;
};
#endif

#define \
AlterLocationInformation    \
{ \
/* Get an index to the locations for the type of piece that is moving */\
	TypeIndex=TypeIndexes[*FromPiece]; \
/* Find moving piece in location array */   \
	PieceIndex=TypeIndex;                  \
	while (NewPosition[++PieceIndex]!=Move.From); \
/* if this is a pawn promotion then delete location and add new piece at Move.To */ \
	if ((*FromPiece==WP && GetRank[Move.To]==8) || (*FromPiece==BP && GetRank[Move.To]==1)) {\
		/* Put last location in this position and reduce count */ \
		NewPosition[PieceIndex]=Position[TypeIndex+Position[TypeIndex]]; \
		NewPosition[TypeIndex]--; \
		if (*FromPiece==WP) {\
			switch (Move.PromotionPiece) { \
				case QUEEN 	: NewPosition[WHITEQUEENS]++; NewPosition[WHITEQUEENS+NewPosition[WHITEQUEENS]]=Move.To; break; \
				case KNIGHT : NewPosition[WHITEKNIGHTS]++; NewPosition[WHITEKNIGHTS+NewPosition[WHITEKNIGHTS]]=Move.To; break; \
				case BISHOP : NewPosition[WHITEBISHOPS]++; NewPosition[WHITEBISHOPS+NewPosition[WHITEBISHOPS]]=Move.To; break; \
				case ROOK 	: NewPosition[WHITEROOKS]++; NewPosition[WHITEROOKS+NewPosition[WHITEROOKS]]=Move.To; break; \
				case KING 	: NewPosition[WHITEKINGS]++; NewPosition[WHITEKINGS+NewPosition[WHITEKINGS]]=Move.To; break; \
			} \
		} else { \
			switch (Move.PromotionPiece) { \
				case QUEEN 	: NewPosition[BLACKQUEENS]++; NewPosition[BLACKQUEENS+NewPosition[BLACKQUEENS]]=Move.To; break; \
				case KNIGHT : NewPosition[BLACKKNIGHTS]++; NewPosition[BLACKKNIGHTS+NewPosition[BLACKKNIGHTS]]=Move.To; break; \
				case BISHOP : NewPosition[BLACKBISHOPS]++; NewPosition[BLACKBISHOPS+NewPosition[BLACKBISHOPS]]=Move.To; break; \
				case ROOK 	: NewPosition[BLACKROOKS]++; NewPosition[BLACKROOKS+NewPosition[BLACKROOKS]]=Move.To; break; \
				case KING 	: NewPosition[BLACKKINGS]++; NewPosition[BLACKKINGS+NewPosition[BLACKKINGS]]=Move.To; break; \
			} \
		}\
	} else {\
/* otherwise, change location to Move.To */\
		NewPosition[PieceIndex]=Move.To;\
	}                                   \
/* determine the square index of the captured piece, 0 if none */\
	int CaptureSquare=0;                                          \
	if (*ToPiece!=EMPTY) CaptureSquare=Move.To;          \
	if (*FromPiece==WP && Move.To-Move.From!=1 && Move.To-Move.From!=2 && CaptureSquare==0) CaptureSquare=Move.To-1;\
	if (*FromPiece==BP && Move.From-Move.To!=1 && Move.From-Move.To!=2 && CaptureSquare==0) CaptureSquare=Move.To+1;\
/* if there was a capture then remove piece location */\
	if (CaptureSquare!=0) {                             \
/* determine piece type array location */              \
		TypeIndex=TypeIndexes[Position[CaptureSquare]];    \
/* find captured piece in locations array */            \
		PieceIndex=TypeIndex+1;                           \
		while (Position[PieceIndex]!=CaptureSquare) {      \
			PieceIndex++;                                   \
		} \
/* remove location */ \
/* replace with whichever piece is at the end of the list */ \
/*	and reduce the list size figure by one */ \
		NewPosition[PieceIndex]=NewPosition[TypeIndex+NewPosition[TypeIndex]];\
  /*		NewPosition[TypeIndex+NewPosition[TypeIndex]]=0;  BOLL */ \
		NewPosition[TypeIndex]--;\
	} \
/* if castling move then change position of rook involved */\
	if (*FromPiece==WK) { \
		if (Move.From==51) { \
			if (Move.To==71) { \
				PieceIndex=WHITEROOKS+1;\
				while (Position[PieceIndex]!=81) {\
					PieceIndex++; \
				} \
				NewPosition[PieceIndex]=61;\
			}\
			if (Move.To==31) {\
				PieceIndex=WHITEROOKS+1;\
				while (Position[PieceIndex]!=11) {\
					PieceIndex++;\
				}\
				NewPosition[PieceIndex]=41;\
			}\
		}    \
	}        \
	if (*FromPiece==BK) {\
		if (Move.From==58) {        \
		  if (Move.To==78) {         \
				PieceIndex=BLACKROOKS+1; \
				while (Position[PieceIndex]!=88) {\
					PieceIndex++;                  \
				}\
				NewPosition[PieceIndex]=68;\
		  }                               \
		  if (Move.To==38) {               \
				PieceIndex=BLACKROOKS+1;      \
				while (Position[PieceIndex]!=18) {\
					PieceIndex++;                  \
				}\
				NewPosition[PieceIndex]=48;\
		  }\
		}  \
	}     \
}        \

#define CreateMove(f, t) \
{ \
  MoveArray[++Amount].From=(f); \
  MoveArray[Amount].To=(t); \
  MoveArray[Amount].PromotionPiece=EMPTY; \
}

#define CreateMove4(f, t, promote) \
{ \
  MoveArray[++Amount].From=(f); \
  MoveArray[Amount].To=(t); \
  MoveArray[Amount].PromotionPiece=promote;\
}


#define CreateMove2(f, t) \
{ \
	if (RandomMoveOrder) { \
		Score=random(1000);  \
	} else { \
	  Score=(UseHistory ? History[(f)][(t)] : 0); \
	  if (Position[(t)]!=EMPTY) { \
		  Score+=BASECAPTURE; \
		  if ((t)==LastMovePiece) Score+=LASTMOVEPIECEBONUS; \
		  Score+=(PieceValues[(Position[(t)])]);     \
		  Score-=(PieceValuesDiv10[(Position[(f)])]); \
	  } \
	  if ((f)==GlobalHashMove.From && (t)==GlobalHashMove.To) \
			Score+=HASHMOVEBONUS; \
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
	MoveArray[u].To=(t); \
	MoveArray[u].Score=Score; \
	MoveArray[u].PromotionPiece=EMPTY; \
}

#define CreateMove3(f, t, promote) \
{ \
	if (RandomMoveOrder) {  \
		Score=random(1000); \
	} else { \
	  Score=(UseHistory ? History[(f)][(t)] : 0); \
	  Score+=PieceValues[promote]; \
	  if (Position[(t)]!=EMPTY) { \
		  Score+=BASECAPTURE; \
		  if ((t)==LastMovePiece) Score+=LASTMOVEPIECEBONUS; \
		  Score+=(PieceValues[(Position[(t)])]);     \
		  Score-=(PieceValuesDiv10[(Position[(f)])]); \
	  } \
	  if ((f)==GlobalHashMove.From && (t)==GlobalHashMove.To) \
			Score+=HASHMOVEBONUS; \
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
	MoveArray[u].To=(t); \
	MoveArray[u].Score=Score; \
   MoveArray[u].PromotionPiece=promote; \
}

#define TimeAndYieldCode \
	 static PollCount=0;\
	 if (!WinBoard) {\
	  if (++PollCount==500) {\
		  MSG msg;\
		  if (ExitCode==0 && PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE)){\
					TranslateMessage(&msg);\
					DispatchMessage(&msg);\
		  }\
		  PollCount=0;\
		}\
	 }\
	 static count=0;\
	 if (++count==500) {\
		if (WinBoard) {\
		  long newtime = GetTickCount();\
		  if (newtime-oldtime>1000 && showAnalysis) {\
				oldtime=newtime;\
				char pv[50], s[10];\
				strcpy(pv, " ");\
				for (int i=0; PrincipalPath.Move[i].From!=0 && i<8; i++) {\
					TranslateMove(PrincipalPath.Move[i], s);\
					strcat(pv, s);\
				}\
				printf( "%i %i %i %i %s\n", PrincipalPath.Depth, PrincipalPath.Value, (newtime-globalstarttime)/10, CurrentPathNodes, pv );\
				fflush(stdout);\
		  }\
		  if (Pondering) {\
				if (!ExitCode) {\
					DWORD dw;\
					static HANDLE inh = GetStdHandle(STD_INPUT_HANDLE);\
					GetConsoleMode(inh, &dw);\
					PeekNamedPipe(inh, NULL, 0, NULL, &dw, NULL);\
					if (dw!=NULL && dw>0) ExitWithMove(); \
				}\
		  } else {\
			  if (searchmethod==BASEINCREMENT) {\
				 if ((newtime-globalstarttime)>=normalmovetime) {\
					if (!Satisfied) {\
					  if (newtime-globalstarttime>=extendmaxmovetime )\
						 if (PrincipalPath.Move[0].From!=0) ExitWithMove();\
					} else {\
					  if (PrincipalPath.Move[0].From!=0) ExitWithMove();\
					}\
				 }\
			  }\
			  if (searchmethod==CHAMPIONSHIP) {\
				 if ((newtime-globalstarttime)>globalmeansearchtime) {\
					if (Satisfied\
					  || newtime-globalstarttime>=(globalmeansearchtime/4.0*7)\
					  || newtime-globalstarttime>=globalsearchtimeleft) {\
						  if (PrincipalPath.Move[0].From!=0) ExitWithMove();\
					}\
				 }\
			  }\
		  }\
		}\
		count=0;\
	 }\

#define UPDATEHISTORY(from, to) \
	if (UseHistory) { \
		if (Depth<=MAXHISTORYDEPTH && from!=0) { \
			History[(from)][(to)]+=TwoPowers[MAXHISTORYDEPTH-Depth];\
			/*History[(from)][(to)]+=(1 << (MAXHISTORYDEPTH-Depth));*/\
			if (History[(from)][(to)]>HISTORYSCOREMAX) { \
				for (int i=0; i<89; i++) \
					for (int j=0; j<89; j++) \
						History[i][j]/=2; \
			} \
		} \
	}


#define ReverseNewPathValue \
NewPath.Value=-NewPath.Value; \
if (NewPath.Value>9000) NewPath.Value--; \
if (NewPath.Value<-9000) NewPath.Value++;

