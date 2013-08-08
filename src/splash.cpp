#include <owl\dialog.h>

class TSplashDialog : public TDialog
{
 public:
  TSplashDialog(TWindow* wnd,
		TResId res = 1,
		TModule* mod = 0) :
	 TDialog(wnd, res, mod)
  {}

  ~TSplashDialog( )
  { KillTimer(1);
	 ReleaseCapture( );
	 Destroy( ); }

  BOOL canClose;

 protected:
  void SetupWindow();

  void EvLButtonDown(UINT, TPoint&)
  { CloseWindow( ); }

  void EvRButtonDown(UINT, TPoint&)
  { CloseWindow( ); }

  BOOL CanClose()
  {
	  return canClose;
  }

  void EvTimer(UINT id)
  { TDialog::EvTimer(id);
    canClose = TRUE;
	 CloseWindow( ); }

DECLARE_RESPONSE_TABLE(TSplashDialog);
};

DEFINE_RESPONSE_TABLE1(TSplashDialog, TDialog)
  EV_WM_TIMER,
  EV_WM_LBUTTONDOWN,
  EV_WM_RBUTTONDOWN,
END_RESPONSE_TABLE;

void TSplashDialog::SetupWindow( )
{
  TDialog::SetupWindow( );

  RECT DesktopRect;
  ::GetWindowRect(GetDesktopWindow( ),
		&DesktopRect);

  canClose = FALSE;

  SetWindowPos(HWND_TOPMOST,
	 (DesktopRect.right -
		GetWindowRect( ).Width( )) / 2,
	 (DesktopRect.bottom -
		GetWindowRect( ).Height( )) / 2,
	 0,0, SWP_NOSIZE);

  SetTimer(1, SPLASH_MILLIS, 0);
  SetCapture( );
}
