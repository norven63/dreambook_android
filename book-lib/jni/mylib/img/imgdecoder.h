#pragma once


HIMAGE GetJPEGBuf(const char* buffer,size_t bufferSize,size_t& bmpBufSize) ;
HIMAGE GetTgaBuf(const char* buffer,size_t bufferSize,size_t& bmpBufSize) ;
HIMAGE GetPNGBuf(const char* buffer,size_t bufferSize,size_t& bmpBufSize) ;
HIMAGE GetGIFBuf(const char* buffer,size_t bufferSize,size_t& bmpBufSize) ;

// ��r��g��b��(a)ת��Ϊbitmap
HIMAGE ToBitmap(const unsigned char* buffer, 
	int width, 
	int height,
	int nChannels);

// ����ͼ��
HIMAGE LoadImgFile(CString sFilePath);
void ReleaseImg(HIMAGE img);
// ����ͼ��
BOOL SaveImgFile(HIMAGE hImg,CString sFilePath);
BOOL SaveBGRABuf(BYTE* pBuf,int nWidth,int nHeight,CString sFilePath);
BOOL SaveRGBABuf(BYTE* pBuf,int nWidth,int nHeight,CString sFilePath);


//������Ƶ�ĵ�һ֡ͼ��
HIMAGE DecodeVideoImage(LPCTSTR szVideoFile);



inline const BITMAPINFOHEADER* GetImageInfo(HIMAGE hImage)
{
	return (BITMAPINFOHEADER*)hImage;
}
inline BYTE* GetImageBuf(HIMAGE hImage)
{
	return ((BYTE*)hImage)+sizeof(BITMAPINFOHEADER);
}
inline CSize GetImageSize(HIMAGE hImage)
{
	const BITMAPINFOHEADER* pbm = GetImageInfo(hImage);
	return CSize(pbm->biWidth,pbm->biHeight<0?-pbm->biHeight:pbm->biHeight);
}
inline int GetImageBitCount(HIMAGE hImage)
{
	const BITMAPINFOHEADER* pbm = GetImageInfo(hImage);
	return (int) pbm->biBitCount;
}
