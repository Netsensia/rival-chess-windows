#include <owl\dialog.h>

class TCenteredDialog : public TDialog
{
 public:
  TCenteredDialog(TWindow* parent, int resId) :
	 TDialog(parent, resId) {}
  void SetupWindow()
  {
	TDialog::SetupWindow();
	TPoint newOrigin(GetSystemMetrics(SM_CXSCREEN) / 2,
				 GetSystemMetrics(SM_CYSCREEN) / 2);
	TRect tempRect = GetWindowRect();
	newOrigin.x -= tempRect.Width() / 2;
	newOrigin.y -= tempRect.Height() / 2;

	MoveWindow(TRect(newOrigin, tempRect.Size()));
  }
};