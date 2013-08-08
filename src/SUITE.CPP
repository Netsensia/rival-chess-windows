#include <stdio.h>
#include <stdlib.h>
#include "suite.h"
#include "resource.h"

TTestSuiteDialog::TTestSuiteDialog(TWindow* parent, TTestSuiteTransferBuffer* transferbuffer)
  : TDialog(parent, IDC_TESTSUITE)
{
	Time=new TEdit(this, IDC_SUITETIME);
	Depth=new TEdit(this, IDC_SUITEDEPTH);
	StartDepth=new TEdit(this, IDC_SUITESTARTDEPTH);
	ExitWhenFound=new TCheckBox(this, IDC_EXITWHENFOUND);
	buffer=transferbuffer;
}

void
TTestSuiteDialog::SetupWindow()
{
	TDialog::SetupWindow();
	char s[10];
	sprintf(s, "%i", buffer->Time);
	Time->SetText(s);
	sprintf(s, "%i", buffer->Depth);
	Depth->SetText(s);
	sprintf(s, "%i", buffer->StartDepth);
	StartDepth->SetText(s);
	ExitWhenFound->SetCheck(buffer->ExitWhenFound);
}

BOOL
TTestSuiteDialog::CanClose()
{
	TDialog::CanClose();
	char s[10];
	Time->GetText(s, 9);
	buffer->Time=atoi(s);
	Depth->GetText(s, 3);
	buffer->Depth=atoi(s);
	StartDepth->GetText(s, 3);
	buffer->StartDepth=atoi(s);
	buffer->ExitWhenFound=(ExitWhenFound->GetCheck()==BF_CHECKED);
	return TRUE;
}

TTestSuiteDialog::~TTestSuiteDialog()
{
	delete Time;
	delete Depth;
   delete StartDepth;
	delete ExitWhenFound;
}

