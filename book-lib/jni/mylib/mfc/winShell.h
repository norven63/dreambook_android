#pragma once

#if defined WIN32 || defined _WIN32_WCE
class WinShell
{
public:
	bool ShellEx(void* pAppName);
};
#endif