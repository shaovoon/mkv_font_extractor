/*
Font Name Library 1.0.0

Copyright (c) 2009 Wong Shao Voon

The Code Project Open License (CPOL)
http://www.codeproject.com/info/cpol10.aspx
*/


#pragma once
#include <vector>
#include <string>
#include "TTFLocale.h"

class TTCLocale
{
public:
	//! default constructor
	TTCLocale(void);
	//! destructor
	~TTCLocale(void);

	struct stTTCHeader
	{
		CHAR tag[4];
		UINT Version;
		ULONG numFonts;
	};


	//! Parse file for font information
	bool Parse(const std::wstring& szFilePath);

	//! Parse the memory buffer for font information
	//!
	//! @param[in]	pFile is the buffer pointer
	//! @param[in]	size is the size of the buffer
	//! @return true for success
	bool Parse(BYTE* pFile, size_t size);

	//! Parse the memory buffer for font information
	//!
	//! @param[in]	pFile is the buffer pointer
	//! @param[in]	size is the size of the buffer
	//! @param[in]	offset is the offset of pFile from the starting byte
	//! @return true for success
	bool Parse(BYTE* pFile, size_t size, size_t offset);
	
	//! Size of the collection of fonts
	size_t Size() { return m_vecTtfPtr.size(); }

	//! Clear all information
	void Clear();

	std::wstring GetFontName(size_t nIndex, int codepage) {return m_vecTtfPtr.at(nIndex)->GetFontName(codepage);};
	std::wstring GetCopyright(size_t nIndex, int codepage) {return m_vecTtfPtr.at(nIndex)->GetCopyright(codepage);};
	std::wstring GetFontFamilyName(size_t nIndex, int codepage) {return m_vecTtfPtr.at(nIndex)->GetFontFamilyName(codepage);};
	std::wstring GetFontSubFamilyName(size_t nIndex, int codepage) {return m_vecTtfPtr.at(nIndex)->GetFontSubFamilyName(codepage);};
	std::wstring GetFontID(size_t nIndex, int codepage) {return m_vecTtfPtr.at(nIndex)->GetFontID(codepage);};
	std::wstring GetVersion(size_t nIndex, int codepage) {return m_vecTtfPtr.at(nIndex)->GetVersion(codepage);};
	std::wstring GetPostScriptName(size_t nIndex, int codepage) {return m_vecTtfPtr.at(nIndex)->GetPostScriptName(codepage);};
	std::wstring GetTrademark(size_t nIndex, int codepage) {return m_vecTtfPtr.at(nIndex)->GetTrademark(codepage);};

	bool IsBold(size_t nIndex) { return m_vecTtfPtr.at(nIndex)->IsBold(); };
	bool IsItalic(size_t nIndex) { return m_vecTtfPtr.at(nIndex)->IsItalic(); };
	bool IsRegular(size_t nIndex) { return m_vecTtfPtr.at(nIndex)->IsRegular(); };

	//! Converts a wide character string to narrow character string
	static std::string WideToNarrow(const std::wstring& szWide);
	//! Converts a narrow character string to wide character string
	//static std::wstring NarrowToWide(const std::string& szNarrow);

	std::vector<TTFLocale*> GetListOfFont()
	{
		return m_vecTtfPtr;
	}

private:
	//! Parse the font using the file
	bool ParseFont();
	//! Parse the font in memory
	bool ParseFontMem(BYTE* pFile, size_t size, size_t offset);

	//! File pointer
	FILE* m_pFile;
	//! The font file
	std::wstring m_szFilePath;
	//! Vector of TTF pointers
	std::vector<TTFLocale*> m_vecTtfPtr;

};
