#include "winreg.h"

class CRegKey
{
public:
	CRegKey();
	virtual ~CRegKey();

public:
  LONG Open (HKEY hKeyRoot, char* pszPath);
  LONG Open (HKEY hKeyRoot,  UINT nID);

  void Close();

	LONG Write (char* pszKey, DWORD dwVal);
	LONG Write (char* pszKey, char* pszVal);
	LONG Write (char* pszKey, const BYTE* pData, DWORD dwLength);

	LONG Read (char* pszKey, DWORD& dwVal);
	LONG Read (char* pszKey, char* sVal);
	LONG Read (char* pszKey, BYTE* pData, DWORD& dwLength);

protected:
	HKEY 	m_hKey;
	char m_sPath[500];
};
