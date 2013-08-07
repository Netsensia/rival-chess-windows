#include <owl\gdiobjec.h>

void DibXY(char far* Bits, int StartX, int StartY, TDib* Dib,
			  int Detect=FALSE, int ColourOffset=0, int Low=0, int High=0);
void CentreDibXY(char far* Bits, int StartX, int StartY, TDib* Dib,
			  int Detect=FALSE, int ColourOffset=0, int Low=0, int High=0);
void FadeXY(char far* Bits, int StartX, int StartY, TDib* Dib,
			  int Detect=FALSE, int ColourOffset=0, int Low=0, int High=0);
void GrabSprite(char far* Bits, long StartX, long StartY, TDib* Dib);
