#include "searcher.h"


/*
int TSearcher::LoneKingAgainstPawns(TPosition Position)
{
	int WinningSide;
	int WinningKing;
	int LosingKing;
	int i, y, wx, wy, lx, ly;
	int Opposition;
	int MostCentralPawn=0;
	int Score=Position[WHITEPAWNS]*PAWNVALUE-Position[BLACKPAWNS]*PAWNVALUE;
	if (Score>0) {
		WinningSide=WHITE;
		WinningKing=Position[WKINGPOSITION];
		LosingKing=Position[BKINGPOSITION];
		// Get most central pawn file
		for (i=1; i<=8; i++) {
			if (Position[WHITEPAWNFILES+i]>0) {
				if (i==1 && i==8)	Score-=EDGEPAWN;
				if (MostCentralPawn==0 || i<5) {
					MostCentralPawn=i;
				}
			}
			MostCentralPawn=i*10;
			for (i=1; i<=Position[WHITEPAWNS]; i++) {
				y=GetY(Position[WHITEPAWNS+i]);
				Score+=(y*y*PUSHPAWN);
				if (GetX(Position[WHITEPAWNS+i])==GetX(MostCentralPawn) &&
				  Position[WHITEPAWNS+i]>MostCentralPawn)
					MostCentralPawn=Position[WHITEPAWNS+i];
			}
		}
	} else {
		WinningSide=BLACK;
		WinningKing=Position[BKINGPOSITION];
		LosingKing=Position[WKINGPOSITION];
		Score=-Score;
		for (i=1; i<=8; i++) {
			if (Position[BLACKPAWNFILES+i]>0) {
				if (i==1 && i==8)	Score-=EDGEPAWN;
				if (MostCentralPawn==0 || i<5) {
					MostCentralPawn=i;
				}
			}
		}
		MostCentralPawn=i*10;
		for (i=1; i<=Position[BLACKPAWNS]; i++) {
			y=9-GetY(Position[WHITEPAWNS+i]);
			Score+=(y*y*PUSHPAWN);
			if (GetX(Position[BLACKPAWNS+i])==GetX(MostCentralPawn) &&
				  Position[WHITEPAWNS+i]<MostCentralPawn)
					MostCentralPawn=Position[BLACKPAWNS+i];
		}
	}

	// send friendly king close to friendly pawn
	Score+=(Tropism[WinningKing][MostCentralPawn]*KINGPAWNTROPISM);

	// penalise for enemy having the opposition
	wx=GetX(WinningKing);
	wy=GetY(WinningKing);
	lx=GetX(LosingKing);
	ly=GetY(LosingKing);
	Opposition=(wx==lx && abs(wy-ly)==2);
	Opposition=Opposition || (wy==ly && abs(wx-lx)==2);
	if (Opposition && WinningSide==Position[MOVER])
		Score-=ENEMYHASOPPOSITION;

	Score/=10.0;
	if (WinningSide==Position[MOVER])
		return Score;
	else
		return -Score;
};*/


