#include <stdio.h>
#include <stdlib.h>
#include "savepgn.h"
#include "resource.h"

TSavePGNDialog::TSavePGNDialog(TWindow* parent, TSavePGNTransferBuffer* transferbuffer)
  : TDialog(parent, IDC_SAVEPGN)
{
	Event=new TEdit(this, IDC_PGNEVENT);
	Append=new TCheckBox(this, IDC_APPENDPGN);
	buffer=transferbuffer;
}

void
TSavePGNDialog::SetupWindow()
{
	TDialog::SetupWindow();
	Event->SetText(buffer->Event);
	Append->SetCheck(buffer->Append);
}

BOOL
TSavePGNDialog::CanClose()
{
	TDialog::CanClose();
	Event->GetText(buffer->Event, 99);
	buffer->Append=(Append->GetCheck()==BF_CHECKED);
	return TRUE;
}

TSavePGNDialog::~TSavePGNDialog()
{
	delete Event;
	delete Append;
}

