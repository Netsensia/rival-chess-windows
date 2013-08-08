#include <stdio.h>
#include <stdlib.h>
#include "opening.h"
#include "resource.h"

TOpeningDialog::TOpeningDialog(TWindow* parent, TOpeningTransferBuffer* transferbuffer)
  : TDialog(parent, IDC_OPENINGBOOK)
{
	MaxPly=new TEdit(this, IDC_MAXOPENINGPLY);
	MaxPositions=new TEdit(this, IDC_MINOPENINGPERCENT);
	MinResults=new TEdit(this, IDC_MINOPENINGRESULTS);
	WinPercent=new TEdit(this, IDC_WINPERCENT);
	WinPercentBlack=new TEdit(this, IDC_WINPERCENTBLACK);
	IncludeUnknown=new TCheckBox(this, IDC_INCLUDEUNKNOWN);
	CountGames=new TCheckBox(this, IDC_OPENINGCOUNT);
	QuitOnError=new TCheckBox(this, IDC_QUITONERROR);
	buffer=transferbuffer;
}

void
TOpeningDialog::SetupWindow()
{
	TDialog::SetupWindow();
	char s[12];
	sprintf(s, "%i", buffer->MaxPly);
	MaxPly->SetText(s);
	sprintf(s, "%i", buffer->MaxPositions);
	MaxPositions->SetText(s);
	sprintf(s, "%i", buffer->MinResults);
	MinResults->SetText(s);
	sprintf(s, "%i", buffer->WinPercent);
	WinPercent->SetText(s);
	sprintf(s, "%i", buffer->WinPercentBlack);
	WinPercentBlack->SetText(s);
	IncludeUnknown->SetCheck(buffer->IncludeUnknown);
	CountGames->SetCheck(buffer->CountGames);
	QuitOnError->SetCheck(buffer->QuitOnError);
}

BOOL
TOpeningDialog::CanClose()
{
	TDialog::CanClose();
	char s[12];
	MaxPly->GetText(s, 3);
	buffer->MaxPly=atoi(s);
	MaxPositions->GetText(s, 10);
	buffer->MaxPositions=atoi(s);
	MinResults->GetText(s, 4);
	buffer->MinResults=atoi(s);
	WinPercent->GetText(s, 4);
	buffer->WinPercent=atoi(s);
	WinPercentBlack->GetText(s, 4);
	buffer->WinPercentBlack=atoi(s);
	buffer->IncludeUnknown=(IncludeUnknown->GetCheck()==BF_CHECKED);
	buffer->CountGames=(CountGames->GetCheck()==BF_CHECKED);
	buffer->QuitOnError=(QuitOnError->GetCheck()==BF_CHECKED);
	return TRUE;
}

TOpeningDialog::~TOpeningDialog()
{
	delete MaxPly;
	delete MaxPositions;
	delete IncludeUnknown;
	delete MinResults;
	delete CountGames;
	delete WinPercent;
	delete WinPercentBlack;
   delete QuitOnError;
}

