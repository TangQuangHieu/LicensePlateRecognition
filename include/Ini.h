/********************************************************************
	created:	2008/11/12 14:49
	filename: 	Ini.h
	author:		moon9kang
	version:	1.0
	update:		2008/11/12 14:49
	purpose:	
*********************************************************************/
// Ini.h: interface for the CIni class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INI_H__EA47B37E_5BFA_4980_B798_95609EC2C0C3__INCLUDED_)
#define AFX_INI_H__EA47B37E_5BFA_4980_B798_95609EC2C0C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>
#include <tchar.h>

// If MFC is linked, we will use CStringArray for great convenience
#ifdef __AFXWIN_H__
	#include <afxtempl.h>
#endif

// Number bases
#define BASE_BINARY			2
#define BASE_OCTAL			8
#define BASE_DECIMAL		10
#define BASE_HEXADECIMAL	16

namespace mg {

//---------------------------------------------------------------
//	    Callback Function Type Definition
//---------------------------------------------------------------
// The callback function used for parsing a "double-null terminated string".
// When called, the 1st parameter passed in will store the newly extracted sub
// string, the 2nd parameter is a 32-bit user defined data, this parameter can
// be NULL. The parsing will terminate if this function returns zero. To use
// the callback, function pointer needs to be passed to "CIni::ParseDNTString".
typedef BOOL (CALLBACK *SUBSTRPROC)(LPCTSTR, LPVOID);

class CIni
{
public:		

	//-----------------------------------------------------------
	//    Constructors & Destructor
	//-----------------------------------------------------------
	CIni(); // Default constructor
	CIni(LPCTSTR lpPathName); // Construct with a given file name
	virtual ~CIni();

	//-----------------------------------------------------------
	//    Ini File Path Name Access
	//-----------------------------------------------------------
	void SetPathName(LPCTSTR lpPathName); // Specify a new file name
	DWORD GetPathName(LPTSTR lpBuffer, DWORD dwBufSize) const; // Retrieve current file name
#ifdef __AFXWIN_H__
	CString GetPathName() const;
#endif
	
	//------------------------------------------------------------
	//    String Access
	//------------------------------------------------------------	
	DWORD GetString(LPCTSTR lpSection, LPCTSTR lpKey, LPTSTR lpBuffer, DWORD dwBufSize, LPCTSTR lpDefault = NULL) const;
#ifdef __AFXWIN_H__
	CString GetString(LPCTSTR lpSection, LPCTSTR lpKey, LPCTSTR lpDefault = NULL) const;
#endif
	BOOL WriteString(LPCTSTR lpSection, LPCTSTR lpKey, LPCTSTR lpValue) const;

	// Read a string from the ini file, append it with another string then write it
	// back to the ini file.
	BOOL AppendString(LPCTSTR Section, LPCTSTR lpKey, LPCTSTR lpString) const;
	
	//------------------------------------------------------------
	//    Ini File String Array Access
	//------------------------------------------------------------	
	// Parse the string retrieved from the ini file and split it into a set of sub strings.
	DWORD GetArray(LPCTSTR lpSection, LPCTSTR lpKey, LPTSTR lpBuffer, DWORD dwBufSize, LPCTSTR lpDelimiter = NULL, BOOL bTrimString = TRUE) const;
#ifdef __AFXWIN_H__
	void GetArray(LPCTSTR lpSection, LPCTSTR lpKey, CStringArray* pArray, LPCTSTR lpDelimiter = NULL, BOOL bTrimString = TRUE) const;
	BOOL WriteArray(LPCTSTR lpSection, LPCTSTR lpKey, const CStringArray* pArray, int nWriteCount = -1, LPCTSTR lpDelimiter = NULL) const;
#endif	
	
	//------------------------------------------------------------
	//    Primitive Data Type Access
	//------------------------------------------------------------
	int GetInt(LPCTSTR lpSection, LPCTSTR lpKey, int nDefault=0, int nBase = BASE_DECIMAL) const;
	BOOL WriteInt(LPCTSTR lpSection, LPCTSTR lpKey, int nValue, int nBase = BASE_DECIMAL) const;
	BOOL IncreaseInt(LPCTSTR lpSection, LPCTSTR lpKey, int nIncrease = 1, int nBase = BASE_DECIMAL) const;
	
	UINT GetUInt(LPCTSTR lpSection, LPCTSTR lpKey, UINT nDefault=0, int nBase = BASE_DECIMAL) const;
	BOOL WriteUInt(LPCTSTR lpSection, LPCTSTR lpKey, UINT nValue, int nBase = BASE_DECIMAL) const;
	BOOL IncreaseUInt(LPCTSTR lpSection, LPCTSTR lpKey, UINT nIncrease = 1, int nBase = BASE_DECIMAL) const;
	
	BOOL GetBool(LPCTSTR lpSection, LPCTSTR lpKey, BOOL bDefault=0) const;
	BOOL WriteBool(LPCTSTR lpSection, LPCTSTR lpKey, BOOL bValue) const;
	BOOL InvertBool(LPCTSTR lpSection, LPCTSTR lpKey) const;
	
	double GetDouble(LPCTSTR lpSection, LPCTSTR lpKey, double fDefault=0) const;
	BOOL WriteDouble(LPCTSTR lpSection, LPCTSTR lpKey, double fValue, int nPrecision = -1) const;
	BOOL IncreaseDouble(LPCTSTR lpSection, LPCTSTR lpKey, double fIncrease, int nPrecision = -1) const;

	TCHAR GetChar(LPCTSTR lpSection, LPCTSTR lpKey, TCHAR cDefault=0) const;
	BOOL WriteChar(LPCTSTR lpSection, LPCTSTR lpKey, TCHAR c) const;

	//------------------------------------------------------------
	//    User-Defined Data Type & Data Block Access
	//------------------------------------------------------------
	POINT GetPoint(LPCTSTR lpSection, LPCTSTR lpKey, POINT ptDefault) const;
	BOOL WritePoint(LPCTSTR lpSection, LPCTSTR lpKey, POINT pt) const;
	
	RECT GetRect(LPCTSTR lpSection, LPCTSTR lpKey, RECT rcDefault) const;
	BOOL WriteRect(LPCTSTR lpSection, LPCTSTR lpKey, RECT rc) const;

	DWORD GetDataBlock(LPCTSTR lpSection, LPCTSTR lpKey, LPVOID lpBuffer, DWORD dwBufSize, DWORD dwOffset = 0) const;
	BOOL WriteDataBlock(LPCTSTR lpSection, LPCTSTR lpKey, LPCVOID lpData, DWORD dwDataSize) const;
	BOOL AppendDataBlock(LPCTSTR lpSection, LPCTSTR lpKey, LPCVOID lpData, DWORD dwDataSize) const;
	
	//------------------------------------------------------------
	//    Section Operations
	//------------------------------------------------------------
	BOOL IsSectionExist(LPCTSTR lpSection) const;
	DWORD GetSectionNames(LPTSTR lpBuffer, DWORD dwBufSize) const;
#ifdef __AFXWIN_H__
	void GetSectionNames(CStringArray* pArray) const;
#endif
	BOOL CopySection(LPCTSTR lpSrcSection, LPCTSTR lpDestSection, BOOL bFailIfExist) const;
	BOOL MoveSection(LPCTSTR lpSrcSection, LPCTSTR lpDestSection, BOOL bFailIfExist = TRUE) const;
	BOOL DeleteSection(LPCTSTR lpSection) const;
	
	//------------------------------------------------------------
	//    Key Operations
	//------------------------------------------------------------
	BOOL IsKeyExist(LPCTSTR lpSection, LPCTSTR lpKey) const;	
	DWORD GetKeyLines(LPCTSTR lpSection, LPTSTR lpBuffer, DWORD dwBufSize) const;
#ifdef __AFXWIN_H__
	void GetKeyLines(LPCTSTR lpSection, CStringArray* pArray) const;
#endif
	DWORD GetKeyNames(LPCTSTR lpSection, LPTSTR lpBuffer, DWORD dwBufSize) const;
#ifdef __AFXWIN_H__	
	void GetKeyNames(LPCTSTR lpSection, CStringArray* pArray) const;
#endif
	BOOL CopyKey(LPCTSTR lpSrcSection, LPCTSTR lpSrcKey, LPCTSTR lpDestSection, LPCTSTR lpDestKey, BOOL bFailIfExist) const;
	BOOL MoveKey(LPCTSTR lpSrcSection, LPCTSTR lpSrcKey, LPCTSTR lpDestSection, LPCTSTR lpDestKey, BOOL bFailIfExist = TRUE) const;
	BOOL DeleteKey(LPCTSTR lpSection, LPCTSTR lpKey) const;

	//------------------------------------------------------------
	// Parse a "Double-Null Terminated String"
	//------------------------------------------------------------
	static BOOL ParseDNTString(LPCTSTR lpString, SUBSTRPROC lpFnStrProc, LPVOID lpParam = NULL);

	//------------------------------------------------------------
	// Check for Whether a String Representing TRUE or FALSE
	//------------------------------------------------------------
	static BOOL StringToBool(LPCTSTR lpString, BOOL bDefault = FALSE);
		
protected:	

	//------------------------------------------------------------
	//    Helper Functions
	//------------------------------------------------------------
	static LPTSTR __StrDupEx(LPCTSTR lpStart, LPCTSTR lpEnd);
	static BOOL __TrimString(LPTSTR lpBuffer);
	LPTSTR __GetStringDynamic(LPCTSTR lpSection, LPCTSTR lpKey, LPCTSTR lpDefault = NULL) const;
	static DWORD __StringSplit(LPCTSTR lpString, LPTSTR lpBuffer, DWORD dwBufSize, LPCTSTR lpDelimiter = NULL, BOOL bTrimString = TRUE);
	static void __ToBinaryString(UINT nNumber, LPTSTR lpBuffer, DWORD dwBufSize);
	static int __ValidateBase(int nBase);
	static void __IntToString(int nNumber, LPTSTR lpBuffer, int nBase);
	static void __UIntToString(UINT nNumber, LPTSTR lpBuffer, int nBase);
	static BOOL CALLBACK __SubStrCompare(LPCTSTR lpString1, LPVOID lpParam);
	static BOOL CALLBACK __KeyPairProc(LPCTSTR lpString, LPVOID lpParam);	
#ifdef __AFXWIN_H__
	static BOOL CALLBACK __SubStrAdd(LPCTSTR lpString, LPVOID lpParam);
#endif

	//------------------------------------------------------------
	//    Member Data
	//------------------------------------------------------------
	LPTSTR m_pszPathName; // Stores path of the associated ini file
};

} // mg

#endif // !defined(AFX_INI_H__EA47B37E_5BFA_4980_B798_95609EC2C0C3__INCLUDED_)
inline CString MgGetModuleFileName()
{
	TCHAR buff[256];
	GetModuleFileName(NULL, buff, sizeof(buff));
	return buff;
}
inline CString MgGetFileExt(LPCTSTR pszFullPath)
{
	TCHAR ext[_MAX_EXT];
	_tsplitpath(pszFullPath, NULL, NULL, NULL, ext);
	return ext;
}
inline BOOL MgIsDirectoryExists(LPCTSTR szDirPath)
{
	BOOL isExists = FALSE;
	DWORD dwAttr = GetFileAttributes(szDirPath);
	//if ((dwAttr != 0xFFFFFFFFUL) && (dwAttr&FILE_ATTRIBUTE_DIRECTORY))
	if ((dwAttr != (DWORD)-1) && (dwAttr&FILE_ATTRIBUTE_DIRECTORY))
		isExists = TRUE;

	return isExists;
}
inline CString MgFormatString(LPCTSTR szFormat, ...)
{
	TCHAR buffer[4096];
	va_list args;
	va_start(args, szFormat);
	_vstprintf(buffer, szFormat, args); //wvsprintf
	va_end(args);
	return CString(buffer);
}
inline FILETIME MgFileTime(unsigned __int64 ft64)
{
	FILETIME ft = { ft64 & 0xFFFFFFFF, ft64 >> 32 };
	return ft;
}
inline CString MgGetFileName(LPCTSTR pszFullPath)
{
	TCHAR fileTitle[_MAX_FNAME];
	TCHAR ext[_MAX_EXT];
	_tsplitpath(pszFullPath, NULL, NULL, fileTitle, ext);
	CString fileName = fileTitle;
	fileName += ext;
	return fileName;
}

inline CString MgGetModuleFileDirectory()
{
	TCHAR CurDir[_MAX_PATH] = { 0, };
	int index = ::GetModuleFileName(NULL, CurDir, sizeof(CurDir));
	index++;
	while (CurDir[--index] != '\\') {
		CurDir[index] = 0x00;
	}
	CurDir[index] = 0x00;
	return CString(CurDir);
}
