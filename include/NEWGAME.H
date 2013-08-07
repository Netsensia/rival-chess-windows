#include <owl\radiobut.h>
#include <owl\dialog.h>

class TNewGameDialog : public TDialog
{
  public:
	  TNewGameDialog(TWindow*, int*);
	  ~TNewGameDialog();
	  void SetupWindow();
	  BOOL CanClose();
  private:
	  int* Info;
	  TRadioButton *Chess, *Kinglet, *Shatranj, *SelfTake;
};


