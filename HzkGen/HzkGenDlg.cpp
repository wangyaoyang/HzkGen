
// HzkGenDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "HzkGen.h"
#include "HzkGenDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CHzkGenDlg 对话框



static 	CHzkBuff	static_HZK;

CHzkGenDlg::CHzkGenDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CHzkGenDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CHzkGenDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_INPUT_CHN_STRING, m_chn_string);
	DDX_Control(pDX, IDC_STATIC_CANVAS, m_canvas);
	DDX_Control(pDX, IDC_STATIC_BITMAP, m_bmp_canvas);
}

BEGIN_MESSAGE_MAP(CHzkGenDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_BROWSE, &CHzkGenDlg::OnBnClickedButtonBrowse)
	ON_BN_CLICKED(IDC_BUTTON_SEARCH, &CHzkGenDlg::OnBnClickedButtonSearch)
	ON_BN_CLICKED(IDC_BUTTON_OUTPUT, &CHzkGenDlg::OnBnClickedButtonOutput)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE_BMP, &CHzkGenDlg::OnBnClickedButtonBrowseBmp)
	ON_BN_CLICKED(IDC_BUTTON_OUTPUT_BMP, &CHzkGenDlg::OnBnClickedButtonOutputBmp)
END_MESSAGE_MAP()


// CHzkGenDlg 消息处理程序

BOOL CHzkGenDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CHzkGenDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CHzkGenDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CRect	rect;
		CDialogEx::OnPaint();
		this->m_bmp_canvas.GetClientRect(rect);
		if (static_HZK.m_nLen > 128 * 64) {//beyond 128 ascii chars
			this->DrawChinese(this->m_canvas.GetDC());
		} else if (static_HZK.m_nLen > 128 * 16)	{//beyond 16 * 8 ascii chars
			this->DrawAscii(this->m_canvas.GetDC(),32,16);
		} else this->DrawAscii(this->m_canvas.GetDC(),16,8);
		this->m_bitmap.m_Show(this->m_bmp_canvas.GetDC(),rect,CSize(0,0));
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CHzkGenDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CHzkGenDlg::DrawChinese(CDC* pDC) {
	CString	szChns = this->m_szChinese;
	CRect	rect;
	
	this->m_canvas.GetClientRect(rect);
	if (static_HZK.m_nLen > 0 && szChns.GetLength() > 0) {
		int		nChns = szChns.GetLength();
		BYTE*	sChns = (BYTE*) szChns.GetBuffer();
		this->m_export_words.m_nWords = 0;
		for (int i = 0; i < nChns; i ++) {
			BYTE	incode[3];
			memset (incode, 0x00, 3);
			if (sChns[i] & 0x80) {
				incode[0] = sChns[i];
				incode[1] = sChns[i+1];
				i ++;
			} else {
				incode[0] = 0xA3;
				incode[1] = 0x80 | sChns[i];
			}
			this->DrawChnWord(static_HZK.m_buff, incode,rect,this->m_canvas.GetDC());
			//rect += CSize(32 * 2,(i / 10) * 32 * 2);	//10 chn word per row
			rect += CSize(32,0);	//10 chn word per row
		}
	}
}

void CHzkGenDlg::DrawAscii(CDC* pDC,int H,int W) {
	CString	szAscii = this->m_szChinese;
	CRect	rect;
	
	this->m_canvas.GetClientRect(rect);
	rect += CSize(W,0);
	if (static_HZK.m_nLen > 0 && szAscii.GetLength() > 0) {
		int		nAscii = szAscii.GetLength();
		int		nBytePerChar = H * (W/8);
		BYTE*	sAscii = (BYTE*) szAscii.GetBuffer();
		CPen		newPen(PS_SOLID,1,RGB(0,0,0));
		CPen		*pOldPen = pDC->SelectObject(&newPen);
		int		row,col;

		this->m_export_words.m_nWords = 0;
		for (int i = 0; i < nAscii; i ++) {
			BYTE	incode = sAscii[i] - ' ';
			CHzkWordTemplate&	wt = this->m_export_words.m_words[i];

			this->m_export_words.m_nWords = i;
			memset (wt.m_gb2312, 0x00, 3);
			wt.m_gb2312[0] = incode;
			for (row = 0; row < H; row ++)
				for (col = 0; col < (W/8); col ++)
					wt.m_template[row][col] = static_HZK.m_buff[incode * nBytePerChar + row * (W/8) + col];
			for(row = 0; row < H; row ++) {	//显示 16/32 rows
				for(col = 0; col < (W/8); col ++) { //1/2 byte
					for(int k=0; k<8 ; k++) { //8bits , 1 byte
						if (wt.m_template[row][col] & (0x80>>k)) { //测试为1的位则显示 
						//if (static_HZK.m_buff[incode * nBytePerChar + row * (W/8) + col] & (0x80>>k)) { //测试为1的位则显示 
							int x = i * W + col * 8 + k;
							int y = row;// + (m / 10) * 32;
							pDC->MoveTo(rect.left + x		, rect.top + y);
							pDC->LineTo(rect.left + x + 1	, rect.top + y + 1);
						}
					}
				}//for
			}//for
			pDC->SelectObject(pOldPen);
			//rect += CSize(8,0);	//10 chn word per row
		}
	}
}

void CHzkGenDlg::DrawChnWord(BYTE* pHzk, BYTE* incode, CRect rect, CDC* pDC) {
	unsigned char qh = 0, wh = 0; 
	unsigned long offset = 0;
	int i,j,k;  

	if (this->m_export_words.m_nWords > MAX_EXPORT_WORD - 1) {
		//this->MessageBox(_T("There is no Chinese word(s) need to be export..."),_T("Warning"),MB_ICONSTOP);
		return;
	}

	CHzkWordTemplate&	wt = this->m_export_words.m_words[this->m_export_words.m_nWords];
	
	this->m_export_words.m_nWords ++;
	//每个汉字，占两个字节, 取其区位号  
	qh = incode[0] - 0xa0; //获得区码  
	wh = incode[1] - 0xa0; //获得位码

	offset = (94*(qh-1)+(wh-1))*128; //得到偏移位置
	memcpy (wt.m_gb2312, incode, 2);
	memcpy (wt.m_template,pHzk + offset,128);

	//显示
	CPen	newPen(PS_SOLID,1,RGB(0,0,0));
	CPen	*pOldPen = pDC->SelectObject(&newPen);
	for(i=0; i<32; i++) { //16
		for(j=0; j<4; j++) { 
			for(k=0; k<8 ; k++) { //8bits , 1 byte
				//if( (  *( *(mat+i*size)+j ) ) & (0x80>>k))
				if (wt.m_template[i][j] & (0x80>>k)) { //测试为1的位则显示 
					int x = (j*8 + k) * 1;// + n * 32 * 2;
					int y = (i + 1) * 1;// + (m / 10) * 32;
					pDC->MoveTo(rect.left + x, rect.top + y);
					pDC->LineTo(rect.left + x + 1, rect.top + y + 1);
				}
			}//for
		}//for
	}//for
	pDC->SelectObject(pOldPen);
}

void CHzkGenDlg::OnBnClickedButtonBrowse()
{
	CFileDialog dlg(TRUE,"bin",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,"(*.bin;*.dat;*.*)|*.bin;*.dat;*.*||");
	if(dlg.DoModal() == IDOK) {
		this->m_szPathHzk = dlg.GetPathName();
		FILE*	fp = fopen(this->m_szPathHzk.GetBuffer(),"r");
		if (fp) {
			fseek(fp,0,SEEK_SET);
			static_HZK.m_nLen = fread(static_HZK.m_buff, 1, MAX_HZK_SIZE, fp);
			//int len = fread(static_HZK.m_buff, 1, 4096, fp);
			static_HZK.m_nLen = ftell(fp);
			TRACE("\noffset = %d",static_HZK.m_nLen);
			this->SetWindowText(this->m_szPathHzk);
			fclose(fp);
		}
	}
}


void CHzkGenDlg::OnBnClickedButtonSearch()
{
	this->m_chn_string.GetWindowText(m_szChinese);
	this->m_export_words.m_nWords = 0;
	this->Invalidate();
}

void CHzkGenDlg::OnBnClickedButtonOutput() {
	if (static_HZK.m_nLen > 128 * 64) {//beyond 128 ascii chars
		this->OnBnClickedButtonOutputChn32();
	} else if (static_HZK.m_nLen > 128 * 16)	{ //beyond 128 ascii chars
		this->OnBnClickedButtonOutputAscii(32,16);
	} else this->OnBnClickedButtonOutputAscii(16,8);
}

void CHzkGenDlg::OnBnClickedButtonOutputAscii(int H, int W) {
	CFileDialog dlg(FALSE,"c",NULL,OFN_OVERWRITEPROMPT,"(*.c;*.cc;*.cpp)|*.c;*.cc;*.cpp||");
	char		sOutputLine[1204];
	if(dlg.DoModal() == IDOK) {
		this->m_szPathHzk = dlg.GetPathName();
		FILE*	fp = fopen(this->m_szPathHzk.GetBuffer(),"wt");
		if (fp) {
			int nBytePerWord = H * (W/8);
			int nLength = static_HZK.m_nLen / nBytePerWord * nBytePerWord;
			memset (sOutputLine, 0x00, 1024);
			fprintf(fp, "\n#include \"lcd_ili9341_font.h\"\n\n");
			fprintf(fp, "unsigned char lcd_ili9341_asc2_%02d%02d[%d] = {",H,W,nLength);
			for (int i = 0; i < nLength; i ++) {
				if (i % nBytePerWord == 0)
					fprintf(fp, "\n/*%c*/ ", i / nBytePerWord + ' ');
				fprintf(fp,"0x%02x,",0x00ff & static_HZK.m_buff[i]);
			}
			fprintf(fp, "\n};\n\n");
			fclose(fp);
		}
	}
}

void CHzkGenDlg::OnBnClickedButtonOutputChn32()
{
	if (	this->m_export_words.m_nWords <= 0) {
		this->MessageBox(_T("There is no Chinese word(s) need to be export..."),_T("Warning"),MB_ICONINFORMATION);
		return;
	}
	CFileDialog dlg(FALSE,"c",NULL,OFN_OVERWRITEPROMPT,"(*.c;*.cc;*.cpp)|*.c;*.cc;*.cpp||");
	char		sOutputLine[1204];
	if(dlg.DoModal() == IDOK) {
		this->m_szPathHzk = dlg.GetPathName();
		FILE*	fp = fopen(this->m_szPathHzk.GetBuffer(),"wt");
		if (fp) {
			memset (sOutputLine, 0x00, 1024);
			fprintf(fp, "\n#include \"lcd_ili9341_font.h\"\n\n");
			fprintf(fp, "static ChineseLattice lattice_%s[] = {",dlg.GetFileTitle().GetBuffer());
			for (int i = 0; i < this->m_export_words.m_nWords; i ++) {
				CHzkWordTemplate&	wt = this->m_export_words.m_words[i];
				fprintf(fp, "\n {	.m_lattice = //%s\n    {", wt.m_gb2312);
				for (int j = 0; j < 32; j ++) {
					fprintf(fp, "{");
					for (int k = 0; k < 4; k ++) fprintf(fp,"0x%02x,",0x00ff & wt.m_template[j][k]);
					fprintf(fp, "},");
				}
				fprintf(fp, "}\n },");
			}
			fprintf(fp, "\n};\n\n");
			fprintf(fp, "ChineseWords %s = {\n", dlg.GetFileTitle().GetBuffer());
			fprintf(fp, "\t.m_nWord = %d,\n", this->m_export_words.m_nWords);
			fprintf(fp, "\t.m_words = lattice_%s,\n", dlg.GetFileTitle().GetBuffer());
			fprintf(fp, "};\n");
			//static_HZK.m_nLen = fread(static_HZK.m_buff, sizeof(BYTE), MAX_HZK_SIZE, fp);
			//this->SetWindowText(this->m_szPathHzk);
			fclose(fp);
		}
	}
}


void CHzkGenDlg::OnBnClickedButtonBrowseBmp()
{
	CFileDialog dlg(TRUE,"bin",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,"(*.bmp)|*.bmp||");
	if(dlg.DoModal() == IDOK) {
		if (	this->m_bitmap.m_Load(dlg.GetPathName()) == 0) {
			this->Invalidate();
		} else {
			this->MessageBox(_T("There is not a validate bitmap file"),_T("Warning"),MB_ICONSTOP);
			return;
		}
	}
}


void CHzkGenDlg::OnBnClickedButtonOutputBmp()
{
	CFileDialog dlg(FALSE,"c",NULL,OFN_OVERWRITEPROMPT,"(*.c;*.cc;*.cpp)|*.c;*.cc;*.cpp||");
	char		sOutputLine[1204];
	if(dlg.DoModal() == IDOK) {
		this->m_szPathHzk = dlg.GetPathName();
		FILE*	fp = fopen(this->m_szPathHzk.GetBuffer(),"wt");
		if (fp) {
			memset (sOutputLine, 0x00, 1024);
			fprintf(fp, "\n#include \"lcd_ili9341_font.h\"\n\n");
			fprintf(fp, "static unsigned char lcd_bitmap_%s[] = {",dlg.GetFileTitle().GetBuffer());
			const int hight = this->m_bitmap.m_GetHeight();
			const int width = this->m_bitmap.m_GetWidth();
			for (int h = 0; h < hight; h ++) {
				fprintf(fp, "\n\t");
				for (int w = 0; w < width; w ++) {
					COLORREF	color = this->m_bitmap.m_PickColor(w,h);
					BYTE	red		= GetRValue(color)	>> 3;	//5 bits
					BYTE	green	= GetGValue(color)	>> 2;	//6 bits
					BYTE	blue		= GetBValue(color)	>> 3;	//5 bits
					//	RRRR RGGG GGGB BBBB
					DWORD	dwColor = ((red << 11) & 0x0000F800) | ((green << 5) & 0x000007E0) | ((blue >> 0) & 0x0000001F);
					fprintf(fp,"0x%02x,0x%02x,",0x00ff & (dwColor >> 8), 0x00ff & dwColor);
				}
			}
			fprintf(fp, "\n};\n\n");
			fprintf(fp, "LCD_Image lcd_image_%s = {\n", dlg.GetFileTitle().GetBuffer());
			fprintf(fp, "\t.m_width = %d,\n", this->m_bitmap.m_GetWidth());
			fprintf(fp, "\t.m_hight = %d,\n", this->m_bitmap.m_GetHeight());
			fprintf(fp, "\t.m_image = lcd_bitmap_%s,\n", dlg.GetFileTitle().GetBuffer());
			fprintf(fp, "};\n\n");
			//static_HZK.m_nLen = fread(static_HZK.m_buff, sizeof(BYTE), MAX_HZK_SIZE, fp);
			//this->SetWindowText(this->m_szPathHzk);
			fclose(fp);
		}
	}
}
