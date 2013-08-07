#include <owl\radiobut.h>
#include <owl\dialog.h>

class TPromoteDialog : public TDialog
{
  public:
	  TPromoteDialog(TWindow*, int*);
	  ~TPromoteDialog();
	  void SetupWindow();
	  BOOL CanClose();
  private:
	  int* Info;
	  TRadioButton *Queen, *Rook, *Bishop, *Knight;
};


