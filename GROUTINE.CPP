#include "groutine.h"

void DibXY(char far* Bits, int StartX, int StartY, TDib* Dib,
			  int Detect, int ColourOffset, int Low, int High)
{
// General Purpose Sprite Drawing Routine
// Draws the sprite represented by 'Dib' to the Bitmap starting with 'Bits'
// to the bitmap location with top left 'StartX', 'StartY'
// if Detect is 256 then the top left colour is transarent, else
// if Detect is non-zero then the colour 'Detect' is transparent
// ColourOffset is the number of colours add to any colour meeting the
// criteria 'Low' <= Colour <= 'High'

	int x, y;
	int DibBit;
	char far* Screen=(char far*)Bits;
	int width=Dib->Width();
	int height=Dib->Height();
	void HUGE* DibBits=Dib->GetBits();
	int BackGround;
	if (Detect==256)
		BackGround=*((unsigned char *)DibBits+(0*(width)+0)); else
		BackGround=(Detect==0) ? 1000 : Detect;
	for (y=0; y<height; y++) {
		for (x=0; x<width && StartX+x<640; x++) {
		  DibBit=*((unsigned char *)DibBits+(y*width+x));
		  if (DibBit!=BackGround) {
			  if (ColourOffset!=0 && DibBit>=Low && DibBit<=High) {
					Screen[640*(StartY+height-y-1)+StartX+x]=DibBit+ColourOffset;
			  } else {
					Screen[640*(StartY+height-y-1)+StartX+x]=DibBit;
			  }
		  }
		}
	}
}

void CentreDibXY(char far* Bits, int StartX, int StartY, TDib* Dib,
			  int Detect, int ColourOffset, int Low, int High)
{
// General Purpose Sprite Drawing Routine
// Draws the sprite represented by 'Dib' to the Bitmap starting with 'Bits'
// to the bitmap location with top left 'StartX', 'StartY'
// if Detect is 256 then the top left colour is transarent, else
// if Detect is non-zero then the colour 'Detect' is transparent
// ColourOffset is the number of colours add to any colour meeting the
// criteria 'Low' <= Colour <= 'High'

	int x, y;
	int DibBit;
	char far* Screen=(char far*)Bits;
	int width=Dib->Width();
	int height=Dib->Height();
	long PixelNum;
	void HUGE* DibBits=Dib->GetBits();
	int BackGround;
	if (Detect==256)
		BackGround=*((unsigned char *)DibBits+(0*(width)+0)); else
		BackGround=(Detect==0) ? 1000 : Detect;
	StartX -= width / 2;
	StartY -= width / 2;
	for (y=0; y<height; y++) {
		for (x=0; x<width && StartX+x<640; x++) {
		  DibBit=*((unsigned char *)DibBits+(y*width+x));
		  if (DibBit!=BackGround) {
			  PixelNum = 640*(StartY+height-y-1)+StartX+x;
			  if (PixelNum >= 0 && PixelNum < 640*416) {
				  if (ColourOffset!=0 && DibBit>=Low && DibBit<=High) {
						Screen[PixelNum]=DibBit+ColourOffset;
				  } else {
						Screen[PixelNum]=DibBit;
				  }
			  }
		  }
		}
	}
}

void FadeXY(char far* Bits, int StartX, int StartY, TDib* Dib,
			  int Detect, int ColourOffset, int Low, int High)
{
// Fades any of the bits pointed to by 'Bits' and masked by 'Dib' that
// fall in the range Low-High by ColourOffset amount
	int x, y;
	int DibBit;
	unsigned char far* Screen=(unsigned char far*)Bits;
	int width=Dib->Width();
	int height=Dib->Height();
	void HUGE* DibBits=Dib->GetBits();
	int BackGround;
	int ScreenPixel;
	if (Detect==256)
		BackGround=*((unsigned char *)DibBits+(0*(width)+0)); else
		BackGround=(Detect==0) ? 1000 : Detect;
	for (y=0; y<height; y++) {
		for (x=0; x<width && StartX+x<640; x++) {
		  DibBit=*((unsigned char *)DibBits+(y*width+x));
		  if (DibBit!=BackGround) {
			  ScreenPixel=Screen[640*(StartY+height-y-1)+StartX+x];
			  if (ScreenPixel>=Low && ScreenPixel<=High) {
					ScreenPixel-=ColourOffset;
					if (ScreenPixel<Low) ScreenPixel=Low;
					Screen[640*(StartY+height-y-1)+StartX+x]=ScreenPixel;
			  }
		  }
		}
	}
}

void GrabSprite(char far* Bits, long StartX, long StartY, TDib* Dib)
{
	long x, y;
	char far* Screen=(char far*)Bits;
	long width=Dib->Width();
	long height=Dib->Height();
	void HUGE* DibBits=Dib->GetBits();
	for (y=0; y<height; y++) {
		for (x=0; x<width; x++) {
		  long Offset=((StartY+height-y-1)*640)+StartX+x;
		  *((unsigned char *)DibBits+(y*width+x))=Screen[Offset];
		}
	}
}
