
#define Case1Start \
	Case1Calls++; \
	timeticker2=GetTickCount();

#define Case1Stop \
	Case1Timer+=(GetTickCount()-timeticker2);

#define Case2Start \
	Case2Calls++; \
	timeticker2=GetTickCount();

#define Case2Stop \
	Case2Timer+=(GetTickCount()-timeticker2);

#define Case3Start \
	Case3Calls++; \
	timeticker2=GetTickCount();

#define Case3Stop \
	Case3Timer+=(GetTickCount()-timeticker2);

#define Case4Start \
	Case4Calls++; \
	timeticker2=GetTickCount();

#define Case4Stop \
	Case4Timer+=(GetTickCount()-timeticker2);

