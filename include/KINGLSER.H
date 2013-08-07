#include "searcher.h"
#include "searchdf.h"

class TKingletSearcher : public TSearcher
{
	 public:
		TKingletSearcher(int);
		virtual void SetInitialPosition(TChessBoard);
		virtual void SetEvaluationParameters(int*);
	 protected:
		virtual int _pascal GetMoveList(TPosition, TMoveArray);
		virtual int _pascal GetQuickMoveList(TPosition, TMoveArray);
		virtual int WinnerWhenNoMoves(TPosition);
		virtual inline int pascal IsCheck(TPosition);
		//virtual int _pascal MakeMove(TPosition, TMove&, TPosition);
		virtual int pascal Evaluate(TPosition, int, int);
	 private:
		int Evalues[12];
		TPosition GlobalPosition;  // used as global when generating moves
		int Amount; // ditto
		TMove TempMove;
};

