
// HzkGenDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"
#include "BitmapDecoder.h"

#define	MAX_CHINESE_WORD	8192
#define	BYTES_PER_WORD	128
#define	MAX_HZK_SIZE		(MAX_CHINESE_WORD * BYTES_PER_WORD)

// CHzkGenDlg �Ի���
class CHzkBuff
{
public:
	int		m_nLen;
	BYTE		m_buff[MAX_HZK_SIZE];
public:
	CHzkBuff() {
		m_nLen = 0;
		memset (m_buff, 0x00, MAX_HZK_SIZE);
	}
	int SetBuff(BYTE* buff,int len) {
		if (buff && 0 < len && len < MAX_HZK_SIZE) {
			memcpy (m_buff, buff, len);
			m_nLen = len;
		} else {
			m_nLen = 0;
			return 0;
		}
	}
};

#define	MAX_EXPORT_WORD	64
class CHzkWordTemplate
{
public:
	BYTE	m_gb2312[3];
	BYTE	m_template[32][4];
public:
	CHzkWordTemplate() {
		memset (this->m_gb2312, 0x00, 3);
		memset (this->m_template, 0x00, 128);
	}
};

class CHzkExportWords
{
public:
	int	m_nWords;
	CHzkWordTemplate	m_words[MAX_EXPORT_WORD];
public:
	CHzkExportWords() {
		m_nWords = 0;
	}
};

class CHzkGenDlg : public CDialogEx
{
// ����
public:
	CHzkGenDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_HZKGEN_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

private:
	CString	m_szPathHzk;
// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
private:
	void DrawAscii(CDC* pDC,int H,int W);
	void DrawChinese(CDC* pDC);
	void DrawChnWord(BYTE* pHzk, BYTE* incode, CRect rect, CDC* pDC);
	void OnBnClickedButtonOutputAscii(int H, int W);
	void OnBnClickedButtonOutputChn32();
public:
	afx_msg void OnBnClickedButtonBrowse();
	afx_msg void OnBnClickedButtonSearch();
private:
	CBitmapDecoder	m_bitmap;
	CHzkExportWords	m_export_words;
	CString	m_szChinese;
	CEdit	m_chn_string;
	CStatic m_canvas;
public:
	afx_msg void OnBnClickedButtonOutput();
private:
	CStatic m_bmp_canvas;
public:
	afx_msg void OnBnClickedButtonBrowseBmp();
	afx_msg void OnBnClickedButtonOutputBmp();
};
