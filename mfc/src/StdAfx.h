// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxcview.h>
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxtempl.h>
#include <afxconv.h>
#ifndef _WIN64
#include <afxdao.h>
#endif

extern void AutoSizeColumns(CListCtrl*);

#ifndef _countof
#define _countof(array) (sizeof(array)/sizeof((array)[0]))
#endif

#pragma warning(disable:4097)
