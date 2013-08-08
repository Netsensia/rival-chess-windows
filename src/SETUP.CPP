#include "setup.h"
#include "newrival.rh"
#include <stdio.h>

TSetupDialog::TSetupDialog(TWindow* parent, DetailsStruct* PassInfo)
  : TDialog(parent, SETUP_DIALOG)
{
	Chess=new TRadioButton(this, IDC_SETUPREGULAR);
	Kinglet=new TRadioButton(this, IDC_SETUPKINGLET);
	Shatranj=new TRadioButton(this, IDC_SETUPSHATRANJ);
	SelfTake=new TRadioButton(this, IDC_SETUPSELFTAKE);
	WhiteToMove=new TCheckBox(this, IDC_WHITE_TO_MOVE);
	WRook1Moved=new TCheckBox(this, IDC_WHITE_ROOK_1_MOVED);
	WRook8Moved=new TCheckBox(this, IDC_WHITE_ROOK_8_MOVED);
	WKingMoved=new TCheckBox(this, IDC_WHITE_KING_MOVED);
	BRook1Moved=new TCheckBox(this, IDC_BLACK_ROOK_1_MOVED);
	BRook8Moved=new TCheckBox(this, IDC_BLACK_ROOK_8_MOVED);
	BKingMoved=new TCheckBox(this, IDC_BLACK_KING_MOVED);
	EnPassant=new TEdit(this, IDC_ENPASSANT);
	FiftyMoves=new TEdit(this, IDC_FIFTYMOVES);
	Details=PassInfo;
}

void
TSetupDialog::SetupWindow()
{
	TDialog::SetupWindow();
	char s[3];
	Chess->SetCheck(Details->Variant==1 ? 1 : 0);
	Kinglet->SetCheck(Details->Variant==2 ? 1 : 0);
	Shatranj->SetCheck(Details->Variant==3 ? 1 : 0);
	SelfTake->SetCheck(Details->Variant==4 ? 1 : 0);
	WhiteToMove->SetCheck(Details->WhiteToMove);
	WKingMoved->SetCheck(Details->WKingMoved);
	WRook1Moved->SetCheck(Details->WRook1Moved);
	WRook8Moved->SetCheck(Details->WRook8Moved);
	BKingMoved->SetCheck(Details->BKingMoved);
	BRook1Moved->SetCheck(Details->BRook1Moved);
	BRook8Moved->SetCheck(Details->BRook8Moved);
	sprintf(s, "%i", Details->EnPassant);
	EnPassant->SetText(s);
	sprintf(s, "%i", Details->FiftyMoves);
	FiftyMoves->SetText(s);
}

BOOL
TSetupDialog::CanClose()
{
	TDialog::CanClose();
	char s[4];
	if (Chess->GetCheck()==BF_CHECKED) Details->Variant=1;
	if (Kinglet->GetCheck()==BF_CHECKED) Details->Variant=2;
	if (Shatranj->GetCheck()==BF_CHECKED) Details->Variant=3;
	if (SelfTake->GetCheck()==BF_CHECKED) Details->Variant=4;
	Details->WhiteToMove=(WhiteToMove->GetCheck()==BF_CHECKED);
	Details->WKingMoved=(WKingMoved->GetCheck()==BF_CHECKED);
	Details->WRook1Moved=(WRook1Moved->GetCheck()==BF_CHECKED);
	Details->WRook8Moved=(WRook8Moved->GetCheck()==BF_CHECKED);
	Details->BKingMoved=(BKingMoved->GetCheck()==BF_CHECKED);
	Details->BRook1Moved=(BRook1Moved->GetCheck()==BF_CHECKED);
	Details->BRook8Moved=(BRook8Moved->GetCheck()==BF_CHECKED);
	EnPassant->GetText(s, 2);
	Details->EnPassant=atoi(s);
	FiftyMoves->GetText(s, 3);
	Details->FiftyMoves=atoi(s);
	return TRUE;
}

TSetupDialog::~TSetupDialog()
{
	delete Chess;
	delete Kinglet;
	delete Shatranj;
   delete SelfTake;
	delete WKingMoved;
	delete BKingMoved;
	delete WRook1Moved;
	delete BRook1Moved;
	delete WRook8Moved;
	delete BRook8Moved;
	delete WhiteToMove;
	delete EnPassant;
	delete FiftyMoves;
}


