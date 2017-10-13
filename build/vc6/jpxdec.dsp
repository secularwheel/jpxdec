# Microsoft Developer Studio Project File - Name="jpxdec" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=jpxdec - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "jpxdec.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "jpxdec.mak" CFG="jpxdec - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "jpxdec - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "jpxdec - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "jpxdec - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "jpxdec - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "jpxdec - Win32 Release"
# Name "jpxdec - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\src\jp2_dec.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpc_dec.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpx_arith_decoder.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpx_bitstream.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpx_bitstuff.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpx_codeblock.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpx_colorspace.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpx_dec.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpx_memmgr.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpx_stream.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpx_synthesis.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpx_tagtree.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\include\jp2_dec.h
# End Source File
# Begin Source File

SOURCE=..\..\include\jpc_dec.h
# End Source File
# Begin Source File

SOURCE=..\..\include\jpx_arith_decoder.h
# End Source File
# Begin Source File

SOURCE=..\..\include\jpx_bitstream.h
# End Source File
# Begin Source File

SOURCE=..\..\include\jpx_bitstuff.h
# End Source File
# Begin Source File

SOURCE=..\..\include\jpx_codeblock.h
# End Source File
# Begin Source File

SOURCE=..\..\include\jpx_colorspace.h
# End Source File
# Begin Source File

SOURCE=..\..\include\jpx_conf.h
# End Source File
# Begin Source File

SOURCE=..\..\include\jpx_dec.h
# End Source File
# Begin Source File

SOURCE=..\..\include\jpx_fixpoint.h
# End Source File
# Begin Source File

SOURCE=..\..\include\jpx_memmgr.h
# End Source File
# Begin Source File

SOURCE=..\..\include\jpx_stream.h
# End Source File
# Begin Source File

SOURCE=..\..\include\jpx_synthesis.h
# End Source File
# Begin Source File

SOURCE=..\..\include\jpx_tagtree.h
# End Source File
# End Group
# End Target
# End Project
