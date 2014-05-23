#pragma once

#include "cstring.h"

typedef struct tagBITMAP
{
	LONG        bmType;
	LONG        bmWidth;
	LONG        bmHeight;
	LONG        bmWidthBytes;
	WORD        bmPlanes;
	WORD        bmBitsPixel;
	LPVOID      bmBits;
} BITMAP, *PBITMAP, *NPBITMAP, *LPBITMAP;



typedef struct tagBITMAPINFOHEADER{
	DWORD      biSize;
	LONG       biWidth;
	LONG       biHeight;
	WORD       biPlanes;
	WORD       biBitCount;
	DWORD      biCompression;
	DWORD      biSizeImage;
	LONG       biXPelsPerMeter;
	LONG       biYPelsPerMeter;
	DWORD      biClrUsed;
	DWORD      biClrImportant;
} BITMAPINFOHEADER, *LPBITMAPINFOHEADER, *PBITMAPINFOHEADER;

typedef struct tagRGBQUAD {
	BYTE    rgbBlue;
	BYTE    rgbGreen;
	BYTE    rgbRed;
	BYTE    rgbReserved;
} RGBQUAD;

typedef struct tagBITMAPINFO {
	BITMAPINFOHEADER    bmiHeader;
	RGBQUAD             bmiColors[1];
} BITMAPINFO, *LPBITMAPINFO, *PBITMAPINFO;


typedef BITMAPINFOHEADER* HIMAGE;		// 一个指向 

#if defined WINDOW || defined IPHONE 
#pragma pack(2)
typedef struct tagBITMAPFILEHEADER {
	WORD    bfType;
	DWORD   bfSize;
	WORD    bfReserved1;
	WORD    bfReserved2;
	DWORD   bfOffBits;
} BITMAPFILEHEADER, *LPBITMAPFILEHEADER, *PBITMAPFILEHEADER;
#pragma pack()
#elif defined(ANDROID)
struct tagBITMAPFILEHEADER {
	WORD    bfType;
	DWORD   bfSize;
	WORD    bfReserved1;
	WORD    bfReserved2;
	DWORD   bfOffBits;
}__attribute__((packed,aligned(2)));
typedef tagBITMAPFILEHEADER BITMAPFILEHEADER;
typedef tagBITMAPFILEHEADER *LPBITMAPFILEHEADER;
typedef tagBITMAPFILEHEADER *PBITMAPFILEHEADER;
#endif
