#include "chessbrd.h"

class TSelfTakeBoard : public TChessBoard
{
    public:
	  TSelfTakeBoard();
	  virtual void VariantName(char* Name);
	  TSelfTakeBoard(int*);
	  int VerifyMove(TMove Move);
	  int GetMoveList(TMoveList& List);
	 private:
	  void AddMove(TMoveList& List, int Fx, int Fy, int Tx, int Ty);
};
