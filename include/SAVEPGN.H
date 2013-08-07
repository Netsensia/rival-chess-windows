#include <owl\radiobut.h>
#include <owl\edit.h>
#include <owl\dialog.h>
#include "newrival.h"

struct TSavePGNTransferBuffer
{
  int Append;
  char Event[100];
};

class TSavePGNDialog : public TDialog
{
  public:
	  TSavePGNDialog(TWindow*, TSavePGNTransferBuffer*);
	  ~TSavePGNDialog();
	  void SetupWindow();
	  BOOL CanClose();
  private:
	  TSavePGNTransferBuffer* buffer;
	  TEdit *Event;
	  TCheckBox *Append;
};


