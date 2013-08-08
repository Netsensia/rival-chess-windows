#include <stdio.h>
#include <stdlib.h>
#include "setupmet.h"
#include "newrival.rh"

TSetupMethodDialog::TSetupMethodDialog(TWindow* parent, TSetupMethodTransferBuffer* transferbuffer)
  : TDialog(parent, IDC_SETUPMETHOD)
{
	Visual=new TRadioButton(this, IDC_VISUAL);
	Fen=new TRadioButton(this, IDC_FEN);
	FenEdit=new TEdit(this, IDC_FENEDIT);
	buffer=transferbuffer;
}

DEFINE_RESPONSE_TABLE1(TSetupMethodDialog, TDialog)
	EV_COMMAND(IDC_VISUAL, WindowEnabler),
	EV_COMMAND(IDC_FEN, WindowEnabler),
END_RESPONSE_TABLE;

void
TSetupMethodDialog::SetupWindow()
{
	TDialog::SetupWindow();
	Visual->SetCheck(BF_UNCHECKED);
	Fen->SetCheck(BF_CHECKED);
	FenEdit->SetText(buffer->FenPos);
	WindowEnabler();
}

void
TSetupMethodDialog::WindowEnabler()
{
	FenEdit->EnableWindow(Fen->GetCheck()==BF_CHECKED);
}

BOOL
TSetupMethodDialog::CanClose()
{
	TDialog::CanClose();
	if (Visual->GetCheck()==BF_CHECKED) {
		buffer->Method=VISUALMETHOD;
	} else {
   	buffer->Method=FENMETHOD;
	}
	FenEdit->GetText(buffer->FenPos, MAXFEN-1);
	return TRUE;
}

TSetupMethodDialog::~TSetupMethodDialog()
{
	delete Visual;
	delete Fen;
	delete FenEdit;
}

