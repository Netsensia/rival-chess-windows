#include <stdio.h>
#include <string.h>
#undef TESTING

int
Power(int x, int y)
{
	int i;
	int res=x;
	for (i=2; i<=y; i++)
		res*=x;
	return res;
}

void
writeMessage(char* fn, char* s)
{
	FILE* f;
	f = fopen(fn, "a");
	fprintf(f, "%s\n", s);
	fclose(f);
}

void
writeMessage(char* fn, char* s, int i)
{
	FILE* f;
	f = fopen(fn, "a");
	char m[100];
	sprintf(m, "%s %i\n", s, i);
	fprintf(f, m);
	fclose(f);
}

void
writeMessage(char* fn, int i)
{
	FILE* f;
	f = fopen(fn, "a");
	char m[100];
	sprintf(m, "%i\n", i);
	fprintf(f, m);
	fclose(f);
}

void
BringWithinRange(int& value, int low, int high)
{
	if (value<low) value=low;
   if (value>high) value=high;
}

void
dncase(char* s)
{
	for (int i=0; i<strlen(s); i++)
		if (s[i]>='A' && s[i]<='Z') s[i]+=('a'-'A');
}

writeLog(char* s)
{
#ifdef TESTING
	FILE* f=fopen("c:\\log.txt", "a");
	fprintf(f, s);
   fprintf(f, "\n");
	fclose(f);
#endif
}

writeLog(int i)
{
#ifdef TESTING
	char s[30];
	sprintf(s, "%i\n", i);
	FILE* f=fopen("c:\\log.txt", "a");
	fprintf(f, s);
	fclose(f);
#endif
}

writeLog(char* s, int i) {
#ifdef TESTING
	FILE *f=fopen("c:\\log.txt", "a");
	char s2[100];
	sprintf(s2, "%s %i\n", s, i);
	fprintf(f, "%s", s2);
	fclose(f);
#endif
}

void
ApplyMinimum(int& value, int min)
{
	if (value<min) value=min;
}

