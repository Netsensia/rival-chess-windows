#include "kingletb.h"

TKingletBoard::TKingletBoard()
  : TChessBoard()
{
}

TKingletBoard::TKingletBoard(int* SetupPosition)
  : TChessBoard(SetupPosition)
{
}

void
TKingletBoard::AddMove(TMoveList& List, int Fx, int Fy, int Tx, int Ty)
{
	TMove m;
	if (Fx<1 || Fx>8 || Tx<1 || Tx>8)
	  throw((int)-3);
	m.From=Fx*10+Fy;
	m.To=Tx*10+Ty;
	m.Capture=Square[m.To];
	List.Moves.Add(m);
}

int
TKingletBoard::GetMoveList(TMoveList& List)
{
	int x, y;
	int wc=0;
	int bc=0;
	for (x=1; x<=8; x++)
	  for (y=1; y<=8; y++) {
		  if (Square[x*10+y]==WP) wc++;
		  if (Square[x*10+y]==BP) bc++;
	  }
	if (wc==0 || bc==0) return 0;

	int i;
	int Mover;
	int Blocked;
	int SquareIndex;
	List.Moves.Flush();
	Mover=Square[MOVER];
	for (x=1; x<=8; x++)
	  for (y=1; y<=8; y++) {
		 SquareIndex=x*10+y;
		 switch (Square[SquareIndex]) {
			 case WP : if (Mover==BLACK) break;
						  if (Square[SquareIndex+1]==EMPTY) {
							 AddMove(List, x, y, x, y+1);
							 if (y==2 && Square[SquareIndex+2]==EMPTY) {
								AddMove(List, x, y, x, y+2);
							 }
						  }
						  if (x<8 && Square[SquareIndex+11]>EMPTY)
							 AddMove(List, x, y, x+1, y+1);
						  if (x>1 && Square[SquareIndex-9]>EMPTY)
							 AddMove(List, x, y, x-1, y+1);
						  if (y==5 && x>1 && x-1==Square[ENPAWN])
							 AddMove(List, x, y, x-1, y+1);
						  if (y==5 && x<8 && x+1==Square[ENPAWN])
							 AddMove(List, x, y, x+1, y+1);
						  break;
			 case BP : if (Mover==WHITE) break;
						  if (y>1 && Square[SquareIndex-1]==EMPTY) {
							 AddMove(List, x, y, x, y-1);
							 if (y==7 && Square[SquareIndex-2]==EMPTY) {
								AddMove(List, x, y, x, y-2);
							 }
						  }
						  if (x>1 && Square[SquareIndex-11]<EMPTY)
							 AddMove(List, x, y, x-1, y-1);
						  if (x<8 && Square[SquareIndex+9]<EMPTY)
							 AddMove(List, x, y, x+1, y-1);
						  if (y==4 && x>1 && x-1==Square[ENPAWN])
							 AddMove(List, x, y, x-1, y-1);
						  if (y==4 && x<8 && x+1==Square[ENPAWN])
							 AddMove(List, x, y, x+1, y-1);
						  break;
			 case WK : if (Mover==BLACK) break;
						  if (x>1 && Square[SquareIndex-10]>=EMPTY) AddMove(List, x, y, x-1, y);
						  if (x<8 && Square[SquareIndex+10]>=EMPTY) AddMove(List, x, y, x+1, y);
						  if (y>1 && Square[SquareIndex-1]>=EMPTY) AddMove(List, x, y, x, y-1);
						  if (y<8 && Square[SquareIndex+1]>=EMPTY) AddMove(List, x, y, x, y+1);
						  if (x>1 && y>1 && Square[SquareIndex-11]>=EMPTY) AddMove(List, x, y, x-1, y-1);
						  if (x<8 && y<8 && Square[SquareIndex+11]>=EMPTY) AddMove(List, x, y, x+1, y+1);
						  if (x>1 && y<8 && Square[SquareIndex-9]>=EMPTY) AddMove(List, x, y, x-1, y+1);
						  if (x<8 && y>1 && Square[SquareIndex+9]>=EMPTY) AddMove(List, x, y, x+1, y-1);
						  if (x==5 && y==1 && !Square[WKINGMOVED]) {
									if (!Square[WROOK8MOVED]) {
									  if (Square[61]==EMPTY && Square[71]==EMPTY) {
												AddMove(List, x, y, x+2, y);
									  }
									}
									if (!Square[WROOK1MOVED]) {
									  if (Square[41]==EMPTY && Square[31]==EMPTY && Square[21]==EMPTY) {
												AddMove(List, x, y, x-2, y);
									  }
									}
						  }
						  break;
			 case BK : if (Mover==WHITE) break;
						  if (x>1 && Square[SquareIndex-10]<=EMPTY) AddMove(List, x, y, x-1, y);
						  if (x<8 && Square[SquareIndex+10]<=EMPTY) AddMove(List, x, y, x+1, y);
						  if (y>1 && Square[SquareIndex-1]<=EMPTY)
							  AddMove(List, x, y, x, y-1);
						  if (y<8 && Square[SquareIndex+1]<=EMPTY) AddMove(List, x, y, x, y+1);
						  if (x>1 && y>1 && Square[SquareIndex-11]<=EMPTY) AddMove(List, x, y, x-1, y-1);
						  if (x<8 && y<8 && Square[SquareIndex+11]<=EMPTY) AddMove(List, x, y, x+1, y+1);
						  if (x>1 && y<8 && Square[SquareIndex-9]<=EMPTY) AddMove(List, x, y, x-1, y+1);
						  if (x<8 && y>1 && Square[SquareIndex+9]<=EMPTY) AddMove(List, x, y, x+1, y-1);
						  if (x==5 && y==8 && !Square[BKINGMOVED]) {
									if (!Square[BROOK8MOVED]) {
									  if (Square[68]==EMPTY && Square[78]==EMPTY) {
												AddMove(List, x, y, x+2, y);
									  }
									}
									if (!Square[BROOK1MOVED]) {
									  if (Square[48]==EMPTY && Square[38]==EMPTY && Square[28]==EMPTY) {
												AddMove(List, x, y, x-2, y);
									  }
									}
						  }
						  break;
			 case WN : if (Square[MOVER]==BLACK) break;
						  if (GetX(SquareIndex)<8 && GetY(SquareIndex)>2 && Square[SquareIndex+8]>=EMPTY) AddMove(List, x, y, x+1, y-2);
						  if (GetX(SquareIndex)>1 && GetY(SquareIndex)<7 && Square[SquareIndex-8]>=EMPTY) AddMove(List, x, y, x-1, y+2);
						  if (GetX(SquareIndex)<8 && GetY(SquareIndex)<7 && Square[SquareIndex+12]>=EMPTY) AddMove(List, x, y, x+1, y+2);
						  if (GetX(SquareIndex)>1 && GetY(SquareIndex)>2 && Square[SquareIndex-12]>=EMPTY) AddMove(List, x, y, x-1, y-2);
						  if (GetX(SquareIndex)<7 && GetY(SquareIndex)>1 && Square[SquareIndex+19]>=EMPTY) AddMove(List, x, y, x+2, y-1);
						  if (GetX(SquareIndex)>2 && GetY(SquareIndex)<8 && Square[SquareIndex-19]>=EMPTY) AddMove(List, x, y, x-2, y+1);
						  if (GetX(SquareIndex)<7 && GetY(SquareIndex)<8 && Square[SquareIndex+21]>=EMPTY) AddMove(List, x, y, x+2, y+1);
						  if (GetX(SquareIndex)>2 && GetY(SquareIndex)>1 && Square[SquareIndex-21]>=EMPTY) AddMove(List, x, y, x-2, y-1);
						  break;
			 case BN : if (Square[MOVER]==WHITE) break;
						  if (GetX(SquareIndex)<8 && GetY(SquareIndex)>2 && Square[SquareIndex+8]<=EMPTY) AddMove(List, x, y, x+1, y-2);
						  if (GetX(SquareIndex)>1 && GetY(SquareIndex)<7 && Square[SquareIndex-8]<=EMPTY) AddMove(List, x, y, x-1, y+2);
						  if (GetX(SquareIndex)<8 && GetY(SquareIndex)<7 && Square[SquareIndex+12]<=EMPTY) AddMove(List, x, y, x+1, y+2);
						  if (GetX(SquareIndex)>1 && GetY(SquareIndex)>2 && Square[SquareIndex-12]<=EMPTY) AddMove(List, x, y, x-1, y-2);
						  if (GetX(SquareIndex)<7 && GetY(SquareIndex)>1 && Square[SquareIndex+19]<=EMPTY) AddMove(List, x, y, x+2, y-1);
						  if (GetX(SquareIndex)>2 && GetY(SquareIndex)<8 && Square[SquareIndex-19]<=EMPTY) AddMove(List, x, y, x-2, y+1);
						  if (GetX(SquareIndex)<7 && GetY(SquareIndex)<8 && Square[SquareIndex+21]<=EMPTY) AddMove(List, x, y, x+2, y+1);
						  if (GetX(SquareIndex)>2 && GetY(SquareIndex)>1 && Square[SquareIndex-21]<=EMPTY) AddMove(List, x, y, x-2, y-1);
						  break;
			 case WR : if (Square[MOVER]==BLACK) break;
						  if (GetX(SquareIndex)<8)
							  for (Blocked=FALSE, i=SquareIndex+10; GetX(i)<=8 && !Blocked; i+=10)
								  if (Square[i]>=EMPTY) {
									  AddMove(List, x, y, GetX(i), y);
									  if (Square[i]!=EMPTY) Blocked=TRUE;
								  } else Blocked=TRUE;
						  if (GetX(SquareIndex)>1)
							  for (Blocked=FALSE, i=SquareIndex-10; GetX(i)>=1 && !Blocked; i-=10)
								  if (Square[i]>=EMPTY) {
									  AddMove(List, x, y, GetX(i), y);
									  if (Square[i]!=EMPTY) Blocked=TRUE;
								  } else Blocked=TRUE;
						  if (GetY(SquareIndex)<8)
							  for (Blocked=FALSE, i=SquareIndex+1; GetY(i)<=8 && !Blocked; i++)
								  if (Square[i]>=EMPTY) {
									  AddMove(List, x, y, x, GetY(i));
									  if (Square[i]!=EMPTY) Blocked=TRUE;
								  } else Blocked=TRUE;
						  if (GetY(SquareIndex)>1)
							  for (Blocked=FALSE, i=SquareIndex-1; GetY(i)>=1 && !Blocked; i--)
								  if (Square[i]>=EMPTY) {
									 AddMove(List, x, y, x, GetY(i));
									 if (Square[i]!=EMPTY) Blocked=TRUE;
								  } else Blocked=TRUE;
						  break;
			 case BR : if (Square[MOVER]==WHITE) break;
						  if (GetX(SquareIndex)<8)
							  for (Blocked=FALSE, i=SquareIndex+10; GetX(i)<=8 && !Blocked; i+=10)
								  if (Square[i]<=EMPTY) {
									  AddMove(List, x, y, GetX(i), y);
									  if (Square[i]!=EMPTY) Blocked=TRUE;
								  } else Blocked=TRUE;
						  if (GetX(SquareIndex)>1)
							  for (Blocked=FALSE, i=SquareIndex-10; GetX(i)>=1 && !Blocked; i-=10)
								  if (Square[i]<=EMPTY) {
									  AddMove(List, x, y, GetX(i), y);
									  if (Square[i]!=EMPTY) Blocked=TRUE;
								  } else Blocked=TRUE;
						  if (GetY(SquareIndex)<8)
							  for (Blocked=FALSE, i=SquareIndex+1; GetY(i)<=8 && !Blocked; i++)
								  if (Square[i]<=EMPTY) {
									 AddMove(List, x, y, x, GetY(i));
									 if (Square[i]!=EMPTY) Blocked=TRUE;
								  } else Blocked=TRUE;
						  if (GetY(SquareIndex)>1)
							  for (Blocked=FALSE, i=SquareIndex-1; GetY(i)>=1 && !Blocked; i--)
								  if (Square[i]<=EMPTY) {
									 AddMove(List, x, y, x, GetY(i));
									 if (Square[i]!=EMPTY) Blocked=TRUE;
								  } else Blocked=TRUE;
						  break;
			 case WB : if (Square[MOVER]==BLACK) break;
						  if (GetX(SquareIndex)<8 && GetY(SquareIndex)<8)
							  for (Blocked=FALSE, i=SquareIndex+11; GetX(i)<=8 && GetY(i)<=8 && !Blocked; i+=11)
								  if (Square[i]>=EMPTY) {
									  AddMove(List, x, y, GetX(i), GetY(i));
									  if (Square[i]!=EMPTY) Blocked=TRUE;
								  } else Blocked=TRUE;
						  if (GetX(SquareIndex)>1 && GetY(SquareIndex)>1)
							  for (Blocked=FALSE, i=SquareIndex-11; GetX(i)>=1 && GetY(i)>=1 && !Blocked; i-=11)
								  if (Square[i]>=EMPTY) {
									  AddMove(List, x, y, GetX(i), GetY(i));
									  if (Square[i]!=EMPTY) Blocked=TRUE;
								  } else Blocked=TRUE;
						  if (GetX(SquareIndex)>1 && GetY(SquareIndex)<8)
							  for (Blocked=FALSE, i=SquareIndex-9; GetX(i)>=1 && GetY(i)<=8 && !Blocked; i-=9)
								  if (Square[i]>=EMPTY) {
									  AddMove(List, x, y, GetX(i), GetY(i));
									  if (Square[i]!=EMPTY) Blocked=TRUE;
								  } else Blocked=TRUE;
						  if (GetX(SquareIndex)<8 && GetY(SquareIndex)>1)
							  for (Blocked=FALSE, i=SquareIndex+9; GetX(i)<=8 && GetY(i)>=1 && !Blocked; i+=9)
								  if (Square[i]>=EMPTY) {
									 AddMove(List, x, y, GetX(i), GetY(i));
									 if (Square[i]!=EMPTY) Blocked=TRUE;
								  } else Blocked=TRUE;
						  break;
			 case BB : if (Square[MOVER]==WHITE) break;
						  if (GetX(SquareIndex)<8 && GetY(SquareIndex)<8)
							  for (Blocked=FALSE, i=SquareIndex+11; GetX(i)<=8 && GetY(i)<=8 && !Blocked; i+=11)
								  if (Square[i]<=EMPTY) {
									  AddMove(List, x, y, GetX(i), GetY(i));
									  if (Square[i]!=EMPTY) Blocked=TRUE;
								  } else Blocked=TRUE;
						  if (GetX(SquareIndex)>1 && GetY(SquareIndex)>1)
							  for (Blocked=FALSE, i=SquareIndex-11; GetX(i)>=1 && GetY(i)>=1 && !Blocked; i-=11)
								  if (Square[i]<=EMPTY) {
									  AddMove(List, x, y, GetX(i), GetY(i));
									  if (Square[i]!=EMPTY) Blocked=TRUE;
								  } else Blocked=TRUE;
						  if (GetX(SquareIndex)>1 && GetY(SquareIndex)<8)
							  for (Blocked=FALSE, i=SquareIndex-9; GetX(i)>=1 && GetY(i)<=8 && !Blocked; i-=9)
								  if (Square[i]<=EMPTY) {
									  AddMove(List, x, y, GetX(i), GetY(i));
									  if (Square[i]!=EMPTY) Blocked=TRUE;
								  } else Blocked=TRUE;
						  if (GetX(SquareIndex)<8 && GetY(SquareIndex)>1)
							  for (Blocked=FALSE, i=SquareIndex+9; GetX(i)<=8 && GetY(i)>=1 && !Blocked; i+=9)
								  if (Square[i]<=EMPTY) {
									 AddMove(List, x, y, GetX(i), GetY(i));
									 if (Square[i]!=EMPTY) Blocked=TRUE;
								  } else Blocked=TRUE;
						  break;
			 case WQ : if (Square[MOVER]==BLACK) break;
						  if (GetX(SquareIndex)<8)
							  for (Blocked=FALSE, i=SquareIndex+10; GetX(i)<=8 && !Blocked; i+=10)
								  if (Square[i]>=EMPTY) {
									  AddMove(List, x, y, GetX(i), y);
									  if (Square[i]!=EMPTY) Blocked=TRUE;
								  } else Blocked=TRUE;
						  if (GetX(SquareIndex)>1)
							  for (Blocked=FALSE, i=SquareIndex-10; GetX(i)>=1 && !Blocked; i-=10)
								  if (Square[i]>=EMPTY) {
									  AddMove(List, x, y, GetX(i), y);
									  if (Square[i]!=EMPTY) Blocked=TRUE;
								  } else Blocked=TRUE;
						  if (GetY(SquareIndex)<8)
							  for (Blocked=FALSE, i=SquareIndex+1; GetY(i)<=8 && !Blocked; i++)
								  if (Square[i]>=EMPTY) {
									  AddMove(List, x, y, x, GetY(i));
									  if (Square[i]!=EMPTY) Blocked=TRUE;
								  } else Blocked=TRUE;
						  if (GetY(SquareIndex)>1)
							  for (Blocked=FALSE, i=SquareIndex-1; GetY(i)>=1 && !Blocked; i--)
								  if (Square[i]>=EMPTY) {
									 AddMove(List, x, y, x, GetY(i));
									 if (Square[i]!=EMPTY) Blocked=TRUE;
								  } else Blocked=TRUE;
						  if (GetX(SquareIndex)<8 && GetY(SquareIndex)<8)
							  for (Blocked=FALSE, i=SquareIndex+11; GetX(i)<=8 && GetY(i)<=8 && !Blocked; i+=11)
								  if (Square[i]>=EMPTY) {
									  AddMove(List, x, y, GetX(i), GetY(i));
									  if (Square[i]!=EMPTY) Blocked=TRUE;
								  } else Blocked=TRUE;
						  if (GetX(SquareIndex)>1 && GetY(SquareIndex)>1)
							  for (Blocked=FALSE, i=SquareIndex-11; GetX(i)>=1 && GetY(i)>=1 && !Blocked; i-=11)
								  if (Square[i]>=EMPTY) {
									  AddMove(List, x, y, GetX(i), GetY(i));
									  if (Square[i]!=EMPTY) Blocked=TRUE;
								  } else Blocked=TRUE;
						  if (GetX(SquareIndex)>1 && GetY(SquareIndex)<8)
							  for (Blocked=FALSE, i=SquareIndex-9; GetX(i)>=1 && GetY(i)<=8 && !Blocked; i-=9)
								  if (Square[i]>=EMPTY) {
									  AddMove(List, x, y, GetX(i), GetY(i));
									  if (Square[i]!=EMPTY) Blocked=TRUE;
								  } else Blocked=TRUE;
						  if (GetX(SquareIndex)<8 && GetY(SquareIndex)>1)
							  for (Blocked=FALSE, i=SquareIndex+9; GetX(i)<=8 && GetY(i)>=1 && !Blocked; i+=9)
								  if (Square[i]>=EMPTY) {
									 AddMove(List, x, y, GetX(i), GetY(i));
									 if (Square[i]!=EMPTY) Blocked=TRUE;
								  } else Blocked=TRUE;

							break;
			 case BQ :  if (Square[MOVER]==WHITE) break;
							if (GetX(SquareIndex)<8)
							  for (Blocked=FALSE, i=SquareIndex+10; GetX(i)<=8 && !Blocked; i+=10)
								  if (Square[i]<=EMPTY) {
									  AddMove(List, x, y, GetX(i), y);
									  if (Square[i]!=EMPTY) Blocked=TRUE;
								  } else Blocked=TRUE;
						  if (GetX(SquareIndex)>1)
							  for (Blocked=FALSE, i=SquareIndex-10; GetX(i)>=1 && !Blocked; i-=10)
								  if (Square[i]<=EMPTY) {
									  AddMove(List, x, y, GetX(i), y);
									  if (Square[i]!=EMPTY) Blocked=TRUE;
								  } else Blocked=TRUE;
						  if (GetY(SquareIndex)<8)
							  for (Blocked=FALSE, i=SquareIndex+1; GetY(i)<=8 && !Blocked; i++)
								  if (Square[i]<=EMPTY) {
									 AddMove(List, x, y, x, GetY(i));
									 if (Square[i]!=EMPTY) Blocked=TRUE;
								  } else Blocked=TRUE;
						  if (GetY(SquareIndex)>1)
							  for (Blocked=FALSE, i=SquareIndex-1; GetY(i)>=1 && !Blocked; i--)
								  if (Square[i]<=EMPTY) {
									 AddMove(List, x, y, x, GetY(i));
									 if (Square[i]!=EMPTY) Blocked=TRUE;
								  } else Blocked=TRUE;
						  if (GetX(SquareIndex)<8 && GetY(SquareIndex)<8)
							  for (Blocked=FALSE, i=SquareIndex+11; GetX(i)<=8 && GetY(i)<=8 && !Blocked; i+=11)
								  if (Square[i]<=EMPTY) {
									  AddMove(List, x, y, GetX(i), GetY(i));
									  if (Square[i]!=EMPTY) Blocked=TRUE;
								  } else Blocked=TRUE;
						  if (GetX(SquareIndex)>1 && GetY(SquareIndex)>1)
							  for (Blocked=FALSE, i=SquareIndex-11; GetX(i)>=1 && GetY(i)>=1 && !Blocked; i-=11)
								  if (Square[i]<=EMPTY) {
									  AddMove(List, x, y, GetX(i), GetY(i));
									  if (Square[i]!=EMPTY) Blocked=TRUE;
								  } else Blocked=TRUE;
						  if (GetX(SquareIndex)>1 && GetY(SquareIndex)<8)
							  for (Blocked=FALSE, i=SquareIndex-9; GetX(i)>=1 && GetY(i)<=8 && !Blocked; i-=9)
								  if (Square[i]<=EMPTY) {
									  AddMove(List, x, y, GetX(i), GetY(i));
									  if (Square[i]!=EMPTY) Blocked=TRUE;
								  } else Blocked=TRUE;
						  if (GetX(SquareIndex)<8 && GetY(SquareIndex)>1)
							  for (Blocked=FALSE, i=SquareIndex+9; GetX(i)<=8 && GetY(i)>=1 && !Blocked; i+=9)
								  if (Square[i]<=EMPTY) {
									 AddMove(List, x, y, GetX(i), GetY(i));
									 if (Square[i]!=EMPTY) Blocked=TRUE;
								  } else Blocked=TRUE;
						  break;
		 }
	}
	List.Amount=List.Moves.GetItemsInContainer();
	return List.Amount;
}

int
TKingletBoard::GameStatus()
{
  int legal;
  TMoveList List;
  if (Resigned!=NOCOLOUR) return 11;
  if (DrawAgreed) return 12;
  if (ThirdRepetition()) return 4;
  legal=(GetMoveList(List)!=0);
  if (!legal) {
	 int x, y;
	 int wc=0;
	 int bc=0;
	 for (x=1; x<=8; x++)
	  for (y=1; y<=8; y++) {
		  if (Square[x*10+y]==WP) wc++;
		  if (Square[x*10+y]==BP) bc++;
	  }
	 if (wc==0) return 6; else
	 if (bc==0) return 7; else
    return 3;
  }
  return 0;
}

void
TKingletBoard::MakeMove(TMove Move, int Store)
{
	Move.PromotionPiece=KING;
	TChessBoard::MakeMove(Move, Store);
}

int
TKingletBoard::VerifyMove(TMove Move)
{
	int i, Piece, diff;
	int From, To;
	TMoveList List;
	From=Move.From;
	To=Move.To;
	if (From/10<1 || From/10>8 || From%10<1 || From%10>8) return 299;
	if (To/10<1 || To/10>8 || To%10<1 || To%10>8) return 300;
	Piece=Square[From];
	if (From==To) return 299;
	if (Piece<EMPTY && Square[MOVER]==BLACK) return 299;
	if (Piece>EMPTY && Square[MOVER]==WHITE) return 299;
	if (Piece==EMPTY) return 299;
	if (Square[MOVER]==WHITE && Square[To]<EMPTY) return 299;
	if (Square[MOVER]==BLACK && Square[To]>EMPTY) return 299;
	switch(Piece)
	{
	  case WK : if ((abs(GetX(To)-GetX(From))>1 || abs(GetY(To)-GetY(From))>1) && !(From==51 && (To==71 || To==31))) return 1;
					if (From==51 && To==71)
					{
					  if (Square[61]!=EMPTY || Square[71]!=EMPTY) return 13;
					  if (Square[WROOK8MOVED]==TRUE) return 3;
					  if (Square[WKINGMOVED]==TRUE) return 4;
					  Square[61]=WK; Square[51]=EMPTY;
					  Square[51]=WK; Square[61]=EMPTY;
					}
					if (From==51 && To==31)
					{
					  if (Square[41]!=EMPTY || Square[31]!=EMPTY || Square[21]!=EMPTY) return 13;
					  if (Square[WROOK1MOVED]==TRUE) return 3;
					  if (Square[WKINGMOVED]==TRUE) return 4;
					  Square[41]=WK; Square[51]=EMPTY;
					  Square[51]=WK; Square[41]=EMPTY;
					}
					break;
	  case BK : if ((abs(GetX(To)-GetX(From))>1 || abs(GetY(To)-GetY(From))>1) && !(From==58 && (To==78 || To==38))) return 1;
					if (From==58 && To==78)
					{
					  if (Square[68]!=EMPTY || Square[78]!=EMPTY) return 13;
					  if (Square[BROOK8MOVED]==TRUE) return 3;
					  if (Square[BKINGMOVED]==TRUE) return 4;
					  Square[68]=BK; Square[58]=EMPTY;
					  Square[58]=BK; Square[68]=EMPTY;
					}
					if (From==58 && To==38)
					{
					  if (Square[48]!=EMPTY || Square[38]!=EMPTY || Square[28]!=EMPTY) return 13;
					  if (Square[BROOK1MOVED]==TRUE) return 3;
					  if (Square[BKINGMOVED]==TRUE) return 4;
					  Square[48]=BK; Square[58]=EMPTY;
					  Square[58]=BK; Square[48]=EMPTY;
					}
					break;
	  case WR : if (GetX(From)!=GetX(To) && GetY(From)!=GetY(To)) return 6;
					if (GetX(From)==GetX(To)) {
					  if (GetY(To)>GetY(From)) {
						  for (i=From+1; i<=To; i++) if (Square[i]<10) return 14; else if (Square[i]>100 && i!=To) return 14;
					  } else {
						  for (i=From-1; i>=To; i--) if (Square[i]<10) return 14; else if (Square[i]>100 && i!=To) return 14;
					  }
					} else {
					  if (GetX(To)>GetX(From)) {
						  for (i=From+10; i<=To; i+=10) if (Square[i]<10) return 14; else if (Square[i]>100 && i!=To) return 14;
					  } else {
						  for (i=From-10; i>=To; i-=10) if (Square[i]<10) return 14; else if (Square[i]>100 && i!=To) return 14;
					  }
					}
					break;
	  case BR : if (GetX(From)!=GetX(To) && GetY(From)!=GetY(To)) return 6;
					if (GetX(From)==GetX(To)) {
					  if (GetY(To)>GetY(From)) {
						  for (i=From+1; i<=To; i++) if (Square[i]>100) return 14; else if (Square[i]<10 && i!=To) return 14;
					  } else {
						  for (i=From-1; i>=To; i--) if (Square[i]>100) return 14; else if (Square[i]<10 && i!=To) return 14;
					  }
					} else {
					  if (GetX(To)>GetX(From)) {
						  for (i=From+10; i<=To; i+=10) if (Square[i]>100) return 14; else if (Square[i]<10 && i!=To) return 14;
					  } else {
						  for (i=From-10; i>=To; i-=10) if (Square[i]>100) return 14; else if (Square[i]<10 && i!=To) return 14;
					  }
					}
					break;
	  case WQ : if (!
						  (
							(From>To && (From-To)%9==0 && GetY(To)>GetY(From)) ||
							(From>To && (From-To)%11==0 && GetY(From)>GetY(To)) ||
							(To>From && (To-From)%11==0 && GetY(To)>GetY(From)) ||
							(To>From && (To-From)%9==0 && GetY(From)>GetY(To)) ||
							(GetX(From)==GetX(To)) ||
							(GetY(From)==GetY(To))
						  )
						) return 7;
					if (GetX(From)==GetX(To) && To>From) for (i=From+1; i<=To; i++) if (Square[i]<10 || (Square[i]>100 && i!=To)) return 20;
					if (GetX(From)==GetX(To) && From>To) for (i=From-1; i>=To; i--) if (Square[i]<10 || (Square[i]>100 && i!=To)) return 20;
					if (GetY(From)==GetY(To) && To>From) for (i=From+10; i<=To; i+=10) if (Square[i]<10 || (Square[i]>100 && i!=To)) return 20;
					if (GetY(From)==GetY(To) && From>To) for (i=From-10; i>=To; i-=10) if (Square[i]<10 || (Square[i]>100 && i!=To)) return 20;
					if (From>To && (From-To)%11==0) for (i=From-11; i>=To; i-=11) if (Square[i]<10 || (Square[i]>100 && i!=To)) return 20;
					if (To>From && (To-From)%11==0) for (i=From+11; i<=To; i+=11) if (Square[i]<10 || (Square[i]>100 && i!=To)) return 20;
					if (From>To && (From-To)%9==0) for (i=From-9; i>=To; i-=9) if (Square[i]<10 || (Square[i]>100 && i!=To)) return 20;
					if (To>From && (To-From)%9==0) for (i=From+9; i<=To; i+=9) if (Square[i]<10 || (Square[i]>100 && i!=To)) return 20;
					break;
	  case BQ : if (!
						  (
							(From>To && (From-To)%9==0 && GetY(To)>GetY(From)) ||
							(From>To && (From-To)%11==0 && GetY(From)>GetY(To)) ||
							(To>From && (To-From)%11==0 && GetY(To)>GetY(From)) ||
							(To>From && (To-From)%9==0 && GetY(From)>GetY(To)) ||
							(GetX(From)==GetX(To)) ||
							(GetY(From)==GetY(To))
						  )
						) return 7;
					if (GetX(From)==GetX(To) && To>From) for (i=From+1; i<=To; i++) if (Square[i]>100 || (Square[i]<10 && i!=To)) return 20;
					if (GetX(From)==GetX(To) && From>To) for (i=From-1; i>=To; i--) if (Square[i]>100 || (Square[i]<10 && i!=To)) return 20;
					if (GetY(From)==GetY(To) && To>From) for (i=From+10; i<=To; i+=10) if (Square[i]>100 || (Square[i]<10 && i!=To)) return 20;
					if (GetY(From)==GetY(To) && From>To) for (i=From-10; i>=To; i-=10) if (Square[i]>100 || (Square[i]<10 && i!=To)) return 20;
					if (From>To && (From-To)%11==0) for (i=From-11; i>=To; i-=11) if (Square[i]>100 || (Square[i]<10 && i!=To)) return 20;
					if (To>From && (To-From)%11==0) for (i=From+11; i<=To; i+=11) if (Square[i]>100 || (Square[i]<10 && i!=To)) return 20;
					if (From>To && (From-To)%9==0) for (i=From-9; i>=To; i-=9) if (Square[i]>100 || (Square[i]<10 && i!=To)) return 20;
					if (To>From && (To-From)%9==0) for (i=From+9; i<=To; i+=9) if (Square[i]>100 || (Square[i]<10 && i!=To)) return 20;
					break;
	  case WP : if (GetY(From)>GetY(To)) return 16; // moving backwards?
					// attempting capture?
					if (Square[To]>100)
					  if (To-From!=11 && From-To!=9) return 9; else break;
					// attempting 2 squares?
					if (GetY(To)-GetY(From)==2 && (GetX(To)==GetX(From))) {
						if (GetY(From)!=2) return 10; // not on correct rank
							else if (Square[From+1]!=EMPTY) return 17; // attempted jump
								else break;
					}
					// attempting to move diagonally (if for capture, would not get this far)
					if (GetX(From)!=GetX(To)) {
						 if (To-From==11 || From-To==9) { // No enemy, en-passant perhaps?
								 if (GetX(To)==Square[ENPAWN] && GetY(To)==6) break;
									else return 18;
						 } else
						 return 8;
					} else {  // must now be going straight forward, no capturing and no 2-square attempt
						if (GetY(To)-GetY(From)>1) return 8; else
							if (Square[To]>100) return 11; else // can't capture like this
								break;
					}
	  case BP : if (GetY(From)<GetY(To)) return 16;  // moving backwards?
					// attempting capture?
					if (Square[To]<10)
					  if (From-To!=11 && To-From!=9) return 9; else break;
					// attempting 2 squares?
					if (GetY(From)-GetY(To)==2 && (GetX(To)==GetX(From))) {
						if (GetY(From)!=7) return 10; // not on correct rank
							else if (Square[From-1]!=EMPTY) return 17; // attempted jump
								else break;
					}
					// attempting to move diagonally (if for capture, would not get this far)
					if (GetX(From)!=GetX(To)) {
						 if (From-To==11 || To-From==9) { // No enemy, en-passant perhaps?
								 if (GetX(To)==Square[ENPAWN] && GetY(To)==3) break;
									else return 18;
						 } else
						 return 8;
					} else {  // must now be going straight forward, no capturing and no 2-square attempt
						if (GetY(From)-GetY(To)>1) return 8; else
							if (Square[To]<10) return 11; else // can't capture like this
								break;
					}
	  case WB : if ((To>From) && GetY(To)>GetY(From)) { // North-East
					  if (abs(From-To)%11!=0) return 12;
					  for (i=From+11; i<=To; i+=11) if (Square[i]<10) return 15; else if (Square[i]>100 && i!=To) return 15;
					} else
					if ((To>From) && GetY(To)<GetY(From)) { // South-East
					  if (abs(From-To)%9!=0) return 12;
					  for (i=From+9; i<=To; i+=9) if (Square[i]<10) return 15; else if (Square[i]>100 && i!=To) return 15;
					} else
					if ((To<From) && GetY(To)>GetY(From)) { // North-West
					  if (abs(From-To)%9!=0) return 12;
					  for (i=From-9; i>=To; i-=9) if (Square[i]<10) return 15; else if (Square[i]>100 && i!=To) return 15;
					} else
					if ((To<From) && GetY(To)<GetY(From)) { // South-West
					  if (abs(From-To)%11!=0) return 12;
					  for (i=From-11; i>=To; i-=11) if (Square[i]<10) return 15; else if (Square[i]>100 && i!=To) return 15;
					} else return 12;
					break;
	  case BB : if ((To>From) && GetY(To)>GetY(From)) { // North-East
					  if (abs(From-To)%11!=0) return 12;
					  for (i=From+11; i<=To; i+=11) if (Square[i]>100) return 15; else if (Square[i]<10 && i!=To) return 15;
					} else
					if ((To>From) && GetY(To)<GetY(From)) { // South-East
					  if (abs(From-To)%9!=0) return 12;
					  for (i=From+9; i<=To; i+=9) if (Square[i]>100) return 15; else if (Square[i]<10 && i!=To) return 15;
					} else
					if ((To<From) && GetY(To)>GetY(From)) { // North-West
					  if (abs(From-To)%9!=0) return 12;
					  for (i=From-9; i>=To; i-=9) if (Square[i]>100) return 15; else if (Square[i]<10 && i!=To) return 15;
					} else
					if ((To<From) && GetY(To)<GetY(From)) { // South-West
					  if (abs(From-To)%11!=0) return 12;
					  for (i=From-11; i>=To; i-=11) if (Square[i]>100) return 15; else if (Square[i]<10 && i!=To) return 15;
					} else return 12;
					break;
	  case WN : diff=abs(From-To);
					if (diff!=12 && diff!=8 && diff!=21 && diff!=19) return 5;
					break;
	  case BN : diff=abs(From-To);
					if (diff!=12 && diff!=8 && diff!=21 && diff!=19) return 5;
					break;
	}
	return 0;
} // end of TChessBoard::VerifyMove(TMove)

void
TKingletBoard::VariantName(char* Name)
{
	strcpy(Name, "Kinglet");
}

