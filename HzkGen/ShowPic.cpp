#include "stdafx.h"
#include "ShowPic.h"

/**** The Public Member Functions *******************************************/
CShowMap::CShowMap(void)
{
	m_Picture = NULL;
	m_hbitmap = NULL;
	m_bmpinfo = (BITMAPINFO*) malloc (sizeof(BITMAPINFO)+256*sizeof(RGBQUAD));
	m_lPal = (LOGPALETTE*) malloc(sizeof(LOGPALETTE)+256*sizeof(PALETTEENTRY));
}	

CShowMap::~CShowMap(void)
{
	if(m_hbitmap) DeleteObject(m_hbitmap);
	if(m_lPal) free(m_lPal);
	if(m_bmpinfo) free(m_bmpinfo);
	MemFree();
}

void CShowMap::FillInfo(void)
{
	int i,j;
	m_bmpinfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	m_bmpinfo->bmiHeader.biWidth = m_width;
	m_bmpinfo->bmiHeader.biHeight = m_depth;
	m_bmpinfo->bmiHeader.biPlanes = m_biPlane;
	m_bmpinfo->bmiHeader.biBitCount = m_biBitCount;
	m_bmpinfo->bmiHeader.biCompression = BI_RGB;
	m_bmpinfo->bmiHeader.biSizeImage = m_PictureSize;
	m_bmpinfo->bmiHeader.biXPelsPerMeter = 0;
	m_bmpinfo->bmiHeader.biYPelsPerMeter = 0;
	m_bmpinfo->bmiHeader.biClrUsed = 0;
	m_bmpinfo->bmiHeader.biClrImportant = 0;
	for(i=0,j=0;i<m_ColorsNum;i++) {
		m_bmpinfo->bmiColors[i].rgbRed = m_Palette[j++];
		m_bmpinfo->bmiColors[i].rgbGreen = m_Palette[j++];
		m_bmpinfo->bmiColors[i].rgbBlue = m_Palette[j++];
		m_bmpinfo->bmiColors[i].rgbReserved = 0;
	}
	m_lPal->palVersion = 0x0300;
	m_lPal->palNumEntries = m_ColorsNum;
	for(i=0,j=0;i<m_ColorsNum;i++) {
		m_lPal->palPalEntry[i].peRed = m_bmpinfo->bmiColors[i].rgbRed;
		m_lPal->palPalEntry[i].peGreen = m_bmpinfo->bmiColors[i].rgbGreen;
		m_lPal->palPalEntry[i].peBlue = m_bmpinfo->bmiColors[i].rgbBlue;
		m_lPal->palPalEntry[i].peFlags = PC_NOCOLLAPSE;
	}
	m_hPcxPal = CreatePalette(m_lPal);
	if(m_hbitmap!=NULL) { DeleteObject(m_hbitmap); m_hbitmap = NULL; }
	return;
}

void CShowMap::Show(CDC* pDC,CRect Rect,CSize Origin)
{
	if(m_Picture==NULL) return;
	if(m_hbitmap==NULL)
	{
		m_hbitmap = CreateDIBitmap(	pDC->m_hDC,
									(BITMAPINFOHEADER*)m_bmpinfo,
									CBM_INIT,
									m_Picture,
									(BITMAPINFO*)m_bmpinfo,
									DIB_RGB_COLORS	);
		if( m_hbitmap == NULL )
		{
			DWORD	error_code = 0;
			error_code = GetLastError();
			TRACE("m_hbitmap EQU 0, Last Error is : %d\n",error_code );
		}
		TRACE0("SHOWMAP.DLL--OK CShowMap::Show() : Create DIBitmap\n");
	}
	/////////////////////////////////////////////////////////
	TRACE0("SHOWMAP.DLL--OK CShowMap::Show() : Begin\n");
	HPALETTE hOldPal = SelectPalette(pDC->m_hDC,m_hPcxPal,FALSE);
	RealizePalette(pDC->m_hDC);

	HDC hMemDC = CreateCompatibleDC(pDC->m_hDC);
	SelectObject(hMemDC,m_hbitmap);
	BitBlt(	pDC->m_hDC,
			Rect.left,	Rect.top,	Rect.right-Rect.left,	Rect.bottom-Rect.top,
			hMemDC,
			Origin.cx,	Origin.cy,
			SRCCOPY
			);
	TRACE( "pDC = %x,hMemDC = %x,m_hbitmap = %x\n",pDC,hMemDC,m_hbitmap );
	DeleteDC(hMemDC);

	SelectPalette(pDC->m_hDC,hOldPal,FALSE);
	TRACE0("SHOWMAP.DLL--OK CShowMap::Show() : Stop\n");
	return;
}

void CShowMap::MemAlloc(long Size)
{
	MemFree();
 	m_Picture = new char[Size];
	return;
}

void CShowMap::MemFree(void)
{
	if(m_Picture) { delete m_Picture; m_Picture = NULL; }
}

/****************************************************************************/	
/****** PCX FORMAT PICTURE **************************************************/
/****************************************************************************/	
long CShowPcx::ReadPcxHead(FILE* fp)
{
	fseek(fp,0L,SEEK_SET);
	if((fread((char*)&m_PcxHead,1,sizeof(CPcxHead),fp)!=sizeof(CPcxHead))
		|| (m_PcxHead.manufacturer!=0x0A)
		|| (m_PcxHead.bits_per_pixel!=8)) {
		fseek(fp,0L,SEEK_SET);								          
		TRACE0("SHOWMAP.DLL--?? CShowPcx::ReadPcxHead() : 不是一个256色PCX图象\n");
		return 0;
	}
	if((m_PcxHead.version>5)
		|| (fseek(fp,-769L,SEEK_END))
		|| (fgetc(fp)!=0x000C)
		|| (fread(m_Palette,1,768,fp)!=768)) {
		fseek(fp,0L,SEEK_SET);
		TRACE0("SHOWMAP.DLL--?? CShowPcx::ReadPcxHead() : 未找到PCX图象的调色板\n");
		return 0;
	}
	m_width = (m_PcxHead.xmax - m_PcxHead.xmin) +1;
	m_depth = (m_PcxHead.ymax - m_PcxHead.ymin) +1;
	m_biBitCount = 8;
	m_biPlane = 1;
	m_ColorsNum = 256;
	m_BytePerLine=(m_width%4)?(m_width +4-m_width%4):m_width;
	m_PictureSize = (long)m_depth * (long)m_BytePerLine;
	return m_PictureSize;
}

void CShowPcx::UnpackPcx(FILE* fp,	char* Pct)
{
	__int16	Pixel	= 0;
	__int16	Count	= 0;
	__int16 Yoff	= m_depth-1;
	__int16 Xoff	= 0;
 
	fseek(fp,128L,SEEK_SET);
	while(Yoff>=0) {
		Pixel = fgetc(fp);
		if((Pixel&0x00C0)==0x00C0) {	//	00c0H=1100 0000B
			Count = Pixel&0x003F;		//	003fH=0011 1111B
			Pixel = fgetc(fp)&0x00FF;	//	00ffH=1111 1111B
			for(int i=0;i<Count;i++) {
				Pct[ (Yoff*m_BytePerLine) + Xoff ] = (char)Pixel;
				if(++Xoff>=m_width) { Xoff = 0; Yoff--; if(Yoff<0) return; }
			}
		}
		else {
				Pct[ (Yoff*m_BytePerLine) + Xoff ] = (char)Pixel;
				if(++Xoff>=m_width) { Xoff = 0; Yoff--; if(Yoff<0) return; }
		}
	}
}

BOOL CShowPcx::OpenPcx(char* FileName)
{
	HANDLE	hMutex = CreateMutex(NULL,FALSE,"NameOfMutexforPcx");
	DWORD	WaitResult;
	FILE*	fp = NULL;

	WaitResult = WaitForSingleObject(hMutex,1000);
	switch(WaitResult) {
		case WAIT_TIMEOUT:	break;
		case WAIT_ABANDONED:break;
		case WAIT_OBJECT_0:
			{
				char	msg[256];
				memset( msg,0,256 );
				sprintf( msg,"SHOWMAP.DLL--OK CShowPcx::OpenPcx() : Open [ %s ]\n",FileName );
				TRACE0( msg );
			}
			if( (fp = fopen( FileName,"rb" ))==NULL ) { ReleaseMutex(hMutex); CloseHandle(hMutex); return FALSE; }
			if(ReadPcxHead(fp)) {
				MemAlloc(m_PictureSize);
				UnpackPcx(fp,m_Picture);
				if( fp ) fclose(fp);
				FillInfo();
				ReleaseMutex(hMutex);
				CloseHandle(hMutex);
				return TRUE;
			}
			if( fp ) fclose(fp);
			ReleaseMutex(hMutex);
			CloseHandle(hMutex);
			return FALSE;
		default:;
	}
	ReleaseMutex(hMutex);
	CloseHandle(hMutex);
	return FALSE;
}

BOOL CShowPcx::ShowPcx(CDC* pDC,CRect Rect,CSize Origin)
{
	HANDLE	hMutex = CreateMutex(NULL,FALSE,"NameOfMutexforPcx");
	DWORD	WaitResult;

	WaitResult = WaitForSingleObject(hMutex,1000);
	switch(WaitResult) {
		case WAIT_TIMEOUT:		break;
		case WAIT_ABANDONED:	break;
		case WAIT_OBJECT_0:		Show(pDC,Rect,Origin);	break;
		default:;
	}
	ReleaseMutex(hMutex);
	CloseHandle(hMutex);
	return FALSE;
}

BOOL CShowPcx::ShowPcx(char* FileName,CDC* pDC,CRect Rect,CSize Origin)
{
	HANDLE	hMutex = CreateMutex(NULL,FALSE,"NameOfMutexforPcx");
	DWORD	WaitResult;
	FILE*	fp = NULL;

	WaitResult = WaitForSingleObject(hMutex,1000);
	switch(WaitResult) {
		case WAIT_TIMEOUT:	break;
		case WAIT_ABANDONED:break;
		case WAIT_OBJECT_0:	
			if( (fp = fopen( FileName,"rb" ))==NULL ) {
				{
					char	msg[256];
					memset( msg,0,256 );
					sprintf( msg,"SHOWMAP.DLL--?? CShowPcx::ShowPcx() : Open [ %s ]\n",FileName );
					TRACE0( msg );
				}
				ReleaseMutex(hMutex);		CloseHandle(hMutex);
				Show(pDC,Rect,CSize(0,0));	return FALSE;
			}
			else {
				char	msg[256];
				memset( msg,0,256 );
				sprintf( msg,"SHOWMAP.DLL--OK CShowPcx::ShowPcx() : Open [ %s ]\n",FileName );
				TRACE0( msg );
			}
			if(ReadPcxHead(fp)) {
				MemAlloc(m_PictureSize);
					UnpackPcx(fp,m_Picture);
					if( fp ) fclose(fp);
					FillInfo();
					Show(pDC,Rect,Origin);
				MemFree();
				ReleaseMutex(hMutex);
				CloseHandle(hMutex);
				return TRUE;
			}
			if( fp ) fclose(fp);
			ReleaseMutex(hMutex);
			CloseHandle(hMutex);
			return FALSE;
		default:;
	}
	ReleaseMutex(hMutex);
	CloseHandle(hMutex);
	return FALSE;
}
/****************************************************************************/
/***************************************************************************/
BOOL CShowBmp::UnpackBmp(FILE* fp)
{
	int i;

	fseek(fp,0L,SEEK_SET);
	if(fread((char*)&m_BmpFileHead,1,sizeof(BITMAPFILEHEADER),fp)
			!=sizeof(BITMAPFILEHEADER))		return 0;
	if(fread((char*)&m_BmpInfoHead,1,sizeof(BITMAPINFOHEADER),fp)
			!=sizeof(BITMAPINFOHEADER))		return 0;
	if(m_BmpInfoHead.biCompression!=0) 	return 0;
	if(m_BmpInfoHead.biBitCount!=8)	return 0;
	m_width = (unsigned)m_BmpInfoHead.biWidth;
	m_depth = (unsigned)m_BmpInfoHead.biHeight;
	m_biPlane = 1;
	m_biBitCount = (unsigned)m_BmpInfoHead.biBitCount;
	m_ColorsNum = 1<<m_BmpInfoHead.biBitCount;
	int	bytes = m_width/(8/m_biBitCount);
	m_BytePerLine = ((bytes+3)/4)*4;
	m_PictureSize = (long)m_depth * (long)m_BytePerLine;

	fseek(fp,54L,SEEK_SET);
	for(i=0;i<m_ColorsNum;i++) {
		m_Palette[3*i+2] = (char)fgetc(fp);
		m_Palette[3*i+1] = (char)fgetc(fp);
		m_Palette[3*i+0] = (char)fgetc(fp);
		fgetc(fp);
	}
	MemAlloc(m_PictureSize);
	for(i=0;i<m_depth;i++) {
		fread(m_Picture+i*m_BytePerLine, m_BytePerLine, 1, fp);
	}
	return TRUE;
}

BOOL CShowBmp::UnpackBmp(char* buff)
{
	int		i = 0;
	char*	offset = &buff[0];

	memcpy( &m_BmpFileHead,offset,sizeof(BITMAPFILEHEADER) );	offset+=sizeof(BITMAPFILEHEADER);
	memcpy( &m_BmpInfoHead,offset,sizeof(BITMAPINFOHEADER) );	offset+=sizeof(BITMAPINFOHEADER);
	if(m_BmpInfoHead.biCompression!=0) 	return 0;
	if(m_BmpInfoHead.biBitCount!=8)	return 0;
	m_width = (unsigned)m_BmpInfoHead.biWidth;
	m_depth = (unsigned)m_BmpInfoHead.biHeight;
	m_biPlane = 1;
	m_biBitCount = (unsigned)m_BmpInfoHead.biBitCount;
	m_ColorsNum = 1<<m_BmpInfoHead.biBitCount;
	int	bytes = m_width/(8/m_biBitCount);
	m_BytePerLine = ((bytes+3)/4)*4;
	m_PictureSize = (long)m_depth * (long)m_BytePerLine;

	offset = &buff[54];
	for(i=0;i<m_ColorsNum;i++)
	{
		m_Palette[3*i+2] = *offset;	offset ++;
		m_Palette[3*i+1] = *offset;	offset ++;
		m_Palette[3*i+0] = *offset;	offset ++;
		offset ++;
	}
	MemAlloc(m_PictureSize);
	for(i=0;i<m_depth;i++)
	{
		memcpy( m_Picture+i*m_BytePerLine, offset, m_BytePerLine );	offset += m_BytePerLine;
	}
	return TRUE;
}

BOOL CShowBmp::OpenBmp(char* FileName)
{
	HANDLE	hMutex = CreateMutex(NULL,FALSE,"NameOfMutexforBmp");
	DWORD	WaitResult;
	FILE*	fp = NULL;

	WaitResult = WaitForSingleObject(hMutex,1000);
	switch(WaitResult) {
		case WAIT_TIMEOUT:	break;
		case WAIT_ABANDONED:break;
		case WAIT_OBJECT_0:	
			if( (fp = fopen( FileName,"rb" ))==NULL ) {
				{
					char	msg[256];
					memset( msg,0,256 );
					sprintf( msg,"SHOWMAP.DLL--?? CShowBmp::OpenBmp() : Open [ %s ]\n",FileName );
					TRACE0( msg );
				}
				ReleaseMutex(hMutex);		CloseHandle(hMutex);	return FALSE;
			}
			else {
				char	msg[256];
				memset( msg,0,256 );
				sprintf( msg,"SHOWMAP.DLL--OK CShowBmp::OpenBmp() : Open [ %s ]\n",FileName );
				TRACE0( msg );
			}
			if(UnpackBmp(fp)) { if(fp) fclose(fp); ReleaseMutex(hMutex); CloseHandle(hMutex); FillInfo(); return TRUE; }
			else { if( fp ) fclose(fp); ReleaseMutex(hMutex); CloseHandle(hMutex); return FALSE; }
		default:;
	}
	ReleaseMutex(hMutex);
	CloseHandle(hMutex);
	return FALSE;
}

BOOL CShowBmp::ShowBmp(CDC* pDC,CRect Rect,CSize Origin)
{
	HANDLE	hMutex = CreateMutex(NULL,FALSE,"NameOfMutexforBmp");
	DWORD	WaitResult;

	WaitResult = WaitForSingleObject(hMutex,1000);
	switch(WaitResult) {
		case WAIT_TIMEOUT:		break;
		case WAIT_ABANDONED:	break;
		case WAIT_OBJECT_0:		Show(pDC,Rect,Origin);	break;
		default:;
	}

	ReleaseMutex(hMutex);
	CloseHandle(hMutex);
	return FALSE;
}

BOOL CShowBmp::ShowBmp(char* buff,CDC* pDC,CRect Rect,CSize Origin)
{
	if(!UnpackBmp(buff)) return FALSE;
	if(!ShowBmp(pDC,Rect,Origin)) return FALSE;
	return FALSE;
}
/***************************************************************************/
