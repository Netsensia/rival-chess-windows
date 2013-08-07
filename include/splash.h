#include <owl\radiobut.h>
#include <owl\dialog.h>

class TSplashDialog : public TDialog
{
 public:
  TSplashDialog(TWindow* wnd,
		TResId res = 1,
		TModule* mod = 0);

  ~TSplashDialog( );

 protected:
  void SetupWindow();

  void EvLButtonDown(UINT, TPoint&);

  void EvRButtonDown(UINT, TPoint&);

  void EvTimer(UINT id);

  DECLARE_RESPONSE_TABLE(TSplashDialog);
};

