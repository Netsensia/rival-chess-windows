#define WIDTH			640
#define HEIGHT			480

extern "C" {
HDC CreateWinGDC(RGBQUAD Colors[], void* far* Bits);
void Destroy100x100WinGDC(HDC hWinGDC);
HPALETTE CreateIdentityPalette(RGBQUAD aRGB[], int nColors);
}

