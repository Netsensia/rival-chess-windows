#include <owl\radiobut.h>
#include <owl\edit.h>
#include <owl\checkbox.h>
#include <owl\dialog.h>
#include <owl\combobox.h>
#include "newrival.h"

struct TComputerOptionsTransferBuffer
{
  int SearchMethod;
  int Level;
  int TimeHour, TimeMinute, TimeSecond;
  int ChampMoves, ChampHour, ChampMinute, ChampSecond;
  int BaseMinutes, Increment;
  int Library;
  int History, RandomMoveOrder;
  int Quiesce, Contempt;
  int OpeningLearning, NullMove, FailHighReductions;
  int MinResults, WhitePercent, BlackPercent;
  int MinimalWindow;
  int Ponder;
  int NullMoveStopMaterial;
  int HashTableSize;
  int HashReadReduce, HashWriteReduce;
  int PawnPush, Recapture, Check, SingleReply;
  int NullMoveReduce, MaxExtend, Aspire;
};

class TComputerOptionsDialog : public TDialog
{
  public:
	  TComputerOptionsDialog(TWindow*, TComputerOptionsTransferBuffer*);
	  ~TComputerOptionsDialog();
	  void SetupWindow();
	  BOOL CanClose();
  private:
	  char s[7];
	  void WindowEnabler();
	  void CmAdvanced();
	  TComputerOptionsTransferBuffer* buffer;
	  TRadioButton *FT, *TPM, *C, *BI, *I;
	  TComboBox *Level;
	  TCheckBox *Library;
	  TEdit* MinResults, *WhitePercent, *BlackPercent;
	  TEdit *TimeHour, *TimeMinute, *TimeSecond;
	  TEdit *ChampMoves, *ChampHour, *ChampMinute, *ChampSecond;
	  TEdit *BaseMinutes, *Increment;
  DECLARE_RESPONSE_TABLE(TComputerOptionsDialog);
};

class TAdvancedComputerOptionsDialog : public TDialog
{
  public:
	  TAdvancedComputerOptionsDialog(TWindow*, TComputerOptionsTransferBuffer*);
	  ~TAdvancedComputerOptionsDialog();
	  void SetupWindow();
	  BOOL CanClose();
  private:
	  char s[6];
	  TComputerOptionsTransferBuffer* buffer;
	  TCheckBox *OpeningLearning, *NullMove, *MinimalWindow, *Ponder;
	  TCheckBox *Recapture, *Check, *PawnPush, *SingleReply, *History;
     TCheckBox *RandomOrder, *FailHighReductions;
	  TEdit *Quiesce, *Contempt, *HashSize, *NullMoveReduce, *MaxExtend;
	  TEdit *Aspire, *NullMoveStopMaterial, *HashReadReduce, *HashWriteReduce;
};


