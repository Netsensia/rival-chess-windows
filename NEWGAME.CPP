#include "newgame.h"
#include "newrival.rh"

TNewGameDialog::TNewGameDialog(TWindow* parent, int* PassInfo)
  : TDialog(parent, NEWGAMEDIALOG)
{
	Chess=new TRadioButton(this, IDC_REGULAR);
	Kinglet=new TRadioButton(this, IDC_KINGLET);
	Shatranj=new TRadioButton(this, IDC_SHATRANJ);
	SelfTake=new TRadioButton(this, IDC_SELFTAKE);
	Info=PassInfo;
}

void
TNewGameDialog::SetupWindow()
{
	TDialog::SetupWindow();
	Chess->SetCheck(*Info==1 ? 1 : 0);
	Kinglet->SetCheck(*Info==2 ? 1 : 0);
	Shatranj->SetCheck(*Info==3 ? 1 : 0);
	SelfTake->SetCheck(*Info==4 ? 1 : 0);
}

BOOL
TNewGameDialog::CanClose()
{
	TDialog::CanClose();
	if (Chess->GetCheck()==BF_CHECKED) *Info=1;
	if (Kinglet->GetCheck()==BF_CHECKED) *Info=2;
	if (Shatranj->GetCheck()==BF_CHECKED) *Info=3;
	if (SelfTake->GetCheck()==BF_CHECKED) *Info=4;
	return TRUE;
}

TNewGameDialog::~TNewGameDialog()
{
	delete Chess;
	delete Kinglet;
	delete Shatranj;
   delete SelfTake;
}


