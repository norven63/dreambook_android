#pragma once

#include "Platform.h"
#include "winuse.h"
#include "mBaseType.h"
#include "mFile.h"
#include "mCollection.h"
#include "mArchive.h"
#include "mCore.h"
#include "mFunc.h"
#include "mIni.h"
#include "mBmp.h"
#include "mSync.h"
#include "mMsg.h"
#include "mWin.h"
#include "UTF.h"
#include "endianDef.h"
#include "Socket.h"
#include "winShell.h"

#include "mCharset.h"
#include "ZipUtil.h"


#define GetTickCount XGetTickCount
#define Sleep XSleep
#define GetLocalTime XGetLocalTime
#define GetSystemTime XGetSystemTime
#define GetLastError XGetLastError
#define SetLastError XSetLastError
#undef ZeroMemory
#define ZeroMemory XZeroMemory
#define PlaySound XPlaySound

#undef isdigit
#undef isalpha
#undef isspace
#define isdigit xisdigit
#define isalpha xisalpha
#define isspace xisspace
#define isalnum xisalnum
