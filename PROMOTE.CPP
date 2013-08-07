#include "promote.h"
#include "newrival.rh"
#include "chessbrd.h"

TPromoteDialog::TPromoteDialog(TWindow* parent, int* PassInfo)
  : TDialog(parent, PROMOTE_DIALOG)
{
	Queen=new TRadioButton(this, IDC_PROMOTEQUEEN);
	Rook=new TRadioButton(this, IDC_PROMOTEROOK);
	Knight=new TRadioButton(this, IDC_PROMOTEKNIGHT);
	Bishop=new TRadioButton(this, IDC_PROMOTEBISHOP);
	Info=PassInfo;
}

void
TPromoteDialog::SetupWindow()
{
	TDialog::SetupWindow();
	Queen->SetCheck(*Info==QUEEN ? 1 : 0);
	Rook->SetCheck(*Info==ROOK ? 1 : 0);
	Knight->SetCheck(*Info==KNIGHT ? 1 : 0);
	Bishop->SetCheck(*Info==BISHOP ? 1 : 0);
}

BOOL
TPromoteDialog::CanClose()
{
	TDialog::CanClose();
	if (Queen->GetCheck()==BF_CHECKED) *Info=QUEEN;
	if (Rook->GetCheck()==BF_CHECKED) *Info=ROOK;
	if (Knight->GetCheck()==BF_CHECKED) *Info=KNIGHT;
	if (Bishop->GetCheck()==BF_CHECKED) *Info=BISHOP;
	return TRUE;
}

TPromoteDialog::~TPromoteDialog()
{
	delete Queen;
	delete Rook;
	delete Knight;
	delete Bishop;
}


