#include "chessbrd.h"

class TKingletBoard : public TChessBoard
{
    public:
	  TKingletBoard();
	  virtual void VariantName(char* Name);
     TKingletBoard(int*);
	  int VerifyMove(TMove Move);
	  int GameStatus();
	  void MakeMove(TMove Move, int Store);
     int GetMoveList(TMoveList& List);
	 private:
	  void AddMove(TMoveList& List, int Fx, int Fy, int Tx, int Ty);
};
