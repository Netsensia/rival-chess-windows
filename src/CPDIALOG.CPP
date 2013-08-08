#include <stdio.h>
#include <stdlib.h>
#include "cpdialog.h"
#include "resource.h"

TComputerOptionsDialog::TComputerOptionsDialog(TWindow* parent, TComputerOptionsTransferBuffer* transferbuffer)
  : TDialog(parent, COMPUTER_OPTIONS_DIALOG)
{
	FT=new TRadioButton(this, IDC_FIXEDLEVEL);
	TPM=new TRadioButton(this, IDC_TIMEPERMOVE);
	C=new TRadioButton(this, IDC_CHAMPIONSHIP);
	BI=new TRadioButton(this, IDC_BASEINCREMENT);
	I=new TRadioButton(this, IDC_INFINITE);
	Level=new TComboBox(this, IDC_LEVEL);
	MinResults=new TEdit(this, IDC_MINRESULTS);
	WhitePercent=new TEdit(this, IDC_WHITEPERCENT);
	BlackPercent=new TEdit(this, IDC_BLACKPERCENT);
	TimeHour=new TEdit(this, IDC_TIMEHOUR);
	TimeMinute=new TEdit(this, IDC_TIMEMINUTE);
	TimeSecond=new TEdit(this, IDC_TIMESECONDS);
	ChampMoves=new TEdit(this, IDC_CHAMPMOVES);
	ChampHour=new TEdit(this, IDC_CHAMPHOUR);
	ChampMinute=new TEdit(this, IDC_CHAMPMINUTE);
	ChampSecond=new TEdit(this, IDC_CHAMPSECOND);
	BaseMinutes=new TEdit(this, IDC_BASEMINUTES);
	Increment=new TEdit(this, IDC_INCREMENT);
	Library=new TCheckBox(this, IDC_USELIBRARY);
	buffer=transferbuffer;
}

DEFINE_RESPONSE_TABLE1(TComputerOptionsDialog, TDialog)
	EV_COMMAND(IDC_INFINITE, WindowEnabler),
	EV_COMMAND(IDC_FIXEDLEVEL, WindowEnabler),
	EV_COMMAND(IDC_TIMEPERMOVE, WindowEnabler),
	EV_COMMAND(IDC_CHAMPIONSHIP, WindowEnabler),
	EV_COMMAND(IDC_BASEINCREMENT, WindowEnabler),
	EV_COMMAND(IDC_USELIBRARY, WindowEnabler),
	EV_COMMAND(IDC_ADVANCED_BUTTON, CmAdvanced),
END_RESPONSE_TABLE;

void
TComputerOptionsDialog::SetupWindow()
{
	TDialog::SetupWindow();
	FT->SetCheck(buffer->SearchMethod==LEVEL ? BF_CHECKED : BF_UNCHECKED);
	TPM->SetCheck(buffer->SearchMethod==TIMEPERMOVE ? BF_CHECKED : BF_UNCHECKED);
	C->SetCheck(buffer->SearchMethod==CHAMPIONSHIP ? BF_CHECKED : BF_UNCHECKED);
	BI->SetCheck(buffer->SearchMethod==BASEINCREMENT ? BF_CHECKED : BF_UNCHECKED);
	I->SetCheck(buffer->SearchMethod==INFINITETIME ? BF_CHECKED : BF_UNCHECKED);
	Level->ClearList();
	Level->InsertString("Ply 1", -1);
	Level->InsertString("Ply 2", -1);
	Level->InsertString("Ply 3", -1);
	Level->InsertString("Ply 4", -1);
	Level->InsertString("Ply 5", -1);
	Level->InsertString("Ply 6", -1);
	Level->InsertString("Ply 7", -1);
	Level->InsertString("Ply 8", -1);
	Level->SetSelIndex(buffer->Level-1);
	sprintf(s, "%i", buffer->MinResults);	MinResults->SetText(s);
	sprintf(s, "%i", buffer->WhitePercent); WhitePercent->SetText(s);
	sprintf(s, "%i", buffer->BlackPercent); BlackPercent->SetText(s);
	sprintf(s, "%02i", buffer->TimeHour);	   TimeHour->SetText(s);
	sprintf(s, "%02i", buffer->TimeMinute);	TimeMinute->SetText(s);
	sprintf(s, "%02i", buffer->TimeSecond);	TimeSecond->SetText(s);
	sprintf(s, "%02i", buffer->ChampMoves);	ChampMoves->SetText(s);
	sprintf(s, "%02i", buffer->ChampHour);	   ChampHour->SetText(s);
	sprintf(s, "%02i", buffer->ChampMinute);	ChampMinute->SetText(s);
	sprintf(s, "%02i", buffer->ChampSecond);	ChampSecond->SetText(s);
	sprintf(s, "%02i", buffer->BaseMinutes);	BaseMinutes->SetText(s);
	sprintf(s, "%02i", buffer->Increment);		Increment->SetText(s);
	Library->SetCheck(buffer->Library);
	WindowEnabler();
}

void
TComputerOptionsDialog::WindowEnabler()
{
	Level->EnableWindow(FT->GetCheck()==BF_CHECKED);
	TimeHour->EnableWindow(TPM->GetCheck()==BF_CHECKED);
	TimeMinute->EnableWindow(TPM->GetCheck()==BF_CHECKED);
	TimeSecond->EnableWindow(TPM->GetCheck()==BF_CHECKED);
	ChampMoves->EnableWindow(C->GetCheck()==BF_CHECKED);
	ChampHour->EnableWindow(C->GetCheck()==BF_CHECKED);
	ChampMinute->EnableWindow(C->GetCheck()==BF_CHECKED);
	ChampSecond->EnableWindow(C->GetCheck()==BF_CHECKED);
	BaseMinutes->EnableWindow(BI->GetCheck()==BF_CHECKED);
	Increment->EnableWindow(BI->GetCheck()==BF_CHECKED);
	MinResults->EnableWindow(Library->GetCheck()==BF_CHECKED);
	WhitePercent->EnableWindow(Library->GetCheck()==BF_CHECKED);
	BlackPercent->EnableWindow(Library->GetCheck()==BF_CHECKED);
}

void
TComputerOptionsDialog::CmAdvanced()
{
	int a=buffer->Quiesce;
	int b=buffer->Contempt;
	int c=buffer->OpeningLearning;
	int d=buffer->NullMove;
	int e=buffer->Ponder;
	int f=buffer->FailHighReductions;
	TAdvancedComputerOptionsDialog* cpadialog=new TAdvancedComputerOptionsDialog(this, buffer);
	if (cpadialog->Execute()!=IDOK) {
		buffer->Quiesce=a;
		buffer->Contempt=b;
		buffer->OpeningLearning=c;
		buffer->NullMove=d;
		buffer->Ponder=e;
      buffer->FailHighReductions=f;
	}
	delete cpadialog;
}

BOOL
TComputerOptionsDialog::CanClose()
{
	TDialog::CanClose();
	if (FT->GetCheck()==BF_CHECKED) buffer->SearchMethod=LEVEL;
	if (TPM->GetCheck()==BF_CHECKED) buffer->SearchMethod=TIMEPERMOVE;
	if (C->GetCheck()==BF_CHECKED) buffer->SearchMethod=CHAMPIONSHIP;
	if (BI->GetCheck()==BF_CHECKED) buffer->SearchMethod=BASEINCREMENT;
	if (I->GetCheck()==BF_CHECKED) buffer->SearchMethod=INFINITETIME;
	buffer->Level=Level->GetSelIndex()+1;
	TimeHour->GetText(s, 3);
	buffer->TimeHour=atoi(s);
	TimeMinute->GetText(s, 3);
	buffer->TimeMinute=atoi(s);
	TimeSecond->GetText(s, 3);
	buffer->TimeSecond=atoi(s);
	ChampMoves->GetText(s, 3);
	buffer->ChampMoves=atoi(s);
	ChampHour->GetText(s, 3);
	buffer->ChampHour=atoi(s);
	ChampMinute->GetText(s, 3);
	buffer->ChampMinute=atoi(s);
	ChampSecond->GetText(s, 3);
	buffer->ChampSecond=atoi(s);
	BaseMinutes->GetText(s, 3);
	buffer->BaseMinutes=atoi(s);
	Increment->GetText(s, 3);
	buffer->Increment=atoi(s);
	buffer->Library=Library->GetCheck();
	MinResults->GetText(s, 5);
	buffer->MinResults=atoi(s);
	WhitePercent->GetText(s, 3);
	buffer->WhitePercent=atoi(s);
	BlackPercent->GetText(s, 3);
	buffer->BlackPercent=atoi(s);
	return TRUE;
}

TComputerOptionsDialog::~TComputerOptionsDialog()
{
	delete FT;
	delete TPM;
	delete C;
   delete Level;
	delete TimeHour;
	delete TimeMinute;
	delete TimeSecond;
	delete ChampMoves;
	delete ChampHour;
	delete ChampMinute;
	delete ChampSecond;
	delete MinResults;
	delete WhitePercent;
	delete BlackPercent;
}

TAdvancedComputerOptionsDialog::TAdvancedComputerOptionsDialog(TWindow* parent, TComputerOptionsTransferBuffer* transferbuffer)
  : TDialog(parent, ADVANCED_COMPUTER_OPTIONS_DIALOG)
{
	HashReadReduce=new TEdit(this, IDC_READHASH);
	HashWriteReduce=new TEdit(this, IDC_WRITEHASH);
	NullMoveStopMaterial=new TEdit(this, IDC_NULLMOVEPIECES);
	Quiesce=new TEdit(this, IDC_QUIESCE);
	HashSize=new TEdit(this, IDC_HASHMEGS);
	Contempt=new TEdit(this, IDC_CONTEMPT);
	NullMoveReduce=new TEdit(this, IDC_NULLMOVEREDUCE);
	Aspire=new TEdit(this, IDC_ASPIRATION);
	MaxExtend=new TEdit(this, IDC_MAXEXTEND);
	OpeningLearning=new TCheckBox(this, IDC_TRANSREF);
	NullMove=new TCheckBox(this, IDC_NULLMOVE);
	FailHighReductions=new TCheckBox(this, IDC_FAILHIGHREDUCTIONS);
	Ponder=new TCheckBox(this, IDC_PONDER);
	MinimalWindow=new TCheckBox(this, IDC_MINWIN);
	Recapture=new TCheckBox(this, IDC_RECAPTUREEXT);
	Check=new TCheckBox(this, IDC_CHECKEXT);
	PawnPush=new TCheckBox(this, IDC_PAWNPUSHEXT);
	SingleReply=new TCheckBox(this, IDC_SINGLEMOVEEXT);
	History=new TCheckBox(this, IDC_USEHISTORY);
	RandomOrder=new TCheckBox(this, IDC_RANDOMORDER);
	buffer=transferbuffer;
}

void
TAdvancedComputerOptionsDialog::SetupWindow()
{
	TDialog::SetupWindow();
	sprintf(s, "%i", buffer->HashReadReduce);
	HashReadReduce->SetText(s);
	sprintf(s, "%i", buffer->HashWriteReduce);
	HashWriteReduce->SetText(s);
	sprintf(s, "%i", buffer->MaxExtend);
	MaxExtend->SetText(s);
	sprintf(s, "%i", buffer->NullMoveStopMaterial);
	NullMoveStopMaterial->SetText(s);
	sprintf(s, "%i", buffer->Aspire);
	Aspire->SetText(s);
	sprintf(s, "%i", buffer->Quiesce);
	Quiesce->SetText(s);
	sprintf(s, "%i", buffer->NullMoveReduce);
	NullMoveReduce->SetText(s);
	sprintf(s, "%i", buffer->Contempt);
	Contempt->SetText(s);
	sprintf(s, "%i", buffer->HashTableSize);
	HashSize->SetText(s);
	OpeningLearning->SetCheck(buffer->OpeningLearning);
	NullMove->SetCheck(buffer->NullMove);
	FailHighReductions->SetCheck(buffer->FailHighReductions);
	Recapture->SetCheck(buffer->Recapture);
	Check->SetCheck(buffer->Check);
	History->SetCheck(buffer->History);
	RandomOrder->SetCheck(buffer->RandomMoveOrder);
	SingleReply->SetCheck(buffer->SingleReply);
	PawnPush->SetCheck(buffer->PawnPush);
	MinimalWindow->SetCheck(buffer->MinimalWindow);
	Ponder->SetCheck(buffer->Ponder);
}

BOOL
TAdvancedComputerOptionsDialog::CanClose()
{
	TDialog::CanClose();
	HashReadReduce->GetText(s, 3);
	buffer->HashReadReduce=atoi(s);
	HashWriteReduce->GetText(s, 3);
	buffer->HashWriteReduce=atoi(s);
	MaxExtend->GetText(s, 3);
	buffer->MaxExtend=atoi(s);
	NullMoveStopMaterial->GetText(s, 2);
	buffer->NullMoveStopMaterial=atoi(s);
	Aspire->GetText(s, 6);
	buffer->Aspire=atoi(s);
	Quiesce->GetText(s, 3);
	buffer->Quiesce=atoi(s);
	NullMoveReduce->GetText(s, 3);
	buffer->NullMoveReduce=atoi(s);
	Contempt->GetText(s, 5);
	buffer->Contempt=atoi(s);
	buffer->OpeningLearning=OpeningLearning->GetCheck();
	buffer->History=History->GetCheck();
	buffer->RandomMoveOrder=RandomOrder->GetCheck();
	buffer->Check=Check->GetCheck();
	buffer->SingleReply=SingleReply->GetCheck();
	buffer->Recapture=Recapture->GetCheck();
	buffer->PawnPush=PawnPush->GetCheck();
	buffer->NullMove=NullMove->GetCheck();
	buffer->FailHighReductions=FailHighReductions->GetCheck();
	buffer->MinimalWindow=MinimalWindow->GetCheck();
	buffer->Ponder=Ponder->GetCheck();
	HashSize->GetText(s, 6);
	buffer->HashTableSize=atoi(s);
	return TRUE;
}

TAdvancedComputerOptionsDialog::~TAdvancedComputerOptionsDialog()
{
	delete NullMoveStopMaterial;
	delete HashReadReduce;
	delete HashWriteReduce;
	delete SingleReply;
	delete MaxExtend;
	delete Aspire;
	delete PawnPush;
	delete Recapture;
	delete Check;
	delete NullMoveReduce;
	delete Quiesce;
	delete Contempt;
	delete OpeningLearning;
	delete NullMove;
	delete FailHighReductions;
	delete MinimalWindow;
	delete Ponder;
	delete HashSize;
	delete History;
	delete RandomOrder;
}


