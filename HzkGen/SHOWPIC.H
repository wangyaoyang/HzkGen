#ifndef		__SHOW_MAP__
/*********************************************************/
#define		MAC		0
#define		IMG		1
#define		PCX		2
#define		BMP		3
#define		GIF		4
#define		TIF		5	

//##ModelId=39B85E4E0163
class CPcxHead
{
public:
	//##ModelId=39B85E4E0204
	unsigned char	manufacturer;
	//##ModelId=39B85E4E0203
	unsigned char	version;
	//##ModelId=39B85E4E01F9
	unsigned char	encoding;
	//##ModelId=39B85E4E01EF
	unsigned char	bits_per_pixel;
	//##ModelId=39B85E4E01E7
	__int16			xmin;
	//##ModelId=39B85E4E01CC
	__int16			ymin;
	//##ModelId=39B85E4E01C7
	__int16			xmax;
	//##ModelId=39B85E4E01BE
	__int16			ymax;
	//##ModelId=39B85E4E01A1
	__int16			hres;
	//##ModelId=39B85E4E0198
	__int16			vres;
	//##ModelId=39B85E4E0195
	unsigned char	palette[48];
	//##ModelId=39B85E4E017B
	unsigned char	reserved;
	//##ModelId=39B85E4E017A
	unsigned char	color_planes;
	//##ModelId=39B85E4E0179
	unsigned char	bytes_per_line;
	//##ModelId=39B85E4E0178
	unsigned char	palette_type;
	//##ModelId=39B85E4E0177
	unsigned char	filer[58];
};

//##ModelId=39B85E4D034D
class CShowMap
{
protected:
	//##ModelId=39B85E4E00D7
	int			m_width;
	//##ModelId=39B85E4E00D6
	int			m_depth;
	//##ModelId=39B85E4E00CD
	long		m_BytePerLine;
	//##ModelId=39B85E4E00CC
	long		m_PictureSize;
	//##ModelId=39B85E4E00C2
	char*		m_Picture;
	//##ModelId=39B85E4E00BB
	char		m_Palette[768];
	//##ModelId=39B85E4E00B8
	BITMAPINFO*	m_bmpinfo;
	//##ModelId=39B85E4E0074
	LOGPALETTE*	m_lPal;
	//##ModelId=39B85E4E0054
   	WORD		m_biPlane;
	//##ModelId=39B85E4E0041
	int			m_biBitCount;
	//##ModelId=39B85E4E0040
	int			m_ColorsNum;
private:
	//##ModelId=39B85E4E0036
	HPALETTE	m_hPcxPal;
	//##ModelId=39B85E4E0022
	HBITMAP		m_hbitmap;
	//##ModelId=39B85E4E0018
	BITMAPFILEHEADER	m_FileHd;
protected:
	//##ModelId=39B85E4E0127
	void		FillInfo(void);
	//##ModelId=39B85E4E010B
	void		MemAlloc(long);
	//##ModelId=39B85E4E0109
	void		MemFree(void);
public:
	//##ModelId=39B85E4E00EC
	CShowMap(void);
	//##ModelId=39B85E4E00EA
	~CShowMap(void);
	//##ModelId=39B85E4E00E0
	void		Show(CDC*,CRect,CSize);
};

//##ModelId=39B85E4E037F
class CShowPcx:public CShowMap
{
private:
	//##ModelId=39B85E4E039D
	CPcxHead	m_PcxHead;
private:
	//##ModelId=39B85E4E03E4
	long		ReadBmpHead(FILE*);
	//##ModelId=39B85E4E03C8
	long		ReadPcxHead(FILE*);
	//##ModelId=39B85E4E03BB
	void		UnpackPcx(FILE*,char*);
public:
	//##ModelId=39B85E4E03B5
	BOOL		OpenPcx(char*);
	//##ModelId=39B85E4E03A7
	BOOL		ShowPcx(CDC*,CRect,CSize);
	//##ModelId=39B85E4E03AB
	BOOL		ShowPcx(char*,CDC*,CRect,CSize);
};

//##ModelId=39B85E4F018C
class CShowBmp:public CShowMap
{
private:
	//##ModelId=39B85E4F01B6
	BITMAPFILEHEADER	m_BmpFileHead;
	//##ModelId=39B85E4F01AA
	BITMAPINFOHEADER	m_BmpInfoHead;
private:
	//##ModelId=39B85E4F01FB
	BOOL		UnpackBmp(FILE* fp);
	//##ModelId=39B85E4F01FD
	BOOL		UnpackBmp(char* buff);
public:
	//##ModelId=39B85E4F01F4
	BOOL		OpenBmp(char* filename);
	//##ModelId=39B85E4F01E6
	BOOL		ShowBmp(CDC*,CRect,CSize);
	//##ModelId=39B85E4F01EA
	BOOL		ShowBmp(char* buff,CDC*,CRect,CSize);
};

//##ModelId=39B85E4E0217
class CShowPicture:public CShowPcx,public CShowBmp
{};
/*********************************************************/
#define	__SHOW_MAP__
#endif
