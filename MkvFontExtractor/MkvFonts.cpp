#include "StdAfx.h"
#include "MkvFonts.h"
#include "../TTFNameLibrary/TTFLocale.h"
#include "../TTFNameLibrary/TTCLocale.h"

MkvFonts::MkvFonts(void)
:
m_pvecFontAttachments(NULL)
{
}

MkvFonts::~MkvFonts(void)
{
	if(m_pvecFontAttachments)
	{
		for(size_t i=0; i<m_pvecFontAttachments->size(); ++i)
		{
			BOOL b = RemoveFontResourceEx(
				m_pvecFontAttachments->at(i).szFileName, 		// name of font file
				FR_PRIVATE,   		
				NULL         		
				);
		}
		delete m_pvecFontAttachments;
		m_pvecFontAttachments = NULL;
	}
}

std::vector<tagAttachment>* MkvFonts::Parse(const CString& szFile)
{
	CStdioFile file(szFile, CFile::modeRead | CFile::typeText );

	CString szRead;
	CString szFileName;
	int nAttachment=0;
	bool bAttachSection = false;
	tagAttachment attachment;
	std::vector<tagAttachment>* pVec = new std::vector<tagAttachment>();
	pVec->reserve(10);
	while(file.ReadString(szRead))
	{
		if(szRead==_T("|+ Chapters"))
			bAttachSection = false;

		if(szRead==_T("|+ Attachments"))
			bAttachSection = true;

		if(bAttachSection)
		{
			if(szRead==_T("| + Attached"))
				++nAttachment;

			if(szRead.Find(_T("|  + File name: "))==0)
			{
				int pos = szRead.Find(_T(": "));
				if(pos!=-1)
				{
					szFileName = 
						szRead.Right(szRead.GetLength()-2-pos);
				}
			}
			if(szRead.Find(_T("|  + Mime type: "))==0)
			{
				int pos = szRead.Find(_T("x-truetype-font"));
				if(pos!=-1)
				{
					attachment.szFileName = szFileName;
					attachment.num = nAttachment;
					pVec->push_back(attachment);
				}
			}
		}
	}
	return pVec;
}

bool MkvFonts::GetMkvToolnixPath(CString& szPath)
{
	HKEY keyHandle;
	TCHAR rgValue [MAX_PATH];
	DWORD size1;
	DWORD Type;

	bool b = true;
	memset(rgValue, 0, sizeof(rgValue));
	if( RegOpenKeyEx(    HKEY_LOCAL_MACHINE,
		_T("SOFTWARE\\mkvmergeGUI\\GUI"),0,
		KEY_QUERY_VALUE, &keyHandle) == ERROR_SUCCESS)
	{
		size1=MAX_PATH;
		RegQueryValueEx( keyHandle, _T("installation_path"), NULL, &Type,
			(LPBYTE)rgValue,&size1);
		szPath.Format(_T("%s"),rgValue);
	}     
	else 
		b = false;

	RegCloseKey(keyHandle);

	return b;
}

HANDLE MkvFonts::SpawnAndRedirect(LPCTSTR commandLine, HANDLE *hStdOutputReadPipe, LPCTSTR lpCurrentDirectory)
{
	HANDLE hStdOutputWritePipe, hStdOutput, hStdError;
	CreatePipe(hStdOutputReadPipe, &hStdOutputWritePipe, NULL, 0);	// create a non-inheritable pipe
	DuplicateHandle(GetCurrentProcess(), hStdOutputWritePipe,
		GetCurrentProcess(), &hStdOutput,	// duplicate the "write" end as inheritable stdout
		0, TRUE, DUPLICATE_SAME_ACCESS);
	DuplicateHandle(GetCurrentProcess(), hStdOutput,
		GetCurrentProcess(), &hStdError,	// duplicate stdout as inheritable stderr
		0, TRUE, DUPLICATE_SAME_ACCESS);
	CloseHandle(hStdOutputWritePipe);								// no longer need the non-inheritable "write" end of the pipe

	PROCESS_INFORMATION pi;
	STARTUPINFO si;
	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	si.dwFlags = STARTF_USESTDHANDLES|STARTF_USESHOWWINDOW;
	si.hStdInput  = GetStdHandle(STD_INPUT_HANDLE);	// (this is bad on a GUI app)
	si.hStdOutput = hStdOutput;
	si.hStdError  = hStdError;
	si.wShowWindow = SW_HIDE;						// IMPORTANT: hide subprocess console window
	TCHAR commandLineCopy[1024];					// CreateProcess requires a modifiable buffer
	_tcscpy(commandLineCopy, commandLine);
	if (!CreateProcess(	NULL, commandLineCopy, NULL, NULL, TRUE,
		CREATE_NEW_CONSOLE, NULL, lpCurrentDirectory, &si, &pi))
	{
		CloseHandle(hStdOutput);
		CloseHandle(hStdError);
		CloseHandle(*hStdOutputReadPipe);
		*hStdOutputReadPipe = INVALID_HANDLE_VALUE;
		return NULL;
	}

	CloseHandle(pi.hThread);
	CloseHandle(hStdOutput);
	CloseHandle(hStdError);
	return pi.hProcess;
}

void MkvFonts::Execute(LPCTSTR commandLine)
{
	HANDLE hOutput, hProcess;
	hProcess = SpawnAndRedirect(commandLine, &hOutput, NULL);
	if (!hProcess) return;

	CHAR buffer[65];
	DWORD read;
	while (ReadFile(hOutput, buffer, 64, &read, NULL))
	{
		buffer[read] = '\0';
	}
	CloseHandle(hOutput);
	CloseHandle(hProcess);
}

bool MkvFonts::ExtractAllFonts(const CString& szFolder)
{
	if(!m_pvecFontAttachments)
		return false;

	for(size_t i=0; i<m_pvecFontAttachments->size(); ++i)
	{
		CString szMkvExtractExe = m_szMkvToolnixPath;

		PathAppend(szMkvExtractExe.GetBuffer(MAX_PATH), _T("mkvextract.exe"));
		szMkvExtractExe.ReleaseBuffer();

		CString szFontFile = m_pvecFontAttachments->at(i).szFileName;
		CString szFontPath = szFolder;
		PathAppend(szFontPath.GetBuffer(MAX_PATH), 
			szFontFile);
		szFontPath.ReleaseBuffer();

		CString szCmdline;
		szCmdline.Format(_T("\"%s\" attachments \"%s\" %d:\"%s\""), 
			szMkvExtractExe, m_szMkvPath, m_pvecFontAttachments->at(i).num, szFontPath);

		Execute(szCmdline);

		int nResults = AddFontResourceEx(
			szFontPath,
			FR_PRIVATE,
			NULL);

		ParseFontFile(szFontPath, m_pvecFontAttachments->at(i).szFontName);
	}

	return true;
}

bool MkvFonts::ParseFontFile(const CString& szFile, CString& szFontFamilyName)
{
	if(szFile.IsEmpty())
	{
		return false;
	}
	if(FALSE==PathFileExists(szFile))
	{
		return false;
	}

	CString szExt = szFile.Right(4);
	szExt == szExt.MakeLower();
	if(szExt==_T(".ttf")||szExt==_T(".otf"))
	{
		TTFLocale ttf;
		if( false == ttf.Parse((LPCWSTR)(szFile)) )
		{
			return false;
		}
		else
		{
			std::vector<int> lcids = ttf.GetListOfLCID();
			if(lcids.size()>0)
				szFontFamilyName = ttf.GetFontName(lcids[0]).c_str();
		}
	}
	if(szExt==_T(".ttc"))
	{
		TTCLocale ttc;
		if( false == ttc.Parse((LPCWSTR)(szFile)) )
		{
			return false;
		}
		else
		{
			std::vector<TTFLocale*> fonts = ttc.GetListOfFont();
			if(fonts.size()>0)
			{
				std::vector<int> codepages = fonts[0]->GetListOfLCID();
				if(codepages.size()>0)
					szFontFamilyName = fonts[0]->GetFontName(codepages[0]).c_str();
			}
		}
	}

	return true;
}
