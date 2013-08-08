#define TITLEVERSION "Rival Chess v1.8.29 - http://www.redhotpawn.com/rival"
#undef TESTING
#define NEWOPENINGS
#undef STUPIDBUG
#undef NORANDOM

//#define SHAREWARE
#define SPLASH_MILLIS 8000

#include <owl\applicat.h>
#include <owl\decframe.h>
#include <owl\framewin.h>
#include <owl\dc.h>
#include <owl\gdiobjec.h>
#include <owl\opensave.h>
#include <time.h>
#include <wing.h>
#include "wingstuf.h"
#include <cstring.h>
#include <owl\checkbox.h>
#include <process.h>
#include <dir.h>
#include <except.h>
#pragma hdrstop
#include "groutine.h"
#include "cpdialog.h"
#include "newgame.h"
#include "kingletb.h"
#include "kinglser.h"
#include "selftbrd.h"
#include "selftser.h"
#include "shatranb.h"
#include "shatrser.h"
#include "newrival.rh"
#include "newrival.h"
#include "debug.h"
#include "splash.cpp"
#include "setup.h"
#include "defines.h"
#include "promote.h"                            
#include "setupmet.h"
#include "opening.h"
#include "savepgn.h"
#include "resource.h"
#include "chessbrd.h"
#include "chessser.h"
#include "utility.h"
#include "suite.h"

#define TIMERTICKS 500
#define DISPLAYTICKS 1000
#define MINSCORE -32000
#define MAXSCORE +32000

#define PATHNUMQUEEN 21
#define PATHNUMKING 22
#define PATHNUMROOK 23
#define PATHNUMKNIGHT 24
#define PATHNUMBISHOP 25
#define PATHNUMEQUALS 26
#define PATHNUMPLUS 27
#define PATHNUMCASTLE 28
#define PATHNUMMATE 29

TFrameWindow* mWindow;

char CommandLinePGN[MAXFILENAMELENGTH];
char VERBOSE[MAXFILENAMELENGTH];
char SUITESUMMARY[MAXFILENAMELENGTH];
char NODES[MAXFILENAMELENGTH];
char TIMINGS[MAXFILENAMELENGTH];
char OPENINGS[MAXFILENAMELENGTH];
char OPTIONS[MAXFILENAMELENGTH];
char PV[MAXFILENAMELENGTH];
char PV2[MAXFILENAMELENGTH];
char workingDirectory[MAXFILENAMELENGTH];

int HashSuccesses, HashCalls, EvaluateCalls, MakeMoveCalls, GMLCalls, GQMLCalls, GMLCMCalls, GQMLCMCalls, StoreHashMoveCalls;
int GetMoveListTimer, GetQuickMoveListTimer, MakeMoveTimer, EvaluateTimer, GetHashMoveTimer, StoreHashMoveTimer;
int Case1Calls, Case2Calls, Case3Calls, Case4Calls;
int Case1Timer, Case2Timer, Case3Timer, Case4Timer;
long globalNodes, globalDepth;
TMove globalPVMove;

char command[100];

// winboard play does not poll the message queue so must have this
long globalstarttime;
int globalmeansearchtime, globalsearchtimeleft, searchmethod;
int optionsbase, extendmaxmovetime, normalmovetime;
int WinBoard=false;
int Pondering=false, PrintInfo=false, Verbose=false, PrintPV=false;
int hashpower;
int showAnalysis;
TMove BackupPonderMove;
long oldtime;

class TRivalWindow : public TWindow
{
	 public:
		 TRivalWindow();
		 ~TRivalWindow();
	 protected:
		 BOOL CanClose();
		 virtual void Paint(TDC& dc, BOOL, TRect& rect);
		 void SetupWindow();
       int SelectedSquareIndex(int, int);
		 void EvTimer(uint);
		 void CmNewGame();
		 void CmSaveGame();
		 void CmSavePosition();
		 void CmSaveGameAsPGN();
		 void CmLoadGame();
		 void CmLoadPosition();
		 void CmTakeBackOneMove();
		 void CmReplayOneMove();
		 void CmTakeBackAllMoves();
		 void CmReplayAllMoves();
		 void CmPlayBest();
		 void CmStopSearch();
		 void CmOptionToggler(WPARAM);
		 void CmLogsToggler(WPARAM);
		 void CmInvertBoard(WPARAM);
		 void CmDisplayChangeToggler(WPARAM);
		 void CmComputerOptions();
		 void CmCreateOpeningBook();
		 void CmHelp(WPARAM);
		 void CmSetupBoard();
		 void CmSolveForMate();
		 void CmRunTestSuite();
		 void CmVariantSelect(WPARAM);
		 void CmAddPiece(WPARAM);
		 void CmExit();
		 void CmAnalysePGN();
		 void CmResign();
		 void CmOfferDraw();
		 void EvLButtonDown(uint, TPoint&);
		 void RunSuite(char*, int);
		 void RunEPDSuite(char*, int, int, int, int);
		 void TestPosition(char*, char*, int, int, char*, int, int&, int&, int&, int&, int&, int, int);
		 void EvRButtonDown(uint, TPoint&);
		 void ComputerMove();
		 void EvKeyDown(uint, uint, uint);
/*************************************************************************
Command Enabler Function Declarations.
**************************************************************************/
		 void CeSetup(TCommandEnabler&);
		 void CeSolve(TCommandEnabler&);
		 void CeChess(TCommandEnabler&);
		 void CeShatranj(TCommandEnabler&);
		 void CeKinglet(TCommandEnabler&);
		 void CeSelfTake(TCommandEnabler&);
		 void CeTakeBackOneMove(TCommandEnabler&);
		 void CeSaveEnabler(TCommandEnabler&);
		 void CeIsNotInSetup(TCommandEnabler&);
		 void CeReplayOneMove(TCommandEnabler&);
		 void CeTakeBackAllMoves(TCommandEnabler&);
		 void CeReplayAllMoves(TCommandEnabler&);
		 void CeVerbose(TCommandEnabler&);
		 void CeTimings(TCommandEnabler&);
		 void CePV(TCommandEnabler&);
		 void CeInvertBoard(TCommandEnabler&);
		 void CePieceSlide(TCommandEnabler&);
		 void CeMessages(TCommandEnabler&);
		 void CeAutoSave(TCommandEnabler&);
		 void CeFlag(TCommandEnabler&);
		 void CeShowAnalysis(TCommandEnabler&);
		 void CeComputerWhite(TCommandEnabler&);
		 void CeComputerBlack(TCommandEnabler&);
		 void CeIsSearching(TCommandEnabler&);
		 void CeIsNotSearching(TCommandEnabler&);
		 void CeResignOrOfferDraw(TCommandEnabler&);
	 private:
		 void OpenVerboseLogs();
		 void OpenPVLogs();
       void OpenPrintInfoLogs();
		 int TestTime;
		 void SetDefaultDisplayOptions();
		 void LoadPieces();
       void SetDefaultOptions();
		 int WhiteMaterial();
		 int BlackMaterial();
		 BOOL IsDrawPosition();
		 int WhiteFell, BlackFell;
		 int SplitFileNames(char* f, char fs[MAXPGNFILES][MAXFILENAMELENGTH]);
		 void ResetVariables();
		 void WriteCreateLog(char*, BOOL);
		 int GameCount;
		 void DeleteOpeningPositions(int);
		 BOOL TryOpenings;
		 void CacheOpenings();
		 void DrawAnalysisNumber(int Number);
		 int MaxOpeningPositions;
		 TOpeningPosition* OpeningPositions;
		 int CachedOpenings;
		 int OpeningPositionCount;
		 int CreateOpeningBook(TWindow*, char[MAXPGNFILES][MAXFILENAMELENGTH], int, int, int, int, int, int, BOOL, int&, int&, int&, int&, int&, int&, BOOL, char*, int);
		 int StoreOpeningPosition(TChessBoard*, int MatchResult);
		 char piecelist[30];
		 void GetReason(int, char*);
		 void GetOptionSwitch(char*, BOOL&);
		 void GetOptionInteger(char*, int&);
		 void ReadOptions();
       void CheckOptions();
		 void WriteOptions();
		 void ResetClocksToPrevious();
		 void VerifyAndMake(TMove);
		 void AutoSaveGame();
		 void CreateSearcherAndBoard();
		 void MakeMove(TChessBoard*, TMove);
		 void InitialiseSearchParameters();
		 void makeWinboardComputerMove();
		 void WriteXBoard(char *);
		 void SaveGeneticStatus(char *);
		 void LoadGeneticStatus(char *);
		 int Searching, CreatingOpeningBook, TestSuite;
		 int SetupBoard[89];
		 int PathBoard[89];
		 time_t SearchStartTime;
		 TRect* ClockRectangle;
		 TRect* AnalysisRectangle;
		 TRect* MoveBoxRectangle;
		 int CloseRequested, EscapeHit;
		 int ShadowOffset;
		 int Fade;
		 TPopupMenu* BoardPopup;
		 TPopupMenu* AnalysisPopup;
		 TPopupMenu* MoveBoxPopup;
		 TPopupMenu* SetupPopup;
		 TPopupMenu* BackGroundPopup;
		 TChessBoard* Game;
		 TSearcher* Searcher;
		 int ComputerMoveFlag;
		 OptionStruct Options;
		 TPath Path2;
		 int GameOver();
		 BOOL VerifySetupPosition(int);
		 BOOL VerifyChessSetupPosition();
		 BOOL VerifyKingletSetupPosition();
		 BOOL VerifyShatranjSetupPosition();
		 BOOL VerifySelfTakeSetupPosition();
		 void MonitorXBoard();
		 void SendXBoard(TMove);
		 void DrawBackGround();
		 void SlidePiece(TMove);
		 void Slide(TDib*, int, int, int, int, int, int);
		 void DrawPieces();
		 void DrawClocks();
		 void DrawClockNumber(int, int, int, int);
		 void DrawCurrentSearchPath(TPath);
		 void DrawPathNumber(int, int, int, int);
		 void DrawMoveNumber(int, int, int, int);
		 void DrawNumber(int, int, float, int, int, int);
		 void UpdateAllGraphics(int DrawBack=FALSE);
		 void far* ScreenBits;
		 void far* BackGroundBits;
		 TPalette* RivalPalette;
		 char messagetext[6];
		 HINSTANCE h;
		 HDC ScreenDc, BackGroundDc;
		 TDib *WhiteSquare, *BlackSquare;
		 TDib *BackGround;
		 TDib *AnalysisBox;
		 TDib *MoveBox;
		 TDib *Pawn, *Bishop, *Rook, *Knight, *Queen, *King;
		 TDib *NumberDib[10];
		 TDib *PathNums[30];
		 TDib *Colon;
		 int BoardOffsetX, BoardOffsetY;
		 TMove Select;
		 int SearchTimeLeft, MeanSearchTime;
		 void RewriteTheOpeningBook();
		 int GetOpening(TMove&, TMoveList);
		 BOOL InSetup;
		 int SetupX, SetupY; // These are used globally for the separate piece handler, e.g. CmWhiteKnight();
	 DECLARE_RESPONSE_TABLE(TRivalWindow);
};

DEFINE_RESPONSE_TABLE1(TRivalWindow, TWindow)
	 EV_COMMAND(CM_FILE_NEW_GAME, CmNewGame),
	 EV_COMMAND(CM_FILE_SAVE_GAME, CmSaveGame),
	 EV_COMMAND(CM_RESIGN, CmResign),
	 EV_COMMAND(CM_OFFERDRAW, CmOfferDraw),
	 EV_COMMAND(CM_FILE_SAVE_POSITION, CmSavePosition),
	 EV_COMMAND(CM_FILE_SAVE_PGN, CmSaveGameAsPGN),
	 EV_COMMAND(CM_FILE_LOAD_GAME, CmLoadGame),
	 EV_COMMAND(CM_LOADPOSITION, CmLoadPosition),
	 EV_COMMAND(CM_ACTION_TAKE_BACK_ONE, CmTakeBackOneMove),
	 EV_COMMAND(CM_ACTION_REPLAY_ONE, CmReplayOneMove),
	 EV_COMMAND(CM_ACTION_TAKE_BACK_ALL, CmTakeBackAllMoves),
	 EV_COMMAND(CM_ACTION_REPLAY_ALL, CmReplayAllMoves),
	 EV_COMMAND(CM_ACTION_PLAY_BEST, CmPlayBest),
	 EV_COMMAND(CM_ACTION_STOP_SEARCH, CmStopSearch),
	 EV_COMMAND(CM_TESTSUITE, CmRunTestSuite),
	 EV_COMMAND(CM_SOLVEFORMATE, CmSolveForMate),
	 EV_COMMAND(CM_FILEEXIT, CmExit),
	 EV_COMMAND(CM_CREATEOPENINGBOOK, CmCreateOpeningBook),
	 EV_COMMAND_AND_ID(CM_VCHESS, CmVariantSelect),
	 EV_COMMAND_AND_ID(CM_VSHATRANJ, CmVariantSelect),
	 EV_COMMAND_AND_ID(CM_VKINGLET, CmVariantSelect),
	 EV_COMMAND_AND_ID(CM_VSELFTAKE, CmVariantSelect),
	 EV_COMMAND_AND_ID(CM_VCHESS, CmVariantSelect),
	 EV_COMMAND_AND_ID(CM_WHITE_PAWN, CmAddPiece),
	 EV_COMMAND_AND_ID(CM_WHITE_KNIGHT, CmAddPiece),
	 EV_COMMAND_AND_ID(CM_WHITE_BISHOP, CmAddPiece),
	 EV_COMMAND_AND_ID(CM_WHITE_ROOK, CmAddPiece),
	 EV_COMMAND_AND_ID(CM_WHITE_QUEEN, CmAddPiece),
	 EV_COMMAND_AND_ID(CM_WHITE_KING, CmAddPiece),
	 EV_COMMAND_AND_ID(CM_BLACK_PAWN, CmAddPiece),
	 EV_COMMAND_AND_ID(CM_BLACK_KNIGHT, CmAddPiece),
	 EV_COMMAND_AND_ID(CM_BLACK_BISHOP, CmAddPiece),
	 EV_COMMAND_AND_ID(CM_BLACK_ROOK, CmAddPiece),
	 EV_COMMAND_AND_ID(CM_BLACK_QUEEN, CmAddPiece),
	 EV_COMMAND_AND_ID(CM_BLACK_KING, CmAddPiece),
	 EV_COMMAND_AND_ID(CM_CLEAR_BOARD, CmAddPiece),
	 EV_COMMAND_AND_ID(CM_CLEAR_SQUARE, CmAddPiece),
	 EV_COMMAND_AND_ID(CM_EXIT_SETUP, CmAddPiece),
	 EV_COMMAND_AND_ID(CM_ABORT_SETUP, CmAddPiece),
	 EV_COMMAND_AND_ID(CM_HELP_ABOUT, CmHelp),
	 EV_COMMAND_AND_ID(CM_HELP_CONTENTS, CmHelp),
	 EV_COMMAND_AND_ID(CM_OPTIONS_PIECE_SLIDE, CmOptionToggler),
	 EV_COMMAND_AND_ID(CM_OPTIONS_MESSAGES, CmOptionToggler),
	 EV_COMMAND_AND_ID(CM_FILE_AUTOSAVE, CmOptionToggler),
	 EV_COMMAND_AND_ID(CM_FLAG, CmOptionToggler),
	 EV_COMMAND_AND_ID(CM_OPTIONS_SHOW_ANALYSIS, CmOptionToggler),
	 EV_COMMAND_AND_ID(CM_OPTIONS_COMPUTER_WHITE, CmOptionToggler),
	 EV_COMMAND_AND_ID(CM_OPTIONS_COMPUTER_BLACK, CmOptionToggler),
	 EV_COMMAND_AND_ID(CM_OPTIONS_SWITCH_SIDES, CmOptionToggler),
	 EV_COMMAND_AND_ID(CM_OPTIONS_SAVE_OPTIONS, CmOptionToggler),
	 EV_COMMAND_AND_ID(CM_DEFAULT_OPTIONS, CmOptionToggler),
	 EV_COMMAND(CM_OPTIONS_COMPUTER_PLAY_OPTIONS, CmComputerOptions),
	 EV_COMMAND(CM_SETUP_BOARD, CmSetupBoard),
	 EV_COMMAND(CM_SPECIALANALYSE_PGN_FILE, CmAnalysePGN),
	 EV_COMMAND_AND_ID(CM_LOGS_VERBOSE, CmLogsToggler),
	 EV_COMMAND_AND_ID(CM_LOGS_TIMINGS, CmLogsToggler),
	 EV_COMMAND_AND_ID(CM_LOGS_PV, CmLogsToggler),
	 EV_COMMAND_AND_ID(CM_INVERT_BOARD, CmDisplayChangeToggler),
	 EV_COMMAND_AND_ID(CM_CHANGE_CHESSBOARD, CmDisplayChangeToggler),
	 EV_COMMAND_AND_ID(CM_CHANGE_PIECES, CmDisplayChangeToggler),
	 EV_COMMAND_AND_ID(CM_CHANGE_BOX_STYLE, CmDisplayChangeToggler),
	 EV_COMMAND_AND_ID(CM_CHANGE_MOVEBOX_STYLE, CmDisplayChangeToggler),
	 EV_COMMAND_AND_ID(CM_CHANGE_WHITE_COLOUR, CmDisplayChangeToggler),
	 EV_COMMAND_AND_ID(CM_CHANGE_BLACK_COLOUR, CmDisplayChangeToggler),
	 EV_COMMAND_AND_ID(CM_CHANGE_BACKGROUND_COLOUR, CmDisplayChangeToggler),
	 EV_COMMAND_AND_ID(CM_CHANGE_BACKGROUND_STYLE, CmDisplayChangeToggler),
	 EV_COMMAND_AND_ID(CM_CHANGE_ANALYSIS_TEXT, CmDisplayChangeToggler),
	 EV_COMMAND_AND_ID(CM_CHANGE_MOVE_TEXT, CmDisplayChangeToggler),
	 EV_COMMAND_AND_ID(CM_DEFAULT_STYLE_1, CmDisplayChangeToggler),
	 EV_COMMAND_ENABLE(CM_VCHESS, CeChess),
	 EV_COMMAND_ENABLE(CM_VSHATRANJ, CeShatranj),
	 EV_COMMAND_ENABLE(CM_VKINGLET, CeKinglet),
	 EV_COMMAND_ENABLE(CM_VSELFTAKE, CeSelfTake),
	 EV_COMMAND_ENABLE(CM_ACTION_TAKE_BACK_ONE, CeTakeBackOneMove),
	 EV_COMMAND_ENABLE(CM_FILE_SAVE_GAME, CeSaveEnabler),
	 EV_COMMAND_ENABLE(CM_FILE_SAVE_PGN, CeSaveEnabler),
	 EV_COMMAND_ENABLE(CM_SETUP_BOARD, CeSetup),
	 EV_COMMAND_ENABLE(CM_SOLVEFORMATE, CeSolve),
	 EV_COMMAND_ENABLE(CM_ACTION_REPLAY_ONE, CeReplayOneMove),
	 EV_COMMAND_ENABLE(CM_ACTION_TAKE_BACK_ALL, CeTakeBackAllMoves),
	 EV_COMMAND_ENABLE(CM_ACTION_REPLAY_ALL, CeReplayAllMoves),
	 EV_COMMAND_ENABLE(CM_ACTION_PLAY_BEST, CeIsSearching),
	 EV_COMMAND_ENABLE(CM_ACTION_STOP_SEARCH, CeIsSearching),
	 EV_COMMAND_ENABLE(CM_OPTIONS_PIECE_SLIDE, CePieceSlide),
	 EV_COMMAND_ENABLE(CM_OPTIONS_MESSAGES, CeMessages),
	 EV_COMMAND_ENABLE(CM_FILE_AUTOSAVE, CeAutoSave),
	 EV_COMMAND_ENABLE(CM_FLAG, CeFlag),
	 EV_COMMAND_ENABLE(CM_LOGS_VERBOSE, CeVerbose),
	 EV_COMMAND_ENABLE(CM_LOGS_TIMINGS, CeTimings),
	 EV_COMMAND_ENABLE(CM_LOGS_PV, CePV),
	 EV_COMMAND_ENABLE(CM_INVERT_BOARD, CeInvertBoard),
	 EV_COMMAND_ENABLE(CM_OPTIONS_SHOW_ANALYSIS, CeShowAnalysis),
	 EV_COMMAND_ENABLE(CM_OPTIONS_COMPUTER_WHITE, CeComputerWhite),
	 EV_COMMAND_ENABLE(CM_OPTIONS_COMPUTER_BLACK, CeComputerBlack),
	 EV_COMMAND_ENABLE(CM_OPTIONS_COMPUTER_PLAY_OPTIONS, CeIsNotSearching),
	 EV_COMMAND_ENABLE(CM_RESIGN, CeResignOrOfferDraw),
	 EV_COMMAND_ENABLE(CM_OFFERDRAW, CeResignOrOfferDraw),
	 EV_WM_LBUTTONDOWN,
	 EV_WM_RBUTTONDOWN,
	 EV_WM_TIMER,
	 EV_WM_KEYDOWN,
END_RESPONSE_TABLE;

TRivalWindow::TRivalWindow()
  : TWindow(0, 0, 0)
{
#ifdef STUPIDBUG
	FILE* f=fopen( "c:\\bc45\\rival\\output\\log.txt", "a" );
	fprintf(f, "%i\n", WinBoard);
	fclose(f);
#endif
	if (WinBoard) {
		Attr.W=0;
		Attr.H=0;
	} else {
		Attr.W=640;
		Attr.H=416;
	}
	Searching=FALSE;
	BoardOffsetX=10;
	BoardOffsetY=10;
	Select.From=0;
	SetDefaultOptions();
	ReadOptions();
	GameCount=0;
#ifndef STUPIDBUG
	FILE *f;
#endif
	OpenPrintInfoLogs();
	OpenPVLogs();
	OpenVerboseLogs();
	strcpy(SUITESUMMARY, workingDirectory);
	strcat(SUITESUMMARY, "\\summary.suite");
	ComputerMoveFlag=FALSE;
	InSetup=FALSE;
	CloseRequested=EscapeHit=FALSE;
	Path2.Move[0].From=0;
	ShadowOffset=8;
	Fade=6;
	WhiteFell=BlackFell=FALSE;
	CreateSearcherAndBoard();
}

BOOL
TRivalWindow::CanClose()
{
	if (Searching || Pondering || CreatingOpeningBook) {
		Searcher->ExitWithoutMove();
		CloseRequested=TRUE;
		return FALSE;
	} else {
		if (!Game->IsGameSaved()) {
		  switch (::MessageBox(this->HWindow, "Save Game?", "Quit Rival", MB_YESNOCANCEL)) {
			  case IDYES : CmSaveGame(); break;
			  case IDCANCEL : return FALSE;
		  }
		}
		KillTimer(1);
		return TRUE;
	}
}

TRivalWindow::~TRivalWindow()
{
	delete RivalPalette;
	delete WhiteSquare;
	delete BlackSquare;
	delete BackGround;
	delete Pawn;
	delete Knight;
	delete King;
	delete Queen;
	delete Rook;
	delete Bishop;
	delete Game;
	delete ClockRectangle;
	delete AnalysisRectangle;
   delete MoveBoxRectangle;
	delete AnalysisBox;
	delete MoveBox;
	delete []NumberDib;
	delete []PathNums;
	delete Searcher;
	delete BoardPopup;
	delete AnalysisPopup;
	delete MoveBoxPopup;
	delete SetupPopup;
	delete BackGroundPopup;
	DeleteOpeningPositions(CachedOpenings);
}

void
TRivalWindow::SetupWindow()
{
	 TWindow::SetupWindow();
	 h=GetApplication()->GetInstance();
	 WhiteSquare=BlackSquare=NULL;
	 BackGround=NULL;
	 Colon=new TDib(h, COLON);
	 LoadPieces();
	 strcpy(piecelist, "0123456789abcdefghx-.QKRNB=+o");
	 NumberDib[0]=new TDib(h, ZERO);	    NumberDib[1]=new TDib(h, ONE);
	 NumberDib[2]=new TDib(h, TWO); 	    NumberDib[3]=new TDib(h, THREE);
	 NumberDib[4]=new TDib(h, FOUR);	    NumberDib[5]=new TDib(h, FIVE);
	 NumberDib[6]=new TDib(h, SIX); 	    NumberDib[7]=new TDib(h, SEVEN);
	 NumberDib[8]=new TDib(h, EIGHT);    NumberDib[9]=new TDib(h, NINE);
	 PathNums[0]=new TDib(h, PATHNUM0);  PathNums[9]=new TDib(h, PATHNUM9);
	 PathNums[18]=new TDib(h, PATHCROSS);  PathNums[19]=new TDib(h, PATHDASH);
	 PathNums[20]=new TDib(h, PATHDOT);
	 PathNums[1]=new TDib(h, PATHNUM1);	 PathNums[2]=new TDib(h, PATHNUM2);
	 PathNums[3]=new TDib(h, PATHNUM3);  PathNums[4]=new TDib(h, PATHNUM4);
	 PathNums[5]=new TDib(h, PATHNUM5);	 PathNums[6]=new TDib(h, PATHNUM6);
	 PathNums[7]=new TDib(h, PATHNUM7);  PathNums[8]=new TDib(h, PATHNUM8);
	 PathNums[10]=new TDib(h, PATHNUMA); PathNums[11]=new TDib(h, PATHNUMB);
	 PathNums[12]=new TDib(h, PATHNUMC); PathNums[13]=new TDib(h, PATHNUMD);
	 PathNums[14]=new TDib(h, PATHNUME); PathNums[15]=new TDib(h, PATHNUMF);
	 PathNums[16]=new TDib(h, PATHNUMG); PathNums[17]=new TDib(h, PATHNUMH);
	 PathNums[PATHNUMQUEEN]=new TDib(h, PATHQUEEN);
	 PathNums[PATHNUMKING]=new TDib(h, PATHKING);
	 PathNums[PATHNUMROOK]=new TDib(h, PATHROOK);
	 PathNums[PATHNUMKNIGHT]=new TDib(h, PATHKNIGHT);
	 PathNums[PATHNUMBISHOP]=new TDib(h, PATHBISHOP);
	 PathNums[PATHNUMMATE]=new TDib(h, PATHM);
	 PathNums[PATHNUMEQUALS]=new TDib(h, PATHEQUALS);
	 PathNums[PATHNUMPLUS]=new TDib(h, PATHPLUS);
	 PathNums[PATHNUMCASTLE]=new TDib(h, PATHNUM0);
	 AnalysisBox=new TDib(h, ANALYSISBOX);
	 MoveBox=new TDib(h, MOVEBOX);

	 TDib* Panel=new TDib(h, PAWN1);
	 RivalPalette=new TPalette(*Panel);

	 ScreenDc=CreateWinGDC(Panel->GetColors(), &ScreenBits);
	 BackGroundDc=CreateWinGDC(Panel->GetColors(), &BackGroundBits);
	 delete Panel;
	 tagRECT r;
	 r.top=0; r.left=0; r.right=639; r.bottom=479;
	 ::FillRect(ScreenDc, &r, (HBRUSH)TBrush(TColor(123, 132, 100)));
	 ::FillRect(BackGroundDc, &r, (HBRUSH)TBrush(TColor(123, 132, 100)));

	 AnalysisRectangle=new TRect(BoardOffsetX+(Pawn->Width()*8)+50, BoardOffsetY, BoardOffsetX+(Pawn->Width()*8)+50+(AnalysisBox->Width()), BoardOffsetY+AnalysisBox->Height());
	 MoveBoxRectangle=new TRect(BoardOffsetX+(Pawn->Width()*8)+50, AnalysisRectangle->bottom+15, BoardOffsetX+(Pawn->Width()*8)+50+(AnalysisBox->Width()), AnalysisRectangle->bottom+15+MoveBox->Height());
	 DrawBackGround();
	 WinGBitBlt(ScreenDc, 0, 0, 639, 479, BackGroundDc, 0, 0);
	 DrawPieces();
	 ClockRectangle=new TRect(BoardOffsetX, BoardOffsetY+(Pawn->Height()*8)+15, BoardOffsetX+(Pawn->Width()*8)+ShadowOffset, BoardOffsetY+(Pawn->Height()*8)+NumberDib[0]->Height()+15+ShadowOffset);
	 DrawClocks();
	 SetTimer(1, TIMERTICKS);
	 SetTimer(2, 1);
	 BoardPopup=new TPopupMenu();
	 AnalysisPopup=new TPopupMenu();
	 MoveBoxPopup=new TPopupMenu();
	 SetupPopup=new TPopupMenu();
	 BackGroundPopup=new TPopupMenu();
	 SetupPopup->AppendMenu(MF_STRING, CM_WHITE_PAWN, "White Pawn");
	 SetupPopup->AppendMenu(MF_STRING, CM_WHITE_KNIGHT, "White Knight");
	 SetupPopup->AppendMenu(MF_STRING, CM_WHITE_BISHOP, "White Bishop");
	 SetupPopup->AppendMenu(MF_STRING, CM_WHITE_ROOK, "White Rook");
	 SetupPopup->AppendMenu(MF_STRING, CM_WHITE_QUEEN, "White Queen");
	 SetupPopup->AppendMenu(MF_STRING, CM_WHITE_KING, "White King");
	 SetupPopup->AppendMenu(MF_SEPARATOR, 0, 0);
	 SetupPopup->AppendMenu(MF_STRING, CM_BLACK_PAWN, "Black Pawn");
	 SetupPopup->AppendMenu(MF_STRING, CM_BLACK_KNIGHT, "Black Knight");
	 SetupPopup->AppendMenu(MF_STRING, CM_BLACK_BISHOP, "Black Bishop");
	 SetupPopup->AppendMenu(MF_STRING, CM_BLACK_ROOK, "Black Rook");
	 SetupPopup->AppendMenu(MF_STRING, CM_BLACK_QUEEN, "Black Queen");
	 SetupPopup->AppendMenu(MF_STRING, CM_BLACK_KING, "Black King");
	 SetupPopup->AppendMenu(MF_SEPARATOR, 0, 0);
	 SetupPopup->AppendMenu(MF_STRING, CM_CLEAR_SQUARE, "Clear Square");
	 SetupPopup->AppendMenu(MF_SEPARATOR, 0, 0);
	 SetupPopup->AppendMenu(MF_STRING, CM_CLEAR_BOARD, "Clear Board");
	 SetupPopup->AppendMenu(MF_SEPARATOR, 0, 0);
	 SetupPopup->AppendMenu(MF_STRING, CM_EXIT_SETUP, "Exit Setup");
	 SetupPopup->AppendMenu(MF_SEPARATOR, 0, 0);
	 SetupPopup->AppendMenu(MF_STRING, CM_ABORT_SETUP, "Abort Setup");

	 BoardPopup->AppendMenu(MF_STRING, CM_FILE_NEW_GAME, "New Game");
	 BoardPopup->AppendMenu(MF_SEPARATOR, 0, 0);
	 BoardPopup->AppendMenu(MF_STRING, CM_ACTION_TAKE_BACK_ONE, "Take Back Move");
	 BoardPopup->AppendMenu(MF_STRING, CM_ACTION_REPLAY_ONE, "Replay Move");
	 BoardPopup->AppendMenu(MF_SEPARATOR, 0, 0);
	 BoardPopup->AppendMenu(MF_STRING, CM_CHANGE_CHESSBOARD, "Change Board");
	 BoardPopup->AppendMenu(MF_STRING, CM_CHANGE_PIECES, "Change Chess Set");
	 BoardPopup->AppendMenu(MF_STRING, CM_CHANGE_WHITE_COLOUR, "Change White Colour");
	 BoardPopup->AppendMenu(MF_STRING, CM_CHANGE_BLACK_COLOUR, "Change Black Colour");

	 AnalysisPopup->AppendMenu(MF_STRING, CM_CHANGE_BOX_STYLE, "Change Analysis Box Colour");
	 AnalysisPopup->AppendMenu(MF_STRING, CM_CHANGE_ANALYSIS_TEXT, "Change Text Colour");
	 AnalysisPopup->AppendMenu(MF_SEPARATOR, 0, 0);
	 AnalysisPopup->AppendMenu(MF_STRING, CM_OPTIONS_SHOW_ANALYSIS, "Show Analysis");

	 MoveBoxPopup->AppendMenu(MF_STRING, CM_CHANGE_MOVEBOX_STYLE, "Change Move Box Colour");
	 MoveBoxPopup->AppendMenu(MF_STRING, CM_CHANGE_MOVE_TEXT, "Change Text Colour");

	 BackGroundPopup->AppendMenu(MF_STRING, CM_CHANGE_BACKGROUND_STYLE, "Change BackGround Style");
	 BackGroundPopup->AppendMenu(MF_STRING, CM_CHANGE_BACKGROUND_COLOUR, "Change BackGround Colour");
	 //CmNewGame();
#ifndef NORANDOM
	 randomize();
#endif
	if (strlen(CommandLinePGN)>4) {
		Game->LoadPGN(CommandLinePGN);
		UpdateAllGraphics();
	}
	ResetVariables();
	CreatingOpeningBook=FALSE;
	OpeningPositions=NULL;
	CacheOpenings();
/*	char s[100];
	sprintf(s, "NMSM %i", Options.NullMoveStopMaterial);
	::MessageBox(this->HWindow, s, "Quit Rival", MB_YESNOCANCEL);*/
}

void
TRivalWindow::CmExit() {
	CloseWindow();
}

void
TRivalWindow::GetOptionSwitch(char* s, BOOL& option)
{
	FILE* f=fopen(OPTIONS, "r");
	dncase(s);
	char r[100];
	if (f!=NULL) {
		while (fscanf(f, "%s", r)!=EOF) {
			dncase(r);
			if (strcmp(r, s)==0) {
				if (fscanf(f, "%s", r)!=EOF) {
					dncase(r);
					if (strcmp(r, "on")==0) option=TRUE;
					if (strcmp(r, "off")==0) option=FALSE;
				}
			}
		}
		fclose(f);
	}
}

int
TRivalWindow::SplitFileNames(char* f, char fs[MAXPGNFILES][MAXFILENAMELENGTH])
{
	if (Verbose) writeMessage(VERBOSE, "Filenames");
	if (Verbose) writeMessage(VERBOSE, f);
	strcpy(fs[0], f);
	return 1;
/*	int i, Len;
	Len=strlen(f);
	BOOL GotPath=FALSE;
	char Path[MAXFILENAMELENGTH];
	char Temp[MAXFILENAMELENGTH];
	int FileCount=0;
	int CharCount=0;
	for (i=0; i<Len; i++) {
		if (f[i]!=' ') {
			if (!GotPath) {
				Path[i]=f[i];
			} else {
				Temp[CharCount++]=f[i];
			}
		}
		if (f[i]==' ' || i==Len-1) {
			if (!GotPath) {
				Path[i]='\0';
				GotPath=TRUE;
			} else {
				Temp[CharCount]='\0';
				strcpy(fs[FileCount], Path);
				if (Path[strlen(Path)-1]!='\\') strcat(fs[FileCount], "\\");
				strcat(fs[FileCount], Temp);
				//::MessageBox(this->HWindow, fs[FileCount], "FileName", MB_ICONEXCLAMATION | MB_OK);
				FileCount++;
				if (FileCount==(MAXPGNFILES-1)) return -1;
			}
			CharCount=0;
		}
	}
	if (FileCount==0) {
		strcpy(fs[0], f);
		return 1;
	}
	return FileCount;*/
}

void
TRivalWindow::GetOptionInteger(char* s, int& option)
{
	FILE* f=fopen(OPTIONS, "r");
	dncase(s);
	char r[100];
	if (f!=NULL) {
		while (fscanf(f, "%s", r)!=EOF) {
			dncase(r);
			if (strcmp(r, s)==0) {
				if (fscanf(f, "%s", r)!=EOF) {
					option=atoi(r);
				}
			}
		}
		fclose(f);
	}
}

void
TRivalWindow::CheckOptions()
{
	BringWithinRange(Options.NullMoveStopMaterial, -1, 7);
	ApplyMinimum(Options.HashTableSize, 0);
	BringWithinRange(Options.Quiesce, 0, 50);
	BringWithinRange(Options.MaxExtend, 0, 40);
   BringWithinRange(Options.Aspire, -32000, 32000);
}


void
TRivalWindow::ReadOptions()
{
	GetOptionSwitch("Verbose", Verbose);
	GetOptionSwitch("Timings", PrintInfo);
	GetOptionSwitch("PrincipalVariation", PrintPV);
	GetOptionSwitch("ComputerWhite", Options.ComputerWhite);
	GetOptionSwitch("ComputerBlack", Options.ComputerBlack);
	GetOptionSwitch("UseHistory", Options.History);
	GetOptionInteger("RandomMoveOrdering", Options.RandomMoveOrdering);
	GetOptionInteger("SearchMethod", Options.SearchMethod);
	GetOptionInteger("Level", Options.Level);
	GetOptionInteger("BaseMinutes", Options.BaseMinutes);
	GetOptionInteger("NullMoveReduceDepth", Options.NullMoveReduce);
	GetOptionInteger("Increment", Options.Increment);
	GetOptionInteger("Time(permove)", Options.Time);
	GetOptionInteger("NullMoveStopMaterial", Options.NullMoveStopMaterial);
	GetOptionInteger("ChampionshipTime", Options.ChampTime);
	GetOptionInteger("ChampionshipMoves", Options.ChampMoves);
	GetOptionInteger("MaxExtensions", Options.MaxExtend);
	GetOptionInteger("AspirationWindow", Options.Aspire);
	GetOptionSwitch("OpeningLearning", Options.OpeningLearning);
	GetOptionSwitch("Openings", Options.Openings);
	GetOptionSwitch("InvertBoard", Options.InvertBoard);
	GetOptionInteger("MinimumBookResults", Options.MinimumBookResults);
	GetOptionInteger("WhitePercent", Options.WhitePercent);
	GetOptionInteger("BlackPercent", Options.BlackPercent);
	GetOptionInteger("HashReadReduce", Options.HashReadReduce);
	GetOptionInteger("HashWriteReduce", Options.HashWriteReduce);
	GetOptionInteger("HashTableSize(K)", Options.HashTableSize);
	GetOptionInteger("QuiescenceDepth", Options.Quiesce);
	GetOptionInteger("Contempt", Options.Contempt);
	GetOptionSwitch("RecaptureExtensions", Options.Recapture);
	GetOptionSwitch("CheckExtensions", Options.Check);
	GetOptionSwitch("PawnPushExtensions", Options.PawnPush);
	GetOptionSwitch("SingleReplyExtensions", Options.SingleReply);
	GetOptionSwitch("NullMove", Options.NullMove);
	GetOptionSwitch("FailHighReductions", Options.FailHighReductions);
	GetOptionSwitch("Ponder", Options.Ponder);
	GetOptionSwitch("MinimalWindow", Options.MinimalWindow);
	GetOptionSwitch("AutoSave", Options.AutoSave);
	GetOptionInteger("Variant", Options.Variant);
	GetOptionSwitch("PieceSlide", Options.PieceSlide);
	GetOptionSwitch("AllMessages", Options.Messages);
	GetOptionSwitch("ShowAnalysis", Options.ShowAnalysis);
	GetOptionInteger("WhiteColour", Options.WhiteOffset);
	GetOptionInteger("BlackColour", Options.BlackOffset);
	GetOptionInteger("BackgroundColour", Options.BackOffset);
	GetOptionInteger("BackgroundStyle", Options.BackGround);
	GetOptionInteger("Chessboard", Options.ChessBoard);
	GetOptionInteger("Pieces", Options.Pieces);
	GetOptionInteger("AnalysisBoxColour", Options.AnalysisBoxStyle);
	GetOptionInteger("MoveBoxColour", Options.MoveBoxStyle);
	GetOptionInteger("AnalysisTextColour", Options.AnalysisTextOffset);
	GetOptionInteger("MoveBoxTextColour", Options.MoveBoxTextOffset);
   CheckOptions();
}

void
TRivalWindow::WriteOptions()
{
	FILE *f;
	if ((f=fopen(OPTIONS, "w"))!=NULL) {
		fprintf(f, "-----------\n");
		fprintf(f, "Log Options\n");
		fprintf(f, "-----------\n");
		fprintf(f, "Verbose %s\n", (Verbose ? "On" : "Off"));
		fprintf(f, "Timings %s\n", (PrintInfo ? "On" : "Off"));
		fprintf(f, "PrincipalVariation %s\n", (PrintPV ? "On" : "Off"));
		fprintf(f, "------------\n");
		fprintf(f, "Play Options\n");
		fprintf(f, "------------\n");
		fprintf(f, "ComputerWhite %s\n", (Options.ComputerWhite ? "On" : "Off"));
		fprintf(f, "ComputerBlack %s\n", (Options.ComputerBlack ? "On" : "Off"));
		fprintf(f, "SearchMethod %i\n", Options.SearchMethod);
		fprintf(f, "Level %i\n", Options.Level);
		fprintf(f, "BaseMinutes %i\n", Options.BaseMinutes);
		fprintf(f, "Increment %i\n", Options.Increment);
		fprintf(f, "Time(permove) %i\n", Options.Time);
		fprintf(f, "ChampionshipTime %i\n", Options.ChampTime);
		fprintf(f, "ChampionshipMoves %i\n", Options.ChampMoves);
		fprintf(f, "OpeningLearning %s\n", (Options.OpeningLearning ? "On" : "Off"));
		fprintf(f, "Openings %s\n", (Options.Openings ? "On" : "Off"));
		fprintf(f, "InvertBoard %s\n", (Options.InvertBoard ? "On" : "Off"));
		fprintf(f, "MinimumBookResults %i\n", Options.MinimumBookResults);
		fprintf(f, "WhitePercent %i\n", Options.WhitePercent);
		fprintf(f, "BlackPercent %i\n", Options.BlackPercent);
		fprintf(f, "HashTableSize(K) %i\n", Options.HashTableSize);
		fprintf(f, "HashReadReduce %i\n", Options.HashReadReduce);
		fprintf(f, "HashWriteReduce %i\n", Options.HashWriteReduce);
		fprintf(f, "QuiescenceDepth %i\n", Options.Quiesce);
		fprintf(f, "Contempt %i\n", Options.Contempt);
		fprintf(f, "MaxExtensions %i\n", Options.MaxExtend);
		fprintf(f, "RecaptureExtensions %s\n", (Options.Recapture ? "On" : "Off"));
		fprintf(f, "CheckExtensions %s\n", (Options.Check ? "On" : "Off"));
		fprintf(f, "PawnPushExtensions %s\n", (Options.PawnPush ? "On" : "Off"));
		fprintf(f, "SingleReplyExtensions %s\n", (Options.SingleReply ? "On" : "Off"));
		fprintf(f, "UseHistory %s\n", (Options.History ? "On" : "Off"));
		fprintf(f, "RandomMoveOrdering %s\n", (Options.RandomMoveOrdering ? "On" : "Off"));
		fprintf(f, "FailHighReductions %s\n", (Options.FailHighReductions ? "On" : "Off"));
		fprintf(f, "NullMove %s\n", (Options.NullMove ? "On" : "Off"));
		fprintf(f, "NullMoveReduceDepth %i\n", Options.NullMoveReduce);
		fprintf(f, "NullMoveStopMaterial %i\n", Options.NullMoveStopMaterial);
		fprintf(f, "AspirationWindow %i\n", Options.Aspire);
		fprintf(f, "Ponder %s\n", (Options.Ponder ? "On" : "Off"));
		fprintf(f, "MinimalWindow %s\n", (Options.MinimalWindow ? "On" : "Off"));
		fprintf(f, "AutoSave %s\n", (Options.AutoSave ? "On" : "Off"));
		fprintf(f, "Variant %i\n", Options.Variant);
		fprintf(f, "---------------\n");
		fprintf(f, "Display Options\n");
		fprintf(f, "---------------\n");
		fprintf(f, "PieceSlide %s\n", (Options.PieceSlide ? "On" : "Off"));
		fprintf(f, "AllMessages %s\n", (Options.Messages ? "On" : "Off"));
		fprintf(f, "ShowAnalysis %s\n", (Options.ShowAnalysis ? "On" : "Off"));
		fprintf(f, "WhiteColour %i\n", Options.WhiteOffset);
		fprintf(f, "BlackColour %i\n", Options.BlackOffset);
		fprintf(f, "BackgroundColour %i\n", Options.BackOffset);
		fprintf(f, "BackgroundStyle %i\n", Options.BackGround);
		fprintf(f, "Chessboard %i\n", Options.ChessBoard);
		fprintf(f, "Pieces %i\n", Options.Pieces);
		fprintf(f, "AnalysisBoxColour %i\n", Options.AnalysisBoxStyle);
		fprintf(f, "MoveBoxColour %i\n", Options.MoveBoxStyle);
		fprintf(f, "AnalysisTextColour %i\n", Options.AnalysisTextOffset);
		fprintf(f, "MoveBoxTextColour %i\n", Options.MoveBoxTextOffset);
		fclose(f);
	}
}

void
TRivalWindow::CacheOpenings()
{
	int j;
	char s[10][MAXFEN];
	char FileFen[MAXFEN];
	int w, b, d;
	CachedOpenings=0;
	FILE* f;

	f=fopen(OPENINGS, "r");
	while (fscanf(f, "%s%s%s%s%s%s%s%i%s%i%s%i",
				&s[0],&s[1],&s[2],&s[3],&s[4],&s[5],&s[6],&w,&s[8],&b,&s[9],&d)!=EOF) {
		CachedOpenings++;
	}
	fclose(f);
	OpeningPositions=new TOpeningPosition[CachedOpenings];
	writeLog("Cached", CachedOpenings);
	CachedOpenings=0;
	f=fopen(OPENINGS, "r");
	while (fscanf(f, "%s%s%s%s%s%s%s%i%s%i%s%i",
				&s[0],&s[1],&s[2],&s[3],&s[4],&s[5],&s[6],&w,&s[8],&b,&s[9],&d)!=EOF) {
		strcpy(FileFen, "");
		for (j=0; j<4; j++) {
			strcat(FileFen, s[j]);
			strcat(FileFen, " ");
		}
		strcat(FileFen, "0 0");
#ifdef DYNAMICFEN
		OpeningPositions[CachedOpenings].Fen = new char[strlen(FileFen)+1];
#endif
		strcpy(OpeningPositions[CachedOpenings].Fen, FileFen);
		OpeningPositions[CachedOpenings].WhiteWins=w;
		OpeningPositions[CachedOpenings].BlackWins=b;
		OpeningPositions[CachedOpenings].Draws=d;
		CachedOpenings++;
	}
	fclose(f);
}

void
TRivalWindow::ResetVariables()
{
	TryOpenings=TRUE;
}

void
TRivalWindow::AutoSaveGame()
{
	if (Options.AutoSave) {
		char FileName[MAXFILENAMELENGTH];
		strcpy(FileName, workingDirectory);
		strcat(FileName, "\\AutoSave");
		char s[10];
		sprintf(s, "%i", GameCount);
		strcat(FileName, s);
		switch (Options.Variant) {
				 case CHESS : strcat(FileName, ".riv"); break;
				 case KINGLET : strcat(FileName, ".rkl"); break;
				 case SHATRANJ : strcat(FileName, ".rsh"); break;
				 case SELFTAKE : strcat(FileName, ".rst"); break;
		 }
		 Game->SaveGame(FileName);
       writeLog(FileName);
	}
}

void
TRivalWindow::MakeMove(TChessBoard* Game, TMove Move)
{
	TPromoteDialog* pd;
	int Info=QUEEN;

	if (Pondering) {
		Pondering = false;
		Searcher->ExitWithoutMove();
	}

	if (Options.SearchMethod==BASEINCREMENT) {
		if (Game->GetMover()==WHITE)
			Game->SetWhiteClock(Game->GetWhiteClock()+(Options.Increment*1000));
		else
			Game->SetBlackClock(Game->GetBlackClock()+(Options.Increment*1000));
	}
	if (Options.SearchMethod==CHAMPIONSHIP && ((Game->TotalMovesMade()+1)%(Options.ChampMoves*2))==0) {
		Game->SetWhiteClock(Game->GetWhiteClock()+(Options.ChampTime*1000));
		Game->SetBlackClock(Game->GetBlackClock()+(Options.ChampTime*1000));
	}

	if (!WinBoard && Options.Variant!=KINGLET &&
		 ((!Options.ComputerWhite && Game->GetMover()==WHITE && GetY(Move.To)==8 && Game->GetSquare(Move.From)==WP) ||
		  (!Options.ComputerBlack && Game->GetMover()==BLACK && GetY(Move.To)==1 && Game->GetSquare(Move.From)==BP)
		 )
		) {

		pd = new TPromoteDialog(this, &Info);
		if (pd->Execute()==IDOK) {
			Move.PromotionPiece=Info;
		}
		delete pd;
	}

   WhiteFell=BlackFell=FALSE;
	Game->MakeMove(Move);

	AutoSaveGame();
}

void
TRivalWindow::EvTimer(uint TimerId)
{
	 static long oldtimedisplay=GetTickCount();
	 if (InSetup) {
		return;
	 }
	 if (ComputerMoveFlag) {
		ComputerMoveFlag = false;
		ComputerMove();
		return;
	 }
	 if (WinBoard) {
		 if (TimerId==2) {
			MonitorXBoard();
		 }
	 } else
	 if (TimerId==1 && Game->GameStatus()==0) {
		Invalidate(FALSE);
		long newtime=GetTickCount();
		int elapsed=newtime-oldtime;
		if (elapsed>TIMERTICKS) {
		  oldtime=newtime;
		  if (TestSuite && newtime-globalstarttime>TestTime) {
        		Searcher->ExitWithMove();
		  }
		  if (Options.SearchMethod==BASEINCREMENT || Options.SearchMethod==CHAMPIONSHIP) {
			 if (Game->GetMover()==WHITE) {
				Game->SetWhiteClock(Game->GetWhiteClock()-elapsed >= 0 ? Game->GetWhiteClock()-elapsed : 0);
				if (Options.Flag && !WinBoard && !WhiteFell && Game->GetWhiteClock()<=0 && Game->TotalMovesMade()>0) {
					KillTimer(1);
					KillTimer(2);
					//::MessageBox(this->HWindow, "White has run out of time.  You may continue to play if you wish.", "White's flag fell", MB_OK);
					SetTimer(1, TIMERTICKS);
					SetTimer(2, 1);
					WhiteFell=TRUE;
				}
			 } else {
				Game->SetBlackClock(Game->GetBlackClock()-elapsed >= 0 ? Game->GetBlackClock()-elapsed : 0);
				if (Options.Flag && !WinBoard && !BlackFell && Game->GetBlackClock()<=0 && Game->TotalMovesMade()>0) {
					KillTimer(1);
					KillTimer(2);
					//::MessageBox(this->HWindow, "Black has run out of time.  You may continue to play if you wish.", "Black's flag fell", MB_OK);
					SetTimer(1, TIMERTICKS);
					SetTimer(2, 1);
					BlackFell=TRUE;
				}
			 }
		  } else {
			 if (Game->GetMover()==WHITE)
				Game->SetWhiteClock(Game->GetWhiteClock()+elapsed);
			 else
				Game->SetBlackClock(Game->GetBlackClock()+elapsed);
		  }
		  DrawClocks();
		  InvalidateRect(*ClockRectangle, FALSE);
		  if (Searching) {
			  if (Options.ShowAnalysis && !WinBoard && (newtime-oldtimedisplay)>=DISPLAYTICKS) {
				 oldtimedisplay=newtime;
				 DrawCurrentSearchPath(Searcher->GetPrincipalPath());
				 InvalidateRect(*AnalysisRectangle, FALSE);
			  }
			  if (!Pondering) {
				  if (Options.SearchMethod==TIMEPERMOVE && (newtime-SearchStartTime)>=(Options.Time*1000-TIMERTICKS)) {
						Searcher->ExitWithMove();
				  }
				  if (Options.SearchMethod==CHAMPIONSHIP) {
					 if ((newtime-SearchStartTime)>(MeanSearchTime/4.0*3))
						 if (Searcher->Satisfied==TRUE
							|| newtime-SearchStartTime>=(MeanSearchTime/4.0*7)
							|| newtime-SearchStartTime>=SearchTimeLeft) {
							 Searcher->ExitWithMove();
						 }
				  }
				  if (Options.SearchMethod==BASEINCREMENT) {
						if ((newtime-SearchStartTime)>=normalmovetime) {
							if (!Searcher->Satisfied) {
								if (newtime-SearchStartTime>=extendmaxmovetime) {
									Searcher->ExitWithMove();
								}
							} else {
								Searcher->ExitWithMove();
							}
						}
				  }
			  }
		  } /*else
		  if (Pondering) {
			  if (Options.ShowAnalysis) {
				 DrawCurrentSearchPath(Searcher->GetCurrentPath(), Searcher->GetPrincipalPath());
				 InvalidateRect(*AnalysisRectangle, FALSE);
			  }
		  }   */
		}
	 }
}

void
TRivalWindow::GetReason(int code, char* reason)
{
	switch(code) {
					case 1 : strcpy( reason, "The King moves one square at a time in any direction" ); break;
					case 2 : strcpy( reason, "That would leave the King in check" );  break;
					case 3 : strcpy( reason, "You can not castle because you have already moved that Rook" );  break;
					case 4 : strcpy( reason, "You can not castle because you have already moved the King" );  break;
					case 5 : strcpy( reason, "The Knight makes L-Shaped moves with a combination of a 1 square move and a 2 square move" );  break;
					case 6 : strcpy( reason, "The Rook moves in straight lines, up, down, left or right" );  break;
					case 7 : strcpy( reason, "The Queen moves in straight lines, up, down, left, right or diagonally" );  break;
					case 8 : strcpy( reason, "A Pawn moves one square forward but has the option of moving two squares on its first move" );  break;
					case 9 : strcpy( reason, "A Pawn may only capture by moving one square diagonally forward" );  break;
					case 10 : strcpy( reason, "A Pawn may only move two squares on its first move" );  break;
					case 11 : strcpy( reason, "A Pawn may only capture diagonally" );  break;
					case 12 : strcpy( reason, "Bishops move diagonally in any direction" );  break;
					case 13 : strcpy( reason, "You can not castle if a piece is in the way" );  break;
					case 14 : strcpy( reason, "The Rook can not jump other pieces" );  break;
					case 15 : strcpy( reason, "The Bishop can not jump other pieces" );  break;
					case 16 : strcpy( reason, "A Pawn can only move forwards" );  break;
					case 17 : strcpy( reason, "A Pawn can not jump other pieces" );  break;
					case 18 : strcpy( reason, "A Pawn may only move diagonally when capturing" );  break;
					case 19 : strcpy( reason, "The Pawn is blocked by a friendly piece" );  break;
					case 20 : strcpy( reason, "The Queen can not jump other pieces" );  break;
					case 21 : strcpy( reason, "You can not castle through check" );  break;
					case 22 : strcpy( reason, "You can not castle when in check" );  break;
					case 30 : strcpy( reason, "A Pawn moves one square forward" );  break;
					case 31 : strcpy( reason, "A Queen (General) moves one square diagonally" );  break;
					case 32 : strcpy( reason, "A Bishop (Elephant) jumps two squares diagonally" );  break;
					case 33 : strcpy( reason, "You cannot take your own King" );  break;
					case 99 : strcpy( reason, "I verified that move but it is not in the Move List" );  break;
					default : strcpy( reason, "An error has occured, an invalid move has been returned" );  break;
	} // switch
}

void
TRivalWindow::VerifyAndMake(TMove Move) {
	if (Game->VerifyMove(Move)==0) {
		SlidePiece(Move);
		MakeMove(Game, Move);
		Select.From=0;
		UpdateAllGraphics();
		if (!GameOver()) {
		  if ((Game->GetMover()==WHITE && Options.ComputerWhite) ||
			 (Game->GetMover()==BLACK && Options.ComputerBlack))
					if (Options.Ponder) ComputerMoveFlag = TRUE;	else ComputerMove();
		}
	 } else {
			if (Options.Messages) {
					char reason[200];
					GetReason(Game->VerifyMove(Move), reason);
					::MessageBox(this->HWindow, reason, "Illegal Move", MB_ICONEXCLAMATION | MB_OK);
			}
	 } // if else ... illegal move
	 UpdateAllGraphics();
}

void
TRivalWindow::EvRButtonDown(uint, TPoint& Point)
{
	TPoint mouseLocation(Point);
	ClientToScreen(mouseLocation);
	if (Options.Variant==SELFTAKE && Select.From!=0) {
	 int SquareX=(Point.x-BoardOffsetX)/Pawn->Width()+1;
	 int SquareY=8-((Point.y-BoardOffsetY)/Pawn->Height());
	 if ((Game->GetMover()==WHITE && Game->GetSquare(SquareX*10+SquareY)!=WK)
			|| (Game->GetMover()==BLACK && Game->GetSquare(SquareX*10+SquareY)!=BK)) {
			Select.To=SquareX*10+SquareY;
			VerifyAndMake(Select);
			return;
	 }
	}
	if (Point.x>BoardOffsetX && Point.y>BoardOffsetY &&
		 Point.x<BoardOffsetX+(8*Pawn->Width()) && Point.y<BoardOffsetY+(8*Pawn->Height())) {
	  if (InSetup) {
		 SetupX=(Point.x-BoardOffsetX)/Pawn->Width()+1;
		 SetupY=8-((Point.y-BoardOffsetY)/Pawn->Height());
		 SetupPopup->TrackPopupMenu(TPM_RIGHTALIGN | TPM_RIGHTBUTTON, mouseLocation, 0, HWindow);
	  } else
		 BoardPopup->TrackPopupMenu(TPM_RIGHTALIGN | TPM_RIGHTBUTTON, mouseLocation, 0, HWindow);
	} else
	if (Point.x>AnalysisRectangle->left && Point.y>AnalysisRectangle->top &&
		 Point.x<AnalysisRectangle->right && Point.y<AnalysisRectangle->bottom) {
		 AnalysisPopup->TrackPopupMenu(TPM_RIGHTALIGN | TPM_RIGHTBUTTON, mouseLocation, 0, HWindow);
	} else
	if (Point.x>MoveBoxRectangle->left && Point.y>MoveBoxRectangle->top &&
		 Point.x<MoveBoxRectangle->right && Point.y<MoveBoxRectangle->bottom) {
		 MoveBoxPopup->TrackPopupMenu(TPM_RIGHTALIGN | TPM_RIGHTBUTTON, mouseLocation, 0, HWindow);
	} else
	BackGroundPopup->TrackPopupMenu(TPM_RIGHTALIGN | TPM_RIGHTBUTTON, mouseLocation, 0, HWindow);
}

int
TRivalWindow::SelectedSquareIndex(int SquareX, int SquareY)
{
	int index;
	if (Options.InvertBoard)
	{
	  index=(9-SquareX)*10+(9-SquareY);
	}
	else
	{
	  index=SquareX*10+SquareY;
	}
	return index;
}

void
TRivalWindow::EvLButtonDown(uint, TPoint& Point)
{
	 if (InSetup) return;
	 if (GameOver() || Searching) return;
	 if (Point.x<BoardOffsetX || Point.y<BoardOffsetY ||
		  Point.x>BoardOffsetX+(8*Pawn->Width()) || Point.y>BoardOffsetY+(8*Pawn->Height()))
			  return;
	 int SquareX=(Point.x-BoardOffsetX)/Pawn->Width()+1;
	 int SquareY=8-((Point.y-BoardOffsetY)/Pawn->Height());
	 if (Select.From==0) {
		 Select.From = SelectedSquareIndex(SquareX, SquareY);
		 if (Game->GetSquare(Select.From)==EMPTY) {
			 Select.From=0;
		 } else
		 if (Game->GetMover()==WHITE && Game->GetSquare(Select.From)>EMPTY) {
			 if (Options.Messages) ::MessageBox(this->HWindow, "It's White's turn", "Rival", MB_ICONEXCLAMATION | MB_OK);
			 Select.From=0;
		 } else
		 if (Game->GetMover()==BLACK && Game->GetSquare(Select.From)<EMPTY) {
			 if (Options.Messages) ::MessageBox(this->HWindow, "It's Black's turn", "Rival", MB_ICONEXCLAMATION | MB_OK);
			 Select.From=0;
		 } else {
			 UpdateAllGraphics();
		 }
	 } else {
		Select.To = SelectedSquareIndex(SquareX, SquareY);
		 if ((Game->GetMover()==WHITE && Game->GetSquare(Select.To)<EMPTY) ||
			 (Game->GetMover()==BLACK && Game->GetSquare(Select.To)>EMPTY)) {
				 Select.From = SelectedSquareIndex(SquareX, SquareY);
				 UpdateAllGraphics();
		 } else {
			 VerifyAndMake(Select);
		 } // if reselecting from square...else
	 } // if selecting from square for first time...else
}

void
TRivalWindow::DrawAnalysisNumber(int Number)
{
		WinGBitBlt(ScreenDc, AnalysisRectangle->left, AnalysisRectangle->top, AnalysisRectangle->right,
							AnalysisRectangle->bottom,
							BackGroundDc,
							AnalysisRectangle->left,
							AnalysisRectangle->top);
		DrawNumber(AnalysisRectangle->left+60, AnalysisRectangle->top+5, Number, 160+(Options.AnalysisBoxStyle*16), 174+(Options.AnalysisBoxStyle*16), (Options.AnalysisTextOffset*16));
		InvalidateRect(*AnalysisRectangle, FALSE);
		UpdateWindow();
}

void
TRivalWindow::DeleteOpeningPositions(int Total)
{
	int j;
	if (OpeningPositions!=NULL) {
#ifdef DYNAMICFEN
		for (int i=0; i<Total; i++) {
			delete[] OpeningPositions[i].Fen;
		}
#endif
		delete[] OpeningPositions;
	}
	OpeningPositions=NULL;
}

void
TRivalWindow::WriteCreateLog(char* s, BOOL Create=FALSE)
{
	char FileName[MAXFILENAMELENGTH];
	strcpy(FileName, workingDirectory);
	strcat(FileName, "\\create.log");
	FILE* f;
	if (Create) {
		if ((f=fopen(FileName, "w"))!=NULL) {
			fclose(f);
		}
	} else {
		if ((f=fopen(FileName, "a"))!=NULL) {
			fprintf(f, "%s", s);
			fclose(f);
		}
	}
	fclose(f);
}

int
TRivalWindow::CreateOpeningBook(
	TWindow* caller,
	char FileName[MAXPGNFILES][MAXFILENAMELENGTH],
	int NumFiles,
	int MaxPly,
	int MaxPositions,
	int MinResults,
	int WinPercent,
	int WinPercentBlack,
	BOOL IncludeUnknownResults,
	int& FilesUsed,
	int& Written,
	int& Games,
	int& GamesUsed,
	int& TotalPositions,
	int& UnverifiedMoves,
	BOOL QuitOnError,
	char* Error,
	int CountGames)
{
	MSG msg;
	BOOL CountingGames=CountGames;
	int oldtime=GetTickCount();
	CreatingOpeningBook=TRUE;
	Games=GamesUsed=Written=TotalPositions=FilesUsed=0;
	strcpy(Error, "\nCompleted Successfully");
	MaxOpeningPositions=MaxPositions;
	int i, j, Count, l, result;
	TMove move;
	BOOL SavedFile=FALSE;
	int MovesRead=TRUE;
	int MatchResult=UNKNOWNRESULT;
	FILE *f;
	TChessBoard* Game=new TChessBoard;
	char FEN[MAXFEN];
	char ps[100];
	strcpy(FEN, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w QKqk - 0 1");
	f=NULL;
	// Clear static variables in Game->ReadPGN
	Game->ReadPGN(f, 0);
	int FileCount=1;
	f=fopen((const char *)FileName[FileCount-1], "r");
	if (f==NULL) return -1;
	// Count Games
	delete AnalysisBox;
	AnalysisBox=new TDib(h, ANALYSISBOX2);
	DrawBackGround();
	WriteCreateLog("", TRUE);
	if (CountGames) {
		while (!CloseRequested) {
			//for (i=0; i<PGNBUFFERSIZE; i++) Game->buffer[i]='\0';
			result=Game->ReadPGN(f, 0);
			while (result==1) result=Game->ReadPGN(f, 0);
			if (result==-1) {
				FileCount++;
				if (FileCount>NumFiles)	{
					break;
				} else {
					fclose(f);
					f=fopen((const char *)FileName[FileCount-1], "r");
					if (f==NULL) break;
					FilesUsed++;
				}
			}
			Games++;
			if (GetTickCount()-oldtime>350) {
				oldtime=GetTickCount();
				if (!CloseRequested && PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE)) {
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
				DrawAnalysisNumber(Games);
			}
			result=Game->ReadPGN(f, 0);
			while (result!=1 && result!=-1) result=Game->ReadPGN(f, 0);
			if (result==-1) {
				FileCount++;
				if (FileCount>NumFiles)	{
					DrawAnalysisNumber(Games);
					break;
				} else {
					fclose(f);
					f=fopen((const char *)FileName[FileCount-1], "r");
					if (f==NULL) break;
					FilesUsed++;
				}
			}
		}
		fclose(f);
		if (!CloseRequested) {
			CountingGames=FALSE;
			char s[100];
			sprintf(s, "About to read %i games from %i file(s), continue?", Games, FilesUsed+1);
			if (::MessageBox(this->HWindow, s, "Create OpeningBook", MB_YESNO)==IDNO) {
				delete Game;
				delete AnalysisBox;
				AnalysisBox=new TDib(h, ANALYSISBOX);
				DrawBackGround();
            CreatingOpeningBook=FALSE;
				return -1;
			}
			UpdateAllGraphics();
			Games=0;
		}
		FileCount=1;
		f=fopen((const char *)FileName[FileCount-1], "r");
	}
	// Read Moves
	OpeningPositionCount=0;
	BOOL SkipGame=FALSE;
	UnverifiedMoves=0;
	FilesUsed=0;
	try {
		DeleteOpeningPositions(CachedOpenings);
		OpeningPositions=new TOpeningPosition[MaxOpeningPositions];
		while (!CloseRequested) {
			//for (i=0; i<PGNBUFFERSIZE; i++) Game->buffer[i]='\0';
			result=Game->ReadPGN(f, 0);
			while (result==1) {
				if (MovesRead) {
					Games++;
					GamesUsed++;
					SkipGame=FALSE;
					if (GetTickCount()-oldtime>350) {
						oldtime=GetTickCount();
						DrawAnalysisNumber(Games);
						if (!CloseRequested && PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE)) {
							TranslateMessage(&msg);
							DispatchMessage(&msg);
						}
					}
					Game->TranslateFenToBoard(FEN, Game->ResetTo);
					Game->MovePointer=Game->MovesStored=0;
					Game->ResetBoard();
					MovesRead=FALSE;
					MatchResult=UNKNOWNRESULT;
				}
				if (strstr(Game->buffer, "[Result")!=NULL) {
					if (strstr(Game->buffer, "1-0")!=NULL) MatchResult=WHITEWIN; else
					if (strstr(Game->buffer, "0-1")!=NULL) MatchResult=BLACKWIN; else
					if (strstr(Game->buffer, "1/2-1/2")!=NULL) MatchResult=DRAW; else
					MatchResult=UNKNOWNRESULT;
					if (MatchResult==UNKNOWNRESULT && !IncludeUnknownResults) GamesUsed--;
				}
				result=Game->ReadPGN(f, 0);
			};
			if (result==99) {
				Game->TranslateFenToBoard(Game->buffer, Game->ResetTo);
				Game->ResetBoard();
			}
			if (result==-1) {
				FilesUsed++;
				FileCount++;
				if (FileCount>NumFiles) {
					goto Finished;
				} else {
					fclose(f);
					f=fopen((const char *)FileName[FileCount-1], "r");
					if (f==NULL) {
						goto Finished;
					}
				}
			}
			if (result==0) {
				MovesRead=TRUE;
				if (!SkipGame && (MatchResult!=UNKNOWNRESULT || IncludeUnknownResults) && Game->MovesStored<MaxPly) {
					Game->TranslateAlgebraicMove(Game->buffer, move);
					if (Game->VerifyMove(move)==0) {
						Game->MakeMove(move, TRUE);
						TotalPositions++;
						if (StoreOpeningPosition(Game, MatchResult)==MAXOPENINGSREACHED) {
							sprintf(Error, "\nMaximum openings reached");
							writeLog("Max openings reached");
							goto Finished;
						}
					} else {
						SkipGame=TRUE;
						if (strcmp(Game->buffer, "-0")!=0 && strcmp(Game->buffer, "-1")!=0 && strcmp(Game->buffer, "/2-1/2")!=0) {
							UnverifiedMoves++;
							sprintf(Error, "\nError: Buffer was %s", Game->buffer);
							sprintf(ps, "\n%i Half moves had been made in this game", Game->MovesStored);
							strcat(Error, ps);
							sprintf(ps, "\nInterpreted Move as %i to %i", move.From, move.To);
							strcat(Error, ps);
							WriteCreateLog(Error);
							if (QuitOnError) goto Finished;
						}
					}
				}
			}
		}
	}
	catch (xalloc) {
		sprintf(Error, "Out of memory");
	}

Finished:
	fclose(f);
	CreatingOpeningBook=FALSE;
	int GamesPlayed;
	float Perc;
	if (!CountingGames) {
		if (!CloseRequested || ::MessageBox(this->HWindow, "Save current openings before closing?", "Exiting Rival", MB_YESNO)==IDYES) {
			strcpy(FEN, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w QKqk - 0 1");
			Game->TranslateFenToBoard(FEN, Game->ResetTo);
			Game->ResetBoard();
			Game->GameSaved=TRUE;
			if ((f=fopen(OPENINGS, "w"))!=NULL) {
				SavedFile=TRUE;
				for (i=0; i<OpeningPositionCount; i++) {
					GamesPlayed=OpeningPositions[i].WhiteWins+OpeningPositions[i].BlackWins+OpeningPositions[i].Draws;
					if (GamesPlayed>=MinResults) {
						// if white to move, then black makes move to get into this position
						// is the win percentage high enough?
						Perc;
						BOOL GoAhead=TRUE;
						if (GamesPlayed>0) {
							if (strstr(OpeningPositions[i].Fen, " w ")) {
								Perc=(float)(OpeningPositions[i].BlackWins)/(float)GamesPlayed;
								GoAhead=(Perc*100)>=(float)WinPercent;
							} else
							if (strstr(OpeningPositions[i].Fen, " b ")) {
								Perc=(float)(OpeningPositions[i].WhiteWins)/(float)GamesPlayed;
								GoAhead=(Perc*100)>=(float)WinPercentBlack;
							}
						}
						if (GoAhead) {
							fprintf(f, "%s W %i B %i D %i\n",
								OpeningPositions[i].Fen,
								OpeningPositions[i].WhiteWins,
								OpeningPositions[i].BlackWins,
								OpeningPositions[i].Draws);
							Written++;
						}
					}
				}
				fclose(f);
			} else {
				::MessageBox(this->HWindow, "Error Creating File!", "Create Opening Book", MB_OK);
			}
		}
	}
	delete AnalysisBox;
	AnalysisBox=new TDib(h, ANALYSISBOX);
	DrawBackGround();
	DeleteOpeningPositions(OpeningPositionCount);
	delete Game;
	UpdateWindow();
	if (!SavedFile) return -1;
	return OpeningPositionCount;
}

int
TRivalWindow::StoreOpeningPosition(TChessBoard* Game, int MatchResult)
{
	int i;
	BOOL Found=FALSE;
	// Get Fen Translation
	char Fen[MAXFEN];
	Game->GetFEN(Fen, FALSE, TRUE);
	// Do we have this position already?
	int FenLen = strlen(Fen);
	for (i=0; i<OpeningPositionCount; i++) {
		if (OpeningPositions[i].FenLen==FenLen)
		{
			if (strcmp(OpeningPositions[i].Fen, Fen)==0) {
				Found=TRUE;
				break;
			}
		}
	}
	// Subtract one for the correct index
	if (Found) {
		switch (MatchResult) {
			case WHITEWIN 	: OpeningPositions[i].WhiteWins++; break;
			case BLACKWIN 	: OpeningPositions[i].BlackWins++; break;
			case DRAW 		: OpeningPositions[i].Draws++; break;
		}
	} else {
		if (OpeningPositionCount<MaxOpeningPositions) {
#ifdef DYNAMICFEN
			OpeningPositions[OpeningPositionCount].Fen = new char[strlen(Fen)+1];
#endif
			strcpy(OpeningPositions[OpeningPositionCount].Fen, Fen);
			OpeningPositions[OpeningPositionCount].WhiteWins=(MatchResult==WHITEWIN);
			OpeningPositions[OpeningPositionCount].BlackWins=(MatchResult==BLACKWIN);
			OpeningPositions[OpeningPositionCount].Draws=(MatchResult==DRAW);
			OpeningPositions[OpeningPositionCount].FenLen = strlen(OpeningPositions[OpeningPositionCount].Fen);
			OpeningPositionCount++;
		} else {
			return MAXOPENINGSREACHED;
		}
	}
	return 0;
}

void
TRivalWindow::CmCreateOpeningBook()
{
	char FileNames[MAXOPENINGFILES][MAXFILENAMELENGTH];
	int Games, GamesUsed, TotalPositions, UniquePositions, Filtered, FilesUsed;
	TOpeningTransferBuffer otb;
	otb.MaxPly=20;
	otb.MaxPositions=50000;
	otb.IncludeUnknown=0;
	otb.WinPercent=40;
	otb.WinPercentBlack=25;
	otb.MinResults=2;
	otb.CountGames=1;
	otb.QuitOnError=0;
	int UnverifiedMoves;
	TOpeningDialog *od = new TOpeningDialog(this, &otb);
	if (od->Execute()==IDOK) {
		TOpenSaveDialog::TData LoadData(/*OFN_ALLOWMULTISELECT|*/OFN_FILEMUSTEXIST|OFN_HIDEREADONLY|OFN_PATHMUSTEXIST, "PGN Files|*.pgn", 0, "", "*");
		if (TFileOpenDialog(this, LoadData, 0, "Select PGN File").Execute()==IDOK) {
			char Error[1000];
			char FileNames[MAXPGNFILES][MAXFILENAMELENGTH];
			int NumFiles=SplitFileNames(LoadData.FileName, FileNames);
			if (NumFiles!=-1) {
				UniquePositions=CreateOpeningBook(this, FileNames, NumFiles, otb.MaxPly, otb.MaxPositions, otb.MinResults, otb.WinPercent, otb.WinPercentBlack, otb.IncludeUnknown, FilesUsed, Filtered, Games, GamesUsed, TotalPositions, UnverifiedMoves, otb.QuitOnError, Error, otb.CountGames);
				if (UniquePositions!=-1) {
					char s[100];
					if (otb.QuitOnError) {
						sprintf(s, "%i file(s) used\n%i positions with minimum results\n%i unique position(s) of %i position(s) examined\n%i game(s) examined\n%i game(s) used\n%s",
							FilesUsed, Filtered, UniquePositions, TotalPositions, Games, GamesUsed, Error);
					} else {
						sprintf(s, "%i file(s) used\n%i positions with minimum results\n%i unique position(s) of %i position(s) examined\n%i game(s) examined\n%i game(s) used\n%i game(s) contained unverified moves (see create.log)",
							FilesUsed, Filtered, UniquePositions, TotalPositions, Games, GamesUsed, UnverifiedMoves);
					}
					::MessageBox(this->HWindow, s, "Create Opening Book", MB_OK);
					WriteCreateLog("\n");
					WriteCreateLog(s);
					CacheOpenings();
				}
				if (CloseRequested && !EscapeHit) CloseWindow();
				EscapeHit=FALSE;
				CloseRequested=FALSE;
			} else {
				char s[100];
				sprintf(s, "Exceeded Maximum number of PGN files (%i)\n", MAXPGNFILES);
				::MessageBox(this->HWindow, s, "Create Opening Book", MB_OK);
			}
		}
	}
}

void
TRivalWindow::WriteXBoard(char* message) {
	if (Verbose) writeMessage(VERBOSE, "Sending Command");
	if (Verbose) writeMessage(VERBOSE, message);
	printf( message );
	fflush(stdout);
}

void
TRivalWindow::SendXBoard(TMove move)
{
	if (move.PromotionPiece==EMPTY) {
		sprintf( messagetext, "move %c%c%c%c\n",
			GetX(move.From)+'a'-1,
			GetY(move.From)+'0',
			GetX(move.To)+'a'-1,
			GetY(move.To)+'0' );
	} else {
		char Promote;
		switch (move.PromotionPiece%100) {
			case QUEEN : Promote='q'; break;
			case KNIGHT : Promote='n'; break;
			case BISHOP : Promote='b'; break;
			case ROOK : Promote='r'; break;
		}
		sprintf( messagetext, "move %c%c%c%c%c\n",
			GetX(move.From)+'a'-1,
			GetY(move.From)+'0',
			GetX(move.To)+'a'-1,
			GetY(move.To)+'0',
			Promote );
	}
	setbuf(stdout, NULL);
	WriteXBoard(messagetext);
}

void
TRivalWindow::makeWinboardComputerMove()
{
	Select.From=0;
	if (!GameOver()) {
		if (Options.Ponder) ComputerMoveFlag = TRUE;	else ComputerMove();
	} else {
		if (Verbose) writeMessage(VERBOSE, "Game over");
	}
}

void
TRivalWindow::MonitorXBoard()
{
		char c;
		static char PreviousCommand[100];
		static count=0;
		count++;
					if (count>1) strcpy(PreviousCommand, command);
					scanf( "%s", command);
					if ((command[0]=='{' ||
							(command[0]>='a' && command[0]<='z') ||
							(command[0]>='A' && command[0]<='Z') ||
							(command[0]>='0' && command[0]<='9'))) {
							if (Verbose) writeMessage(VERBOSE, "Receieved Command", count);
							if (Verbose) writeMessage(VERBOSE, command);
					}
					if (strcmp("quit", command)==0) {
						CloseWindow();
					} else
					if (strcmp("edit", command)==0) {
						int colour=WHITE;
						if (strcmp(PreviousCommand, "a2a3")==0) {
							Game->Square[MOVER]=BLACK;
						}
						int NewBoard[89];
						int finished=FALSE;
						int piece=EMPTY;
						char editcom[10];
						while (!finished) {
							scanf("%s", editcom);
							if (strcmp(editcom, "#")==0) {
								for (int i=0; i<89; i++)
									NewBoard[i]=EMPTY;
							} else
							if (strcmp(editcom, "c")==0) {
								colour=(colour==WHITE ? BLACK : WHITE);
							} else
							if (strcmp(editcom, ".")==0) {
								finished=TRUE;
							} else {
								switch (editcom[0]) {
									case 'P' : piece=PAWN; break;
									case 'Q' : piece=QUEEN; break;
									case 'R' : piece=ROOK; break;
									case 'N' : piece=KNIGHT; break;
									case 'B' : piece=BISHOP; break;
									case 'K' : piece=KING; break;
								}
								int newx=editcom[1]-(int)'a'+1;
								int newy=editcom[2]-(int)'0';
								NewBoard[newx*10+newy]=piece+100*(colour==BLACK);
								NewBoard[MOVER]=Game->GetMover();
								NewBoard[WROOK1MOVED]=!(NewBoard[11]==WR);// && Game->GetMover()==BLACK);
								NewBoard[WROOK8MOVED]=!(NewBoard[81]==WR);// && Game->GetMover()==BLACK);
								NewBoard[BROOK1MOVED]=!(NewBoard[18]==BR);// && Game->GetMover()==WHITE);
								NewBoard[BROOK8MOVED]=!(NewBoard[88]==BR);// && Game->GetMover()==WHITE);
								NewBoard[WKINGMOVED]=!(NewBoard[51]==WR);// && Game->GetMover()==BLACK);
								NewBoard[BKINGMOVED]=!(NewBoard[58]==BR);// && Game->GetMover()==WHITE);
								NewBoard[ENPAWN]=0;
								NewBoard[FIFTYMOVES]=0;
								NewBoard[MOVER]=Game->GetMover();
							}
						}
						delete Game;
						Game=new TChessBoard(NewBoard);
						Options.ComputerWhite=FALSE;
						Options.ComputerBlack=FALSE;
					} else
					if (strcmp("undo", command)==0) {
//						Game->TakeBackMove();
					} else
					if (strcmp("time", command)==0) {
						int hundreths;
						scanf("%i", &hundreths);
						if (Verbose) writeMessage(VERBOSE, "I have left ", hundreths);
						// current mover is the opposition
						if (Game->GetMover()==WHITE) {
							Game->SetBlackClock(hundreths*10);
						} else {
							Game->SetWhiteClock(hundreths*10);
						}
					} else
					if (strcmp("otim", command)==0) {
						int hundreths;
						scanf("%i", &hundreths);
						if (Verbose) writeMessage(VERBOSE, "Opposition has left", hundreths);
						// current mover is the opposition
						if (Game->GetMover()==WHITE) {
							Game->SetWhiteClock(hundreths*10);
						} else {
							Game->SetBlackClock(hundreths*10);
						}
					} else
					if (strcmp("easy", command)==0) {
						Options.Ponder = FALSE;
					} else
					if (strcmp("hard", command)==0) {
						Options.Ponder = TRUE;
					} else
					if (strcmp("post", command)==0) {
						Options.ShowAnalysis = TRUE;
					} else
					if (strcmp("nopost", command)==0) {
						Options.ShowAnalysis = FALSE;
					} else
					if (strcmp("white", command)==0) {
						Options.ComputerBlack = TRUE;
						Options.ComputerWhite = FALSE;
						Game->Square[MOVER]=WHITE;
					} else
					if (strcmp("black", command)==0) {
						Options.ComputerWhite = TRUE;
						Options.ComputerBlack = FALSE;
						Game->Square[MOVER]=BLACK;
					} else
					if (strcmp("draw", command)==0) {
						if (IsDrawPosition()) {
							WriteXBoard( "offer draw\n" );
						}
					} else
					if (strcmp("level", command)==0) {
						int moves, minutes, increment;
						scanf("%i", &moves);
						scanf("%i", &minutes);
						scanf("%i", &increment);
						Options.Level = 2;
						Options.ChampMoves=moves;
						Options.ChampTime=minutes*60;
						Options.BaseMinutes=minutes;
						Options.Increment=increment;
						if (moves==0) {
							searchmethod=Options.SearchMethod=BASEINCREMENT;
						} else {
							searchmethod=Options.SearchMethod=CHAMPIONSHIP;
						}
						if (Verbose) writeMessage(VERBOSE, "ChampTime", Options.ChampTime);
						if (Verbose) writeMessage(VERBOSE, "ChampMoves", Options.ChampMoves);
					} else
					if (strcmp("force", command)==0) {
						Options.ComputerWhite=false;
						Options.ComputerBlack=false;
					} else
					if (strcmp("new", command)==0) {
						CmNewGame();
					} else
					if (strcmp("go", command)==0) {
							if (!GameOver()) {
								if (Game->GetMover()==WHITE) {
									Options.ComputerWhite=true;
                           Options.ComputerBlack=false;
								} else {
									Options.ComputerBlack=true;
									Options.ComputerWhite=false;
								}
								if ((Options.ComputerWhite && Game->GetMover()==WHITE) ||
									 (Options.ComputerBlack && Game->GetMover()==BLACK)) {
								  if (Options.Ponder) ComputerMoveFlag = TRUE;
									else ComputerMove();
								}
							}
					} else
					if (	command[0]>='a' && command[0]<='h' &&
							command[1]>='1' && command[1]<='8' &&
							command[2]>='a' && command[2]<='h' &&
							command[3]>='1' && command[3]<='8') {
							int x1 = command[0]-(int)'a'+1;
							int y1 = command[1]-(int)'0';
							int x2 = command[2]-(int)'a'+1;
							int y2 = command[3]-(int)'0';
							TPoint p1(	BoardOffsetX+5+(Pawn->Width()*(x1-1)),
											BoardOffsetY+5+(Pawn->Height()*(8-y1)));
							TPoint p2(	BoardOffsetX+5+(Pawn->Width()*(x2-1)),
											BoardOffsetY+5+(Pawn->Height()*(8-y2)));
							if (command[4]=='q' || command[4]=='Q') Select.PromotionPiece=QUEEN;
							if (command[4]=='r' || command[4]=='R') Select.PromotionPiece=ROOK;
							if (command[4]=='b' || command[4]=='B') Select.PromotionPiece=BISHOP;
							if (command[4]=='n' || command[4]=='N') Select.PromotionPiece=KNIGHT;
							if (Verbose) {
								int s1x=(p1.x-BoardOffsetX)/Pawn->Width()+1;
								int s1y=8-((p1.y-BoardOffsetY)/Pawn->Height());
								int s2x=(p2.x-BoardOffsetX)/Pawn->Width()+1;
								int s2y=8-((p2.y-BoardOffsetY)/Pawn->Height());
								FILE *log = fopen(VERBOSE, "a");
								fprintf( log, "\nMaking opposition move: %c%c-%c%c-%c with %i%i-%i%i-%i\n",
									(char)x1+'a'-1, (char)y1+'0', (char)x2+'a'-1, (char)y2+'0', command[4],
									s1x,s1y,s2x,s2y, Select.PromotionPiece );
								fclose(log);
							}
							Select.From = x1*10+y1;
							Select.To = x2*10+y2;
							if (Game->VerifyMove(Select)==0) {
								MakeMove(Game, Select);
							} else {
								char error[200];
								char reason[200];
								GetReason(Game->VerifyMove(Select), reason);
								sprintf(error, "Illegal move (%s): %c%c%c%c%c",
									reason,
									command[0],
									command[1],
									command[2],
									command[3],
									(strlen(command)>4 ? command[4] : ' '));
								WriteXBoard(error);
							}
							if ((Game->GetMover()==WHITE && Options.ComputerWhite)
								|| (Game->GetMover()==BLACK && Options.ComputerBlack))
							makeWinboardComputerMove();
					}
}

void
TRivalWindow::Slide(TDib* PieceDib, int ColourOffset, int Distance, int SquareX, int SquareY, int AddX, int AddY)
{
	TClientDC dc(*this);
	dc.SelectObject(*RivalPalette);
	dc.RealizePalette();
	TDib* BackGround=new TDib(h, PAWN1);
	for (int i=0; i<=Distance; i++) {
		GrabSprite((char *)ScreenBits, SquareX+AddX*i, SquareY+AddY*i, BackGround);
		DibXY((char *)ScreenBits, SquareX+AddX*i, SquareY+AddY*i, PieceDib, 256, ColourOffset, 224, 238);
		WinGBitBlt((HDC)dc, SquareX-1+AddX*i, SquareY-1+AddY*i, SquareX+Pawn->Width()+1+AddX*i,SquareY+Pawn->Height()+1+AddY*i, ScreenDc, SquareX-1+AddX*i, SquareY-1+AddY*i);
//		InvalidateRect(TRect(SquareX-1+AddX*i, SquareY-1+AddY*i, SquareX+Pawn->Width()+1+AddX*i,SquareY+Pawn->Height()+1+AddY*i), FALSE);
		DibXY((char *)ScreenBits, SquareX+AddX*i, SquareY+AddY*i, BackGround);
	}
	delete BackGround;
}

void
TRivalWindow::SlidePiece(TMove Move)
{
	if (!Options.PieceSlide) return;
	int FromX = GetX(Move.From);
	int FromY = GetY(Move.From);
	int ToX = GetX(Move.To);
	int ToY = GetY(Move.To);

	int Piece=Game->GetSquare(FromX*10+FromY)%100;
	int IsWhite=Game->GetSquare(FromX*10+FromY)<100;

	if (Options.InvertBoard)
	{
		FromX = 9 - FromX;
		FromY = 9 - FromY;
		ToX = 9 - ToX;
		ToY = 9 - ToY;
	}

	TDib* PieceDib;
	int SquareX=(FromX-1)*Pawn->Width()+BoardOffsetX;
	int SquareY=(8-FromY)*Pawn->Height()+BoardOffsetY;

	if ( (FromX+FromY)%2==0 )
		DibXY((char *)ScreenBits, SquareX, SquareY, BlackSquare); else
		DibXY((char *)ScreenBits, SquareX, SquareY, WhiteSquare);

	int ColourOffset=(IsWhite ? Options.WhiteOffset*16 : Options.BlackOffset*16);
	int Distance;
	switch(Piece) {
		case KING   : if (Options.Pieces==0) PieceDib=new TDib(h, KING1); else if (Options.Pieces==1) PieceDib=new TDib(h, KING2); else PieceDib=new TDib(h, KING3); break;
		case QUEEN  : if (Options.Pieces==0) PieceDib=new TDib(h, QUEEN1); else if (Options.Pieces==1) PieceDib=new TDib(h, QUEEN2); else PieceDib=new TDib(h, QUEEN3); break;
		case ROOK   : if (Options.Pieces==0) PieceDib=new TDib(h, ROOK1); else if (Options.Pieces==1) PieceDib=new TDib(h, ROOK2); else PieceDib=new TDib(h, ROOK3); break;
		case KNIGHT : if (Options.Pieces==0) PieceDib=new TDib(h, KNIGHT1); else if (Options.Pieces==1) PieceDib=new TDib(h, KNIGHT2); else PieceDib=new TDib(h, KNIGHT3); break;
		case BISHOP : if (Options.Pieces==0) PieceDib=new TDib(h, BISHOP1); else if (Options.Pieces==1) PieceDib=new TDib(h, BISHOP2); else PieceDib=new TDib(h, BISHOP3); break;
		case PAWN   : if (Options.Pieces==0) PieceDib=new TDib(h, PAWN1); else if (Options.Pieces==1) PieceDib=new TDib(h, PAWN2); else PieceDib=new TDib(h, PAWN3); break;
	}

	if (ToX==FromX) {
		if (ToY>FromY) {
		  Distance=(ToY-FromY)*Pawn->Height();
		  Slide(PieceDib, ColourOffset, Distance, SquareX, SquareY, 0, -1);
		}
		if (ToY<FromY) {
		  Distance=(FromY-ToY)*Pawn->Height();
		  Slide(PieceDib, ColourOffset, Distance, SquareX, SquareY, 0, 1);
		}
	} else
	if (ToY==FromY) {
		if (ToX>FromX) {
		  Distance=(ToX-FromX)*Pawn->Width();
		  Slide(PieceDib, ColourOffset, Distance, SquareX, SquareY, 1, 0);
		}
		if (FromX>ToX) {
		  Distance=(FromX-ToX)*Pawn->Width();
		  Slide(PieceDib, ColourOffset, Distance, SquareX, SquareY, -1, 0);
		}
	} else
	if (Piece!=KNIGHT && FromY>ToY && FromX>ToX) {
		Distance=(FromY-ToY)*Pawn->Width();
		Slide(PieceDib, ColourOffset, Distance, SquareX, SquareY, -1, 1);
	} else
	if (Piece!=KNIGHT && FromY<ToY && FromX>ToX) {
		Distance=(ToY-FromY)*Pawn->Width();
		Slide(PieceDib, ColourOffset, Distance, SquareX, SquareY, -1, -1);
	} else
	if (Piece!=KNIGHT && FromY>ToY && FromX<ToX) {
		Distance=(FromY-ToY)*Pawn->Width();
		Slide(PieceDib, ColourOffset, Distance, SquareX, SquareY, 1, 1);
	} else
	if (Piece!=KNIGHT && FromY<ToY && FromX<ToX) {
		Distance=(ToY-FromY)*Pawn->Width();
		Slide(PieceDib, ColourOffset, Distance, SquareX, SquareY, 1, -1);
	}
	if (Piece==KNIGHT) {
		if (ToY>FromY) {
			Distance=(ToY-FromY)*Pawn->Height();
			Slide(PieceDib, ColourOffset, Distance, SquareX, SquareY, 0, -1);
			SquareY-=Distance;
		}
		if (FromY>ToY) {
			Distance=(FromY-ToY)*Pawn->Height();
			Slide(PieceDib, ColourOffset, Distance, SquareX, SquareY, 0, 1);
			SquareY+=Distance;
		}
		if (FromX>ToX) {
			Distance=(FromX-ToX)*Pawn->Height();
			Slide(PieceDib, ColourOffset, Distance, SquareX, SquareY, -1, 0);
		}
		if (ToX>FromX) {
			Distance=(ToX-FromX)*Pawn->Height();
			Slide(PieceDib, ColourOffset, Distance, SquareX, SquareY, 1, 0);
		}
	}

	delete PieceDib;
}

void
TRivalWindow::DrawClockNumber(int Offset, int Number, int ColonAfter, int ColourOffset)
{
	if (WinBoard) return;
// Draws two digit numbers
	if (Offset<0) Offset+=ClockRectangle->right-ShadowOffset; else
	  Offset+=ClockRectangle->left;
	FadeXY((char *)ScreenBits, Offset+ShadowOffset, ClockRectangle->top+ShadowOffset, NumberDib[Number/10], 256, Fade, 224-((Options.BackOffset*16)), 238-((Options.BackOffset*16)));
	DibXY((char *)ScreenBits, Offset, ClockRectangle->top, NumberDib[Number/10], 256, ColourOffset, 224, 238);
	FadeXY((char *)ScreenBits, Offset+16+ShadowOffset, ClockRectangle->top+ShadowOffset, NumberDib[Number%10], 256, Fade, 224-((Options.BackOffset*16)), 238-((Options.BackOffset*16)));
	DibXY((char *)ScreenBits, Offset+16, ClockRectangle->top, NumberDib[Number%10], 256, ColourOffset, 224, 238);
	if (ColonAfter) FadeXY((char *)ScreenBits, Offset+32+ShadowOffset, ClockRectangle->top+ShadowOffset, Colon, 256, Fade, 224-((Options.BackOffset*16)), 238-((Options.BackOffset*16)));
	if (ColonAfter) DibXY((char *)ScreenBits, Offset+32, ClockRectangle->top, Colon, 256, ColourOffset, 224, 238);
}

void
TRivalWindow::DrawClocks()
{
	if (WinBoard) return;
	int WhiteClock=Game->GetWhiteClock();
	int BlackClock=Game->GetBlackClock();
	WinGBitBlt(ScreenDc, ClockRectangle->left,
								ClockRectangle->top,
								ClockRectangle->right,
								ClockRectangle->bottom,
								BackGroundDc,
								ClockRectangle->left,
								ClockRectangle->top);
	DrawClockNumber(0, (int)(WhiteClock/1000)/3600, TRUE, Options.WhiteOffset*16);
	DrawClockNumber(48, (int)(WhiteClock/1000)%3600/60, TRUE, Options.WhiteOffset*16);
	DrawClockNumber(96, (int)(WhiteClock/1000)%60, FALSE, Options.WhiteOffset*16);
// minus values represent distance relative to end of clock rectangle
	DrawClockNumber(-128, (int)(BlackClock/1000)/3600, TRUE, Options.BlackOffset*16);
	DrawClockNumber(-80, (int)(BlackClock/1000)%3600/60, TRUE, Options.BlackOffset*16);
	DrawClockNumber(-32, (int)(BlackClock/1000)%60, FALSE, Options.BlackOffset*16);
}

void
TRivalWindow::DrawNumber(int X, int Y, float Number, int FadeLow, int FadeHigh, int ColourOffset)
{
	if (WinBoard) return;
	 char s[20];
	 if ((Number-(long)Number)!=0.0)
		sprintf(s, "%.2f", Number); else
		sprintf(s, "%li", (long)Number);
	 for (int i=0; i<strlen(s); i++) {
		 if (s[i]=='-') s[i]=67; else
		 if (s[i]=='.') s[i]=68;
		 FadeXY((char *)ScreenBits, X+2, Y+2, PathNums[s[i]-48], 256,
			 Fade, FadeLow, FadeHigh);
		 DibXY((char *)ScreenBits, X, Y, PathNums[s[i]-48], 256, ColourOffset, 224, 238);
		 X+=PathNums[0]->Width();
	 }
}

void
TRivalWindow::DrawPathNumber(int Column, int Row, int Number, int ColourOffset)
{
	if (WinBoard) return;
	FadeXY((char far*)ScreenBits,
			AnalysisRectangle->left+Column+2,
			AnalysisRectangle->top+Row+2,
			PathNums[Number],
			256,
			Fade,
			160+(Options.AnalysisBoxStyle*16),
			174+(Options.AnalysisBoxStyle*16));
	DibXY((char far*)ScreenBits,
			AnalysisRectangle->left+Column,
			AnalysisRectangle->top+Row,
			PathNums[Number],
			256,
			ColourOffset, 224, 238);
}

void
TRivalWindow::DrawMoveNumber(int Column, int Row, int Number, int ColourOffset)
{
	if (WinBoard) return;
	FadeXY((char far*)ScreenBits,
			MoveBoxRectangle->left+Column+2,
			MoveBoxRectangle->top+Row+2,
			PathNums[Number],
			256,
			Fade,
			160+(Options.MoveBoxStyle*16),
			174+(Options.MoveBoxStyle*16));
	DibXY((char far*)ScreenBits,
			MoveBoxRectangle->left+Column,
			MoveBoxRectangle->top+Row,
			PathNums[Number],
			256,
			ColourOffset, 224, 238);
}

/*void
TRivalWindow::DrawMove(int xOffset, int yOffset, int yStep)
{
	int NWidth=PathNums[0]->Width();
	int NHeight=PathNums[0]->Height();
	DrawPathNumber(NWidth*(xOffset)+2, yOffset+NHeight*(yStep), Path2.Move[i].From/10+9, (Options.AnalysisTextOffset*16));
	DrawPathNumber(NWidth*(xOffset+1+(7*((i%2)==0)))+2, yOffset+NHeight*(yStep), Path2.Move[i].From%10, (Options.AnalysisTextOffset*16));
	if (Path2.Move[i].Capture!=EMPTY) {
		DrawPathNumber(NWidth*(xOffset+2+(7*((i%2)==0)))+2, yOffset+NHeight*(yStep), 18, (Options.AnalysisTextOffset*16));
	}
	DrawPathNumber(NWidth*((xOffset+2+(7*((i%2)==0)))+(Path2.Move[i].Capture!=EMPTY))+2, yOffset+NHeight*(yStep), Path2.Move[i].To/10+9, (Options.AnalysisTextOffset*16));
	DrawPathNumber(NWidth*((xOffset+3+(7*((i%2)==0)))+(Path2.Move[i].Capture!=EMPTY))+2, yOffset+NHeight*(yStep), Path2.Move[i].To%10, (Options.AnalysisTextOffset*16));
} */

void
TRivalWindow::DrawCurrentSearchPath(TPath Path2)
{
	if (WinBoard) return;
	int i, j, index;
	int NWidth=PathNums[0]->Width();
	int NHeight=PathNums[0]->Height();
	TMove PathMove;
	char pv[MAXDEPTH*7];
	strcpy(pv, "");
	WinGBitBlt(ScreenDc, AnalysisRectangle->left,
								AnalysisRectangle->top,
								AnalysisRectangle->right,
								AnalysisRectangle->bottom,
								BackGroundDc,
								AnalysisRectangle->left,
								AnalysisRectangle->top);
	if (Path2.Move[0].From!=0) {
		TChessBoard *PathGame;
		switch (Options.Variant) {
			case CHESS : PathGame=new TChessBoard(PathBoard); break;
			case SHATRANJ : PathGame=new TShatranjBoard(PathBoard); break;
			case KINGLET : PathGame=new TKingletBoard(PathBoard); break;
			case SELFTAKE : PathGame=new TSelfTakeBoard(PathBoard); break;
		}
		DrawNumber(AnalysisRectangle->left+(Path2.Depth<10 ? 59 : 50), AnalysisRectangle->top+35, Path2.Depth, 160+(Options.AnalysisBoxStyle*16), 174+(Options.AnalysisBoxStyle*16), (Options.AnalysisTextOffset*16));
		DrawNumber(AnalysisRectangle->left+(Path2.Value<1000 && Path2.Value>=0 ? 72 : 61)+NWidth*3, AnalysisRectangle->top+35, Path2.Value/100.0, 160+(Options.AnalysisBoxStyle*16), 174+(Options.AnalysisBoxStyle*16), (Options.AnalysisTextOffset*16));
		for (i=0; Path2.Move[i].From!=0; i++) {
			PathMove=Path2.Move[i];
			char s[10];
			PathGame->TranslateMoveToAlgebraic(s, PathMove);
			strcat(pv, s);
			strcat(pv, " ");
			if (strlen(s)>6) s[6]='\0';
			for (j=0; j<strlen(s); j++) {
				if (strchr(piecelist, s[j])!=NULL) {
					index=strchr(piecelist, s[j])-piecelist;
					if (i<16) {
						DrawPathNumber(NWidth*(j+1+(7*((i%2)==1)))+(i%2==1 ? 2 : -3), 66+NHeight*(i/2), index, (Options.AnalysisTextOffset*16));
					}
				}
			}
			PathGame->MakeMove(PathMove, TRUE);
		}
		delete PathGame;
	}
	if (Searching || Pondering || Path2.Move[0].From!=0)
		DrawNumber(AnalysisRectangle->left+60, AnalysisRectangle->top+5, Path2.Nodes, 160+(Options.AnalysisBoxStyle*16), 174+(Options.AnalysisBoxStyle*16), (Options.AnalysisTextOffset*16));
	if (Path2.Nodes!=globalNodes &&
			(globalPVMove.From!=Path2.Move[0].From || globalPVMove.To!=Path2.Move[0].To ||
					globalDepth!=Path2.Depth)) {
		globalNodes=Path2.Nodes;
		globalPVMove=Path2.Move[0];
		globalDepth=Path2.Depth;
	}
}

void
TRivalWindow::DrawPieces()
{
	if (WinBoard) return;
	 int Width=Pawn->Width();
	 int Height=Pawn->Height();
	 for (int x=1; x<=8; x++) {
		 for (int y=1; y<=8; y++) {
			 int Piece, IsWhite;
			 if (Options.InvertBoard)
			 {
				 Piece=(InSetup ? SetupBoard[(9-x)*10+(9-y)]%100 : Game->GetSquare((9-x)*10+(9-y))%100);
				 IsWhite=(InSetup ? SetupBoard[(9-x)*10+(9-y)]<100 : Game->GetSquare((9-x)*10+(9-y))<100);
			 }
			 else
			 {
				 Piece=(InSetup ? SetupBoard[x*10+y]%100 : Game->GetSquare(x*10+y)%100);
				 IsWhite=(InSetup ? SetupBoard[x*10+y]<100 : Game->GetSquare(x*10+y)<100);
			 }
			 switch (Piece) {
				 case KING : DibXY((char *)ScreenBits, (x-1)*Width+BoardOffsetX, (8-y)*Height+BoardOffsetY, King, 256,
						  (IsWhite ? Options.WhiteOffset*16 : Options.BlackOffset*16), 224, 238); break;
				 case PAWN : DibXY((char *)ScreenBits, (x-1)*Width+BoardOffsetX, (8-y)*Height+BoardOffsetY, Pawn, 256,
						  (IsWhite ? Options.WhiteOffset*16 : Options.BlackOffset*16), 224, 238); break;
				 case QUEEN : DibXY((char *)ScreenBits, (x-1)*Width+BoardOffsetX, (8-y)*Height+BoardOffsetY, Queen, 256,
						  (IsWhite ? Options.WhiteOffset*16 : Options.BlackOffset*16), 224, 238); break;
				 case ROOK : DibXY((char *)ScreenBits, (x-1)*Width+BoardOffsetX, (8-y)*Height+BoardOffsetY, Rook, 256,
						  (IsWhite ? Options.WhiteOffset*16 : Options.BlackOffset*16), 224, 238); break;
				 case KNIGHT : DibXY((char *)ScreenBits, (x-1)*Width+BoardOffsetX, (8-y)*Height+BoardOffsetY, Knight, 256,
						  (IsWhite ? Options.WhiteOffset*16 : Options.BlackOffset*16), 224, 238); break;
				 case BISHOP : DibXY((char *)ScreenBits, (x-1)*Width+BoardOffsetX, (8-y)*Height+BoardOffsetY, Bishop, 256,
						  (IsWhite ? Options.WhiteOffset*16 : Options.BlackOffset*16), 224, 238); break;
			 }
		 }
	 }
	 if (Select.From!=0) {
		 tagRECT r;
		 int rectMoveX, rectMoveY;
		 if (Options.InvertBoard)
		 {
			 rectMoveX = 8-GetX(Select.From);
			 rectMoveY = GetY(Select.From)-1;
		 }
		 else
		 {
			 rectMoveX = GetX(Select.From)-1;
			 rectMoveY = 8-GetY(Select.From);
		 }
		 r.left=(rectMoveX)*Pawn->Width()+BoardOffsetX; r.right=r.left+Pawn->Width();
		 r.top=(rectMoveY)*Pawn->Height()+BoardOffsetY; r.bottom=r.top+Pawn->Height();
		 ::FrameRect(ScreenDc, &r, (HBRUSH)TBrush(TColor(123, 132, 100)));
		 r.left++; r.right--; r.top++; r.bottom--;
		 ::FrameRect(ScreenDc, &r, (HBRUSH)TBrush(TColor(123, 132, 100)));
	 }
	 if (Options.ShowAnalysis) DrawCurrentSearchPath(Path2);
	 DrawNumber(MoveBoxRectangle->left+50, MoveBoxRectangle->top+6,
					Game->LastMoveMade()/2+(Game->LastMoveMade()%2!=0),
					160+(Options.MoveBoxStyle*16),
					174+(Options.MoveBoxStyle*16),
					(Options.MoveBoxTextOffset*16));

// You probably just wouldn't believe how tricky it was to figure this next
// lot out.
	 int WhitePlaysOdd=(Game->ResetTo[MOVER]==WHITE);
	 int EndMoveNum=Game->LastMoveMade();
	 int StartMoveNum=EndMoveNum-5;
	 if (WhitePlaysOdd) {
		if (StartMoveNum%2==0) StartMoveNum++;
	 } else {
		if (StartMoveNum%2!=0) {
		  if (StartMoveNum>1) StartMoveNum++;
		  if (StartMoveNum==1 && EndMoveNum==6) StartMoveNum=2;
		}
	 }
	 int Indent=FALSE;
	 if (StartMoveNum<1) StartMoveNum=1;
	 if (!WhitePlaysOdd && StartMoveNum==1)
		Indent=TRUE;
	 int NWidth=PathNums[0]->Width();
	 int NHeight=PathNums[0]->Height();
	 TMove Move;
	 int Down=0;
	 int i;
	 int W=Game->GetWhiteClock();
	 int B=Game->GetBlackClock();
	 Game->TakeBackAllMoves();
	 for (i=1; i<StartMoveNum; i++) Game->ReplayMove();
	 for (i=StartMoveNum; i<=EndMoveNum; i++) {
		 Move=Game->MovesMade[i];
		 char s[100];
		 int index;
		 Game->TranslateMoveToAlgebraic(s, Move);
		 if (strlen(s)>6) s[6]='\0';
		 // Modify this lot
		 for (int j=0; j<strlen(s); j++) {
			if (strchr(piecelist, s[j])!=NULL) {
				index=strchr(piecelist, s[j])-piecelist;
				DrawMoveNumber(NWidth*(j+1)-2+(Indent*(NWidth*7+5)), 38+NHeight*Down, index, (Options.MoveBoxTextOffset*16));
			}
		 }
		 Indent=!Indent;
		 if (!Indent) Down++;
		 Game->ReplayMove();
	 }
	 Game->SetWhiteClock(W);
	 Game->SetBlackClock(B);
}

void
TRivalWindow::DrawBackGround()
{
	 if (WinBoard) return;
	 delete WhiteSquare;
	 delete BlackSquare;
	 switch (Options.ChessBoard) {
					 case 0 : WhiteSquare=new TDib(h, WHITE1);
								 BlackSquare=new TDib(h, BLACK1);
								 break;
					 case 1 : WhiteSquare=new TDib(h, WHITE2);
								 BlackSquare=new TDib(h, BLACK2);
								 break;
					 case 2 : WhiteSquare=new TDib(h, WHITE3);
								 BlackSquare=new TDib(h, BLACK3);
								 break;
	 }
	 delete BackGround;
	 switch (Options.BackGround) {
		 case 0 : BackGround=new TDib(h, BACK1); break;
		 case 1 : BackGround=new TDib(h, BACK2); break;
	 }
	 int Width=BackGround->Width();
	 int Height=BackGround->Height();
	 int x;
	 int y;
	 int NumX=640/Width+1;
	 int NumY=416/Height+1;
	 for (x=0; x<NumX; x++)
		 for (y=0; y<NumY; y++)
			 DibXY((char *)BackGroundBits, (x*Width), (y*Height), BackGround,
				  0, -(Options.BackOffset*16), 224, 238);
	 FadeXY((char *)BackGroundBits, AnalysisRectangle->left+ShadowOffset, AnalysisRectangle->top+ShadowOffset, AnalysisBox, 0, Fade, 224-((Options.BackOffset*16)), 238-((Options.BackOffset*16)));
	 DibXY((char *)BackGroundBits, AnalysisRectangle->left, AnalysisRectangle->top, AnalysisBox,
				  0, (Options.AnalysisBoxStyle*16), 160, 174);
	 FadeXY((char *)BackGroundBits, MoveBoxRectangle->left+ShadowOffset, MoveBoxRectangle->top+ShadowOffset, MoveBox, 0, Fade, 224-((Options.BackOffset*16)), 238-((Options.BackOffset*16)));
	 DibXY((char *)BackGroundBits, MoveBoxRectangle->left, MoveBoxRectangle->top, MoveBox,
				  0, (Options.MoveBoxStyle*16), 160, 174);
	 Width=WhiteSquare->Width();
	 Height=WhiteSquare->Height();
	 for (x=0; x<8; x++)
		 for (y=0; y<8; y++)  {
			  FadeXY((char *)BackGroundBits, (x*Width)+BoardOffsetX+ShadowOffset, (y*Height)+BoardOffsetY+ShadowOffset, WhiteSquare, 0, Fade, 224-((Options.BackOffset*16)), 238-((Options.BackOffset*16)));
		 }
	 for (x=0; x<8; x++)
		 for (y=0; y<8; y++)  {
			  if (((x+y)%2)==0)
					DibXY((char *)BackGroundBits, (x*Width)+BoardOffsetX, (y*Height)+BoardOffsetY, WhiteSquare); else
					DibXY((char *)BackGroundBits, (x*Width)+BoardOffsetX, (y*Height)+BoardOffsetY, BlackSquare);
		 }
	 tagRECT r;
	 r.left=BoardOffsetX-1; r.right=BoardOffsetX+(8*Width)+1;
	 r.top=BoardOffsetY-1; r.bottom=BoardOffsetY+(8*Height)+1;
	 ::FrameRect(BackGroundDc, &r, (HBRUSH)TBrush(TColor(0, 0, 0)));
}

void
TRivalWindow::UpdateAllGraphics(int DrawBack)
{
	if (WinBoard) return;
	if (DrawBack) DrawBackGround(); // Draws To BackGroundDc
	WinGBitBlt(ScreenDc, 0, 0, 639, 479, BackGroundDc, 0, 0); // Puts BackGround onto ScreenDc
	DrawPieces();  // Draws To ScreenDc
	DrawClocks();
	Invalidate(FALSE);  // Draws ScreenDc to Client Window Dc
	UpdateWindow();
}

void
TRivalWindow::RewriteTheOpeningBook()
{
	int Status=Game->GameStatus();
	int i, j;
	char s[MAXFEN];
	Game->TakeBackAllMoves();
	BOOL Found=TRUE;
	while (Found) {
		Found=FALSE;
		Game->ReplayMove();
		Game->GetFEN(s, TRUE, TRUE);
		for (j=0; j<CachedOpenings; j++) {
			if (strcmp(OpeningPositions[j].Fen, s)==0) {
				switch (Status) {
					case 1 : OpeningPositions[j].WhiteWins++; break;
					case 2 : OpeningPositions[j].BlackWins++; break;
					case 3 : OpeningPositions[j].Draws++; break;
					case 4 : OpeningPositions[j].Draws++; break;
					case 5 : OpeningPositions[j].Draws++; break;
				}
				Found=TRUE;
			}
		}
	}
	Game->ReplayAllMoves();
	FILE* f=fopen(OPENINGS, "w");
	for (i=0; i<CachedOpenings; i++) {
		fprintf(f, "%s W %i B %i D %i\n",
				OpeningPositions[i].Fen,
				OpeningPositions[i].WhiteWins,
				OpeningPositions[i].BlackWins,
				OpeningPositions[i].Draws);
	}
	fclose(f);
}

void
TRivalWindow::Paint(TDC& dc, BOOL, TRect& rect)
{
	if (WinBoard) return;
	dc.SelectObject(*RivalPalette);
	dc.RealizePalette();
	WinGBitBlt((HDC)dc, rect.left, rect.top, rect.right, rect.bottom,
		 ScreenDc, rect.left, rect.top);
}

int
TRivalWindow::GameOver()
{
	int Status=Game->GameStatus();
	int i;
	if (Status>0 && Options.OpeningLearning) {
		RewriteTheOpeningBook();
	}
	if (WinBoard) {
		if (WhiteMaterial()+BlackMaterial()==0) {
			WriteXBoard( "offer draw\n");
			return FALSE;
		}
		switch(Status) {
			case 1 : WriteXBoard( "1-0 {White mates}\n"); return TRUE;
			case 2 : WriteXBoard( "0-1 {Black mates}\n"); return TRUE;
			case 3 : WriteXBoard( "1/2-1/2 {Stalemate}\n"); return TRUE;
			case 4 : WriteXBoard( "1/2-1/2 {Draw by repetition}\n"); return TRUE;
			case 5 : WriteXBoard( "1/2-1/2 {Draw by fifty moves}\n"); return TRUE;
			case 6, 7, 8, 9 : return TRUE;
			case 0 : return FALSE;
		}
		return FALSE;
	} else {
		switch(Status)
		{
			case 1 : ::MessageBox(this->HWindow, "CheckMate\n\nWhite Wins", "Game Over", MB_ICONEXCLAMATION | MB_OK); return TRUE;
			case 2 : ::MessageBox(this->HWindow, "CheckMate\n\nBlack Wins", "Game Over", MB_ICONEXCLAMATION | MB_OK); return TRUE;
			case 3 : ::MessageBox(this->HWindow, "StaleMate", "Game Over", MB_ICONEXCLAMATION | MB_OK); return TRUE;
			case 4 : ::MessageBox(this->HWindow, "Draw by Three Repititions Rule", "Game Over", MB_ICONEXCLAMATION | MB_OK); return TRUE;
			case 5 : ::MessageBox(this->HWindow, "Draw by Fifty Move Rule", "Game Over", MB_ICONEXCLAMATION | MB_OK); return TRUE;
			case 6 : ::MessageBox(this->HWindow, "All White Pawns Lost - Black Wins", "Game Over", MB_ICONEXCLAMATION | MB_OK); return TRUE;
			case 7 : ::MessageBox(this->HWindow, "All Black Pawns Lost - White Wins", "Game Over", MB_ICONEXCLAMATION | MB_OK); return TRUE;
			case 8 : ::MessageBox(this->HWindow, "Bare King - White Wins", "Game Over", MB_ICONEXCLAMATION | MB_OK); return TRUE;
			case 9 : ::MessageBox(this->HWindow, "Bare King - Black Wins", "Game Over", MB_ICONEXCLAMATION | MB_OK); return TRUE;
			case 0 : return FALSE;
		}
		return FALSE;
	}
}

void
TRivalWindow::CreateSearcherAndBoard()
{
	switch (Options.Variant) {
			case CHESS   : Searcher=new TChessSearcher(Options.HashTableSize);
								if (InSetup)
									Game=new TChessBoard(SetupBoard);
								else
									Game=new TChessBoard;
								break;
			case KINGLET : Searcher=new TKingletSearcher(Options.HashTableSize);
								if (InSetup)
									Game=new TKingletBoard(SetupBoard);
								else
									Game=new TKingletBoard;
								break;
			case SHATRANJ: Searcher=new TShatranjSearcher(Options.HashTableSize);
								if (InSetup)
									Game=new TShatranjBoard(SetupBoard);
								else
									Game=new TShatranjBoard;
								break;
			case SELFTAKE: Searcher=new TSelfTakeSearcher(Options.HashTableSize);
								if (InSetup)
									Game=new TSelfTakeBoard(SetupBoard);
								else
									Game=new TSelfTakeBoard;
	}
	if (Options.SearchMethod==BASEINCREMENT) {
		Game->SetWhiteStart(Options.BaseMinutes*60*1000);
		Game->SetBlackStart(Options.BaseMinutes*60*1000);
	}
	if (Options.SearchMethod==CHAMPIONSHIP) {
		Game->SetWhiteStart(Options.ChampTime*1000);
		Game->SetBlackStart(Options.ChampTime*1000);
	}
	GameOver();
}

void
TRivalWindow::CmNewGame()
{
	GameCount++;
	if (Searching || Pondering) Searcher->ExitWithoutMove();
	if (WinBoard) {
		Options.ComputerWhite=FALSE;
		Options.ComputerBlack=TRUE;
		delete Game;
		delete Searcher;
		Game=new TChessBoard;
		Searcher=new TChessSearcher(Options.HashTableSize);
		Path2.Move[0].From=0;
		UpdateAllGraphics();
	} else {
		 TNewGameDialog* Ngd;
		 int Info=Options.Variant;
		 Ngd=new TNewGameDialog(this, &Info);
		 if (Ngd->Execute()==IDOK) {
			InSetup=FALSE;
			Options.Variant=Info;
			Select.From=0;
			delete Game;
			delete Searcher;
			CreateSearcherAndBoard();
			Path2.Move[0].From=0;
			UpdateAllGraphics();
			Options.ComputerWhite=FALSE;
		 } else {
			delete Ngd;
			return;
		 }
		 delete Ngd;
	}
	if (Options.SearchMethod==BASEINCREMENT) {
		Game->SetWhiteStart(Options.BaseMinutes*60*1000);
		Game->SetBlackStart(Options.BaseMinutes*60*1000);
	}
	if (Options.SearchMethod==CHAMPIONSHIP) {
		Game->SetWhiteStart(Options.ChampTime*1000);
		Game->SetBlackStart(Options.ChampTime*1000);
	}
	if (Verbose) writeMessage(VERBOSE, "Game", GameCount);
	if (Verbose) writeMessage(VERBOSE, "=========");
	WhiteFell=BlackFell=FALSE;
	ResetVariables();
}

void
TRivalWindow::TestPosition(
char* output,
char* Fen,
int Depth,
int Time,
char* Move,
int Score,
int& TotalNodes,
int& TotalSolved,
int& TotalMovesNotScore,
int& TotalScoresNotMove,
int& TotalMilliseconds,
int PositionNum,
int ExitWhenFound)
{
	TestTime=Time*1000;
	int StartTime;
	char PathMoveString[10];
	TPath TestPath;
	if (Verbose) {
		writeMessage(VERBOSE, "Translating Fen to board");
		writeMessage(VERBOSE, Fen);
	}
	if (Game->TranslateFenToBoard(Fen, SetupBoard)>0) {
		delete Game;
		delete Searcher;
		if (Verbose) writeMessage(VERBOSE, "Creating Searcher and Board");
		InSetup=TRUE;
		CreateSearcherAndBoard();
		 InSetup=FALSE;
		Options.SearchMethod=INFINITETIME;
		if (Verbose) writeMessage(VERBOSE, "Setting initial position");
		Searcher->SetInitialPosition(*Game);
		InitialiseSearchParameters();
		Searcher->SetMaxDepth(Depth);
		if (Verbose) writeMessage(VERBOSE, "Updating graphics");
		UpdateAllGraphics();
		Searcher->ClearHashTable();
		_fmemcpy((void *)PathBoard, (void *)Game->Square, 89*sizeof(int));
		StartTime=GetTickCount();
		if (Verbose) writeMessage(VERBOSE, "Searching...");
		TMove StructMove;
		if (ExitWhenFound) {
			if (Verbose) writeMessage(VERBOSE, "Translating Move...");
			if (Verbose) writeMessage(VERBOSE, Move);
			Game->TranslateAlgebraicMove(Move, StructMove);
			if (Verbose) writeMessage(VERBOSE, "From", StructMove.From);
			if (Verbose) writeMessage(VERBOSE, "To", StructMove.To);
		} else {
			StructMove.From=0;
		}
		Searcher->SetStopMove(StructMove);
		Searcher->SetStopScore(Score);
		Searching=TRUE;
		TestSuite=TRUE;
		globalstarttime=GetTickCount();
		TestPath=Searcher->Search();
		TestPath=Searcher->GetPrincipalPath();
		StructMove.From=0;
		Searcher->SetStopMove(StructMove);
		Searcher->SetStopScore(MAXSCORE);
		Searching=FALSE;
		//if (CloseRequested) CloseWindow();
		TestSuite=FALSE;
		if (Verbose) writeMessage(VERBOSE, "Finished searching...");
		TotalNodes+=TestPath.Nodes;
		TotalMilliseconds+=(GetTickCount()-StartTime);
		Game->TranslateMoveToAlgebraic(PathMoveString, TestPath.Move[0]);
		char s[MAXLINELENGTH];
		sprintf(s, "**************************\nPosition %i\n%s\n==========================", PositionNum, Fen);
		writeMessage(output, s);
		Searcher->WritePV(output, NULL);
		if (strcmp(PathMoveString, Move)==0) {
			if (TestPath.Value>=Score) {
				TotalSolved++;
				writeMessage(output, "Solution found.");
			}	else {
				TotalMovesNotScore++;
				writeMessage(output, "Move but not score.");
			}
		} else
		if (TestPath.Value>=Score) {
			TotalScoresNotMove++;
			writeMessage(output, "Score but not move.");
		} else
		writeMessage(output, "Solution not found.");
	}
}

void
TRivalWindow::RunEPDSuite(char* FileName, int Task, int Depth=0, int Time=0, int ExitWhenFound=0)
{
	FILE *f;
	char output[MAXFILENAMELENGTH];
	char ch;
	static int Count=0;
	static int TotalNodes=0;
	static int TotalSolved=0;
	static int TotalMilliseconds=0;
	static int TotalPositions=0;
	static float TotalAverage=0.0;
	int PreviousSolved;
	int PreviousNodes;
	int PreviousMillis;
	int PreviousPositions;
	float PreviousAverage;
	PreviousSolved=TotalSolved;
	PreviousNodes=TotalNodes;
	PreviousMillis=TotalMilliseconds;
	PreviousPositions=TotalPositions;
	PreviousAverage=TotalAverage;
	int MinTree;
	char Move[10];
	strcpy(Move, "None");
	if (Task!=EXECUTE) {
		switch (Task) {
			case CLEARVALUES :
					TotalAverage=0.0;
					TotalNodes=0;
					TotalMilliseconds=0;
					TotalSolved=0;
					Count=0;
					TotalPositions=0;
					break;
			case WRITESUMMARY :
				MinTree=(Power((int)(TotalAverage)/(TotalPositions), Depth/2)*2)-1;
				FILE* f=fopen(SUITESUMMARY, "a");
				fprintf(f, "***************************\n");
				fprintf(f, "Test suite summary\n");
				fprintf(f, "---------------------------\n");
				fprintf(f, "Suite starting with file %s\n", FileName);
				fprintf(f, "Transposition table size %i\n", Options.HashTableSize);
				fprintf(f, "Quiescence depth %i\n", Options.Quiesce);
				fprintf(f, "Minimal window %s\n", (Options.MinimalWindow ? "On" : "Off"));
				fprintf(f, "Aspiration window %i\n", Options.Aspire);
				fprintf(f, "History heurisitic %s\n", (Options.History ? "On" : "Off"));
				fprintf(f, "Check extensions %s\n", (Options.Check ? "On" : "Off"));
				fprintf(f, "Recapture extensions %s\n", (Options.Recapture ? "On" : "Off"));
				fprintf(f, "Pawn push extensions %s\n", (Options.PawnPush ? "On" : "Off"));
				fprintf(f, "---------------------------\n");
				fprintf(f, "Depth %i\n", Depth);
				fprintf(f, "Time per position %i\n", Time);
				fprintf(f, "Average number of moves per position %i\n", (int)(TotalAverage)/(TotalPositions));
				fprintf(f, "Estimated sum of nodes in minimal tree(s) %i\n", MinTree*TotalPositions );
				fprintf(f, "Total suites %i\n", Count);
				fprintf(f, "Total positions %i\n", TotalPositions);
				fprintf(f, "Total nodes %i\n", TotalNodes);
				fprintf(f, "Total seconds %.2f\n", TotalMilliseconds/1000.0);
				fprintf(f, "Total correct moves %i\n", TotalSolved);
				fprintf(f, "---------------------------\n");
				fclose(f);
				break;
		}
	} else {
		Count++;
		char s[MAXLINELENGTH];
		int len;
		strcpy(output, FileName);
		strcat(output, ".suite");
		f=fopen(output, "a");
		fclose(f);
		f=fopen(FileName, "r");
		if (f!=NULL) {
			char board[MAXFEN];
			char mover[2], flags[6], enpawn[4];
			char fen[MAXFEN];
			while (fscanf(f, "%s", board)!=EOF && !CloseRequested) {
				strcpy(fen, board);
				if (fscanf(f, "%s", mover)!=EOF) {
					strcat(fen, " "); strcat(fen, mover);
					if (fscanf(f, "%s", flags)!=EOF) {
						strcat(fen, " "); strcat(fen, flags);
						if (fscanf(f, "%s", enpawn)!=EOF) {
							strcat(fen, " "); strcat(fen, enpawn);
							if (fscanf(f, "%s", s)!=EOF) {
								if (strcmp(s, "bm")==0) {
									if (fscanf(f, "%s", s)!=NULL) {
										strcpy(Move, s);
										if (Move[strlen(Move)-1]==';') {
											Move[strlen(Move)-1]='\0';
										}
									}
								} else {
									strcpy(Move, "None");
								}
								int D1, D2; // dummy variables
								//if (::MessageBox(this->HWindow, fen, Move, MB_YESNO)==IDNO) exit(0);
								TotalPositions++;
								TestPosition(output,
									fen,
									Depth,
									Time,
									Move,
									MAXSCORE,
									TotalNodes,
									TotalSolved,
									D1, D2,
									TotalMilliseconds,
									TotalPositions,
									ExitWhenFound);
								ch='0';
								TotalAverage+=((float)GMLCMCalls/GMLCalls);
								while (ch!='\n' && ch!=EOF) ch=fgetc(f);
							} // fscanf s
						} // fscanf enpawn
					} // fscanf flags
				} // fscanf mover
			} // while fscanf board
			fclose(f);
		} else {
			sprintf(s, "Error reading file %s", FileName);
			::MessageBox(this->HWindow, s, "Run Test Suite", MB_OK);
		}
		if ((f=fopen(output, "a"))!=NULL) {
			fprintf(f, "Results for test suite %s\n", FileName);
			fprintf(f, "---------------------------\n");
			fprintf(f, "Depth %i\n", Depth);
			fprintf(f, "Time per position %i\n", Time);
			MinTree=(Power((int)(TotalAverage-PreviousAverage)/(TotalPositions-PreviousPositions), Depth/2)*2)-1;
			fprintf(f, "Average number of moves per position %i\n", (int)(TotalAverage-PreviousAverage)/(TotalPositions-PreviousPositions));
			fprintf(f, "Estimated sum of nodes in minimal tree(s) %i\n", MinTree );
			fprintf(f, "Total positions %i\n", TotalPositions-PreviousPositions);
			fprintf(f, "Total nodes %i\n", TotalNodes-PreviousNodes);
			fprintf(f, "Total seconds %.2f\n", (TotalMilliseconds-PreviousMillis)/1000.0);
			fprintf(f, "Total correct moves %i\n", TotalSolved-PreviousSolved);
			fprintf(f, "---------------------------\n");
			fclose(f);
		}
	}
}

void
TRivalWindow::RunSuite(char* FileName, int Task=EXECUTE)
// if FileName is NULL, will use the Task value to either clear
// the static values or to write a summary
{
	FILE *f;
	char output[MAXFILENAMELENGTH];
	char ch;
	static int Count=0;
	static int TotalNodes=0;
	static int TotalSolved=0;
	static int TotalMovesNotScore=0;
	static int TotalScoresNotMove=0;
	static int TotalMilliseconds=0;
	static int TotalPositions=0;
	int PreviousNodes=TotalNodes;
	int PreviousSolved=TotalSolved;
	int PreviousMNS=TotalMovesNotScore;
	int PreviousSNM=TotalScoresNotMove;
	int PreviousMillis=TotalMilliseconds;
	int PreviousPositions=TotalPositions;
	int Depth=6;
	int Time=60;
	char Move[10];
	strcpy(Move, "None");
	int Score=0;
	if (FileName==NULL) {
		switch (Task) {
			case CLEARVALUES :
					TotalNodes=0;             
					TotalMilliseconds=0;
					TotalSolved=0;
					TotalMovesNotScore=0;
					TotalScoresNotMove=0;
					Count=0;
					TotalPositions=0;
					break;
			case WRITESUMMARY :
				char s2[5000];
				char s1[MAXLINELENGTH];
				strcpy(s2, "");
				sprintf(s1, "---------------------------\n");
				strcat(s2, s1);
				sprintf(s1, "Test suite summary\n");
				strcat(s2, s1);
				sprintf(s1, "---------------------------\n");
				strcat(s2, s1);
				sprintf(s1, "Total suites %i\n", Count);
				strcat(s2, s1);
				sprintf(s1, "Total positions %i\n", TotalPositions);
				strcat(s2, s1);
				sprintf(s1, "Total nodes %i\n", TotalNodes);
				strcat(s2, s1);
				sprintf(s1, "Total seconds %.2f\n", TotalMilliseconds/1000.0);
				strcat(s2, s1);
				sprintf(s1, "Total correct moves and score %i\n", TotalSolved);
				strcat(s2, s1);
				sprintf(s1, "Total correct moves but not score %i\n", TotalMovesNotScore);
				strcat(s2, s1);
				sprintf(s1, "Total correct scores but not move %i\n", TotalScoresNotMove);
				strcat(s2, s1);
				sprintf(s1, "---------------------------\n");
				strcat(s2, s1);
				writeMessage(SUITESUMMARY, s2);
				::MessageBox(this->HWindow, s2, "Test Suite", MB_OK);
				break;
		}
	} else {
		Count++;
		char s[MAXLINELENGTH];
		int len;
		f=fopen(FileName, "r");
		strcpy(output, workingDirectory);
		strcat(output, FileName);
		strcat(output, ".suite");
		if (f!=NULL) {
			while (fscanf(f, "%s", s)!=EOF && !CloseRequested) {
				len=strlen(s);
				if (len>0) {
					if (s[0]==';') {
						ch='0';
						while (ch!='\n' && ch!=EOF) ch=fgetc(f);
					} else {
						dncase(s);
						if (Verbose) writeMessage(VERBOSE, "Read command");
						if (Verbose) writeMessage(VERBOSE, s);
						if (strcmp(s, "depth")==0) {
							if (fscanf(f, "%s", s)!=NULL) Depth=atoi(s);
						} else
						if (strcmp(s, "time")==0) {
							if (fscanf(f, "%s", s)!=NULL) Time=atoi(s);
						} else
						if (strcmp(s, "move")==0) {
							if (fscanf(f, "%s", s)!=NULL) strcpy(Move, s);
						} else
						if (strcmp(s, "score")==0) {
							if (fscanf(f, "%s", s)!=NULL) {
								dncase(s);
								if (strcmp(s, "mate")==0) Score=9000; else
									Score=atoi(s);
							}
						} else
						if (strcmp(s, "fen:")==0) {
							char board[MAXFEN];
							char mover[2], flags[6], enpawn[4];
							char fen[MAXFEN];
							if (fscanf(f, "%s%s%s%s", board, mover, flags, enpawn)!=NULL) {
								strcpy(fen, board);
								strcat(fen, " "); strcat(fen, mover);
								strcat(fen, " "); strcat(fen, flags);
								strcat(fen, " "); strcat(fen, enpawn);
								TotalPositions++;
								if (Verbose) writeMessage(VERBOSE, "Testing Position", TotalPositions);
								if (Verbose) writeMessage(VERBOSE, fen);
								TestPosition(output,
									fen,
									Depth,
									Time,
									Move,
									Score,
									TotalNodes,
									TotalSolved,
									TotalMovesNotScore,
									TotalScoresNotMove,
									TotalMilliseconds,
									TotalPositions,
									TRUE);
							}
						}
					}
				}
			}
			fclose(f);
		} else {
			sprintf(s, "Error reading file %s", FileName);
			::MessageBox(this->HWindow, s, "Run Test Suite", MB_OK);
		}
		if ((f=fopen(output, "a"))!=NULL) {
			fprintf(f, "Results for test suite %s\n", FileName);
			fprintf(f, "---------------------------\n");
			fprintf(f, "Total positions %i\n", TotalPositions-PreviousPositions);
			fprintf(f, "Total nodes %i\n", TotalNodes-PreviousNodes);
			fprintf(f, "Total seconds %.2f\n", (TotalMilliseconds-PreviousMillis)/1000.0);
			fprintf(f, "Total correct moves and score %i\n", TotalSolved-PreviousSolved);
			fprintf(f, "Total correct moves but not score %i\n", TotalMovesNotScore-PreviousMNS);
			fprintf(f, "Total correct scores but not move %i\n", TotalScoresNotMove-PreviousSNM);
			fprintf(f, "---------------------------\n");
			fclose(f);
		}
	}
}

void
TRivalWindow::CmRunTestSuite()
{
	int i;
	BOOL GoAhead=TRUE;
	if (Searching || Pondering) Searcher->ExitWithoutMove();
	TOpenSaveDialog::TData LoadData(/*OFN_ALLOWMULTISELECT|*/OFN_FILEMUSTEXIST|OFN_HIDEREADONLY|OFN_PATHMUSTEXIST, "EPD files|*.epd|Rival suite files|*.rsu", 0, "", "*");
	if (TFileOpenDialog(this, LoadData, 0, "Select Test Suite").Execute()==IDOK) {
		char FileNames[MAXTESTSUITEFILES][MAXFILENAMELENGTH];
		int NumFiles=SplitFileNames(LoadData.FileName, FileNames);
		char dnFile[MAXFILENAMELENGTH];
		strcpy(dnFile, FileNames[0]);
		dncase(dnFile);
		int len=strlen(dnFile);
		char SuiteType=NOSUITE;
		if (len>3) {
			if (dnFile[len-3]=='e' && dnFile[len-2]=='p' && dnFile[len-1]=='d') SuiteType=EPD;
			if (dnFile[len-3]=='r' && dnFile[len-2]=='s' && dnFile[len-1]=='u') SuiteType=RTS;
		}
		if (SuiteType==NOSUITE) {
			::MessageBox(this->HWindow, "Unrecognised file type", "Run Test Suite", MB_OK);
			GoAhead=FALSE;
		}
		TTestSuiteTransferBuffer stb;
		if (SuiteType==EPD) {
			GoAhead=FALSE;
			stb.StartDepth=2;
			stb.Depth=7;
			stb.Time=6000;
			stb.ExitWhenFound=FALSE;
			TTestSuiteDialog* sd=new TTestSuiteDialog(this, &stb);
			if (sd->Execute()==IDOK) GoAhead=TRUE;
			delete sd;
		}
		if (GoAhead) {
			if (Verbose) writeMessage(VERBOSE, "Clearing Suite Values");
			FILE* f=fopen(SUITESUMMARY, "w"); fclose(f);
			char output[MAXFILENAMELENGTH];
			for (i=0; i<NumFiles; i++) {
				strcpy(output, FileNames[i]);
				strcat(output, ".suite");
				FILE* f=fopen(output, "w"); fclose(f);
			}
         int Depth;
			for (Depth=stb.StartDepth; Depth<=stb.Depth && !CloseRequested; Depth++) {
				if (SuiteType==RTS) RunSuite(NULL, CLEARVALUES); else
				if (SuiteType==EPD) RunEPDSuite(NULL, CLEARVALUES);
				try {
					for (i=0; i<NumFiles; i++) {
						if (Verbose) writeMessage(VERBOSE, "Running Suite", i);
						if (Verbose) writeMessage(VERBOSE, FileNames[i]);
						if (SuiteType==RTS) RunSuite(FileNames[i]); else
						if (SuiteType==EPD) RunEPDSuite(FileNames[i], EXECUTE, Depth, stb.Time, stb.ExitWhenFound);
					}
				} catch (int error) {
					::MessageBox(this->HWindow, "An error has occured", "Run Test Suite", MB_OK);
				}
				if (Verbose) writeMessage(VERBOSE, "Writing Summary");
				if (SuiteType==RTS) RunSuite(NULL, WRITESUMMARY); else
				if (SuiteType==EPD) RunEPDSuite(FileNames[0], WRITESUMMARY, Depth, stb.Time);
			}
			::MessageBox(this->HWindow, "Done", "Test Suite", MB_OK);
		}
	}
	CloseRequested=FALSE;
}

void
TRivalWindow::CmSolveForMate()
{
	if (!GameOver()) {
		::MessageBox(this->HWindow, "This routine is being re-written.", "Solve for Mate", MB_OK);
/*		if (Searching || Pondering) Searcher->ExitWithoutMove();
		Searcher->SetMaxDepth(49);
		Searcher->SetContempt(Options.Contempt);
		Searcher->SetUseHistory(Options.NullMove);
		Searcher->SetUseMinimalWindow(Options.MinimalWindow);
		Searcher->SetInitialPosition(*Game);
		Searcher->ClearHashTable();
		int oldMethod=Options.SearchMethod;
		Options.SearchMethod=INFINITETIME;
		Searching=TRUE;
		TPath solvePath=Searcher->SolveSearch();
		Searching=FALSE;
		Options.SearchMethod=oldMethod;
		if (solvePath.Move[0].From!=0 && solvePath.Value>9900) {
			int MateMoves=(10000-solvePath.Value+1)/2;
			int count=-1;
			char s[100], all[300];
			char p, c;
			strcpy( all, "" );
			while (solvePath.Move[++count].From!=0) {
				switch (Game->Square[solvePath.Move[count].From]) {
					case WP : c='P'; break;
					case WQ : c='Q'; break;
					case WR : c='R'; break;
					case WN : c='N'; break;
					case WB : c='B'; break;
					case WK : c='K'; break;
					case BP : c='P'; break;
					case BQ : c='Q'; break;
					case BR : c='R'; break;
					case BN : c='N'; break;
					case BB : c='B'; break;
					case BK : c='K'; break;
				}
				if ((c=='P' && GetY(solvePath.Move[count].To)==8) ||
					(c=='P' && GetY(solvePath.Move[count].To)==8))
				{
					switch (solvePath.Move[count].PromotionPiece) {
						case QUEEN : p='Q'; break;
						case BISHOP : p='B'; break;
						case KNIGHT : p='N'; break;
						case ROOK : p='R'; break;
					 default : p='?';
					}
					sprintf( s, "%c%c%c-%c%c=%c  ",
						c,
						(char)(GetX(solvePath.Move[count].From)+'a'-1),
						(char)(GetY(solvePath.Move[count].From)+'0'),
						(char)(GetX(solvePath.Move[count].To)+'a'-1),
						(char)(GetY(solvePath.Move[count].To)+'0'),
						p);
					strcat( all, s );
				} else {
					sprintf( s, "%c%c%c-%c%c  ",
						c,
						(char)(GetX(solvePath.Move[count].From)+'a'-1),
						(char)(GetY(solvePath.Move[count].From)+'0'),
						(char)(GetX(solvePath.Move[count].To)+'a'-1),
						(char)(GetY(solvePath.Move[count].To)+'0'));
					strcat( all, s );
				}
				Game->MakeMove(solvePath.Move[count]);
			 }
			 sprintf( s, "Mate found in %i.\nUse right arrow or menu option to play moves.\n%s", MateMoves, all );
			 ::MessageBox(this->HWindow, s, "Solve for Mate", MB_OK);
			 for (int i=0; i<count; i++)
				Game->TakeBackMove();
			 UpdateAllGraphics();

		} else {
			::MessageBox(this->HWindow, "No mate found", "Solve for Mate", MB_OK);
		} */
	}
}

int
TRivalWindow::WhiteMaterial()
{
	int x, y;
	int Material=0;
	for (x=1; x<=8; x++) {
		for (y=1; y<=8; y++) {
			switch (Game->Square[x*10+y]) {
				case WP : Material+=1; break;
				case WB : Material+=3; break;
				case WN : Material+=3; break;
				case WR : Material+=5; break;
				case WQ : Material+=9; break;
			}
		}
	}
	return Material;
}

int
TRivalWindow::BlackMaterial()
{
	int x, y;
	int Material=0;
	for (x=1; x<=8; x++) {
		for (y=1; y<=8; y++) {
			switch (Game->Square[x*10+y]) {
				case BP : Material+=1; break;
				case BB : Material+=3; break;
				case BN : Material+=3; break;
				case BR : Material+=5; break;
				case BQ : Material+=9; break;
			}
		}
	}
	return Material;
}

BOOL
TRivalWindow::IsDrawPosition()
{
	BOOL acceptDraw=FALSE;
	if (Options.ComputerWhite+Options.ComputerBlack==1) {
		int Difference;
		if (Options.ComputerWhite)
			Difference=BlackMaterial()-WhiteMaterial();
		else
			Difference=WhiteMaterial()-BlackMaterial();
		if (Game->TotalMovesMade()<40) acceptDraw=(Difference>=2); else
			if (Game->TotalMovesMade()<80) acceptDraw=(Difference>=1); else
				acceptDraw=(Difference>=0);
	}
	return acceptDraw;
}

void
TRivalWindow::CmResign()
{
	if (::MessageBox(this->HWindow, "Do you wish to resign?", "Resign", MB_YESNO)==IDYES) {
		if (Searching || Pondering) {
			Searcher->ExitWithMove();
			Game->SetResigned(Game->Square[MOVER]==WHITE ? BLACK : WHITE);
		} else {
			Game->SetResigned(Game->Square[MOVER]);
		}
	}
}

void
TRivalWindow::CmOfferDraw()
{
	if (::MessageBox(this->HWindow, "Do you wish to offer a draw?", "Offer Draw", MB_YESNO)==IDYES) {
		if (IsDrawPosition()) {
			::MessageBox(this->HWindow, "Rival accepts your offer of a draw.", "Offer Draw", MB_OK);
			if (Searching || Pondering) {
				Searcher->ExitWithoutMove();
			}
			Game->SetDrawAgreed(TRUE);
		} else {
			::MessageBox(this->HWindow, "Rival declines your offer of a draw.", "Resign", MB_OK);
		}
	}
}

void
TRivalWindow::CmSaveGameAsPGN()
{
	if (Game->TotalMovesMade()>0) {
		TSavePGNTransferBuffer tb;
		if (Options.ComputerWhite)	strcpy(tb.Event, "Rival vs "); else strcpy(tb.Event, "Other vs ");
		if (Options.ComputerWhite)	strcat(tb.Event, "Rival"); else strcat(tb.Event, "Other");
		tb.Append=TRUE;
		TSavePGNDialog* spd=new TSavePGNDialog(this, &tb);
		if (spd->Execute()==IDOK) {
			TOpenSaveDialog::TData
				 SaveData(OFN_FILEMUSTEXIST|OFN_HIDEREADONLY|OFN_PATHMUSTEXIST,
						"PGN Files (*.pgn)|*.pgn|",
							  0, "", "pgn");
			if (TFileSaveDialog(this, SaveData, 0, "Save Game").Execute()==IDOK) {
				Game->SavePGN(SaveData.FileName, tb.Append, tb.Event);
				Select.From=0;
				UpdateAllGraphics();
			}
		}
		delete spd;
	} else {
		if (::MessageBox(this->HWindow, "No moves have been made in the current game.  Would you like to save the position in FEN format?",
			"Save as PGN", MB_YESNO)==IDYES) {
         	CmSavePosition();
		}
	}
}

void
TRivalWindow::CmSavePosition()
{
	char Fen[MAXFEN];
	Game->GetFEN(Fen);
	TOpenSaveDialog::TData
		 SaveData(OFN_FILEMUSTEXIST|OFN_HIDEREADONLY|OFN_PATHMUSTEXIST,
				"FEN Files (*.fen)|*.fen|",
					  0, "", "fen");
	if (TFileSaveDialog(this, SaveData, 0, "Save Game").Execute()==IDOK) {
		FILE *f=fopen(SaveData.FileName, "w");
		fprintf(f, "%s\n", Fen);
		fclose(f);
		Select.From=0;
		UpdateAllGraphics();
	}
}

void
TRivalWindow::CmSaveGame()
{
	 char s[100];
	 switch (Options.Variant) {
		 case CHESS   : strcpy(s, "Rival Chess Files (*.riv)|*.riv|"); break;
		 case KINGLET : strcpy(s, "Rival Kinglet Files (*.rkl)|*.rkl|"); break;
		 case SHATRANJ: strcpy(s, "Rival Shatranj Files (*.rsh)|*.rsh|"); break;
		 case SELFTAKE: strcpy(s, "Rival SelfTake Files (*.rst)|*.rst|"); break;
	 }
// This is a dreadful waste of code but a bug was holding me up.
// Something is wrong, or I don't understand the Windows Save Dialog.
	 if (Options.Variant==CHESS) {
		 TOpenSaveDialog::TData
		 SaveData(OFN_FILEMUSTEXIST|OFN_HIDEREADONLY|OFN_PATHMUSTEXIST, s,
					  0, "", "riv");
	 if (TFileSaveDialog(this, SaveData, 0, "Save Game").Execute()==IDOK) {
			 Game->SaveGame(SaveData.FileName);
		 Select.From=0;
		 UpdateAllGraphics();
	 }
	 }
	 if (Options.Variant==KINGLET) {
		 TOpenSaveDialog::TData
		 SaveData(OFN_FILEMUSTEXIST|OFN_HIDEREADONLY|OFN_PATHMUSTEXIST, s,
					  0, "", "rkl");
	 if (TFileSaveDialog(this, SaveData, 0, "Save Game").Execute()==IDOK) {
			 Game->SaveGame(SaveData.FileName);
		 Select.From=0;
		 UpdateAllGraphics();
	 }
	 }
	 if (Options.Variant==SHATRANJ) {
		 TOpenSaveDialog::TData
		 SaveData(OFN_FILEMUSTEXIST|OFN_HIDEREADONLY|OFN_PATHMUSTEXIST, s,
					  0, "", "rsh");
	 if (TFileSaveDialog(this, SaveData, 0, "Save Game").Execute()==IDOK) {
			 Game->SaveGame(SaveData.FileName);
		 Select.From=0;
		 UpdateAllGraphics();
	 }
	 }
	 if (Options.Variant==SELFTAKE) {
		 TOpenSaveDialog::TData
		 SaveData(OFN_FILEMUSTEXIST|OFN_HIDEREADONLY|OFN_PATHMUSTEXIST, s,
					  0, "", "rst");
	 if (TFileSaveDialog(this, SaveData, 0, "Save Game").Execute()==IDOK) {
			 Game->SaveGame(SaveData.FileName);
		 Select.From=0;
		 UpdateAllGraphics();
	 }
	 }
}

#define CHESSPGN 101
#define KINGLETPGN 102
#define SHATRANJPGN 103
#define SELFTAKEPGN 104

int GetExtension(char* s)
{
	char rs[100];
	int Type=0;
	dncase(s);
	char e[4]={'0', '0', '0', '\0'};
	for (int i=strlen(s)-1; i>=0; i--) {
		if (s[i]=='.') {
			if (strlen(s)>(i+1)) e[0]=s[i+1];
			if (strlen(s)>(i+2)) e[1]=s[i+2];
			if (strlen(s)>(i+3)) e[2]=s[i+3];
         break;
		}
	}
	if (e[0]=='p' && e[1]=='g' && e[2]=='n') {
		Type=CHESSPGN;
		FILE *f=fopen(s, "r");
		if (f!=NULL) {
			while (fscanf(f, "%s", rs)!=EOF) {
				if (strcmp(rs, "[Variant")==0) {
					if (fscanf(f, "%s", rs)!=EOF) {
						if (strcmp(rs, "\"Kinglet\"]")==0) Type=KINGLETPGN; else
						if (strcmp(rs, "\"Shatranj\"]")==0) Type=SHATRANJPGN; else
						if (strcmp(rs, "\"SelfTake\"]")==0) Type=SELFTAKEPGN;
					}
				}
			}
			fclose(f);
		}
	}
	if (e[0]=='r' && e[1]=='i' && e[2]=='v') Type=CHESS;
	if (e[0]=='r' && e[1]=='k' && e[2]=='l') Type=KINGLET;
	if (e[0]=='r' && e[1]=='s' && e[2]=='h') Type=SHATRANJ;
	if (e[0]=='r' && e[1]=='s' && e[2]=='t') Type=SELFTAKE;
	return Type;
}

void
TRivalWindow::CmLoadPosition()
{
	int Variant=Options.Variant;
	char Fen[MAXFEN];
	char c;
   int count=0;
	if (Searching || Pondering) Searcher->ExitWithoutMove();
	TOpenSaveDialog::TData
		 LoadData(OFN_FILEMUSTEXIST|OFN_HIDEREADONLY|OFN_PATHMUSTEXIST,
					  "FEN Files|*.fen|",
					  0, "", "*");
	if (TFileOpenDialog(this, LoadData, 0, "Load Game").Execute()==IDOK) {
		 FILE *f=fopen(LoadData.FileName, "r");
		 if (f!=NULL) {
			 while ((c=fgetc(f))!=EOF && count<MAXFEN) Fen[count++]=c;
			 if (Game->TranslateFenToBoard(Fen, SetupBoard)>0) {
				 delete Game;
				 delete Searcher;
				 Options.Variant=Variant;
				 InSetup=TRUE;
				 CreateSearcherAndBoard();
				 InSetup=FALSE;
				 Select.From=0; // Make user selection invalid; wipes out cursor.
			 } else {
				 ::MessageBox(this->HWindow, "Error Reading FEN", "Error", MB_OK);
				 return;
			 }
			 fclose(f);
		 } else {
			 ::MessageBox(this->HWindow, "Error Opening File", "Error", MB_OK);
			 return;
		 }
		 Path2.Move[0].From=0;
		 UpdateAllGraphics();
		 Options.ComputerWhite=FALSE;
		 Options.ComputerBlack=FALSE;
		 ResetVariables();
	}
}


void
TRivalWindow::CmLoadGame()
{
/*************************************************************************
We stop a search if one is occuring.  This is done before bringing up the
dialog box to give the search time to terminate because we delete the
game and searcher objects if and when a game is loaded.  Mostly, all
would be well but occasionally the search would not terminate before
deletion.
**************************************************************************/
	if (Searching || Pondering) Searcher->ExitWithoutMove();
	TOpenSaveDialog::TData
		 LoadData(OFN_FILEMUSTEXIST|OFN_HIDEREADONLY|OFN_PATHMUSTEXIST,
					  "All Rival Files|*.pgn;*.riv;*.rkl;*.rsh;*.rst|PGN Files|*.pgn|Rival Chess|*.riv|Rival Shatranj|*.rsh|Rival Kinglet|*.rkl|Rival Selftake|*.rst|",
					  0, "", "*");
	if (TFileOpenDialog(this, LoadData, 0, "Load Game").Execute()==IDOK) {
	  if (GetExtension(LoadData.FileName)==0) {
		 ::MessageBox(this->HWindow, "Invalid File Name", "Error", MB_OK);
	  } else {
		 delete Game;
		 delete Searcher;
		 InSetup=FALSE;
		 int Ext=GetExtension(LoadData.FileName);
		 if (Ext==CHESSPGN) Options.Variant=CHESS; else
		 if (Ext==KINGLETPGN) Options.Variant=KINGLET; else
		 if (Ext==SHATRANJPGN) Options.Variant=SHATRANJ; else
		 if (Ext==SELFTAKEPGN) Options.Variant=SELFTAKE; else
		 Options.Variant=Ext;
		 CreateSearcherAndBoard();
		 if (Ext==CHESSPGN || Ext==KINGLETPGN || Ext==SHATRANJPGN || Ext==SELFTAKEPGN) {
			Game->LoadPGN(LoadData.FileName);
		 } else {
			Game->LoadGame(LoadData.FileName);
		 }
		 Select.From=0; // Make user selection invalid; wipes out cursor.
	  }
	  Path2.Move[0].From=0;
	  UpdateAllGraphics();
	  Options.ComputerWhite=FALSE;
	  Options.ComputerBlack=FALSE;
	}
	ResetVariables();
}

void
TRivalWindow::CmTakeBackOneMove()
{
	 if (Game->LastMoveMade()>0) {
      UpdateWindow();
		TMove LastMove;
		if (Searching || Pondering) Searcher->ExitWithoutMove();
		LastMove=Game->GetLastMoveDetails();
		LastMove.Score=LastMove.From;
		LastMove.From=LastMove.To;
		LastMove.To=LastMove.Score;
		SlidePiece(LastMove);
		Game->TakeBackMove();
		Select.From=0;
		UpdateAllGraphics();
		if (Game->GetMover()==WHITE) {
		  if (Options.ComputerWhite) Options.ComputerWhite=FALSE;
		} else {
		  if (Options.ComputerBlack) Options.ComputerBlack=FALSE;
		}
	 }
	 ResetVariables();
}

void
TRivalWindow::CmReplayOneMove()
{
	 if (Game->LastMoveMade()<Game->TotalMovesMade()) {
		UpdateWindow();
		if (Searching || Pondering) Searcher->ExitWithoutMove();
		Game->ReplayMove();
		TMove LastMove;
		LastMove=Game->GetLastMoveDetails();
		Game->TakeBackMove();
		SlidePiece(LastMove);
		Game->ReplayMove();
		Select.From=0;
		UpdateAllGraphics();
	 }
}

void
TRivalWindow::CmReplayAllMoves()
{
	 if (Game->LastMoveMade()<Game->TotalMovesMade()) {
		if (Searching || Pondering) Searcher->ExitWithoutMove();
		Game->ReplayAllMoves();
		Select.From=0;
		UpdateAllGraphics();
	 }
}

void
TRivalWindow::CmTakeBackAllMoves()
{
	 if (Game->LastMoveMade()>0) {
		if (Searching || Pondering) Searcher->ExitWithoutMove();
		Game->TakeBackAllMoves();
		Select.From=0;
		UpdateAllGraphics();
		if (Game->GetMover()==WHITE) {
			 if (Options.ComputerWhite) Options.ComputerWhite=FALSE;
		} else {
			 if (Options.ComputerBlack) Options.ComputerBlack=FALSE;
		}
	 }
	 ResetVariables();
}

void
TRivalWindow::CmPlayBest()
{
	 if (Searching) Searcher->ExitWithMove();
}

void
TRivalWindow::CmStopSearch()
{
	 if (Searching || Pondering) Searcher->ExitWithoutMove();
}

void
TRivalWindow::LoadPieces()
{
	 delete Pawn;
	 delete Knight;
	 delete Queen;
	 delete Rook;
	 delete King;
	 delete Bishop;
	 switch (Options.Pieces) {
				  case 0 : Pawn=new TDib(h, PAWN1);
							  Knight=new TDib(h, KNIGHT1);
							  Bishop=new TDib(h, BISHOP1);
							  Queen=new TDib(h, QUEEN1);
							  King=new TDib(h, KING1);
							  Rook=new TDib(h, ROOK1);
							  break;
				  case 1 : Pawn=new TDib(h, PAWN2);
							  Knight=new TDib(h, KNIGHT2);
							  Bishop=new TDib(h, BISHOP2);
							  Queen=new TDib(h, QUEEN2);
							  King=new TDib(h, KING2);
							  Rook=new TDib(h, ROOK2);
							  break;
				  case 2 : Pawn=new TDib(h, PAWN3);
							  Knight=new TDib(h, KNIGHT3);
							  Bishop=new TDib(h, BISHOP3);
							  Queen=new TDib(h, QUEEN3);
							  King=new TDib(h, KING3);
							  Rook=new TDib(h, ROOK3);
							  break;
	 }
}

void
TRivalWindow::CmOptionToggler(WPARAM OptionId)
{
	 switch (OptionId) {
		 case CM_OPTIONS_PIECE_SLIDE :
				Options.PieceSlide=!Options.PieceSlide;
				break;
		 case CM_OPTIONS_MESSAGES :
				Options.Messages=!Options.Messages;
				break;
		 case CM_FILE_AUTOSAVE :
				Options.AutoSave=!Options.AutoSave;
				break;
		 case CM_FLAG :
				Options.Flag=!Options.Flag;
				break;
		 case CM_DEFAULT_OPTIONS :
				SetDefaultOptions();
				LoadPieces();
				UpdateAllGraphics(TRUE);
				break;
		 case CM_OPTIONS_SHOW_ANALYSIS :
				Options.ShowAnalysis=!Options.ShowAnalysis;
				if (!Options.ShowAnalysis) {
					WinGBitBlt(ScreenDc,     AnalysisRectangle->left,  AnalysisRectangle->top,
													 AnalysisRectangle->right, AnalysisRectangle->bottom,
								  BackGroundDc, AnalysisRectangle->left,  AnalysisRectangle->top);
					InvalidateRect(*AnalysisRectangle, FALSE);
				} else {
					UpdateAllGraphics();
				}
				break;
		 case CM_OPTIONS_COMPUTER_WHITE :
				Options.ComputerWhite=!Options.ComputerWhite;
				if (Options.ComputerWhite && Game->GetMover()==WHITE) if (Options.Ponder) ComputerMoveFlag = TRUE;	else ComputerMove();
				if (!Options.ComputerWhite && Game->GetMover()==WHITE && Searching) Searcher->ExitWithoutMove();
				if (!Options.ComputerWhite && Game->GetMover()==BLACK && Pondering) Searcher->ExitWithoutMove();
				TryOpenings=TRUE;
				break;
		 case CM_OPTIONS_COMPUTER_BLACK :
				Options.ComputerBlack=!Options.ComputerBlack;
				if (Options.ComputerBlack && Game->GetMover()==BLACK) if (Options.Ponder) ComputerMoveFlag = TRUE;	else ComputerMove();
				if (!Options.ComputerBlack && Game->GetMover()==BLACK && Searching) Searcher->ExitWithoutMove();
				if (!Options.ComputerBlack && Game->GetMover()==WHITE && Pondering) Searcher->ExitWithoutMove();
            TryOpenings=TRUE;
				break;
		 case CM_OPTIONS_SWITCH_SIDES :
				if (Game->GetMover()==WHITE) {
				  Options.ComputerWhite=!Searching;
				  Options.ComputerBlack=Searching;
				} else {
				  Options.ComputerWhite=Searching;
				  Options.ComputerBlack=!Searching;
				}
				if (Searching || Pondering)
				  Searcher->ExitWithoutMove();
				else
				  if (Options.Ponder) ComputerMoveFlag = TRUE;	else ComputerMove();
				break;
		 case CM_OPTIONS_SAVE_OPTIONS :
				WriteOptions();
				break;
	 }
}

void
TRivalWindow::CmVariantSelect(WPARAM OptionId)
{
	 BOOL GoAhead=false;
	 memcpy((void *)SetupBoard, (void *)Game->Square, sizeof(int)*89);
	 switch (OptionId) {
		 case CM_VCHESS :
				if (VerifyChessSetupPosition()) {
					Options.Variant=CHESS;
					GoAhead=TRUE;
				}
				break;
		 case CM_VSHATRANJ :
				if (VerifyShatranjSetupPosition()) {
					Options.Variant=SHATRANJ;
					GoAhead=TRUE;
				}
				break;
		 case CM_VKINGLET :
				if (VerifyKingletSetupPosition()) {
					Options.Variant=KINGLET;
					GoAhead=TRUE;
				}
				break;
		 case CM_VSELFTAKE :
				if (VerifySelfTakeSetupPosition()) {
					Options.Variant=SELFTAKE;
					GoAhead=TRUE;
				}
				break;
	 }
	 if (GoAhead) {
    	if (Searching || Pondering) Searcher->ExitWithoutMove();
		delete Game;
		delete Searcher;
		InSetup=TRUE;
		CreateSearcherAndBoard();
		InSetup=FALSE;
		UpdateAllGraphics();
	 }
}

void
TRivalWindow::OpenVerboseLogs()
{
	if (Verbose) {
		strcpy(VERBOSE, workingDirectory);
		strcat(VERBOSE, "\\verbose.txt");
		FILE *f=fopen(VERBOSE, "w"); fclose(f);
	}
}

void
TRivalWindow::OpenPrintInfoLogs()
{
	if (PrintInfo) {
		strcpy(NODES, workingDirectory);
		strcat(NODES, "\\nodes.txt");
		strcpy(TIMINGS, workingDirectory);
		strcat(TIMINGS, "\\timings.txt");
		FILE *f=fopen(NODES, "w"); fclose(f);
		f=fopen(TIMINGS, "w"); fclose(f);
	}
}

void
TRivalWindow::OpenPVLogs()
{
	if (PrintPV) {
		strcpy(PV, workingDirectory);
		strcat(PV, "\\pv.txt");
		strcpy(PV2, workingDirectory);
		strcat(PV2, "\\pv.html");
		FILE *f=fopen(PV, "w"); fclose(f);
		f=fopen(PV2, "w"); fclose(f);
	}
}

void
TRivalWindow::CmLogsToggler(WPARAM OptionId)
{
	 FILE *f;
	 switch (OptionId) {
		 case CM_LOGS_VERBOSE :
			Verbose=!Verbose;
			OpenVerboseLogs();
			break;
		 case CM_LOGS_TIMINGS :
			PrintInfo=!PrintInfo;
			OpenPrintInfoLogs();
			break;
		 case CM_LOGS_PV :
			PrintPV=!PrintPV;
			OpenPVLogs();
			break;
	 }
}

void
TRivalWindow::CmInvertBoard(WPARAM OptionId)
{
	Options.InvertBoard = !Options.InvertBoard;
}

void
TRivalWindow::SetDefaultDisplayOptions()
{
	Options.BackGround=1;
	Options.BackOffset=2;
	Options.Pieces=0;
	Options.AnalysisTextOffset=-4;
	Options.MoveBoxTextOffset=-4;
	Options.WhiteOffset=0;
	Options.BlackOffset=-2;
	Options.ChessBoard=0;
	Options.AnalysisBoxStyle=0;
	Options.MoveBoxStyle=0;
	Options.Pieces=0;
}

void
TRivalWindow::SetDefaultOptions()
{
	Options.NullMoveStopMaterial=3;
	Options.FailHighReductions=TRUE;
	Options.NullMove=FALSE;
	Options.NullMoveReduce=2;
	Options.Aspire=200;
	Options.MaxExtend=10;
	Options.Check=TRUE;
	Options.Recapture=TRUE;
	Options.PawnPush=FALSE;
	Options.MinimumBookResults=20;
	Options.WhitePercent=40;
	Options.BlackPercent=25;
	Options.WhiteOffset=0;
	Options.BlackOffset=-2;
	Options.BackOffset=2;
	Options.AnalysisTextOffset=-4;
	Options.MoveBoxTextOffset=-4;
	Options.PieceSlide=TRUE;
	Options.ComputerWhite=FALSE;
	Options.ComputerBlack=TRUE;
	Options.Messages=TRUE;
	Options.ShowAnalysis=TRUE;
	Options.SearchMethod=LEVEL;
	Options.Level=4;
	Options.Time=5;
	Options.BaseMinutes=2;
	Options.Increment=12;
	Options.ChampMoves=60;
	Options.ChampTime=600;
	Options.AutoSave=TRUE;
	Options.Variant=CHESS;
	Options.BackGround=1;
	Options.ChessBoard=0;
	Options.Pieces=0;
	Options.AnalysisBoxStyle=0;
	Options.MoveBoxStyle=0;
	Options.Flag=TRUE;
	Options.HashTableSize=8000;
   Options.InvertBoard=FALSE;
	Options.RandomMoveOrdering=FALSE;
	Options.OpeningLearning=TRUE;
	Options.Ponder=FALSE;
	Options.Openings=TRUE;
	Options.OpeningLearning=TRUE;
	Options.MinimalWindow=TRUE;
	Options.Quiesce=50;
	Options.Contempt=-75;
	Options.History=0;
}

void
TRivalWindow::CmDisplayChangeToggler(WPARAM OptionId)
{
	 switch (OptionId) {
		 case CM_INVERT_BOARD :
				Options.InvertBoard = !Options.InvertBoard;
				break;
		 case CM_CHANGE_CHESSBOARD :
				Options.ChessBoard++;
				if (Options.ChessBoard>1)
					Options.ChessBoard=0;
				break;
		 case CM_CHANGE_PIECES :
				Options.Pieces++;
				if (Options.Pieces>2)
				  Options.Pieces=0;
				break;
		 case CM_CHANGE_BOX_STYLE :
				Options.AnalysisBoxStyle++;
				if (Options.AnalysisBoxStyle>4)
					Options.AnalysisBoxStyle=-1;
				break;
		 case CM_CHANGE_MOVEBOX_STYLE :
				Options.MoveBoxStyle++;
				if (Options.MoveBoxStyle>4)
					Options.MoveBoxStyle=-1;
				break;
		 case CM_CHANGE_WHITE_COLOUR :
				do { Options.WhiteOffset--;
				} while (Options.WhiteOffset==Options.BlackOffset);
				if (Options.WhiteOffset<-5) {
					Options.WhiteOffset=0-(Options.BlackOffset==0);
				}
				break;
		 case CM_CHANGE_BLACK_COLOUR :
				do { Options.BlackOffset--;
				} while (Options.BlackOffset==Options.WhiteOffset);
				if (Options.BlackOffset<-5) {
					Options.BlackOffset=0-(Options.WhiteOffset==0);
				}
				break;
		 case CM_CHANGE_ANALYSIS_TEXT :
				Options.AnalysisTextOffset--;
				if (Options.AnalysisTextOffset<-5)
					Options.AnalysisTextOffset=0;
				break;
		 case CM_CHANGE_MOVE_TEXT :
				Options.MoveBoxTextOffset--;
				if (Options.MoveBoxTextOffset<-5)
					Options.MoveBoxTextOffset=0;
				break;
		 case CM_CHANGE_BACKGROUND_COLOUR :
				Options.BackOffset++;
				if (Options.BackOffset>5)
					Options.BackOffset=0;
				break;
		 case CM_CHANGE_BACKGROUND_STYLE :
				Options.BackGround++;
				if (Options.BackGround>1)
					Options.BackGround=0;
				break;
		 case CM_DEFAULT_STYLE_1 :
				SetDefaultDisplayOptions();
				break;
	 }
	 LoadPieces();
	 UpdateAllGraphics(TRUE);
}

void
TRivalWindow::CmAnalysePGN()
{
	int firstmates=0, secondmates=0;
	int firstbyresigns=0, secondbyresigns=0;
	int firstontime=0, secondontime=0;
	int firstunknown=0, secondunknown=0;
	int draws=0;
	int white=0, black=0;
	int first=0, second=0;
	int numgames=0;
	if (Searching || Pondering) Searcher->ExitWithoutMove();
	TOpenSaveDialog::TData
		 LoadData(OFN_FILEMUSTEXIST|OFN_HIDEREADONLY|OFN_PATHMUSTEXIST,
					  "Winboard Log Files|*.wblog|All Files|*.*|",
					  0, "", "*");
	if (TFileOpenDialog(this, LoadData, 0, "Choose PGN File").Execute()==IDOK) {
		FILE *pgn = fopen(LoadData.FileName, "r");
		char message[1000];
		int quit = 0;
		while (!quit) {
			quit = (fscanf(pgn, "%s", message)==EOF);
			if (!quit && strcmp(message, ">first")==0) {
				quit = (fscanf(pgn, "%s", message)==EOF);
				if (!quit && strcmp(message, ":")==0) {
					quit = (fscanf(pgn, "%s", message)==EOF);
					if (!quit && strcmp(message, "result")==0) {
						quit = (fscanf(pgn, "%s", message)==EOF);
						if (!quit) {
							numgames++;
							if (strcmp(message, "1/2-1/2")==0) draws++; else
							if (strcmp(message, "1-0")==0) {
								quit = (fscanf(pgn, "%s", message)==EOF);
								if (!quit) quit = (fscanf(pgn, "%s", message)==EOF);
								if (!quit)
									if (strcmp(message, "mates}")==0 || strcmp(message, "Mates}")==0)
										{if (numgames%2==1) firstmates++; else secondmates++;} else
									if (strcmp(message, "wins")==0)
										{if (numgames%2==1) firstontime++; else secondontime++;} else
									if (strcmp(message, "resigns}")==0 || strcmp(message, "Resigns}")==0)
										{if (numgames%2==1) firstbyresigns++; else secondbyresigns++;} else
									{if (numgames%2==1) firstunknown++; else secondunknown++;}
								if (numgames%2==1) first++; else second++;
								white++;

							} else
							if (strcmp(message, "0-1")==0) {
								quit = (fscanf(pgn, "%s", message)==EOF);
								if (!quit) quit = (fscanf(pgn, "%s", message)==EOF);
								if (!quit)
									if (strcmp(message, "mates}")==0 || strcmp(message, "Mates}")==0)
										{if (numgames%2==0) firstmates++; else secondmates++;} else
									if (strcmp(message, "wins")==0)
										{if (numgames%2==0) firstontime++; else secondontime++;} else
									if (strcmp(message, "resigns}")==0 || strcmp(message, "Resigns}")==0)
										{if (numgames%2==0) firstbyresigns++; else secondbyresigns++;} else
									{if (numgames%2==0) firstunknown++; else secondunknown++;}
								if (numgames%2==0) first++; else second++;
								black++;
							} else {
								::MessageBox(this->HWindow, "Failed", "Failed", MB_OK);
								return;
							}
						}
					}
				}
			}
		}
		char s[1000];
		sprintf(message, "Games\t\t%i\nWhite wins\t%i\nBlack wins\t%i\nDraws\t\t%i\n", numgames, white, black, draws);
		sprintf(s, "1st engine\t%i\n2nd engine\t%i\n", first, second );
		strcat( message, s );
		sprintf(s, "1st by mate\t%i\n1st on time\t%i\n1st by resign\t%i\n1st by unknown\t%i\n", firstmates, firstontime, firstbyresigns, firstunknown );
		strcat( message, s );
		sprintf(s, "2nd by mate\t%i\n2nd on time\t%i\n2nd by resign\t%i\n2nd by unknown\t%i\n", secondmates, secondontime, secondbyresigns, secondunknown );
		strcat( message, s );
		::MessageBox(this->HWindow, message, "Analysis", MB_OK);
		fclose(pgn);
	}
}

void
TRivalWindow::CmSetupBoard()
{
	TSetupMethodDialog* Smd;
	TSetupMethodTransferBuffer smtb;
	char s[MAXFEN];
	Game->GetFEN(s);
	strcpy(smtb.FenPos, s);
	Smd=new TSetupMethodDialog(this, &smtb);
	if (Smd->Execute()==IDOK) {
		WhiteFell=BlackFell=FALSE;
		Path2.Move[0].From=0;
		if (smtb.Method==FENMETHOD) {
			if (Game->TranslateFenToBoard(smtb.FenPos, SetupBoard)>0) {
				  if (VerifySetupPosition(Options.Variant)) {
					 if (Searching || Pondering) Searcher->ExitWithoutMove();
					 delete Game;
					 delete Searcher;
					 InSetup=TRUE;
					 CreateSearcherAndBoard();
					 UpdateAllGraphics();
					 InSetup=FALSE;
					 Options.ComputerWhite=FALSE;
					 Options.ComputerBlack=FALSE;
					 oldtime=GetTickCount();
					 ResetClocksToPrevious();
					 ResetVariables();
				  }
			} else {
				::MessageBox(HWindow, "Error reading position", "Invalid Position", MB_OK);
			}
		} else {
      	Options.InvertBoard = FALSE;
			if (::MessageBox(HWindow, "Use right mouse button to edit squares", "Setup Board", MB_OKCANCEL)==IDCANCEL) return;
			if (Searching || Pondering) Searcher->ExitWithoutMove();
			InSetup=TRUE;
			Select.From=0;
			_fmemcpy((void *)SetupBoard, (void *)Game->Square, 89*sizeof(int));
		}
	}
}

BOOL
TRivalWindow::VerifyChessSetupPosition()
{
	 int x, y;
	 int Error=0;
	 int WKings=0, BKings=0;
	 int WPawns=0, BPawns=0;
	 int WQueens=0, BQueens=0;
	 int WRooks=0, BRooks=0;
	 int WKnights=0, BKnights=0;
	 int WBishops=0, BBishops=0;
	 for (x=1; x<=8; x++)
		 for (y=1; y<=8; y++) {
			 int Piece=SetupBoard[x*10+y];
			 if (Piece==WK) WKings++;
			 if (Piece==BK) BKings++;
			 if (Piece==WN) WKnights++;
			 if (Piece==WB) WBishops++;
			 if (Piece==WQ) WQueens++;
			 if (Piece==WR) WRooks++;
			 if (Piece==WP) WPawns++;
			 if (Piece==BN) BKnights++;
			 if (Piece==BB) BBishops++;
			 if (Piece==BQ) BQueens++;
			 if (Piece==BR) BRooks++;
			 if (Piece==BP) BPawns++;
			 if ((Piece==WP || Piece==BP) && (y==1 || y==8))
				 Error=1;
		 }
	 if (WKings!=1 || BKings!=1) {
		 ::MessageBox(HWindow, "Each side must have one king for regular chess", "Invalid Position", MB_OK);
		 return FALSE;
	 }
	 if (Error==1) {
		 ::MessageBox(HWindow, "Invalid Pawn Placement for regular chess", "Invalid Position", MB_OK);
		 return FALSE;
	 }
	 if ((WRooks+WPawns>10) || (WKnights+WPawns>10) ||
		  (WBishops+WPawns>10) || (WQueens+WPawns>10) ||
		  (BRooks+BPawns>10) || (BKnights+BPawns>10) ||
		  (BBishops+BPawns>10) || (BQueens+BPawns>10)) {
		 ::MessageBox(HWindow, "Too many placements of same type of piece for regular chess (The number of Pawns plus the number of the same type of piece must not be more than 10)", "Invalid Position", MB_OK);
		 return FALSE;
	 }
	 TChessBoard cb(SetupBoard);
	 cb.Square[MOVER]=!cb.Square[MOVER];
	 if (cb.IsCheck()) {
		 ::MessageBox(HWindow, "In regular chess, the side not on the move must not be in check", "Invalid Position", MB_OK);
		 return FALSE;
	 }
	 return TRUE;
}

BOOL
TRivalWindow::VerifySelfTakeSetupPosition()
{
	 int x, y;
	 int Error=0;
	 int WKings=0, BKings=0;
	 int WPawns=0, BPawns=0;
	 int WQueens=0, BQueens=0;
	 int WRooks=0, BRooks=0;
	 int WKnights=0, BKnights=0;
	 int WBishops=0, BBishops=0;
	 for (x=1; x<=8; x++)
		 for (y=1; y<=8; y++) {
			 int Piece=SetupBoard[x*10+y];
			 if (Piece==WK) WKings++;
			 if (Piece==BK) BKings++;
			 if (Piece==WN) WKnights++;
			 if (Piece==WB) WBishops++;
			 if (Piece==WQ) WQueens++;
			 if (Piece==WR) WRooks++;
			 if (Piece==WP) WPawns++;
			 if (Piece==BN) BKnights++;
			 if (Piece==BB) BBishops++;
			 if (Piece==BQ) BQueens++;
			 if (Piece==BR) BRooks++;
			 if (Piece==BP) BPawns++;
			 if ((Piece==WP || Piece==BP) && (y==1 || y==8))
				 Error=1;
		 }
	 if (WKings!=1 || BKings!=1) {
		 ::MessageBox(HWindow, "Each side must have one king", "Invalid Selftake Position", MB_OK);
		 return FALSE;
	 }
	 if (Error==1) {
		 ::MessageBox(HWindow, "Invalid Pawn Placement", "Invalid Selftake Position", MB_OK);
		 return FALSE;
	 }
	 if ((WRooks+WPawns>10) || (WKnights+WPawns>10) ||
		  (WBishops+WPawns>10) || (WQueens+WPawns>10) ||
		  (BRooks+BPawns>10) || (BKnights+BPawns>10) ||
		  (BBishops+BPawns>10) || (BQueens+BPawns>10)) {
		 ::MessageBox(HWindow, "Too many placements of same type of piece (The number of Pawns plus the number of the same type of piece must not be more than 10)", "Invalid Selftake Position", MB_OK);
		 return FALSE;
	 }
	 TSelfTakeBoard cb(SetupBoard);
	 cb.Square[MOVER]=!cb.Square[MOVER];
	 if (cb.IsCheck()) {
		 ::MessageBox(HWindow, "In self-take chess, the side not on the move must not be in check", "Invalid Selftake Position", MB_OK);
		 return FALSE;
	 }
	 return TRUE;
}

BOOL
TRivalWindow::VerifyKingletSetupPosition()
{
	 int x, y;
	 int Error=0;
	 int WKings=0, BKings=0;
	 int WPawns=0, BPawns=0;
	 int WQueens=0, BQueens=0;
	 int WRooks=0, BRooks=0;
	 int WKnights=0, BKnights=0;
	 int WBishops=0, BBishops=0;
	 for (x=1; x<=8; x++)
		 for (y=1; y<=8; y++) {
			 int Piece=SetupBoard[x*10+y];
			 if (Piece==WK) WKings++;
			 if (Piece==BK) BKings++;
			 if (Piece==WN) WKnights++;
			 if (Piece==WB) WBishops++;
			 if (Piece==WQ) WQueens++;
			 if (Piece==WR) WRooks++;
			 if (Piece==WP) WPawns++;
			 if (Piece==BN) BKnights++;
			 if (Piece==BB) BBishops++;
			 if (Piece==BQ) BQueens++;
			 if (Piece==BR) BRooks++;
			 if (Piece==BP) BPawns++;
			 if ((Piece==WP || Piece==BP) && (y==1 || y==8))
				 Error=1;
		 }
	 if (Error==1) {
		 ::MessageBox(HWindow, "Invalid Pawn Placement for Kinglet", "Invalid Kinglet Position", MB_OK);
		 return FALSE;
	 }
	 if ((WRooks>2) || (WKnights>2) ||
		  (WBishops>2) || (WQueens>2) ||
		  (BRooks>2) || (BKnights>2) ||
		  (BBishops>2) || (BQueens>2)) {
		 ::MessageBox(HWindow, "Too many placements of same type of piece for Kinglet (Pawns may only promote to kings when using the Kinglet rules)", "Invalid Kinglet Position", MB_OK);
		 return FALSE;
	 }
	 if ((WKings+WPawns>9) || (BKings+BPawns>9)) {
		 ::MessageBox(HWindow, "Too many kings for Kinglet (Kings+Pawns must be no more than 9)", "Invalid Kinglet Position", MB_OK);
		 return FALSE;
	 }
/*	 if ((WPawns<1) || (BPawns<1)) {
		 ::MessageBox(HWindow, "Each side must have at least one pawn", "Invalid Kinglet Position", MB_OK);
		 return FALSE;
	 }*/
	 return TRUE;
}

BOOL
TRivalWindow::VerifyShatranjSetupPosition()
{
	 int x, y;
	 int Error=0;
	 int WKings=0, BKings=0;
	 int WPawns=0, BPawns=0;
	 int WQueens=0, BQueens=0;
	 int WRooks=0, BRooks=0;
	 int WKnights=0, BKnights=0;
	 int WBishops=0, BBishops=0;
	 for (x=1; x<=8; x++)
		 for (y=1; y<=8; y++) {
			 int Piece=SetupBoard[x*10+y];
			 if (Piece==WK) WKings++;
			 if (Piece==BK) BKings++;
			 if (Piece==WN) WKnights++;
			 if (Piece==WB) WBishops++;
			 if (Piece==WQ) WQueens++;
			 if (Piece==WR) WRooks++;
			 if (Piece==WP) WPawns++;
			 if (Piece==BN) BKnights++;
			 if (Piece==BB) BBishops++;
			 if (Piece==BQ) BQueens++;
			 if (Piece==BR) BRooks++;
			 if (Piece==BP) BPawns++;
			 if ((Piece==WP || Piece==BP) && (y==1 || y==8))
				 Error=1;
		 }
	 if (WKings!=1 || BKings!=1) {
		 ::MessageBox(HWindow, "Each side must have one king for Shatranj", "Invalid Shatranj Position", MB_OK);
		 return FALSE;
	 }
	 if (Error==1) {
		 ::MessageBox(HWindow, "Invalid Pawn Placement for Shatranj", "Invalid Shatranj Position", MB_OK);
		 return FALSE;
	 }
	 if ((WRooks+WPawns>10) || (WKnights+WPawns>10) ||
		  (WBishops+WPawns>10) || (WQueens+WPawns>10) ||
		  (BRooks+BPawns>10) || (BKnights+BPawns>10) ||
		  (BBishops+BPawns>10) || (BQueens+BPawns>10)) {
		 ::MessageBox(HWindow, "Too many placements of same type of piece for Shatranj (The number of Pawns plus the number of the same type of piece must not be more than 10)", "Invalid Shatranj Position", MB_OK);
		 return FALSE;
	 }
	 TShatranjBoard cb(SetupBoard);
	 cb.Square[MOVER]=!cb.Square[MOVER];
	 if (cb.IsCheck()) {
		 ::MessageBox(HWindow, "In shatranj, the side not on the move must not be in check", "Invalid Shatranj Position", MB_OK);
		 return FALSE;
	 }
	 return TRUE;
}

BOOL
TRivalWindow::VerifySetupPosition(int Variant)
{
	switch (Variant) {
		case CHESS : return VerifyChessSetupPosition();
		case KINGLET : return VerifyKingletSetupPosition();
		case SHATRANJ : return VerifyShatranjSetupPosition();
      case SELFTAKE : return VerifySelfTakeSetupPosition();
	}
}

void
TRivalWindow::CmAddPiece(WPARAM WParam)
{
	DetailsStruct Details;
	int i;
	TSetupDialog sd(this, &Details);
	switch (WParam) {
		case CM_WHITE_PAWN : SetupBoard[SetupX*10+SetupY]=WP; break;
		case CM_WHITE_KNIGHT : SetupBoard[SetupX*10+SetupY]=WN; break;
		case CM_WHITE_BISHOP : SetupBoard[SetupX*10+SetupY]=WB; break;
		case CM_WHITE_ROOK : SetupBoard[SetupX*10+SetupY]=WR; break;
		case CM_WHITE_QUEEN : SetupBoard[SetupX*10+SetupY]=WQ; break;
		case CM_WHITE_KING : SetupBoard[SetupX*10+SetupY]=WK; break;
		case CM_BLACK_PAWN : SetupBoard[SetupX*10+SetupY]=BP; break;
		case CM_BLACK_KNIGHT : SetupBoard[SetupX*10+SetupY]=BN; break;
		case CM_BLACK_BISHOP : SetupBoard[SetupX*10+SetupY]=BB; break;
		case CM_BLACK_ROOK : SetupBoard[SetupX*10+SetupY]=BR; break;
		case CM_BLACK_QUEEN : SetupBoard[SetupX*10+SetupY]=BQ; break;
		case CM_BLACK_KING : SetupBoard[SetupX*10+SetupY]=BK; break;
		case CM_CLEAR_BOARD : for (i=11; i<=88; i++) SetupBoard[i]=EMPTY; break;
      case CM_CLEAR_SQUARE : SetupBoard[SetupX*10+SetupY]=EMPTY; break;
		case CM_ABORT_SETUP : InSetup=FALSE; break;
		case CM_EXIT_SETUP :
			  Details.Variant=Options.Variant;
			  Details.WRook1Moved=(SetupBoard[11]!=WR);
			  Details.WRook8Moved=(SetupBoard[81]!=WR);
			  Details.BRook1Moved=(SetupBoard[18]!=BR);
			  Details.BRook8Moved=(SetupBoard[88]!=BR);
			  Details.WKingMoved=(SetupBoard[51]!=WK);
			  Details.BKingMoved=(SetupBoard[58]!=BK);
			  Details.WhiteToMove=(SetupBoard[MOVER]==WHITE);
			  Details.EnPassant=0;
			  Details.FiftyMoves=0;
			  Options.ComputerWhite=FALSE;
			  Options.ComputerBlack=FALSE;
			  if (sd.Execute()==IDOK) {
				  SetupBoard[WROOK1MOVED]=(Details.WRook1Moved || SetupBoard[11]!=WR);
				  SetupBoard[WROOK8MOVED]=(Details.WRook8Moved || SetupBoard[81]!=WR);
				  SetupBoard[BROOK1MOVED]=(Details.BRook1Moved || SetupBoard[18]!=BR);
				  SetupBoard[BROOK8MOVED]=(Details.BRook8Moved || SetupBoard[88]!=BR);
				  SetupBoard[WKINGMOVED]=(Details.WKingMoved || SetupBoard[51]!=WK);
				  SetupBoard[BKINGMOVED]=(Details.BKingMoved || SetupBoard[58]!=BK);
				  SetupBoard[ENPAWN]=Details.EnPassant;
				  SetupBoard[FIFTYMOVES]=Details.FiftyMoves;
				  SetupBoard[MOVER]=(Details.WhiteToMove ? WHITE : BLACK);
				  if (VerifySetupPosition(Details.Variant)) {
					 delete Game;
					 delete Searcher;
					 Options.Variant=Details.Variant;
					 CreateSearcherAndBoard();
					 Options.ComputerWhite=FALSE;
					 Options.ComputerBlack=FALSE;
					 oldtime=GetTickCount();
					 ResetClocksToPrevious();
					 InSetup=FALSE;
                ResetVariables();
				  }
			  }
	}
	UpdateAllGraphics();
}

void
TRivalWindow::CmComputerOptions()
{
//	int OrigMethod=Options.SearchMethod;
	TComputerOptionsDialog *cpdialog;
	TComputerOptionsTransferBuffer transferbuffer;
	transferbuffer.SearchMethod=Options.SearchMethod;
	transferbuffer.Level=Options.Level;
	transferbuffer.TimeHour=Options.Time/3600;
	transferbuffer.TimeMinute=Options.Time%3600/60;
	transferbuffer.TimeSecond=Options.Time%60;
	transferbuffer.ChampMoves=Options.ChampMoves;
	transferbuffer.ChampHour=Options.ChampTime/3600;
	transferbuffer.ChampMinute=Options.ChampTime%3600/60;
	transferbuffer.ChampSecond=Options.ChampTime%60;
	transferbuffer.BaseMinutes=Options.BaseMinutes;
	transferbuffer.Increment=Options.Increment;
	transferbuffer.Library=Options.Openings;
	transferbuffer.Quiesce=Options.Quiesce;
	transferbuffer.Contempt=Options.Contempt;
	transferbuffer.NullMove=Options.NullMove;
	transferbuffer.FailHighReductions=Options.FailHighReductions;
	transferbuffer.Recapture=Options.Recapture;
	transferbuffer.Check=Options.Check;
	transferbuffer.PawnPush=Options.PawnPush;
	transferbuffer.MaxExtend=Options.MaxExtend;
	transferbuffer.Aspire=Options.Aspire;
	transferbuffer.NullMoveStopMaterial=Options.NullMoveStopMaterial;
	transferbuffer.NullMoveReduce=Options.NullMoveReduce;
	transferbuffer.OpeningLearning=Options.OpeningLearning;
	transferbuffer.Ponder=Options.Ponder;
	transferbuffer.History=Options.History;
	transferbuffer.SingleReply=Options.SingleReply;
	transferbuffer.MinimalWindow=Options.MinimalWindow;
	transferbuffer.HashTableSize=Options.HashTableSize;
	transferbuffer.RandomMoveOrder=Options.RandomMoveOrdering;
	transferbuffer.HashReadReduce=Options.HashReadReduce;
	transferbuffer.HashWriteReduce=Options.HashWriteReduce;
	transferbuffer.MinResults=Options.MinimumBookResults;
	transferbuffer.WhitePercent=Options.WhitePercent;
	transferbuffer.BlackPercent=Options.BlackPercent;
	cpdialog=new TComputerOptionsDialog(this, &transferbuffer);
	if (cpdialog->Execute()==IDOK) {
	  Options.SearchMethod=transferbuffer.SearchMethod;
	  Options.Level=transferbuffer.Level;
	  Options.Time=(transferbuffer.TimeHour*3600)+(transferbuffer.TimeMinute*60)+transferbuffer.TimeSecond;
	  Options.ChampTime=(transferbuffer.ChampHour*3600)+(transferbuffer.ChampMinute*60)+transferbuffer.ChampSecond;
	  Options.ChampMoves=transferbuffer.ChampMoves;
	  Options.BaseMinutes=transferbuffer.BaseMinutes;
	  Options.Increment=transferbuffer.Increment;
	  Options.Openings=transferbuffer.Library;
	  Options.Quiesce=transferbuffer.Quiesce;
	  Options.History=transferbuffer.History;
	  Options.MinimumBookResults=transferbuffer.MinResults;
	  Options.WhitePercent=transferbuffer.WhitePercent;
	  Options.BlackPercent=transferbuffer.BlackPercent;
	  Options.Recapture=transferbuffer.Recapture;
	  Options.Check=transferbuffer.Check;
	  Options.PawnPush=transferbuffer.PawnPush;
	  Options.SingleReply=transferbuffer.SingleReply;
	  Options.RandomMoveOrdering=transferbuffer.RandomMoveOrder;
	  Options.HashReadReduce=transferbuffer.HashReadReduce;
	  Options.HashWriteReduce=transferbuffer.HashWriteReduce;
	  Options.MaxExtend=transferbuffer.MaxExtend;
	  Options.NullMoveStopMaterial=transferbuffer.NullMoveStopMaterial;
	  Options.Aspire=transferbuffer.Aspire;
	  Options.NullMoveReduce=transferbuffer.NullMoveReduce;
	  if (Options.Quiesce>50) Options.Quiesce=50;
	  Options.Ponder=transferbuffer.Ponder;
	  if (!Options.Ponder && Pondering) Searcher->ExitWithoutMove();
	  Options.Contempt=transferbuffer.Contempt;
	  Options.NullMove=transferbuffer.NullMove;
	  Options.FailHighReductions=transferbuffer.FailHighReductions;
	  Options.OpeningLearning=transferbuffer.OpeningLearning;
	  Options.HashTableSize=transferbuffer.HashTableSize;
	  delete Searcher;
	  switch (Options.Variant) {
			case CHESS : Searcher=new TChessSearcher(Options.HashTableSize); break;
			case KINGLET : Searcher=new TKingletSearcher(Options.HashTableSize); break;
			case SHATRANJ : Searcher=new TShatranjSearcher(Options.HashTableSize); break;
			case SELFTAKE : Searcher=new TSelfTakeSearcher(Options.HashTableSize); break;
	  }
	  Options.MinimalWindow=transferbuffer.MinimalWindow;
	  if (Options.SearchMethod==BASEINCREMENT) {
			Game->SetWhiteStart(Options.BaseMinutes*60*1000);
			Game->SetBlackStart(Options.BaseMinutes*60*1000);
	  } else
	  if (Options.SearchMethod==CHAMPIONSHIP) {
			Game->SetWhiteStart(Options.ChampTime*1000);
			Game->SetBlackStart(Options.ChampTime*1000);
	  } else {
		  Game->SetWhiteClock(0);
		  Game->SetBlackClock(0);
	  }
	  CheckOptions();
	}
	delete cpdialog;
}

#define MAXREPLIES 2000

#ifdef NEWOPENINGS
int
TRivalWindow::GetOpening(TMove& Move, TMoveList PreviousMoves)
{
	TMoveList List;
	int i, j;
	char Fen[MAXFEN];
	TMove Reply[MAXREPLIES];
	int ReplyIndexes[MAXREPLIES];
	int Replies=0;
	int GamesPlayed;
	BOOL GoAhead;
	float Percentage;
	if (Verbose) writeMessage(VERBOSE, "Trying openings");
	BOOL WhiteToMove=Game->GetMover()==WHITE;
	_fmemcpy((void *)SetupBoard, (void *)Game->Square, 89*sizeof(int));
	TChessBoard* TempGame=new TChessBoard(SetupBoard);
	int MoveCount=Game->GetMoveList(List);
	for (i=1; i<=MoveCount; i++) {
		TempGame->MakeMove(List.Moves[i], TRUE);
		TempGame->GetFEN(Fen, TRUE, TRUE);
		for (j=0; j<CachedOpenings; j++) {
			if (strcmp(Fen, OpeningPositions[j].Fen)==0) {
				GamesPlayed=OpeningPositions[j].WhiteWins+OpeningPositions[j].BlackWins+OpeningPositions[j].Draws;
				GoAhead=(GamesPlayed<Options.MinimumBookResults);
				if (!GoAhead) {
					if (GamesPlayed==0) GamesPlayed=0.1;
					if (WhiteToMove) {
						Percentage=((float)OpeningPositions[j].WhiteWins/(float)GamesPlayed);
						GoAhead=((Percentage*100)>Options.WhitePercent);
					} else {
						Percentage=((float)OpeningPositions[j].BlackWins/(float)GamesPlayed);
						GoAhead=((Percentage*100)>Options.BlackPercent);
					}
				}
				if (GoAhead) {                   
					ReplyIndexes[Replies]=j;
					Reply[Replies++]=List.Moves[i];
					if (Replies==MAXREPLIES) goto done;
				}
			}
		}
		TempGame->TakeBackMove();
	}
done:
	delete TempGame;
	if (Replies==0) {
		if (Verbose) writeMessage(VERBOSE, "No more openings");
		TryOpenings=FALSE;
		return(FALSE);
	}
	int ReplyNum=random(Replies-1);
	Move=Reply[ReplyNum];
	Move.PromotionPiece=EMPTY;
	return(Game->VerifyMove(Move)==0);
}

#else
int
TRivalWindow::GetOpening(TMove& Move, TMoveList PreviousMoves)
{
  TMove Reply[MAXREPLIES];
  int NumberOfMoves;
  int Hit;
  int Replies=0;
  int i;
  char NumberInLine;
  TMove PlayMove;
  FILE *OpeningFile;
#ifndef NORANDOM
  randomize();
#endif
  NumberOfMoves=PreviousMoves.Moves.GetItemsInContainer();
  if ((OpeningFile=fopen(OPENINGS, "r"))!=NULL) {
	 Reply[0].From=0;
	 while ((NumberInLine=fgetc(OpeningFile))!=EOF) {
		NumberInLine-=65;
		Hit=TRUE;
		for (i=1; i<=NumberInLine; i++) {
			PlayMove.From=fgetc(OpeningFile)-65;
			PlayMove.To=fgetc(OpeningFile)-65;
			if (Hit && i<=NumberOfMoves && (PlayMove.From!=PreviousMoves.Moves[i].From || PlayMove.To!=PreviousMoves.Moves[i].To)) Hit=FALSE;
			if (i==NumberOfMoves+1 && Hit && Replies+1<MAXREPLIES) Reply[++Replies]=PlayMove;
		}
		fgetc(OpeningFile); // quality
	 }
	 fclose(OpeningFile);
  }
  if (Replies==0) {
	  return(FALSE);
  }
  Move=Reply[random(Replies)+1];
  return(Game->VerifyMove(Move)==0);
}
#endif

/*************************************************************************
Command Enabler Functions.
**************************************************************************/

void
TRivalWindow::CeIsNotInSetup(TCommandEnabler& ce)
{
	 ce.Enable(!InSetup);
}

void
TRivalWindow::CeResignOrOfferDraw(TCommandEnabler& ce)
{
	 ce.Enable(Game->GameStatus()==0 && (Options.ComputerWhite+Options.ComputerBlack==1));
}

void
TRivalWindow::CeSaveEnabler(TCommandEnabler& ce)
{
#ifdef SHAREWARE
	 ce.Enable(false);
#else
	 ce.Enable(!InSetup);
#endif
}

void
TRivalWindow::CeTakeBackAllMoves(TCommandEnabler& ce)
{
	 ce.Enable(Game->LastMoveMade()>0 && !InSetup);
}

void
TRivalWindow::CeTakeBackOneMove(TCommandEnabler& ce)
{
	 ce.Enable(Game->LastMoveMade()>0 && !InSetup);
}

void
TRivalWindow::CeReplayAllMoves(TCommandEnabler& ce)
{
	 ce.Enable(Game->LastMoveMade()<Game->TotalMovesMade() && !InSetup);
}

void
TRivalWindow::CeReplayOneMove(TCommandEnabler& ce)
{
	 ce.Enable(Game->LastMoveMade()<Game->TotalMovesMade() && !InSetup);
}

void
TRivalWindow::CePieceSlide(TCommandEnabler& ce)
{
	 ce.SetCheck(Options.PieceSlide);
}

void
TRivalWindow::CeMessages(TCommandEnabler& ce)
{
	 ce.SetCheck(Options.Messages);
}

void
TRivalWindow::CeAutoSave(TCommandEnabler& ce)
{
	 ce.SetCheck(Options.AutoSave);
}

void
TRivalWindow::CeFlag(TCommandEnabler& ce)
{
	 ce.SetCheck(Options.Flag);
}

void
TRivalWindow::CeVerbose(TCommandEnabler& ce)
{
	 ce.SetCheck(Verbose);
}

void
TRivalWindow::CeTimings(TCommandEnabler& ce)
{
	 ce.SetCheck(PrintInfo);
}

void
TRivalWindow::CePV(TCommandEnabler& ce)
{
	 ce.SetCheck(PrintPV);
}

void
TRivalWindow::CeInvertBoard(TCommandEnabler& ce)
{
	 ce.SetCheck(Options.InvertBoard);
}

void
TRivalWindow::CeChess(TCommandEnabler& ce)
{
	 ce.SetCheck(Options.Variant==CHESS);
	 ce.Enable(!InSetup);
}

void
TRivalWindow::CeSetup(TCommandEnabler& ce)
{
	 ce.Enable(!InSetup && !Searching);
}

void
TRivalWindow::CeSolve(TCommandEnabler& ce)
{
	 ce.Enable(!InSetup && !Searching);
}

void
TRivalWindow::CeShatranj(TCommandEnabler& ce)
{
	 ce.SetCheck(Options.Variant==SHATRANJ);
	 ce.Enable(!InSetup);
}

void
TRivalWindow::CeKinglet(TCommandEnabler& ce)
{
	 ce.SetCheck(Options.Variant==KINGLET);
	 ce.Enable(!InSetup);
}

void
TRivalWindow::CeSelfTake(TCommandEnabler& ce)
{
	 ce.SetCheck(Options.Variant==SELFTAKE);
	 ce.Enable(!InSetup);
}


void
TRivalWindow::CeShowAnalysis(TCommandEnabler& ce)
{
	 ce.SetCheck(Options.ShowAnalysis);
}

void
TRivalWindow::CeComputerWhite(TCommandEnabler& ce)
{
	 ce.SetCheck(Options.ComputerWhite);
}

void
TRivalWindow::CeComputerBlack(TCommandEnabler& ce)
{
	 ce.SetCheck(Options.ComputerBlack);
}

void
TRivalWindow::CeIsSearching(TCommandEnabler& ce)
{
	 ce.Enable(Searching);
}

void
TRivalWindow::CeIsNotSearching(TCommandEnabler& ce)
{
	 ce.Enable(!Searching);
}

void
TRivalWindow::CmHelp(WPARAM WParam)
{
	 switch (WParam) {
		case CM_HELP_CONTENTS :
			WinHelp("NewRival.Hlp", HELP_FINDER, 0);
			break;
		case CM_HELP_ABOUT :
			TDialog About(this, ABOUT);
			About.Execute();
			break;
	 }
}

void
TRivalWindow::EvKeyDown(uint Key, uint, uint f)
{
	switch (Key) {
	  case VK_RETURN : if (Searching)
								Searcher->ExitWithMove();
							 break;
	  case VK_ESCAPE : if (Searching || Pondering)
								Searcher->ExitWithoutMove();
							 if (CreatingOpeningBook) {
								CloseRequested=TRUE;
								EscapeHit=TRUE;
							 }
							 break;
	}
}


void
TRivalWindow::InitialiseSearchParameters()
{
	switch (Options.SearchMethod) {
		  case INFINITETIME : Searcher->SetMaxDepth(49); break;
		  case LEVEL : Searcher->SetMaxDepth(Options.Level); break;
		  case TIMEPERMOVE : Searcher->SetMaxDepth(49); break;
		  case BASEINCREMENT : Searcher->SetMaxDepth(49);
			  extendmaxmovetime = 0;
			  if (Options.Increment>0) {
				  normalmovetime = Options.Increment*1000;
				  int Player=(Game->GetMover()==WHITE ? Game->GetWhiteClock() : Game->GetBlackClock());
				  if (Player>10000) {
						extendmaxmovetime=Player/3;
						normalmovetime+=((Player-10000)/35);
				  } else {
						normalmovetime*=0.75;
				  }
				  if (extendmaxmovetime>normalmovetime*4)	extendmaxmovetime=normalmovetime*4;
			  } else {
				  int Player=(Game->GetMover()==WHITE ? Game->GetWhiteClock() : Game->GetBlackClock());
				  int Opponent=(Game->GetMover()==BLACK ? Game->GetWhiteClock() : Game->GetBlackClock());
				  normalmovetime=(Player)/(25+(15*(Game->TotalMovesMade()<24)));
				  if (Opponent-(normalmovetime*1.5) > Player) normalmovetime*=0.75;
				  if (normalmovetime<200) normalmovetime=200;
				  extendmaxmovetime=normalmovetime*1.5;
			  }
			  break;
		  case CHAMPIONSHIP : Searcher->SetMaxDepth(49);
					  int MovesMade=((Game->LastMoveMade()/2)%Options.ChampMoves);
					  int MovesLeft=(Options.ChampMoves-MovesMade);
					  SearchTimeLeft=-3000+(Game->GetMover()==WHITE ? Game->GetWhiteClock() : Game->GetBlackClock());
					  MeanSearchTime=(SearchTimeLeft)/MovesLeft;
					  globalmeansearchtime=MeanSearchTime;
					  globalsearchtimeleft=SearchTimeLeft;
					  if (Verbose) writeMessage(VERBOSE, "MovesMade", MovesMade);
					  if (Verbose) writeMessage(VERBOSE, "MovesLeft", MovesLeft);
					  if (Verbose) writeMessage(VERBOSE, "ChampTime", Options.ChampTime);
					  if (Verbose) writeMessage(VERBOSE, "ChampMoves", Options.ChampMoves);
					  break;
	}
	Searcher->SetMaxQuiesceDepth(Options.Quiesce);
	Searcher->SetNullMoveStopMaterial(Options.NullMoveStopMaterial);
	Searcher->SetContempt(Options.Contempt);
	Searcher->SetUseHistory(Options.History);
	Searcher->SetUseSingleReplyExtensions(Options.SingleReply);
	Searcher->SetRandomMoveOrder(Options.RandomMoveOrdering);
	Searcher->SetHashReadReduce(Options.HashReadReduce);
	Searcher->SetHashWriteReduce(Options.HashWriteReduce);
	Searcher->SetUseMinimalWindow(Options.MinimalWindow);
	Searcher->SetUseNullMove(Options.NullMove);
	Searcher->SetUseFailHighReductions(Options.FailHighReductions);
	Searcher->SetNullMoveReduceDepth(Options.NullMoveReduce);
	Searcher->SetUseCheckExtensions(Options.Check);
	Searcher->SetUsePawnPushExtensions(Options.PawnPush);
	Searcher->SetMaxExtensions(Options.MaxExtend);
	Searcher->SetAspireWindow(Options.Aspire);
	Searcher->SetUseRecaptureExtensions(Options.Recapture);
	Searcher->SetPrintPV(PrintPV);
}

void TRivalWindow::ResetClocksToPrevious()
{
	 if (Game->GetMover()==WHITE) {
		Game->SetWhiteClock(Game->WhiteClocks[Game->LastMoveMade()]);
	 } else {
		Game->SetBlackClock(Game->BlackClocks[Game->LastMoveMade()]);
	 }
}

void
TRivalWindow::ComputerMove()
{
	globalNodes=0;
	globalPVMove.From=0;
   globalDepth=0;
	showAnalysis = Options.ShowAnalysis;
	globalstarttime = GetTickCount();
	TPath Path;
	Select.From=0;
/* When computer control is first assigned, we set the clock of the
	side to move back to the value it held after the last move was
	made.
*/
	 ResetClocksToPrevious();
	 InitialiseSearchParameters();
	 if (Verbose) writeMessage(VERBOSE, "Search Parameters Initialised");
	 do {
		TMove OpeningMove;
		OpeningMove.From=0;
		if (!(TryOpenings && Options.Openings && Options.Variant==CHESS && Game->IsOriginalPosition() && (!(Options.ComputerWhite && Options.ComputerBlack)) && GetOpening(OpeningMove, Game->GetPreviousMoves()))) {
		  Searcher->SetInitialPosition(*Game);
		  if (Verbose) writeMessage(VERBOSE, "Initial Position Set");
		  SearchStartTime=GetTickCount();
		  HashCalls=HashSuccesses=MakeMoveCalls=GMLCalls=GQMLCalls=GMLCMCalls=GQMLCMCalls=StoreHashMoveCalls=EvaluateCalls=1;
		  GetMoveListTimer=GetQuickMoveListTimer=EvaluateTimer=MakeMoveTimer=GetHashMoveTimer=StoreHashMoveTimer=0;
		  Case1Calls=Case2Calls=Case3Calls=Case4Calls=1;
		  Case1Timer=Case2Timer=Case3Timer=Case4Timer=0;
#ifdef TESTING
		  Searcher->ClearHashTable();
#endif
		  if (Options.ComputerWhite && Options.ComputerBlack) {
			  Searcher->ClearHashTable();
			  if (Verbose) writeMessage(VERBOSE, "Hash Table Cleared");
			  if (Options.SearchMethod==LEVEL && Options.Level<3) {
				  Searcher->SetMaxDepth(3);
				  if (Verbose) writeMessage(VERBOSE, "Changed depth to 3 for computer play");
			  }
		  }
		  Searching=TRUE;
		  Searcher->StaleHashTable();
		  _fmemcpy((void *)PathBoard, (void *)Game->Square, 89*sizeof(int));
		  Path=Searcher->Search();
		  Path2=Searcher->GetPrincipalPath();
		  if (Verbose) writeMessage(VERBOSE, "Finished Searching");
		  Searching=FALSE;
		} else {
		  _fmemcpy((void *)PathBoard, (void *)Game->Square, 89*sizeof(int));
		  OpeningMove.Capture=Game->GetSquare(OpeningMove.To);
		  Path.Move[0]=OpeningMove;
		  Path.Move[1].From=0;
		  Path.Nodes=0;
		}
/*************************************************************************
If CloseRequested is TRUE then this signals that user has requested
that the window be closed but that the Searcher was searching at this point.
The Searcher would have been sent a RequestClose() message and would have
terminated the search.
It is very important to set the side to move to
non-computer control because the remainder of this function will be
called after we call CloseWindow().
**************************************************************************/
		if (Path.Move[0].From!=0) {
		  if (Game->VerifyMove(Path.Move[0])==0) {
			 if (Verbose) {
				FILE *log = fopen( VERBOSE, "a" );
				fprintf(log, "Move %i-%i verified, normalmovetime=%i\n", Path.Move[0].From, Path.Move[0].To, normalmovetime );
				fclose(log);
			 }
			 UpdateWindow();
			 if (!WinBoard) SlidePiece(Path.Move[0]);
			 MakeMove(Game, Path.Move[0]);
			 if (WinBoard) SendXBoard(Path.Move[0]);
			 if (OpeningMove.From!=0) {
				Path2.Move[0]=OpeningMove;
				Path2.Move[1].From=0;
				Path2.Depth=0;
				Path2.Value=0;
            Path2.Nodes=0;
			 }
			FILE *f;
			float t=(GetTickCount()-SearchStartTime+1)/1000.0;
			long n=Path2.Nodes;
			 if (PrintInfo) {
				 if (OpeningMove.From==0) {
					char s[100];
					string s2;
					sprintf(s, "Searched %li nodes in %f Seconds\n", n, t);
					s2=s;
					sprintf(s, "Nodes/Seconds = %li\n", (int)(n/t));
					s2+=s;
					sprintf(s, "Average time spent at each node = %.4fms\n", ((float)t/n)*1000.0);
					s2+=s;
					sprintf(s, "Hash Successes = %i/%i\n", HashSuccesses, HashCalls);
					s2+=s;
					sprintf(s, "Score %i\n", Path.Value);
					s2+=s;
					sprintf(s, "Depth %i\n", Path.Depth);
					s2+=s;
					sprintf(s, "Average Number of Moves Generated %.1f\n", (float)GMLCMCalls/GMLCalls);
					s2+=s;
					sprintf(s, "Average Number of QuickMoves Generated %.1f\n", (float)GQMLCMCalls/GQMLCalls);
					s2+=s;
					sprintf(s, "MakeMove ACT %ims/%i=%.4fms\n", MakeMoveTimer, MakeMoveCalls, (float)MakeMoveTimer/MakeMoveCalls);
					s2+=s;
					sprintf(s, "GetMoveList ACT %ims/%i=%.4fms\n", GetMoveListTimer, GMLCalls, (float)GetMoveListTimer/GMLCalls);
					s2+=s;
					sprintf(s, "GetQuickMoveList ACT %ims/%i=%.4fms\n", GetQuickMoveListTimer, GQMLCalls, (float)GetQuickMoveListTimer/GQMLCalls);
					s2+=s;
					sprintf(s, "GetHashMove ACT %ims/%i=%.4fms\n", GetHashMoveTimer, HashCalls, (float)GetHashMoveTimer/HashCalls);
					s2+=s;
					sprintf(s, "StoreHashMove ACT %ims/%i=%.4fms\n", StoreHashMoveTimer, StoreHashMoveCalls, (float)StoreHashMoveTimer/StoreHashMoveCalls);
					s2+=s;
					sprintf(s, "Evaluate ACT %ims/%i=%.4fms\n", EvaluateTimer, EvaluateCalls, (float)EvaluateTimer/EvaluateCalls);
					s2+=s;
					sprintf(s, "Case 1 ACT %ims/%i=%.4fms pn=%.4fms\n", Case1Timer, Case1Calls, (float)Case1Timer/Case1Calls, (float)Case1Timer/n);
					s2+=s;
					sprintf(s, "Case 2 ACT %ims/%i=%.4fms pn=%.4fms\n", Case2Timer, Case2Calls, (float)Case2Timer/Case2Calls, (float)Case2Timer/n);
					s2+=s;
					sprintf(s, "Case 3 ACT %ims/%i=%.4fms pn=%.4fms\n", Case3Timer, Case3Calls, (float)Case3Timer/Case3Calls, (float)Case3Timer/n);
					s2+=s;
					sprintf(s, "Case 4 ACT %ims/%i=%.4fms pn=%.4fms\n", Case4Timer, Case4Calls, (float)Case4Timer/Case4Calls, (float)Case4Timer/n);
					s2+=s;
					sprintf(s, "MakeMove Calls/Node %.2f, Time %.4fms\n", (float)MakeMoveCalls/n, ((float)MakeMoveCalls/n)*((float)MakeMoveTimer/MakeMoveCalls));
					s2+=s;
					sprintf(s, "GetMoveList Calls/Node %.2f, Time %.4fms\n", (float)GMLCalls/n, ((float)GMLCalls/n)*((float)GetMoveListTimer/GMLCalls));
					s2+=s;
					sprintf(s, "GetQuickMoveList Calls/Node %.2f, Time %.4fms\n", (float)GQMLCalls/n, ((float)GQMLCalls/n)*((float)GetQuickMoveListTimer/GQMLCalls));
					s2+=s;
					sprintf(s, "GetHashMove Calls/Node %.2f, Time %.4fms\n", (float)HashCalls/n, ((float)HashCalls/n)*((float)GetHashMoveTimer/HashCalls));
					s2+=s;
					sprintf(s, "StoreHashMove Calls/Node %.2f, Time %.4fms\n", (float)StoreHashMoveCalls/n, ((float)StoreHashMoveCalls/n)*((float)StoreHashMoveTimer/StoreHashMoveCalls));
					s2+=s;
					sprintf(s, "Evaluate Calls/Node %.2f, Time %.4fms\n", (float)EvaluateCalls/n, ((float)EvaluateCalls/n)*((float)EvaluateTimer/EvaluateCalls));
					s2+=s;
					//::MessageBox(this->HWindow, s2.c_str(), "Information Box", MB_OK);
					if (PrintInfo) {
						if ((f=fopen( TIMINGS, "w" ))!=NULL) {
							fprintf( f, s2.c_str() );
							fclose(f);
						};
					}
				}
			 }
			 if (PrintInfo) {
					if ((f=fopen( NODES, "a" ))!=NULL) {
						if (Path2.Nodes>0) {
							fprintf(f, "Searched %li nodes in %f Seconds [%i]\n", n, t, (int)((float)n/t));
						} else {
							fprintf(f, "No nodes searched (may be book move)\n");
						}
						fclose(f);
					}
			 }
		  } else {
				if (Verbose) {
					FILE *log = fopen( VERBOSE, "a" );
					fprintf(log, "Move %i%-%i not verified\n", Path.Move[0].From, Path.Move[0].To );
					fclose(log);
				}
			 char s[100];
			 sprintf(s, "Move not Verified %i-%i\n Error Code %i", Path.Move[0].From, Path.Move[0].To, Game->VerifyMove(Path.Move[0]));
			 ::MessageBox(this->HWindow, s, "Debug Information", MB_OK);
			 if (Game->GetMover()==WHITE) Options.ComputerWhite=FALSE; else Options.ComputerBlack=FALSE;
		  }
		} else {
		  if (Game->GetMover()==WHITE) Options.ComputerWhite=FALSE;
			  else Options.ComputerBlack=FALSE;
		  if (Verbose) writeMessage(VERBOSE, "No Move Returned");
		  if (CloseRequested) CloseWindow();
		}
		UpdateAllGraphics();
	 } while (!GameOver() && ((Game->GetMover()==WHITE && Options.ComputerWhite) || (Game->GetMover()==BLACK && Options.ComputerBlack)));
	 TPath PonderPath;
	 // Take the opponent's move from the main line and make the move
	 // on a temporary board, then search.
	 // If no opponent's move exists, take the opponent's move from the
	 // last ponder.
	 if (Options.Ponder && Path.Move[0].From!=0) {
		 Pondering=TRUE;
		 TMove PonderMove = Path.Move[1];
		 if (PonderMove.From==0) {
			 Searcher->SetMaxDepth(1);
			 PonderPath=Searcher->Search();
			 PonderMove=PonderPath.Move[0];
		 }
		 if (PonderMove.From!=0) {
			 if (Game->VerifyMove(PonderMove)==0) {
				 TChessBoard* PonderBoard=new TChessBoard();
				 *PonderBoard=*Game;
				 PonderBoard->MakeMove(PonderMove);
				 Searcher->StaleHashTable();
				 Searcher->SetInitialPosition(*PonderBoard);
				 PonderPath=Searcher->Search();
				 delete PonderBoard;
				 if (PrintInfo) {
					 FILE *log = fopen( "nodes.txt", "a" );
					 fprintf(log, "Finished Pondering %i nodes for %i-%i.  Got %i-%i\n",
						Searcher->GetCurrentPathNodes(),
						PonderMove.From, PonderMove.To, PonderPath.Move[0].From, PonderPath.Move[0].To );
					 fclose(log);
				 }
			 }
		  Pondering = false;
		  if (CloseRequested) CloseWindow();
		 }
	 }
}

class TRivalApp : public TApplication
{
	public:
	  TRivalApp(char far* name) : TApplication(name) { };
	  void InitMainWindow();
};

void
TRivalApp::InitMainWindow()
{
	EnableBWCC();
	EnableCtl3d();
	TRivalWindow* Client=new TRivalWindow;
	char title[200];
	strcpy(title, TITLEVERSION);
#ifdef SHAREWARE
	strcat(title, " - unregistered");
#endif
	MainWindow=new TFrameWindow(0, title, Client, TRUE);
	MainWindow->SetIcon(this, IDI_ICONKING);
//	MainWindow=Frame;
	MainWindow->Attr.X=0;
	MainWindow->Attr.Y=0;
	MainWindow->Attr.Style &= ~(WS_MAXIMIZEBOX | WS_THICKFRAME);
	MainWindow->AssignMenu(MENU_1);
	MainWindow->Attr.AccelTable=ID_MENU_1_ACCELERATORS;
   mWindow = MainWindow;
#ifdef SHAREWARE
	TSplashDialog* splash =
	  new TSplashDialog(GetMainWindow( ), 101);
	splash->Execute( );
#endif
}

int
OwlMain(int argc, char *args[])
{
	FILE *f;
	oldtime=GetTickCount();
	int parms = argc - 1;
	globalNodes=0;
#ifdef TESTING
	PrintInfo=true;
	Verbose=true;
	PrintPV=true;
	f=fopen( "c:\\log.txt", "w" );
	fclose(f);
#endif
	strcpy(CommandLinePGN, "");
	if (getcwd(workingDirectory, sizeof(workingDirectory))==NULL) {
		strcpy(workingDirectory, ".\\");
	}
	if (strlen(workingDirectory)>0) {
		if (workingDirectory[strlen(workingDirectory)-1]=='\\') {
			workingDirectory[strlen(workingDirectory)-1]='\0';
		}
	}
	for (int i=1; i<=parms; i++) {
		dncase(args[i]);
		if (strcmp(args[i],"-winboard")==0 || strcmp(args[i],"-w")==0) {
#ifdef STUPIDBUG
	f=fopen( "c:\\bc45\\rival\\output\\log.txt", "w" );
	fprintf(f, "%s\n", args[i]);
	fclose(f);
#endif
			WinBoard = true;
		}
		if (strstr(args[i],".pgn")!=NULL) {
			strcpy(CommandLinePGN, args[i]);
		}
	}
	strcpy(OPTIONS, workingDirectory);
	strcat(OPTIONS, "\\newrival.opt");
	strcpy(OPENINGS, workingDirectory);
	strcat(OPENINGS, "\\newrival.opn");
#ifndef NORANDOM
	randomize();
#endif
	return TRivalApp("Rival Chess for Windows").Run();
}

