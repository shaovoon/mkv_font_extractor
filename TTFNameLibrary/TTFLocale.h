/*
Font Name Library 1.0.0

Copyright (c) 2009 Wong Shao Voon

The Code Project Open License (CPOL)
http://www.codeproject.com/info/cpol10.aspx
*/

#pragma once

#include <string>
#include <vector>
#include <map>
#include <cstdio>
#include "EndianSwap.h"
#include "MemPtrReader.h"

class TTFLocale
{
public:
	//! default constructor
	TTFLocale(void);
	//! destructor
	~TTFLocale(void);

	struct stOffsetTable
	{
		UINT	version;
		USHORT	numTables;
		USHORT	searchRange;
		USHORT	entrySelector;
		USHORT	rangeShift;
	};

	struct stTableDirectory
	{
		CHAR	tag[4];
		ULONG	checkSum;
		ULONG	offset;
		ULONG	length;
	};
	
	struct stNamingTable
	{
		USHORT	FormatSelector;
		USHORT	NumberOfNameRecords;
		USHORT	OffsetStartOfStringStorage;
	};

	struct stNameRecord
	{
		USHORT	PlatformID;
		USHORT	EncodingID;
		USHORT	LanguageID;
		USHORT	NameID;
		USHORT	StringLength;
		USHORT	OffsetFromStorageArea;
	};

	//! Parse file for font information
	bool Parse(const std::wstring& szFilePath);

	//! Meant to be used by TTC class only
	bool Parse(FILE* pFile, size_t nOffsetFromStart);

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

	//! Clear all information
	void Clear();

	std::wstring GetFontName(int lcid) {return m_mapLCIDtoNames[lcid].m_szFontName;};
	std::wstring GetCopyright(int lcid) {return m_mapLCIDtoNames[lcid].m_szCopyright;};
	std::wstring GetFontFamilyName(int lcid) {return m_mapLCIDtoNames[lcid].m_szFontFamilyName;};
	std::wstring GetFontSubFamilyName(int lcid) {return m_mapLCIDtoNames[lcid].m_szFontSubFamilyName;};
	std::wstring GetFontID(int lcid) {return m_mapLCIDtoNames[lcid].m_szFontID;};
	std::wstring GetVersion(int lcid) {return m_mapLCIDtoNames[lcid].m_szVersion;};
	std::wstring GetPostScriptName(int lcid) {return m_mapLCIDtoNames[lcid].m_szPostScriptName;};
	std::wstring GetTrademark(int lcid) {return m_mapLCIDtoNames[lcid].m_szTrademark;};

	bool IsBold();
	bool IsItalic();
	bool IsRegular();

	//! Converts a wide character string to narrow character string
	static std::string WideToNarrow(const std::wstring& szWide);
	//! Converts a narrow character string to wide character string
	static std::wstring NarrowToWide(const std::string& szNarrow, LCID lcid);

	//std::vector<int> GetListOfCodepage();
	std::vector<int> GetListOfLCID();

private:
	//! Parse the OffsetTable
	bool ParseOffsetTable();
	//! Parse the OffsetTable in memory
	bool ParseOffsetTableMem();
	//! Parse the Naming tables for names
	bool ParseName(stTableDirectory& TableDirectory);
	//! Parse the Naming tables for names in memory buffer
	bool ParseNameMem(stTableDirectory& TableDirectory);
	//! Parse for the bold, italic or regular font properties
	void ParseStyles(int lcid);

	int ConvertLCIDtoCodePage(LCID lcid);



	//! Get the descriptive name for the numeric name id
	std::wstring GetNameID(USHORT uNameID);

	//! Set the (wide string) value to the name id
	void SetNameIDValue(USHORT uNameID, const std::wstring& str, LCID lcid);
	//! Set the (narrow string) value to the name id
	void SetNameIDValue(USHORT uNameID, const std::string& str, LCID lcid);

	//! Convert all wide characters to lower case
	std::wstring ToLower(const std::wstring& szNormal);
	//! Convert all narrow characters to lower case
	std::string ToLower(const std::string& szNormal);

	//! File pointer
	std::FILE* m_pFile;
	//! The font file
	std::wstring m_szFilePath;
	
	struct stNames
	{
		//! Font name
		std::wstring m_szFontName;
		//! Copyright
		std::wstring m_szCopyright;
		//! Font family name
		std::wstring m_szFontFamilyName;
		//! States whether the font is bold, italics or regular
		std::wstring m_szFontSubFamilyName;
		//! Font ID
		std::wstring m_szFontID;
		//! Font version text
		std::wstring m_szVersion;
		//! Font Postscript name
		std::wstring m_szPostScriptName;
		//! Trademark
		std::wstring m_szTrademark;
	};
	std::map<LCID, int> m_mapLCIDtoCodepage;
	std::map<int, stNames> m_mapLCIDtoNames;
	//! Indicates if the file pointer is passed in from caller
	bool m_bExternalFilePtr;
	//! Memory buffer reader
	MemPtrReader* m_pMemPtrReader;
	//! IsBold?
	bool m_bBold;
	//! IsRegular?
	bool m_bRegular;
	//! IsItalic?
	bool m_bItalic;
};
