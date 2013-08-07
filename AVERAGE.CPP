#include <stdio.h>

int GenNum;

void
ScanToNextSetOfValues(FILE *f)
{
	int count=0;
	while (count<3) {
      count+=(fgetc(f)=='\n');
	}
}

int
GetNextValue(FILE *f)
{
	int Value;
	char ch;
	do {
	  ch=fgetc(f);
	} while (ch<'0' || ch>'9');
   Value=(ch-48);
	while ((ch=fgetc(f))!=',') {
	  Value*=10;
	  Value+=(ch-48);
	}
   return Value;
}

//#define MEMBERZERO
//#define SINGLETOTALS

#define GENSIZE 10
#define MEMBERSIZE 12

FILE *o;

long
GetGenomeValueTotals(FILE *f)
{
	long GenomeTotal=0;
	int CellValue;
	int i, j;
//	fprintf(o, "\nGeneration %i\n", GenNum);
	fprintf(o, "\n");
	for (j=0; j<GENSIZE; j++) {
	  ScanToNextSetOfValues(f);
	  for (i=0; i<MEMBERSIZE; i++) {
		 CellValue=GetNextValue(f);
		 if (j==0) {
			fprintf(o, "%i\t", CellValue);
			GenomeTotal+=CellValue;
		 }
	  }
	  while (fgetc(f)!='\n');
	}
	return GenomeTotal/GENSIZE;
}

void
main(void)
{
	FILE *f;
	char s[100];
	o=fopen("HELLO", "w");

	for (GenNum=0; GenNum<=60; GenNum++) {
	  sprintf(s, "GAREPO~1.%i", GenNum);
	  if ((f=fopen(s, "r"))!=NULL) {
		 printf("\nGeneration %i, Average total=%li", GenNum, GetGenomeValueTotals(f));
	  }
	  fclose(f);
	}
	fclose(o);
}
