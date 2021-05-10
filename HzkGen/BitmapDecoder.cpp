#include "StdAfx.h"
#include ".\bitmapdecoder.h"

CBitmapDecoder::CBitmapDecoder(void)
{
	m_BmpInfo = NULL;
	m_Palette = NULL;
	m_Picture = NULL;
	m_bLoad = false;
	m_bShown = false;
}

CBitmapDecoder::~CBitmapDecoder(void)
{
	m_Release();
}

void CBitmapDecoder::m_Release()
{
	if( m_BmpInfo ) { delete m_BmpInfo; m_BmpInfo = NULL; }
	if( m_Palette ) { delete m_Palette; m_Palette = NULL; }
	if( m_Picture ) { delete m_Picture; m_Picture = NULL; }
}

LONG CBitmapDecoder::m_GetWidth()
{
	if( m_bLoad ) return m_BmpInfo->bmiHeader.biWidth;
	else return -1;
}

LONG CBitmapDecoder::m_GetHeight()
{
	if( m_bLoad ) return m_BmpInfo->bmiHeader.biHeight;
	else return -1;
}

DWORD CBitmapDecoder::m_Load(CFile& file)
{
	m_Release();
	///////////////////////////////////////////////////////
	BITMAPINFOHEADER	bmpInfoHeader;
	file.Read( &m_BmpFileHead,sizeof(BITMAPFILEHEADER) );
	file.Read( &bmpInfoHeader,sizeof(BITMAPINFOHEADER) );
	m_nColors = 0x01 << bmpInfoHeader.biBitCount;
	m_BmpInfo = (BITMAPINFO*)new char[ sizeof(BITMAPINFO)+m_nColors*sizeof(RGBQUAD) ];
	m_Palette = (LOGPALETTE*)new char[ sizeof(LOGPALETTE)+m_nColors*sizeof(PALETTEENTRY) ];
	m_Picture = (BYTE*)new char[ bmpInfoHeader.biSizeImage ];
	///////////////////////////////////////////////////////
	memcpy( &m_BmpInfo->bmiHeader,&bmpInfoHeader,sizeof(BITMAPINFOHEADER) );
	if( m_nColors <= 256 )	//只有16色或256色的图片需要调色板
		file.Read( m_BmpInfo->bmiColors,m_nColors*sizeof(RGBQUAD) );
	file.Read( m_Picture,m_BmpInfo->bmiHeader.biSizeImage*sizeof(BYTE) );
	m_bLoad = true;
	return 0;
}

DWORD CBitmapDecoder::m_Load(CString szPath)
{
	BOOL	bOpen = false;
	CFile	file;
	if( bOpen = file.Open( szPath, CFile::typeBinary|CFile::modeReadWrite ) )
	{
		m_Release();
		///////////////////////////////////////////////////////
		BITMAPINFOHEADER	bmpInfoHeader;
		file.Read( &m_BmpFileHead,sizeof(BITMAPFILEHEADER) );
		file.Read( &bmpInfoHeader,sizeof(BITMAPINFOHEADER) );
		m_nColors = 0x01 << bmpInfoHeader.biBitCount;
		m_BmpInfo = (BITMAPINFO*)new char[ sizeof(BITMAPINFO)+m_nColors*sizeof(RGBQUAD) ];
		m_Palette = (LOGPALETTE*)new char[ sizeof(LOGPALETTE)+m_nColors*sizeof(PALETTEENTRY) ];
		m_Picture = (BYTE*)new char[ bmpInfoHeader.biSizeImage ];
		///////////////////////////////////////////////////////
		memcpy( &m_BmpInfo->bmiHeader,&bmpInfoHeader,sizeof(BITMAPINFOHEADER) );
		if( m_nColors <= 256 )	//只有16色或256色的图片需要调色板
			file.Read( m_BmpInfo->bmiColors,m_nColors*sizeof(RGBQUAD) );
		file.Read( m_Picture,m_BmpInfo->bmiHeader.biSizeImage*sizeof(BYTE) );
		file.Close();
		m_bLoad = true;
		return 0;
	}
	else return GetLastError();
}

DWORD CBitmapDecoder::m_Show(CDC* pDC,CRect Rect,CSize Origin)
{
	if( !m_bLoad ) return 0;
	if( m_Picture != NULL )
	{
		CDC			memDC;
		CDC&		devDC = *pDC;
		CBitmap		newbmp;
		CBitmap*	oldbmp = NULL;

		memDC.CreateCompatibleDC(&devDC);
		newbmp.CreateCompatibleBitmap( &devDC,	m_BmpInfo->bmiHeader.biWidth,
												m_BmpInfo->bmiHeader.biHeight );
		oldbmp=memDC.SelectObject( &newbmp );
		::StretchDIBits(memDC.m_hDC,
						0,0,
						m_BmpInfo->bmiHeader.biWidth,
						m_BmpInfo->bmiHeader.biHeight,
						0,0,
						m_BmpInfo->bmiHeader.biWidth,
						m_BmpInfo->bmiHeader.biHeight,
						m_Picture,
						m_BmpInfo,
						DIB_RGB_COLORS,
						SRCCOPY);
		::StretchBlt(	devDC.m_hDC,
						Origin.cx,
						Origin.cy,
						m_BmpInfo->bmiHeader.biWidth,
						m_BmpInfo->bmiHeader.biHeight,
						memDC.m_hDC,
						0,0,
						m_BmpInfo->bmiHeader.biWidth,
						m_BmpInfo->bmiHeader.biHeight,
						SRCCOPY);
		memDC.SelectObject(oldbmp);
		m_bShown = true;
	}
	return 0;
}


COLORREF CBitmapDecoder::m_PickColor(int cx, int cy)
{
	COLORREF	color = RGB(255,255,255);
	if( !m_bLoad || m_Picture == NULL ) return color;
	if( cx >= m_BmpInfo->bmiHeader.biWidth ||
		cy >= m_BmpInfo->bmiHeader.biHeight ) return color;
	switch( m_BmpInfo->bmiHeader.biBitCount )
	{
	case 1:
	case 4:
	case 8:
	case 16:
		break;
	case 24:
		{	//bitmap图像的屏幕排列顺序：左下->右下->左上->右上
			double		yOffset	=	(m_BmpInfo->bmiHeader.biHeight-cy-1)*
									(m_BmpInfo->bmiHeader.biSizeImage)/
									(m_BmpInfo->bmiHeader.biHeight)/3;
			CColor24*	pixels	=	(CColor24*)m_Picture;
			CColor24*	oneLine	=	pixels + (int)yOffset;
			color = RGB( oneLine[cx].m_Red, oneLine[cx].m_Green, oneLine[cx].m_Blue );
		}
		break;
	case 32:
	default:;
	}
	return color;
}
