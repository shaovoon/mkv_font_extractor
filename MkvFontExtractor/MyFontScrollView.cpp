// MyScrollView.cpp : implementation file
//

#include "stdafx.h"
#include "MyFontScrollView.h"
#include <ctime>
#include <cstdio>
#include <algorithm>
#include <strsafe.h>

// CMyScrollView

IMPLEMENT_DYNCREATE(CMyFontScrollView, CScrollView)

CMyFontScrollView::CMyFontScrollView()
:
m_pMkvFonts(NULL),
nBkgdColorIndex(0)
{
	m_listBkgdColor.push_back(RGB(150,5,237));
	//m_listBkgdColor.push_back(RGB(34,150,255));
	m_listBkgdColor.push_back(RGB(255,146,36));
	m_listBkgdColor.push_back(RGB(0,176,176));
	m_listBkgdColor.push_back(RGB(236,60,152));

	using namespace std;
	srand((unsigned int)time(NULL));
	random_shuffle(m_listBkgdColor.begin(),m_listBkgdColor.end());
}

CMyFontScrollView::~CMyFontScrollView()
{
}


BEGIN_MESSAGE_MAP(CMyFontScrollView, CScrollView)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// CMyFontScrollView drawing

void CMyFontScrollView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	CSize sizeTotal;
	sizeTotal.cx = 200;
	sizeTotal.cy = 200;
	SetScrollSizes(MM_TEXT, sizeTotal);
}

void CMyFontScrollView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();

	DisplayAllFonts(pDC);
}

BOOL CMyFontScrollView::OnEraseBkgnd(CDC* pDC)
{
	CRect rect;
	GetClientRect(rect);
	//CBrush brush(RGB(255,255,255));
	CBrush brush(m_listBkgdColor.at(nBkgdColorIndex));
	pDC->FillRect(&rect,&brush);

	return TRUE;

	//return CScrollView::OnEraseBkgnd(pDC);
}

bool CMyFontScrollView::DisplayAllFonts(CDC* pDC)
{
	if(!m_pMkvFonts||!m_pMkvFonts->m_pvecFontAttachments)
		return false;

	int nHeight = 0;
	pDC->SetBkMode(TRANSPARENT);
	for(size_t i=0; i<m_pMkvFonts->m_pvecFontAttachments->size(); ++i)
	{
		LOGFONT lf;
		memset(&lf, 0, sizeof(lf));
		lf.lfHeight = -MulDiv(24, pDC->GetDeviceCaps(LOGPIXELSY), 72);
		lf.lfWeight = FW_NORMAL;
		lf.lfOutPrecision = OUT_TT_ONLY_PRECIS;
		int nSize = sizeof(lf.lfFaceName)/sizeof(wchar_t);
		StringCchCopyW(lf.lfFaceName, nSize, m_pMkvFonts->m_pvecFontAttachments->at(i).szFontName);

		// create and select it
		CFont newFont;
		if (!newFont.CreateFontIndirect(&lf))
			continue;
		CFont* pOldFont = pDC->SelectObject(&newFont);

		pDC->SetTextColor(Alphablend(m_listBkgdColor.at(nBkgdColorIndex),RGB(0,0,0),100));
		pDC->TextOut( 10+2, nHeight+2, lf.lfFaceName, wcslen(lf.lfFaceName));
		pDC->SetTextColor(RGB(0,0,0));
		pDC->TextOut( 10, nHeight, lf.lfFaceName, wcslen(lf.lfFaceName));
		CSize size = pDC->GetOutputTextExtent(lf.lfFaceName, wcslen(lf.lfFaceName));
		nHeight += size.cy;

		// Put back the old font
		pDC->SelectObject(pOldFont);	
	}
	CSize sizeTotal;
	sizeTotal.cx = 200;
	sizeTotal.cy = nHeight;
	SetScrollSizes(MM_TEXT, sizeTotal);

	return true;
}

COLORREF CMyFontScrollView::Alphablend(COLORREF dest, COLORREF source, BYTE nAlpha)
{
	if( 0 == nAlpha )
		return dest;

	if( 255 == nAlpha )
		return source;

	BYTE nInvAlpha = ~nAlpha;

	BYTE nSrcRed   = GetRValue(source); 
	BYTE nSrcGreen = GetGValue(source); 
	BYTE nSrcBlue  = GetBValue(source); 

	BYTE nDestRed   = GetRValue(dest); 
	BYTE nDestGreen = GetGValue(dest); 
	BYTE nDestBlue  = GetBValue(dest); 

	BYTE nRed  = ( nSrcRed   * nAlpha + nDestRed * nInvAlpha   )>>8;
	BYTE nGreen= ( nSrcGreen * nAlpha + nDestGreen * nInvAlpha )>>8;
	BYTE nBlue = ( nSrcBlue  * nAlpha + nDestBlue * nInvAlpha  )>>8;

	return RGB(nRed, nGreen, nBlue);
}

void CMyFontScrollView::ChangeBkgdColor()
{
	++nBkgdColorIndex;
	if(nBkgdColorIndex>=m_listBkgdColor.size())
		nBkgdColorIndex = 0;
}

// CMyScrollView diagnostics

#ifdef _DEBUG
void CMyFontScrollView::AssertValid() const
{
	CScrollView::AssertValid();
}

#ifndef _WIN32_WCE
void CMyFontScrollView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}
#endif
#endif //_DEBUG



