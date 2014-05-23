#pragma once


#ifdef WINDOW

bool WinEnumFilesInFolder(LPCTSTR strFolderPath,
						  LPCTSTR strFilter,
						  std::vector<std::wstring>& saFile,
						  DWORD flag);

DWORD WinGetTickCount();
void WinSleep(DWORD dwMiliSec);


void WinDeleteObject(HANDLE handle);

void WinGetLocalTime( LPSYSTEMTIME lpSystemTime ); 
void WinGetSystemTime( LPSYSTEMTIME lpSystemTime ); 


#endif