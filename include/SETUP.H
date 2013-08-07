#include <owl\radiobut.h>
#include <owl\dialog.h>
#include <owl\edit.h>

struct DetailsStruct {
	  int Variant;
     int WhiteToMove;
	  int WKingMoved, BKingMoved;
	  int WRook1Moved, BRook1Moved;
	  int WRook8Moved, BRook8Moved;
	  int EnPassant;
     int FiftyMoves;
};

class TSetupDialog : public TDialog
{
  public:
	  TSetupDialog(TWindow*, DetailsStruct*);
	  ~TSetupDialog();
	  void SetupWindow();
	  BOOL CanClose();
  private:
	  TCheckBox *WhiteToMove;
	  TCheckBox *WKingMoved, *BRook1Moved, *WRook1Moved;
	  TCheckBox *BKingMoved, *WRook8Moved, *BRook8Moved;
	  TEdit *EnPassant, *FiftyMoves;
	  TRadioButton *Chess, *Shatranj, *Kinglet, *SelfTake;
	  DetailsStruct* Details;
};


