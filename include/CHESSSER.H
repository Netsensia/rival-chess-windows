#include "searcher.h"
#include "searchdf.h"

class TChessSearcher : public TSearcher
{
	 public:
      TChessSearcher(int);
	 protected:
		virtual int UseAllMovesOnQuiesce(TPosition);
	 private:
		int KingLine(TPosition Position, int From, int To);
		TPosition GlobalPosition;  // used as global when generating moves
		void EnhanceMoveList(TPosition, TMoveArray, int);
};

