#include "searcher.h"
#include "searchdf.h"

class TShatranjSearcher : public TSearcher
{
	 public:
      TShatranjSearcher(int);
		virtual void SetInitialPosition(TChessBoard);
		virtual void SetEvaluationParameters(int*);
	 protected:
		int UseAllMovesOnQuiesce(TPosition Position);
		virtual int _pascal GetMoveList(TPosition, TMoveArray);
		virtual int _pascal GetQuickMoveList(TPosition, TMoveArray);
		virtual int WinnerWhenNoMoves(TPosition);
		virtual inline int pascal IsCheck(TPosition);
	 private:
		TPosition GlobalPosition;  // used as global when generating moves
		int Amount; // ditto
		TMove TempMove;
};

