// MkvFontExtractorDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "MyFontScrollView.h"
#include "MkvFonts.h"

// CMkvFontExtractorDlg dialog
class CMkvFontExtractorDlg : public CDialog
{
// Construction
public:
	CMkvFontExtractorDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_MKVFONTEXTRACTOR_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;
	MkvFonts m_MkvFonts;
	CMyFontScrollView* m_pScrollView;
	CString m_szPrevMkvPath;
	bool m_bFirstBkgdColor;
	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnLoadMkvFonts();
	CComboBox m_cboFontFile;
	afx_msg void OnBnClickedBtnSaveFont();
	CButton m_btnSaveFont;
	CEdit m_edtMkvFile;
	afx_msg void OnBnClickedBtnBrowseMkv();
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
