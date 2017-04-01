#pragma once

#include <vector>

struct tagAttachment
{
	int num;
	CString szFileName;
	CString szFontName;
};

class MkvFonts
{
public:
	MkvFonts(void);
	~MkvFonts(void);

	std::vector<tagAttachment>* Parse(const CString& szFile);
	bool GetMkvToolnixPath(CString& szPath);
	void Execute(LPCTSTR commandLine);
	HANDLE SpawnAndRedirect(LPCTSTR commandLine, HANDLE *hStdOutputReadPipe, LPCTSTR lpCurrentDirectory);
	bool ExtractAllFonts(const CString& szFolder);
	bool ParseFontFile(const CString& szFile, CString& szFontFamilyName);

	std::vector<tagAttachment>* m_pvecFontAttachments;
	CString m_szMkvToolnixPath;
	CString m_szMkvPath;
	CString m_szLocalAppPath;
};
