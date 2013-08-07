#include <owl\radiobut.h>
#include <owl\edit.h>
#include <owl\dialog.h>
#include "newrival.h"
#include "defines.h"

#define VISUALMETHOD 0
#define FENMETHOD 1

struct TSetupMethodTransferBuffer
{
  int  Method;
  char FenPos[MAXFEN];
};

class TSetupMethodDialog : public TDialog
{
  public:
	  TSetupMethodDialog(TWindow*, TSetupMethodTransferBuffer*);
	  ~TSetupMethodDialog();
	  void SetupWindow();
	  BOOL CanClose();
  private:
	  void WindowEnabler();
	  TSetupMethodTransferBuffer* buffer;
	  TRadioButton *Visual, *Fen;
	  TEdit *FenEdit;
  DECLARE_RESPONSE_TABLE(TSetupMethodDialog);
};


