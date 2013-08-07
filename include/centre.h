#include <owl\radiobut.h>
#include <owl\dialog.h>

class TCenteredDialog : public TDialog
{
 public:
  TCenteredDialog(TWindow* parent, int resId) :
	 TDialog(parent, resId);
  void SetupWindow();
};

