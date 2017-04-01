// MkvFontExtractorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MkvFontExtractor.h"
#include "MkvFontExtractorDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CMkvFontExtractorDlg dialog




CMkvFontExtractorDlg::CMkvFontExtractorDlg(CWnd* pParent /*=NULL*/)
	:
	m_pScrollView(NULL),
	m_bFirstBkgdColor(true),
	CDialog(CMkvFontExtractorDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMkvFontExtractorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CBO_FONT_FILE, m_cboFontFile);
	DDX_Control(pDX, IDC_BTN_SAVE_FONT, m_btnSaveFont);
	DDX_Control(pDX, IDC_EDT_MKV_FILE, m_edtMkvFile);
}

BEGIN_MESSAGE_MAP(CMkvFontExtractorDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BTN_LOAD_MKV_FONTS, &CMkvFontExtractorDlg::OnBnClickedBtnLoadMkvFonts)
	ON_BN_CLICKED(IDC_BTN_SAVE_FONT, &CMkvFontExtractorDlg::OnBnClickedBtnSaveFont)
	ON_BN_CLICKED(IDC_BTN_BROWSE_MKV, &CMkvFontExtractorDlg::OnBnClickedBtnBrowseMkv)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CMkvFontExtractorDlg message handlers

BOOL CMkvFontExtractorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// Initialize the scroll view
	CRect rect;
	GetClientRect(&rect);
	CClientDC dc(this);
	int PosY = 78*dc.GetDeviceCaps(LOGPIXELSY)/96;
	int nMargin = 9*dc.GetDeviceCaps(LOGPIXELSY)/96;
	CRect rect2(nMargin,PosY,rect.Width()-nMargin*2,rect.Height()-nMargin-PosY);
	CRuntimeClass *pClass = RUNTIME_CLASS(CMyFontScrollView);
	m_pScrollView = (CMyFontScrollView*)pClass->CreateObject();
	m_pScrollView->Create(NULL, NULL, WS_CHILD | WS_VISIBLE, rect2, this, 10001);
	m_pScrollView->ModifyStyleEx(0, WS_EX_CLIENTEDGE);
	m_pScrollView->OnInitialUpdate();
	m_pScrollView->SetWindowPos(
		&m_btnSaveFont,
		nMargin,PosY,
		rect.Width()-nMargin*2,
		rect.Height()-nMargin-PosY,
		SWP_SHOWWINDOW);

	m_pScrollView->SetMkvFonts(&m_MkvFonts);
	m_cboFontFile.EnableWindow(FALSE);
	m_btnSaveFont.EnableWindow(FALSE);

	CString szMkvToolnixPath;
	if(false==m_MkvFonts.GetMkvToolnixPath(szMkvToolnixPath))
	{
		MessageBox(_T("MkvToolnix is not installed!"), _T("Error!"), MB_ICONERROR);
		return TRUE;
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMkvFontExtractorDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMkvFontExtractorDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMkvFontExtractorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CMkvFontExtractorDlg::OnBnClickedBtnLoadMkvFonts()
{
	CWaitCursor wait;
	CString szMkvPath;
	m_edtMkvFile.GetWindowText(szMkvPath);
	if(szMkvPath.IsEmpty())
	{
		MessageBox(_T("Mkv file is not specified!"), 
			_T("Error!"), MB_ICONERROR);
		return;
	}

	if(m_szPrevMkvPath == szMkvPath)
	{
		if(m_bFirstBkgdColor==false)
			m_pScrollView->ChangeBkgdColor();
		m_pScrollView->Invalidate();
		m_bFirstBkgdColor=false;
		return;
	}
	m_szPrevMkvPath = szMkvPath;

	if(m_MkvFonts.m_pvecFontAttachments)
		m_MkvFonts.m_pvecFontAttachments->clear();
	m_cboFontFile.ResetContent();

	CString szMkvToolnixPath;
	if(false==m_MkvFonts.GetMkvToolnixPath(szMkvToolnixPath))
	{
		MessageBox(_T("MkvToolnix not installed"), _T("Error!"), MB_ICONERROR);
		return;
	}

	TCHAR szFontTextPath[MAX_PATH];
	CString szLocalAppPath;

	if(SUCCEEDED(SHGetFolderPath(NULL, 
		CSIDL_LOCAL_APPDATA |CSIDL_FLAG_CREATE, 
		NULL, 
		0, 
		szFontTextPath))) 
	{
		PathAppend(szFontTextPath, _T("Temp\\Low\\MkvFontExtract"));
		CreateDirectory(szFontTextPath, NULL);
		szLocalAppPath = szFontTextPath;
		PathAppend(szFontTextPath, _T("info.txt"));
	}
	else
	{
		MessageBox(_T("Cannot find temp folder!"), _T("Error!"), MB_ICONERROR);
		return;
	}

	CString szMkvInfoExe = szMkvToolnixPath;

	PathAppend(szMkvInfoExe.GetBuffer(MAX_PATH), _T("mkvinfo.exe"));
	szMkvInfoExe.ReleaseBuffer();

	CString szCmdline;
	szCmdline.Format(_T("\"%s\" \"%s\" -r \"%s\""), 
		szMkvInfoExe, szMkvPath, szFontTextPath);

	m_MkvFonts.Execute(szCmdline);

	std::vector<tagAttachment>*& pvec = m_MkvFonts.m_pvecFontAttachments;

	if(pvec)
	{
		delete pvec;
		pvec = NULL;
	}

	pvec = m_MkvFonts.Parse(szFontTextPath);
	if(pvec)
	{
		if(pvec->size()==0)
		{
			MessageBox(_T("No fonts found"), _T("Error!"), MB_ICONERROR);
			m_cboFontFile.EnableWindow(FALSE);
			return;
		}
	}

	m_MkvFonts.m_szMkvToolnixPath = szMkvToolnixPath;
	m_MkvFonts.m_szMkvPath = szMkvPath;
	m_MkvFonts.m_szLocalAppPath = szLocalAppPath;

	m_MkvFonts.ExtractAllFonts(szLocalAppPath);

	if(pvec)
	{
		for(size_t i=0; i<pvec->size(); ++i)
		{
			m_cboFontFile.AddString(pvec->at(i).szFontName);
		}
		m_cboFontFile.EnableWindow(TRUE);
		m_btnSaveFont.EnableWindow(TRUE);
		
	}
	else
	{
		m_cboFontFile.EnableWindow(FALSE);
		m_btnSaveFont.EnableWindow(FALSE);
	}

	if(m_bFirstBkgdColor==false)
		m_pScrollView->ChangeBkgdColor();
	m_pScrollView->Invalidate();
	m_bFirstBkgdColor=false;
}

void CMkvFontExtractorDlg::OnBnClickedBtnSaveFont()
{
	CWaitCursor wait;

	if(m_cboFontFile.GetCurSel()==-1)
	{
		MessageBox(_T("No font selected"), _T("Error!"), MB_ICONERROR);
		return;
	}

	if(!m_MkvFonts.m_pvecFontAttachments)
	{
		MessageBox(_T("Internal error!"), _T("Error!"), MB_ICONERROR);
		return;
	}

	CString szExt;
	std::vector<tagAttachment>* pvec = m_MkvFonts.m_pvecFontAttachments;
	szExt = pvec->at(m_cboFontFile.GetCurSel()).szFileName.Right(3);
	CString szFontName =
		m_MkvFonts.m_pvecFontAttachments->at(m_cboFontFile.GetCurSel()).szFontName;
	szFontName += _T(".");
	szFontName += szExt;
	CFileDialog fSaveDlg(FALSE, szExt, 
		szFontName, 
		OFN_HIDEREADONLY|OFN_FILEMUSTEXIST, 
		_T("Font files (*.ttf)|*.ttf|(*.otf)|*.otf||"), this);

	if(fSaveDlg.DoModal()==IDOK)
	{
		CString szMkvExtractExe = m_MkvFonts.m_szMkvToolnixPath;

		PathAppend(szMkvExtractExe.GetBuffer(MAX_PATH), _T("mkvextract.exe"));
		szMkvExtractExe.ReleaseBuffer();

		CString szFontFile = fSaveDlg.GetPathName();

		CString szCmdline;
		szCmdline.Format(_T("\"%s\" attachments \"%s\" %d:\"%s\""), 
			szMkvExtractExe, m_MkvFonts.m_szMkvPath, m_cboFontFile.GetCurSel()+1, szFontFile);

		m_MkvFonts.Execute(szCmdline);

		if(PathFileExists(szFontFile))
		{
			//CString args;
			//args.Format(_T(" /n, /select,%s"), szFontFile);
			//ShellExecute(GetSafeHwnd(), 
				//_T("open"), _T("explorer.exe"), args, NULL, SW_SHOWNORMAL);
		}
		else
		{
			MessageBox(_T("Operation fails!"), _T("Error!"), MB_ICONERROR);
			return;
		}
	}
}

void CMkvFontExtractorDlg::OnBnClickedBtnBrowseMkv()
{
	CFileDialog fOpenDlg(TRUE, _T("mkv"), _T("*.mkv"), OFN_HIDEREADONLY|OFN_FILEMUSTEXIST, 
		_T("Matroska files (*.mkv)|*.mkv||"), this);

	fOpenDlg.m_pOFN->lpstrTitle=_T("Matroska file");

	CString szMkvPath;
	if(fOpenDlg.DoModal()==IDOK)
	{
		szMkvPath = fOpenDlg.GetPathName();
		m_edtMkvFile.SetWindowText(szMkvPath);
	}

}

void CMkvFontExtractorDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if(m_pScrollView && m_pScrollView->GetSafeHwnd())
	{
		CClientDC dc(this);
		int PosY = 78*dc.GetDeviceCaps(LOGPIXELSY)/96;
		int nMargin = 9*dc.GetDeviceCaps(LOGPIXELSY)/96;
		m_pScrollView->SetWindowPos(
			&m_btnSaveFont,
			nMargin,
			PosY,
			cx-nMargin*2,
			cy-nMargin-PosY,
			SWP_SHOWWINDOW);
	}
}
