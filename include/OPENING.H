#include <owl\radiobut.h>
#include <owl\edit.h>
#include <owl\dialog.h>
#include "newrival.h"

struct TOpeningTransferBuffer
{
  int MaxPly, MaxPositions, MinResults, IncludeUnknown, CountGames, WinPercent;
  int WinPercentBlack;
  int QuitOnError;
};

class TOpeningDialog : public TDialog
{
  public:
	  TOpeningDialog(TWindow*, TOpeningTransferBuffer*);
	  ~TOpeningDialog();
	  void SetupWindow();
	  BOOL CanClose();
  private:
	  TOpeningTransferBuffer* buffer;
	  TEdit *MaxPly, *MaxPositions, *MinResults, *WinPercent, *WinPercentBlack;
	  TCheckBox *IncludeUnknown, *CountGames, *QuitOnError;
};


