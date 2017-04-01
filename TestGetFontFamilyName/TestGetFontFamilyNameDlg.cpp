// TestGetFontFamilyNameDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TestGetFontFamilyName.h"
#include "TestGetFontFamilyNameDlg.h"
//#include "../TTFNameLibrary/TTF.h"
#include "../TTFNameLibrary/TTFLocale.h"
#include "../TTFNameLibrary/TTCLocale.h"
#include <sstream>
#include <shlwapi.h>
#include <strsafe.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTestGetFontFamilyNameDlg dialog




CTestGetFontFamilyNameDlg::CTestGetFontFamilyNameDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTestGetFontFamilyNameDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTestGetFontFamilyNameDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDT_FILE, m_edtFile);
	DDX_Control(pDX, IDC_EDT_INFO, m_edtInfo);
}

BEGIN_MESSAGE_MAP(CTestGetFontFamilyNameDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BTN_BROWSE, &CTestGetFontFamilyNameDlg::OnBnClickedBtnBrowse)
	ON_BN_CLICKED(IDC_BTN_PARSE, &CTestGetFontFamilyNameDlg::OnBnClickedBtnParse)
END_MESSAGE_MAP()


// CTestGetFontFamilyNameDlg message handlers

BOOL CTestGetFontFamilyNameDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTestGetFontFamilyNameDlg::OnPaint()
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
		//CDialog::OnPaint();
		CPaintDC dc(this);
		dc.SetBkMode(TRANSPARENT);

		LOGFONT lf;
		memset(&lf, 0, sizeof(lf));
		lf.lfHeight = -MulDiv(24, dc.GetDeviceCaps(LOGPIXELSY), 72);
		lf.lfWeight = FW_NORMAL;
		lf.lfOutPrecision = OUT_TT_ONLY_PRECIS;
		int nSize = sizeof(lf.lfFaceName)/sizeof(wchar_t);
		StringCchCopyW(lf.lfFaceName, nSize, m_szFontName);

		// create and select it
		CFont newFont;
		if (!newFont.CreateFontIndirect(&lf))
			return;
		CFont* pOldFont = dc.SelectObject(&newFont);

		dc.SetTextColor(RGB(160,160,160));
		dc.TextOut( 12, 42, lf.lfFaceName, wcslen(lf.lfFaceName));
		dc.SetTextColor(RGB(0,0,0));
		dc.TextOut( 10, 40, lf.lfFaceName, wcslen(lf.lfFaceName));
		CSize size = dc.GetOutputTextExtent(lf.lfFaceName, wcslen(lf.lfFaceName));

		// Put back the old font
		dc.SelectObject(pOldFont);	
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTestGetFontFamilyNameDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CTestGetFontFamilyNameDlg::OnBnClickedBtnBrowse()
{
	wchar_t szFilters[]=
		L"TTF Font Files (*.ttf)|*.ttf|OTF Font Files (*.otf)|*.otf|TTC Font Files (*.ttc)|*.ttc|All Files (*.*)|*.*||";

	CFileDialog fileDlg (TRUE, L"ttf", NULL,
		OFN_FILEMUSTEXIST| OFN_HIDEREADONLY|OFN_EXPLORER, szFilters, this);
	if( fileDlg.DoModal ()==IDOK )
	{
		m_edtFile.SetWindowText(fileDlg.GetPathName());
		m_szExt=fileDlg.GetFileExt();
		m_szExt.MakeLower();
		//OnBnClickedBtnParse();
	}
}

void CTestGetFontFamilyNameDlg::OnBnClickedBtnParse()
{
	CString szFile;
	m_edtFile.GetWindowText(szFile);
	if(szFile.IsEmpty())
	{
		MessageBox(L"Font file is not specified!", L"Error", MB_ICONERROR);
		return;
	}
	if(FALSE==PathFileExists(szFile))
	{
		MessageBox(L"Font file does not exist!", L"Error", MB_ICONERROR);
		return;
	}

	m_szExt = szFile.Right(3);
	m_szExt.MakeLower();
	if(m_szExt=="ttf"||m_szExt=="otf")
	{
		TTFLocale ttf;
		if( false == ttf.Parse((LPCWSTR)(szFile)) )
		{
			MessageBox(L"Font file cannot be parsed!", L"Error", MB_ICONERROR);
			return;
		}
		else
		{
			if(m_szFontName!=_T(""))
			{
				BOOL b = RemoveFontResourceEx(
					m_szCurrFile, 		// name of font file
					FR_PRIVATE,   		
					NULL         		
					);
			}
			std::vector<int> vec = ttf.GetListOfLCID();

			if(vec.size()<=0)
				return;

			m_szFontName = ttf.GetFontName(vec[0]).c_str();

			int nResults = AddFontResourceEx(
				szFile,
				FR_PRIVATE,
				NULL);

			m_szCurrFile = szFile;

			std::wostringstream stream;
			int i=0;
			//for(size_t i=0; i<vec.size(); ++i)
			{
				stream<<L"FontName : "<<ttf.GetFontName(vec[i])<<L"\r\n\r\n";
				stream<<L"Copyright : "<<ttf.GetCopyright(vec[i])<<L"\r\n\r\n";
				stream<<L"FontFamilyName : "<<ttf.GetFontFamilyName(vec[i])<<L"\r\n\r\n";
				stream<<L"FontSubFamilyName : "<<ttf.GetFontSubFamilyName(vec[i])<<L"\r\n\r\n";
				stream<<L"FontID : "<<ttf.GetFontID(vec[i])<<L"\r\n\r\n";
				stream<<L"Version : "<<ttf.GetVersion(vec[i])<<L"\r\n\r\n";
				stream<<L"PostScriptName : "<<ttf.GetPostScriptName(vec[i])<<L"\r\n\r\n";
				stream<<L"Trademark : "<<ttf.GetTrademark(vec[i])<<L"\r\n\r\n";
			}
			m_edtInfo.SetWindowText(stream.str().c_str());
		}
	}
	if(m_szExt=="ttc")
	{
		TTCLocale ttc;
		if( false == ttc.Parse((LPCWSTR)(szFile)) )
		{
			MessageBox(L"Font file cannot be parsed!", L"Error", MB_ICONERROR);
			return;
		}
		else
		{
			std::vector<TTFLocale*> vec = ttc.GetListOfFont();

			CString szStr=L"";
			for(size_t i=0; i<ttc.Size(); ++i)
			{
				std::wostringstream stream;
				std::vector<int> lcids = vec[i]->GetListOfLCID();
				//m_szFontName = vec[i]->GetFontName(lcids[0]).c_str();

				int j=0;
				//for(size_t j=0; j<lcids.size();++j)
				{
					stream<<L"FontName : "<<ttc.GetFontName(i,lcids[j])<<L"\r\n\r\n";
					stream<<L"Copyright : "<<ttc.GetCopyright(i,lcids[j])<<L"\r\n\r\n";
					stream<<L"FontFamilyName : "<<ttc.GetFontFamilyName(i,lcids[j])<<L"\r\n\r\n";
					stream<<L"FontSubFamilyName : "<<ttc.GetFontSubFamilyName(i,lcids[j])<<L"\r\n\r\n";
					stream<<L"FontID : "<<ttc.GetFontID(i,lcids[j])<<L"\r\n\r\n";
					stream<<L"Version : "<<ttc.GetVersion(i,lcids[j])<<L"\r\n\r\n";
					stream<<L"PostScriptName : "<<ttc.GetPostScriptName(i,lcids[j])<<L"\r\n\r\n";
					stream<<L"Trademark : "<<ttc.GetTrademark(i,lcids[j])<<L"\r\n\r\n";
				}
				if(i!=ttc.Size()-1)
					stream<<L"=========================================="<<L"\r\n\r\n";

				szStr += stream.str().c_str();
			}
			if(ttc.Size()>0)
			{
				std::vector<int> lcids = vec[0]->GetListOfLCID();
				m_szFontName = vec[0]->GetFontName(lcids[0]).c_str();
			}
			m_edtInfo.SetWindowText(szStr);
		}
	}
	Invalidate();
}
