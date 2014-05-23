#pragma once


//////////////////////////////////////////////////////////////////////////
// º¯ÊýÉùÃ÷

DWORD GetPrivateProfileString(
	LPCWSTR lpAppName,
	LPCWSTR lpKeyName,
	LPCWSTR lpDefault, 
	LPWSTR  lpReturnedString,
	DWORD   Size,
	LPCWSTR lpFileName );

int GetPrivateProfileInt(
  LPCTSTR lpAppName,
  LPCTSTR lpKeyName, 
  INT nDefault,
  LPCTSTR lpFileName );

BOOL WritePrivateProfileString(
  LPCTSTR lpAppName,
  LPCTSTR lpKeyName,
  LPCTSTR lpString,
  LPCTSTR lpFileName);

BOOL WritePrivateProfileInt(
	LPCTSTR lpAppName,
	LPCTSTR lpKeyName,
	INT     Value,
	LPCTSTR lpFileName);

