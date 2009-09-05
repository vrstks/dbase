# Microsoft Developer Studio Project File - Name="DBFExplorer" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=DBFExplorer - Win32 DLL Unicode Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "DBFExplorer.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "DBFExplorer.mak" CFG="DBFExplorer - Win32 DLL Unicode Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "DBFExplorer - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "DBFExplorer - Win32 DLL Debug" (based on "Win32 (x86) Application")
!MESSAGE "DBFExplorer - Win32 Unicode Release" (based on "Win32 (x86) Application")
!MESSAGE "DBFExplorer - Win32 DLL Unicode Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "DBFExplorer - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\vc6\release\dbfexplorer"
# PROP Intermediate_Dir "..\..\vc6\release\dbfexplorer"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /MT /W4 /GX /O1 /I "zlib" /D "__USE_FILE_OFFSET64" /Yu"stdafx.h" /FD /GF /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 version.lib /nologo /subsystem:windows /machine:I386 /out:"..\..\bin/DBFExplorer95.exe"

!ELSEIF  "$(CFG)" == "DBFExplorer - Win32 DLL Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\vc6\debug\dbfexplorer"
# PROP Intermediate_Dir "..\..\vc6\debug\dbfexplorer"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /MDd /W4 /Gm /GX /Zi /Od /I "zlib" /D "_DEBUG" /D "_AFXDLL" /D "__USE_FILE_OFFSET64" /Yu"stdafx.h" /FD /GZ /GF /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 version.lib /nologo /subsystem:windows /debug /machine:I386 /out:"..\..\bin/DBFExplorer95_d.6.exe" /pdbtype:sept

!ELSEIF  "$(CFG)" == "DBFExplorer - Win32 Unicode Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "DBFExplorer___Win32_Unicode_Release"
# PROP BASE Intermediate_Dir "DBFExplorer___Win32_Unicode_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\vc6\release\dbfexplorer\unicode"
# PROP Intermediate_Dir "..\..\vc6\release\dbfexplorer\unicode"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /MD /W4 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# SUBTRACT BASE CPP /nologo
# ADD CPP /MT /W4 /GX /O1 /I "zlib" /D "UNICODE" /D "_UNICODE" /D "__USE_FILE_OFFSET64" /Yu"stdafx.h" /FD /GF /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 version.lib /nologo /subsystem:windows /machine:I386 /out:"bin/DBFExplorer.exe"
# ADD LINK32 version.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /machine:I386 /out:"..\..\bin/DBFExplorer.exe"
# SUBTRACT LINK32 /nodefaultlib

!ELSEIF  "$(CFG)" == "DBFExplorer - Win32 DLL Unicode Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "DBFExplorer___Win32_Unicode_Debug"
# PROP BASE Intermediate_Dir "DBFExplorer___Win32_Unicode_Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\vc6\debug\dbfexplorer\unicode"
# PROP Intermediate_Dir "..\..\vc6\debug\dbfexplorer\unicode"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /MDd /W4 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# SUBTRACT BASE CPP /nologo
# ADD CPP /MDd /W4 /Gm /GX /Zi /Od /I "zlib" /D "_DEBUG" /D "UNICODE" /D "_UNICODE" /D "_AFXDLL" /D "__USE_FILE_OFFSET64" /Yu"stdafx.h" /FD /GZ /GF /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 version.lib /nologo /subsystem:windows /debug /machine:I386 /out:"bin/DBFExplorer_d.exe" /pdbtype:sept
# ADD LINK32 version.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /debug /machine:I386 /out:"..\..\bin/DBFExplorer_d.6.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "DBFExplorer - Win32 Release"
# Name "DBFExplorer - Win32 DLL Debug"
# Name "DBFExplorer - Win32 Unicode Release"
# Name "DBFExplorer - Win32 DLL Unicode Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\src\DBFExplorer.cpp
# End Source File
# Begin Source File

SOURCE=.\src\DBFExplorer.rc
# End Source File
# Begin Source File

SOURCE=.\src\DialogResize.cpp
# End Source File
# Begin Source File

SOURCE=.\src\FieldEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\src\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\src\msaccess.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RecordListBox.cpp
# End Source File
# Begin Source File

SOURCE=.\src\StaticLink.cpp
# End Source File
# Begin Source File

SOURCE=.\src\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\src\AboutDlg.h
# End Source File
# Begin Source File

SOURCE=.\src\AddFieldDlg.h
# End Source File
# Begin Source File

SOURCE=.\src\ChildFrm.h
# End Source File
# Begin Source File

SOURCE=..\dbf.h
# End Source File
# Begin Source File

SOURCE=..\dbf.hpp
# End Source File
# Begin Source File

SOURCE=..\dbf.inl
# End Source File
# Begin Source File

SOURCE=..\dbf_mfc.h
# End Source File
# Begin Source File

SOURCE=..\dbf_mfc.inl
# End Source File
# Begin Source File

SOURCE=.\src\DBFExplorer.h
# End Source File
# Begin Source File

SOURCE=.\src\DBFExplorerDoc.h
# End Source File
# Begin Source File

SOURCE=.\src\DBFExplorerView.h
# End Source File
# Begin Source File

SOURCE=.\src\DialogResize.h
# End Source File
# Begin Source File

SOURCE=.\src\ExportFileDialog.h
# End Source File
# Begin Source File

SOURCE=.\src\FieldEdit.h
# End Source File
# Begin Source File

SOURCE=.\ioapi\ioapi.h
# End Source File
# Begin Source File

SOURCE=.\src\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\src\MemoEditorDlg.h
# End Source File
# Begin Source File

SOURCE=.\src\RecordListBox.h
# End Source File
# Begin Source File

SOURCE=.\src\resource.h
# End Source File
# Begin Source File

SOURCE=.\src\StaticLink.h
# End Source File
# Begin Source File

SOURCE=.\src\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\src\StructureDlg.h
# End Source File
# Begin Source File

SOURCE=.\ioapi\zconf.h
# End Source File
# Begin Source File

SOURCE=.\ioapi\zlib.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\src\res\appicon.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\DBFExplorer.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\DBFExplorerDoc.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\finger.cur
# End Source File
# Begin Source File

SOURCE=.\src\res\Toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\src\res\xptheme.bin
# End Source File
# End Group
# Begin Group "ioapi"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ioapi\ioapi.c
# ADD CPP /I "..\..\zlib"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "dbf"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\dbf.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\dbf_mfc.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "Dialogs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\AboutDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\src\AddFieldDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\src\ExportFileDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\src\MemoEditorDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\src\PickDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\src\StructureDlg.cpp
# End Source File
# End Group
# Begin Group "Doc/View"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\ChildFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\src\DBFExplorerDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\src\DBFExplorerView.cpp
# End Source File
# End Group
# End Target
# End Project
