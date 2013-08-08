#include <windows.h>
#include <wing.h>
#include "wingstuf.h"
#include <stdlib.h>

HBITMAP ghBitmapMonochrome = 0;

HDC CreateWinGDC(RGBQUAD Colors[], void* far* Bits)
{
	HDC hWinGDC;
	HBITMAP hBitmapNew;
	struct {
		BITMAPINFOHEADER InfoHeader;
		RGBQUAD ColorTable[256];
	} Info;


	// Set up an optimal bitmap
	if (WinGRecommendDIBFormat((BITMAPINFO far *)&Info) == FALSE)
		return 0;

	// Set the width and height of the DIB but preserve the
	// sign of biHeight in case top-down DIBs are faster
	Info.InfoHeader.biHeight *= -HEIGHT;

	Info.InfoHeader.biWidth = WIDTH;

	for (int i=0; i<256; i++) {
		 Info.ColorTable[i].rgbRed=Colors[i].rgbRed;//random(256);
		 Info.ColorTable[i].rgbGreen=Colors[i].rgbGreen;//random(256);
		 Info.ColorTable[i].rgbBlue=Colors[i].rgbBlue;//random(256);
	}
	//*** DON'T FORGET A COLOR TABLE! ***
	//*** COLOR TABLE CODE HERE ***

	// Create a WinGDC and Bitmap, then select away
	hWinGDC = WinGCreateDC();
	if (hWinGDC)
	{
//		if (BitsNum==1)
			hBitmapNew = WinGCreateBitmap(hWinGDC, (BITMAPINFO far *)&Info, Bits);
//			hBitmapNew = WinGCreateBitmap(hWinGDC, (BITMAPINFO far *)&Info, &MapBits);
//		hBitmapNew = LoadBitmap( hInst, MAKEINTRESOURCE(BITMAP_NUMBER) );
//		hBitmapNew = CreateDC(0);
		if (hBitmapNew)
		{
			ghBitmapMonochrome = (HBITMAP)SelectObject(hWinGDC,
				hBitmapNew);
		}
		else
		{
			DeleteDC(hWinGDC);
			hWinGDC = 0;

		}
		CreateIdentityPalette( Info.ColorTable, 256) ;
	}

	return hWinGDC;
}

void Destroy100x100WinGDC(HDC hWinGDC)
{
	HBITMAP hBitmapOld;

	if (hWinGDC && ghBitmapMonochrome)
	{
		// Select the stock 1x1 monochrome bitmap back in
		hBitmapOld = (HBITMAP)SelectObject(hWinGDC, ghBitmapMonochrome);
		DeleteObject(hBitmapOld);
		DeleteDC(hWinGDC);
	}
}

HPALETTE CreateIdentityPalette(RGBQUAD aRGB[], int nColors)
{
	int i;
	struct {
		WORD Version;
		WORD NumberOfEntries;
		PALETTEENTRY aEntries[256];
	} Palette =
	{
		0x300,
		256
	};

	//*** Just use the screen DC where we need it
	HDC hdc = GetDC(NULL);

	//*** For SYSPAL_NOSTATIC, just copy the color table into
	//*** a PALETTEENTRY array and replace the first and last entries
	//*** with black and white
	if (GetSystemPaletteUse(hdc) == SYSPAL_NOSTATIC)

	{
		//*** Fill in the palette with the given values, marking each
		//*** as PC_NOCOLLAPSE
		for(i = 0; i < nColors; i++)
		{
			Palette.aEntries[i].peRed = aRGB[i].rgbRed;
			Palette.aEntries[i].peGreen = aRGB[i].rgbGreen;
			Palette.aEntries[i].peBlue = aRGB[i].rgbBlue;
			Palette.aEntries[i].peFlags = PC_NOCOLLAPSE;
		}

		//*** Mark any unused entries PC_NOCOLLAPSE
		for (; i < 256; ++i)
		{
			Palette.aEntries[i].peFlags = PC_NOCOLLAPSE;

		}

		//*** Make sure the last entry is white
		//*** This may replace an entry in the array!
		Palette.aEntries[255].peRed = 255;
		Palette.aEntries[255].peGreen = 255;
		Palette.aEntries[255].peBlue = 255;
		Palette.aEntries[255].peFlags = 0;

		//*** And the first is black
		//*** This may replace an entry in the array!
		Palette.aEntries[0].peRed = 0;
		Palette.aEntries[0].peGreen = 0;
		Palette.aEntries[0].peBlue = 0;
		Palette.aEntries[0].peFlags = 0;

	}
	else
	//*** For SYSPAL_STATIC, get the twenty static colors into
	//*** the array, then fill in the empty spaces with the
	//*** given color table
	{
		int nStaticColors;
		int nUsableColors;

		//*** Get the static colors from the system palette
		nStaticColors = GetDeviceCaps(hdc, NUMCOLORS);
		GetSystemPaletteEntries(hdc, 0, 256, Palette.aEntries);

		//*** Set the peFlags of the lower static colors to zero
		nStaticColors = nStaticColors / 2;

		for (i=0; i<nStaticColors; i++)
			Palette.aEntries[i].peFlags = 0;

		//*** Fill in the entries from the given color table
		nUsableColors = nColors - nStaticColors;
		for (; i<nUsableColors; i++)
		{
			Palette.aEntries[i].peRed = aRGB[i].rgbRed;
			Palette.aEntries[i].peGreen = aRGB[i].rgbGreen;
			Palette.aEntries[i].peBlue = aRGB[i].rgbBlue;
			Palette.aEntries[i].peFlags = PC_NOCOLLAPSE;
		}

		//*** Mark any empty entries as PC_NOCOLLAPSE

		for (; i<256 - nStaticColors; i++)
			Palette.aEntries[i].peFlags = PC_NOCOLLAPSE;

		//*** Set the peFlags of the upper static colors to zero
		for (i = 256 - nStaticColors; i<256; i++)
			Palette.aEntries[i].peFlags = 0;
	}

	//*** Remember to release the DC!
	ReleaseDC(NULL, hdc);

	//*** Return the palette
	return CreatePalette((LOGPALETTE *)&Palette);
}
