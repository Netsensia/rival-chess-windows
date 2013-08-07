#include <owl\radiobut.h>
#include <owl\edit.h>
#include <owl\dialog.h>
#include "newrival.h"
#include "defines.h"

struct TTestSuiteTransferBuffer
{
  int Time, Depth, StartDepth;
  int ExitWhenFound;
};

class TTestSuiteDialog : public TDialog
{
  public:
	  TTestSuiteDialog(TWindow*, TTestSuiteTransferBuffer*);
	  ~TTestSuiteDialog();
	  void SetupWindow();
	  BOOL CanClose();
  private:
	  TTestSuiteTransferBuffer* buffer;
	  TEdit *Time, *Depth, *StartDepth;
	  TCheckBox *ExitWhenFound;
};


