#pragma once
#include "afx.h"

class CBitmapDecoder
{
	class CColor24
	{
	public:
		BYTE	m_Blue;
		BYTE	m_Green;
		BYTE	m_Red;
	};
public:
	CBitmapDecoder(void);
	~CBitmapDecoder(void);
private:
	bool				m_bLoad;
	bool				m_bShown;
	BITMAPFILEHEADER	m_BmpFileHead;
	BITMAPINFO*			m_BmpInfo;
	LOGPALETTE*			m_Palette;
	DWORD				m_nColors;
	BYTE*				m_Picture;
private:
	void		m_Release();
public:
	DWORD		m_Load(CString szPath);
	DWORD		m_Load(CFile&	f);
	DWORD		m_Show(CDC* pDC,CRect Rect,CSize Origin);
	COLORREF	m_PickColor(int cx, int cy);
	LONG		m_GetWidth();
	LONG		m_GetHeight();
};

class CJpegDecoder
{
private:
	bool				m_bLoad;
	bool				m_bShown;
	BYTE*				m_Picture;
	DWORD				m_nColors;

};