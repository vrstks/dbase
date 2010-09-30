# Microsoft Developer Studio Project File - Name="dbf_wx" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=dbf_wx - Win32 DLL Unicode Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "dbf_wx.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "dbf_wx.mak" CFG="dbf_wx - Win32 DLL Unicode Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "dbf_wx - Win32 DLL Unicode Debug" (based on "Win32 (x86) Application")
!MESSAGE "dbf_wx - Win32 DLL Debug" (based on "Win32 (x86) Application")
!MESSAGE "dbf_wx - Win32 Unicode Release" (based on "Win32 (x86) Application")
!MESSAGE "dbf_wx - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "dbf_wx - Win32 DLL Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswuddll"
# PROP BASE Intermediate_Dir "vc_mswuddll\dbf_wx"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\vc6\debug\dbf_wx\unicode"
# PROP Intermediate_Dir "..\..\vc6\debug\dbf_wx\unicode"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W4 /Gm /GR /Zi /Od /I ".\..\..\lib\vc_dll\mswud" /I ".\..\..\include" /I "." /I ".\..\..\samples" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /Fd"vc_mswuddll\dbf_wx.pdb" /FD /GZ /EHsc /c
# ADD CPP /MDd /W4 /Gm /GR /Zi /Od /I "$(WXWIN)\lib\vc_dll\mswud" /I "$(WXWIN)\include" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_UNICODE" /Yu"precomp.h" /Fd"..\..\vc6\debug\dbf_wx\unicode\dbf_wx.pdb" /FD /GZ /EHsc /GF /c
# ADD BASE MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i ".\..\..\lib\vc_dll\mswud" /i ".\..\..\include" /i "." /i ".\..\..\samples" /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d "_UNICODE" /d "WXUSINGDLL" /d "_WINDOWS" /d "NOPCH"
# ADD RSC /l 0x409 /i "$(WXWIN)\lib\vc_dll\mswud" /i "$(WXWIN)\include" /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d "_UNICODE" /d "WXUSINGDLL" /d "_WINDOWS" /d "NOPCH"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /subsystem:windows /debug /machine:I386 /libpath:".\..\..\lib\vc_dll"
# ADD LINK32 user32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"..\..\bin/wxdbf_d.6.exe" /libpath:"$(WXWIN)\lib\vc_dll"

!ELSEIF  "$(CFG)" == "dbf_wx - Win32 DLL Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswddll"
# PROP BASE Intermediate_Dir "vc_mswddll\dbf_wx"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\vc6\debug\dbf_wx"
# PROP Intermediate_Dir "..\..\vc6\debug\dbf_wx"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W4 /Gm /GR /Zi /Od /I ".\..\..\lib\vc_dll\mswd" /I ".\..\..\include" /I "." /I ".\..\..\samples" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /Fd"vc_mswddll\dbf_wx.pdb" /FD /GZ /EHsc /c
# ADD CPP /MDd /W4 /Gm /GR /Zi /Od /I "$(WXWIN)\lib\vc_dll\mswd" /I "$(WXWIN)\include" /D "__WXDEBUG__" /D "WXUSINGDLL" /Yu"precomp.h" /Fd"..\..\vc6\debug\dbf_wx\dbf_wx.pdb" /FD /GZ /EHsc /GF /c
# ADD BASE MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i ".\..\..\lib\vc_dll\mswd" /i ".\..\..\include" /i "." /i ".\..\..\samples" /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d "WXUSINGDLL" /d "_WINDOWS" /d "NOPCH"
# ADD RSC /l 0x409 /i "$(WXWIN)\lib\vc_dll\mswd" /i "$(WXWIN)\include" /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d "WXUSINGDLL" /d "_WINDOWS" /d "NOPCH"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /subsystem:windows /debug /machine:I386 /libpath:".\..\..\lib\vc_dll"
# ADD LINK32 user32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"..\..\bin/wxdbf95_d.6.exe" /libpath:"$(WXWIN)\lib\vc_dll"

!ELSEIF  "$(CFG)" == "dbf_wx - Win32 Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_mswu"
# PROP BASE Intermediate_Dir "vc_mswu\dbf_wx"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\vc6\release\dbf_wx\unicode"
# PROP Intermediate_Dir "..\..\vc6\release\dbf_wx\unicode"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W4 /GR /O2 /I ".\..\..\lib\vc_lib\mswu" /I ".\..\..\include" /I "." /I ".\..\..\samples" /D "WIN32" /D "__WXMSW__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /Fd"vc_mswu\dbf_wx.pdb" /FD /EHsc /c
# ADD CPP /MT /W4 /GR /O1 /I "$(WXWIN)\lib\vc_lib\mswu" /I "$(WXWIN)\include" /D "_UNICODE" /Yu"precomp.h" /Fd"..\..\vc6\release\dbf_wx\dbf_wx.pdb" /FD /EHsc /GF /c
# ADD BASE MTL /nologo /D "WIN32" /D "__WXMSW__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "__WXMSW__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i ".\..\..\lib\vc_lib\mswu" /i ".\..\..\include" /i "." /i ".\..\..\samples" /d "__WXMSW__" /d "_UNICODE" /d "_WINDOWS" /d "NOPCH"
# ADD RSC /l 0x409 /i "$(WXWIN)\lib\vc_lib\mswu" /i "$(WXWIN)\include" /d "__WXMSW__" /d "_UNICODE" /d "_WINDOWS" /d "NOPCH"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /subsystem:windows /machine:I386 /libpath:".\..\..\lib\vc_lib"
# ADD LINK32 gdi32.lib kernel32.lib user32.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib /nologo /subsystem:windows /machine:I386 /out:"..\..\bin/wxdbf.exe" /libpath:"$(WXWIN)\lib\vc_lib"

!ELSEIF  "$(CFG)" == "dbf_wx - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_msw"
# PROP BASE Intermediate_Dir "vc_msw\dbf_wx"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\vc6\release\dbf_wx"
# PROP Intermediate_Dir "..\..\vc6\release\dbf_wx"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W4 /GR /O2 /I ".\..\..\lib\vc_lib\msw" /I ".\..\..\include" /I "." /I ".\..\..\samples" /D "WIN32" /D "__WXMSW__" /D "_WINDOWS" /D "NOPCH" /Fd"vc_msw\dbf_wx.pdb" /FD /EHsc /c
# ADD CPP /MT /W4 /GR /O1 /I "$(WXWIN)\lib\vc_lib\msw" /I "$(WXWIN)\include" /Yu"precomp.h" /Fd"..\..\vc6\release\dbf_wx\dbf_wx.pdb" /FD /EHsc /GF /c
# ADD BASE MTL /nologo /D "WIN32" /D "__WXMSW__" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "__WXMSW__" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i ".\..\..\lib\vc_lib\msw" /i ".\..\..\include" /i "." /i ".\..\..\samples" /d "__WXMSW__" /d "_WINDOWS" /d "NOPCH"
# ADD RSC /l 0x409 /i "$(WXWIN)\lib\vc_lib\msw" /i "$(WXWIN)\include" /d "__WXMSW__" /d "_WINDOWS" /d "NOPCH"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /subsystem:windows /machine:I386 /libpath:".\..\..\lib\vc_lib"
# ADD LINK32 gdi32.lib kernel32.lib user32.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib /nologo /subsystem:windows /machine:I386 /out:"..\..\bin/wxdbf95.exe" /libpath:"$(WXWIN)\lib\vc_lib"

!ENDIF 

# Begin Target

# Name "dbf_wx - Win32 DLL Unicode Debug"
# Name "dbf_wx - Win32 DLL Debug"
# Name "dbf_wx - Win32 Unicode Release"
# Name "dbf_wx - Win32 Release"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\art.cpp
# End Source File
# Begin Source File

SOURCE=.\src\app.cpp
# End Source File
# Begin Source File

SOURCE=.\src\appframe.cpp
# End Source File
# Begin Source File

SOURCE=.\src\datalist.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\src\datamodel.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\src\dbfdlgs.cpp
# End Source File
# Begin Source File

SOURCE=.\src\dbflist.cpp
# End Source File
# Begin Source File

SOURCE=.\src\dbfmodel.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\src\dbfutil.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\src\link.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\src\precomp.cpp
# ADD CPP /Yc"precomp.h"
# End Source File
# Begin Source File

SOURCE=.\src\wxdbf.rc
# End Source File
# Begin Source File

SOURCE=.\src\wx29.cpp
# End Source File
# Begin Source File

SOURCE=.\src\wxext.cpp
# End Source File
# Begin Source File

SOURCE=.\src\wxstreamc.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\app.h
# End Source File
# Begin Source File

SOURCE=.\src\appframe.h
# End Source File
# Begin Source File

SOURCE=.\src\datamodel.h
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

SOURCE=..\dbf_wx.h
# End Source File
# Begin Source File

SOURCE=..\dbf_wx.inl
# End Source File
# Begin Source File

SOURCE=.\src\dbfdlgs.h
# End Source File
# Begin Source File

SOURCE=.\src\dbfdoc.h
# End Source File
# Begin Source File

SOURCE=.\src\dbfframe.h
# End Source File
# Begin Source File

SOURCE=.\src\dbflist.h
# End Source File
# Begin Source File

SOURCE=.\src\dbfmodel.h
# End Source File
# Begin Source File

SOURCE=.\src\dbfutil.h
# End Source File
# Begin Source File

SOURCE=.\src\dbfview.h
# End Source File
# Begin Source File

SOURCE=.\src\precomp.h
# End Source File
# Begin Source File

SOURCE=.\src\wx29.h
# End Source File
# Begin Source File

SOURCE=.\src\wxext.h
# End Source File
# End Group
# Begin Group "dbf"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\d4base.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\dbf.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\dbf_wx.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\ioapi\ioapi.c
# ADD CPP /I "..\..\zlib"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "Doc/View"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\dbfdoc.cpp
# End Source File
# Begin Source File

SOURCE=.\src\dbfframe.cpp
# End Source File
# Begin Source File

SOURCE=.\src\dbfview.cpp
# End Source File
# Begin Source File

SOURCE=.\src\doc.cpp
# End Source File
# End Group
# End Target
# End Project
