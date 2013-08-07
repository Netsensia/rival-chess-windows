#include "searcher.h"
#include "searchdf.h"

class TSelfTakeSearcher : public TSearcher
{
	 public:
		TSelfTakeSearcher(int);
	 protected:
		virtual int _pascal GetMoveList(TPosition, TMoveArray);
		virtual int UseAllMovesOnQuiesce(TPosition);
	 private:
		int Evalues[12];
		TMove DrawMoves[25];
		TMove Ply1DrawMovesPly0[25];
		TMove Ply1DrawMovesPly1[25];
		TPosition GlobalPosition;  // used as global when generating moves
		int Amount; // ditto
		int QueenKingTropism[89][89];
		int KnightKingTropism[89][89];
		int RookKingTropism[89][89];
		BOOL SameDiagonal[89][89];
		TMove TempMove;
};

