#include "chessbrd.h"

class TShatranjBoard : public TChessBoard
{
	 public:
	  TShatranjBoard();
     TShatranjBoard(int*);
	  virtual void VariantName(char* Name);
	  int VerifyMove(TMove Move);
	  int GameStatus();
	  void MakeMove(TMove Move, int Store);
	  int GetMoveList(TMoveList& List);
     int IsCheck();
	 private:
	  void AddMove(TMoveList& List, int Fx, int Fy, int Tx, int Ty);
};
