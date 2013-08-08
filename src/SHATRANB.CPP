#include "shatranb.h"

TShatranjBoard::TShatranjBoard()
  : TChessBoard()
{
}

TShatranjBoard::TShatranjBoard(int* SetupPosition)
  : TChessBoard(SetupPosition)
{
}

void
TShatranjBoard::AddMove(TMoveList& List, int Fx, int Fy, int Tx, int Ty)
{
	TMove m;
	int MeChecked;
	int YouChecked;
	if (Fx<1 || Fx>8 || Tx<1 || Tx>8 || (Square[Tx*10+Ty]%100)==6)
	  throw((int)-3);
	BackUpBoard(BACKUP);
	m.From=Fx*10+Fy;
	m.To=Tx*10+Ty;
	MakeMove(m, FALSE);
	if (Square[MOVER]==WHITE) Square[MOVER]=BLACK; else Square[MOVER]=WHITE; // Want to see check from point of view of non-Square[MOVER]
	MeChecked=IsCheck();
	if (!MeChecked) {
	  m.Capture=Square[m.To];
	  if (Square[MOVER]==WHITE) Square[MOVER]=BLACK; else Square[MOVER]=WHITE; // Want to see check from point of view of non-Square[MOVER]
	  List.Moves.Add(m);
	}
	BackUpBoard(RESTORE);
}

int
TShatranjBoard::GetMoveList(TMoveList& List)
{
	int x, y, i;
	int Mover;
	int Blocked;
	int SquareIndex;
	List.Moves.Flush();
	Mover=Square[MOVER];
	int Pieces=0;
	for (x=1; x<=8; x++)
	  for (y=1; y<=8; y++) {
		 if (Mover==WHITE && Square[x*10+y]<WK) Pieces++;
		 if (Mover==BLACK && Square[x*10+y]<BK && Square[x*10+y]>EMPTY) Pieces++;
	  }
	if (Pieces==0) return 0;
	for (x=1; x<=8; x++)
	  for (y=1; y<=8; y++) {
		 SquareIndex=x*10+y;
		 switch (Square[SquareIndex]) {
			 case WP : if (Mover==BLACK) break;
						  if (Square[SquareIndex+1]==EMPTY) {
							 AddMove(List, x, y, x, y+1);
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
			 case WQ : if (Mover==BLACK) break;
						  if (x>1 && y>1 && Square[SquareIndex-11]>=EMPTY) AddMove(List, x, y, x-1, y-1);
						  if (x<8 && y<8 && Square[SquareIndex+11]>=EMPTY) AddMove(List, x, y, x+1, y+1);
						  if (x>1 && y<8 && Square[SquareIndex-9]>=EMPTY) AddMove(List, x, y, x-1, y+1);
						  if (x<8 && y>1 && Square[SquareIndex+9]>=EMPTY) AddMove(List, x, y, x+1, y-1);
						  break;
			 case BQ : if (Mover==WHITE) break;
						  if (x>1 && y>1 && Square[SquareIndex-11]<=EMPTY) AddMove(List, x, y, x-1, y-1);
						  if (x<8 && y<8 && Square[SquareIndex+11]<=EMPTY) AddMove(List, x, y, x+1, y+1);
						  if (x>1 && y<8 && Square[SquareIndex-9]<=EMPTY) AddMove(List, x, y, x-1, y+1);
						  if (x<8 && y>1 && Square[SquareIndex+9]<=EMPTY) AddMove(List, x, y, x+1, y-1);
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
						  break;
			 case WB : if (Square[MOVER]==BLACK) break;
						  if (GetX(SquareIndex)<7 && GetY(SquareIndex)<7 && Square[SquareIndex+22]>=EMPTY) AddMove(List, x, y, x+2, y+2);
						  if (GetX(SquareIndex)>2 && GetY(SquareIndex)>2 && Square[SquareIndex-22]>=EMPTY) AddMove(List, x, y, x-2, y-2);
						  if (GetX(SquareIndex)<7 && GetY(SquareIndex)>2 && Square[SquareIndex+18]>=EMPTY) AddMove(List, x, y, x+2, y-2);
						  if (GetX(SquareIndex)>2 && GetY(SquareIndex)<7 && Square[SquareIndex-18]>=EMPTY) AddMove(List, x, y, x-2, y+2);
						  break;
			 case BB : if (Square[MOVER]==WHITE) break;
						  if (GetX(SquareIndex)<7 && GetY(SquareIndex)<7 && Square[SquareIndex+22]<=EMPTY) AddMove(List, x, y, x+2, y+2);
						  if (GetX(SquareIndex)>2 && GetY(SquareIndex)>2 && Square[SquareIndex-22]<=EMPTY) AddMove(List, x, y, x-2, y-2);
						  if (GetX(SquareIndex)<7 && GetY(SquareIndex)>2 && Square[SquareIndex+18]<=EMPTY) AddMove(List, x, y, x+2, y-2);
						  if (GetX(SquareIndex)>2 && GetY(SquareIndex)<7 && Square[SquareIndex-18]<=EMPTY) AddMove(List, x, y, x-2, y+2);
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
		 }
	}
	List.Amount=List.Moves.GetItemsInContainer();
	return List.Amount;
}

int
TShatranjBoard::GameStatus()
{
  int legal;
  TMoveList List;
  if (Resigned!=NOCOLOUR) return 11;
  if (DrawAgreed) return 12;
  if (ThirdRepetition()) return 4;
  legal=(GetMoveList(List)!=0);
  if (!legal) {
		  int Pieces=0;
		  for (int x=1; x<=8; x++)
			 for (int y=1; y<=8; y++) {
				if (Square[MOVER]==WHITE && Square[x*10+y]<WK) Pieces++;
				if (Square[MOVER]==BLACK && Square[x*10+y]<BK && Square[x*10+y]>EMPTY) Pieces++;
			 }
		  if (Pieces==0) return 8+(Square[MOVER]==WHITE); else
		  if (IsCheck()) {
			  return 1+(Square[MOVER]==WHITE);
		  }
		  return 3;
  }
  return 0;
}

void
TShatranjBoard::MakeMove(TMove Move, int Store)
{
	TChessBoard::MakeMove(Move, Store);
}

int
TShatranjBoard::VerifyMove(TMove Move)
{
	int i, Piece, diff;
	int From, To;
	int Checked;
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
	  case WK : if ((abs(GetX(To)-GetX(From))>1 || abs(GetY(To)-GetY(From))>1)) return 1;
					break;
	  case BK : if ((abs(GetX(To)-GetX(From))>1 || abs(GetY(To)-GetY(From))>1)) return 1;
					break;
	  case WQ : if (To-From!=11 && To-From!=-11 && To-From!=9 && To-From!=-9) return 31;
					break;
	  case BQ : if (To-From!=11 && To-From!=-11 && To-From!=9 && To-From!=-9) return 31;
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
	  case WP : if (GetY(From)>GetY(To)) return 16; // moving backwards?
					// attempting capture?
					if (Square[To]>100)
					  if (To-From!=11 && From-To!=9) return 9; else break;
					// attempting to move diagonally (if for capture, would not get this far)
					if (GetX(From)!=GetX(To)) {
						 if (To-From==11 || From-To==9) { // No enemy, en-passant perhaps?
								 if (GetX(To)==Square[ENPAWN] && GetY(To)==6) break;
									else return 18;
						 } else
						 return 8;
					} else {  // must now be going straight forward, no capturing and no 2-square attempt
						if (GetY(To)-GetY(From)>1) return 30; else
							if (Square[To]>100) return 11; else // can't capture like this
								break;
					}
	  case BP : if (GetY(From)<GetY(To)) return 16;  // moving backwards?
					// attempting capture?
					if (Square[To]<10)
					  if (From-To!=11 && To-From!=9) return 9; else break;
					// attempting to move diagonally (if for capture, would not get this far)
					if (GetX(From)!=GetX(To)) {
						 if (From-To==11 || To-From==9) { // No enemy, en-passant perhaps?
								 if (GetX(To)==Square[ENPAWN] && GetY(To)==3) break;
									else return 18;
						 } else
						 return 8;
					} else {  // must now be going straight forward, no capturing and no 2-square attempt
						if (GetY(From)-GetY(To)>1) return 30; else
							if (Square[To]<10) return 11; else // can't capture like this
								break;
					}
	  case WB : if (abs(From-To)!=18 && abs(From-To)!=22) return 32;
					break;
	  case BB : if (abs(From-To)!=18 && abs(From-To)!=22) return 32;
					break;
	  case WN : diff=abs(From-To);
					if (diff!=12 && diff!=8 && diff!=21 && diff!=19) return 5;
					break;
	  case BN : diff=abs(From-To);
					if (diff!=12 && diff!=8 && diff!=21 && diff!=19) return 5;
					break;
	}
// Is the mover in check once this move is made?
check_for_check:
//	BackUpBoard(BACKUP);
	memcpy(BackUpSquare, Square, sizeof(int)*89);
	MakeMove(Move, FALSE);
	if (Square[MOVER]==WHITE) Square[MOVER]=BLACK; else Square[MOVER]=WHITE; // Want to see check from point of view on non-Square[MOVER]
	Checked=IsCheck();
	memcpy(Square, BackUpSquare, sizeof(int)*89);
//	BackUpBoard(RESTORE);
	if (Checked) return 2;
		else return 0;
} // end of TChessBoard::VerifyMove(TMove)

int
TShatranjBoard::IsCheck()
{
	int i;
	int KingSquare;
	BOOL Block;
	if (Square[MOVER]==WHITE)
	{
		for (i=11, KingSquare=0; i<=88; i++)
			if (Square[i]==WK) KingSquare=i;
		if (KingSquare<11 || KingSquare>88) {
			throw((int)-2);
		}
// Attacking Black Knight?
		if (GetX(KingSquare)>2 && GetY(KingSquare)>1 && Square[KingSquare-21]==BN) return TRUE;
		if (GetX(KingSquare)>2 && GetY(KingSquare)<8 && Square[KingSquare-19]==BN) return TRUE;
		if (GetX(KingSquare)>1 && GetY(KingSquare)>2 && Square[KingSquare-12]==BN) return TRUE;
		if (GetX(KingSquare)>1 && GetY(KingSquare)<7 && Square[KingSquare-8]==BN) return TRUE;
		if (GetX(KingSquare)<7 && GetY(KingSquare)<8 && Square[KingSquare+21]==BN) return TRUE;
		if (GetX(KingSquare)<7 && GetY(KingSquare)>1 && Square[KingSquare+19]==BN) return TRUE;
		if (GetX(KingSquare)<8 && GetY(KingSquare)<7 && Square[KingSquare+12]==BN) return TRUE;
		if (GetX(KingSquare)<8 && GetY(KingSquare)>2 && Square[KingSquare+8]==BN) return TRUE;

		int KingX=GetX(KingSquare);
		int KingY=GetY(KingSquare);
		int* KingPointer=Square+KingSquare;
		  if (KingX>2 && KingY<7 && *(KingPointer-18)==BB) goto InCheck;
		  if (KingX<7 && KingY>2 && *(KingPointer+18)==BB) goto InCheck;
		  if (KingX>2 && KingY>2 && *(KingPointer-22)==BB) goto InCheck;
		  if (KingX<7 && KingY<7 && *(KingPointer+22)==BB) goto InCheck;
		  if (KingX<8 && KingY<8 && *(KingPointer+11)==BQ) goto InCheck;
		  if (KingX>1 && KingY>1 && *(KingPointer-11)==BQ) goto InCheck;
		  if (KingX<8 && KingY>1 && *(KingPointer+9)==BQ) goto InCheck;
		  if (KingX>1 && KingY<8 && *(KingPointer-9)==BQ) goto InCheck;

		// Attacking Black Pawn?
		if (KingSquare>=11+9 && Square[KingSquare-9]==BP) return TRUE;
		if (KingSquare<=88-11 && Square[KingSquare+11]==BP) return TRUE;

		for (Block=FALSE, i=KingSquare+1; !Block && GetY(i)<=8; i++) if (Square[i]!=EMPTY) if (Square[i]==BR) return TRUE; else Block=TRUE;
		for (Block=FALSE, i=KingSquare-1; !Block && GetY(i)>=1; i--) if (Square[i]!=EMPTY) if (Square[i]==BR) return TRUE; else Block=TRUE;
		for (Block=FALSE, i=KingSquare+10; !Block && GetX(i)<=8; i+=10) if (Square[i]!=EMPTY) if (Square[i]==BR) return TRUE; else Block=TRUE;
		for (Block=FALSE, i=KingSquare-10; !Block && GetX(i)>=1; i-=10) if (Square[i]!=EMPTY) if (Square[i]==BR) return TRUE; else Block=TRUE;
// Attacking Black King
		if (GetX(KingSquare)>1 && GetY(KingSquare)>1 && Square[KingSquare-11]==BK) return TRUE;
		if (GetX(KingSquare)>1 && GetY(KingSquare)<8 && Square[KingSquare-9]==BK) return TRUE;
		if (GetX(KingSquare)<8 && GetY(KingSquare)>1 && Square[KingSquare+9]==BK) return TRUE;
		if (GetX(KingSquare)<8 && GetY(KingSquare)<8 && Square[KingSquare+11]==BK) return TRUE;
		if (GetX(KingSquare)>1 && Square[KingSquare-10]==BK) return TRUE;
		if (GetY(KingSquare)>1 && Square[KingSquare-1]==BK) return TRUE;
		if (GetX(KingSquare)<8 && Square[KingSquare+10]==BK) return TRUE;
		if (GetY(KingSquare)<8 && Square[KingSquare+1]==BK) return TRUE;
	}
	if (Square[MOVER]==BLACK)
	{
		for (i=11, KingSquare=0; i<=88; i++)
			if (Square[i]==BK) KingSquare=i;
		if (KingSquare<11 || KingSquare>88) {
			throw((int)-2);
		}
// Attacking White Knight?
		if (GetX(KingSquare)>2 && GetY(KingSquare)>1 && Square[KingSquare-21]==WN) return TRUE;
		if (GetX(KingSquare)>2 && GetY(KingSquare)<8 && Square[KingSquare-19]==WN) return TRUE;
		if (GetX(KingSquare)>1 && GetY(KingSquare)>2 && Square[KingSquare-12]==WN) return TRUE;
		if (GetX(KingSquare)>1 && GetY(KingSquare)<7 && Square[KingSquare-8]==WN) return TRUE;
		if (GetX(KingSquare)<7 && GetY(KingSquare)<8 && Square[KingSquare+21]==WN) return TRUE;
		if (GetX(KingSquare)<7 && GetY(KingSquare)>1 && Square[KingSquare+19]==WN) return TRUE;
		if (GetX(KingSquare)<8 && GetY(KingSquare)<7 && Square[KingSquare+12]==WN) return TRUE;
		if (GetX(KingSquare)<8 && GetY(KingSquare)>2 && Square[KingSquare+8]==WN) return TRUE;

		int KingX=GetX(KingSquare);
		int KingY=GetY(KingSquare);
      int* KingPointer=Square+KingSquare;
		if (KingX>2 && KingY<7 && *(KingPointer-18)==WB) goto InCheck;
		if (KingX<7 && KingY>2 && *(KingPointer+18)==WB) goto InCheck;
		if (KingX>2 && KingY>2 && *(KingPointer-22)==WB) goto InCheck;
		if (KingX<7 && KingY<7 && *(KingPointer+22)==WB) goto InCheck;
		if (KingX<8 && KingY<8 && *(KingPointer+11)==WQ) goto InCheck;
		if (KingX>1 && KingY>1 && *(KingPointer-11)==WQ) goto InCheck;
		if (KingX<8 && KingY>1 && *(KingPointer+9)==WQ) goto InCheck;
		if (KingX>1 && KingY<8 && *(KingPointer-9)==WQ) goto InCheck;



// Attacking White Pawn?
		if (KingSquare<=88-9 && Square[KingSquare+9]==WP) return TRUE;
		if (KingSquare>=11+11 && Square[KingSquare-11]==WP) return TRUE;
		for (Block=FALSE, i=KingSquare+1; !Block && GetY(i)<=8; i++) if (Square[i]!=EMPTY) if (Square[i]==WR) return TRUE; else Block=TRUE;
		for (Block=FALSE, i=KingSquare-1; !Block && GetY(i)>=1; i--) if (Square[i]!=EMPTY) if (Square[i]==WR) return TRUE; else Block=TRUE;
		for (Block=FALSE, i=KingSquare+10; !Block && GetX(i)<=8; i+=10) if (Square[i]!=EMPTY) if (Square[i]==WR) return TRUE; else Block=TRUE;
		for (Block=FALSE, i=KingSquare-10; !Block && GetX(i)>=1; i-=10) if (Square[i]!=EMPTY) if (Square[i]==WR) return TRUE; else Block=TRUE;
// Attacking White King
		if (GetX(KingSquare)>1 && GetY(KingSquare)>1 && Square[KingSquare-11]==WK) return TRUE;
		if (GetX(KingSquare)>1 && GetY(KingSquare)<8 && Square[KingSquare-9]==WK) return TRUE;
		if (GetX(KingSquare)<8 && GetY(KingSquare)>1 && Square[KingSquare+9]==WK) return TRUE;
		if (GetX(KingSquare)<8 && GetY(KingSquare)<8 && Square[KingSquare+11]==WK) return TRUE;
		if (GetX(KingSquare)>1 && Square[KingSquare-10]==WK) return TRUE;
		if (GetY(KingSquare)>1 && Square[KingSquare-1]==WK) return TRUE;
		if (GetX(KingSquare)<8 && Square[KingSquare+10]==WK) return TRUE;
		if (GetY(KingSquare)<8 && Square[KingSquare+1]==WK) return TRUE;
	}
	return FALSE;
InCheck:
   return TRUE;
}

void
TShatranjBoard::VariantName(char* Name)
{
	strcpy(Name, "Shatranj");
}

