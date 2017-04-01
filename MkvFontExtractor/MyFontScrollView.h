#pragma once
#include "MkvFonts.h"
#include <vector>

// CMyFontScrollView view

class CMyFontScrollView : public CScrollView
{
	DECLARE_DYNCREATE(CMyFontScrollView)

public:
	void SetMkvFonts(MkvFonts* p) { m_pMkvFonts = p; }
	void ChangeBkgdColor();

private:
	MkvFonts* m_pMkvFonts;
	std::vector<COLORREF> m_listBkgdColor;
	size_t nBkgdColorIndex;

protected:
	CMyFontScrollView();           // protected constructor used by dynamic creation
	virtual ~CMyFontScrollView();
	bool DisplayAllFonts(CDC* pDC);
	COLORREF Alphablend(COLORREF dest, COLORREF source, BYTE nAlpha);

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

public:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnInitialUpdate();     // first time after construct

	DECLARE_MESSAGE_MAP()
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};


