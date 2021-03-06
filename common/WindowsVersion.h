/****************************************************************** 
*                                                                 * 
*  VersionHelpers.h -- This module defines helper functions to    * 
*                      promote version check with proper          * 
*                      comparisons.                               * 
*                                                                 * 
*  Copyright (c) Microsoft Corp.  All rights reserved.            * 
*                                                                 * 
******************************************************************/  
#ifndef _versionhelpers_H_INCLUDED_  
#define _versionhelpers_H_INCLUDED_  

#include <Windows.h>
#include <functional>
  
#ifndef ___XP_BUILD  
#define WINAPI_PARTITION_DESKTOP   (WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP)  
#define WINAPI_FAMILY WINAPI_FAMILY_DESKTOP_APP  
#define WINAPI_FAMILY_PARTITION(Partitions)     (Partitions)  
  
#define _WIN32_WINNT_NT4                    0x0400  
#define _WIN32_WINNT_WIN2K                  0x0500  
#define _WIN32_WINNT_WINXP                  0x0501  
#define _WIN32_WINNT_WS03                   0x0502  
#define _WIN32_WINNT_WIN6                   0x0600  
#define _WIN32_WINNT_VISTA                  0x0600  
#define _WIN32_WINNT_WS08                   0x0600  
#define _WIN32_WINNT_LONGHORN               0x0600  
#define _WIN32_WINNT_WIN7                   0x0601  
#define _WIN32_WINNT_WIN8                   0x0602  
#endif  
  
#ifdef _MSC_VER  
#pragma once  
#endif  // _MSC_VER  
  
#pragma region Application Family  
  
#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)  
  
#include <specstrings.h>    // for _In_, etc.  
  
#if !defined(__midl) && !defined(SORTPP_PASS)  
  
#if (NTDDI_VERSION >= NTDDI_WINXP)  
  
#ifdef __cplusplus  
  
#define VERSIONHELPERAPI inline bool  
  
#else  // __cplusplus  
  
#define VERSIONHELPERAPI FORCEINLINE BOOL  
  
#endif // __cplusplus  
  
#define _WIN32_WINNT_WINBLUE                0x0603  
#define _WIN32_WINNT_WIN10                  0x0A00  
  
//typedef NTSTATUS ( WINAPI *fnRtlGetVersion )(PRTL_OSVERSIONINFOW lpVersionInformation);  
  
VERSIONHELPERAPI  
IsWindowsVersionOrGreater(WORD wMajorVersion, WORD wMinorVersion, WORD wServicePackMajor)  
{  
    RTL_OSVERSIONINFOEXW verInfo = { 0 };  
    verInfo.dwOSVersionInfoSize = sizeof( verInfo );  
	typedef LONG(__stdcall *fnRtlGetVersion)(PRTL_OSVERSIONINFOW);
	//typedef NTSTATUS(*fnRtlGetVersion)(PRTL_OSVERSIONINFOW lpVersionInformation);  
    static auto RtlGetVersion = (fnRtlGetVersion)GetProcAddress( GetModuleHandleW( L"ntdll.dll" ), "RtlGetVersion" );  
    //static auto RtlGetVersion = (fnRtlGetVersion)GetProcAddress( GetModuleHandleW( L"ntdll.dll" ), "RtlGetVersion" );  
  
    if (RtlGetVersion != 0 && RtlGetVersion( (PRTL_OSVERSIONINFOW)&verInfo ) == 0)  
    {  
        if (verInfo.dwMajorVersion > wMajorVersion)  
            return true;  
        else if (verInfo.dwMajorVersion < wMajorVersion)  
            return false;  
  
        if (verInfo.dwMinorVersion > wMinorVersion)  
            return true;  
        else if (verInfo.dwMinorVersion < wMinorVersion)  
            return false;  
  
        if (verInfo.wServicePackMajor >= wServicePackMajor)  
            return true;  
    }  
  
    return false;  
}  
  
VERSIONHELPERAPI  
IsWindowsXPOrGreater()  
{  
    return IsWindowsVersionOrGreater( HIBYTE( _WIN32_WINNT_WINXP ), LOBYTE( _WIN32_WINNT_WINXP ), 0 );  
}  
  
VERSIONHELPERAPI  
IsWindowsXPSP1OrGreater()  
{  
    return IsWindowsVersionOrGreater( HIBYTE( _WIN32_WINNT_WINXP ), LOBYTE( _WIN32_WINNT_WINXP ), 1 );  
}  
  
VERSIONHELPERAPI  
IsWindowsXPSP2OrGreater()  
{  
    return IsWindowsVersionOrGreater( HIBYTE( _WIN32_WINNT_WINXP ), LOBYTE( _WIN32_WINNT_WINXP ), 2 );  
}  
  
VERSIONHELPERAPI  
IsWindowsXPSP3OrGreater()  
{  
    return IsWindowsVersionOrGreater( HIBYTE( _WIN32_WINNT_WINXP ), LOBYTE( _WIN32_WINNT_WINXP ), 3 );  
}  
  
VERSIONHELPERAPI  
IsWindowsVistaOrGreater()  
{  
    return IsWindowsVersionOrGreater( HIBYTE( _WIN32_WINNT_VISTA ), LOBYTE( _WIN32_WINNT_VISTA ), 0 );  
}  
  
VERSIONHELPERAPI  
IsWindowsVistaSP1OrGreater()  
{  
    return IsWindowsVersionOrGreater( HIBYTE( _WIN32_WINNT_VISTA ), LOBYTE( _WIN32_WINNT_VISTA ), 1 );  
}  
  
VERSIONHELPERAPI  
IsWindowsVistaSP2OrGreater()  
{  
    return IsWindowsVersionOrGreater( HIBYTE( _WIN32_WINNT_VISTA ), LOBYTE( _WIN32_WINNT_VISTA ), 2 );  
}  
  
VERSIONHELPERAPI  
IsWindows7OrGreater()  
{  
    return IsWindowsVersionOrGreater( HIBYTE( _WIN32_WINNT_WIN7 ), LOBYTE( _WIN32_WINNT_WIN7 ), 0 );  
}  
  
VERSIONHELPERAPI  
IsWindows7SP1OrGreater()  
{  
    return IsWindowsVersionOrGreater( HIBYTE( _WIN32_WINNT_WIN7 ), LOBYTE( _WIN32_WINNT_WIN7 ), 1 );  
}  
  
VERSIONHELPERAPI  
IsWindows8OrGreater()  
{  
    return IsWindowsVersionOrGreater( HIBYTE( _WIN32_WINNT_WIN8 ), LOBYTE( _WIN32_WINNT_WIN8 ), 0 );  
}  
  
VERSIONHELPERAPI  
IsWindows8Point1OrGreater()  
{  
    return IsWindowsVersionOrGreater( HIBYTE( _WIN32_WINNT_WINBLUE ), LOBYTE( _WIN32_WINNT_WINBLUE ), 0 );  
}  
  
VERSIONHELPERAPI  
IsWindows10OrGreater()  
{  
    return IsWindowsVersionOrGreater( HIBYTE( _WIN32_WINNT_WIN10 ), LOBYTE( _WIN32_WINNT_WIN10 ), 0 );  
}  
  
  
VERSIONHELPERAPI  
IsWindowsServer()  
{  
    OSVERSIONINFOEXW osvi = { sizeof( osvi ), 0, 0, 0, 0, { 0 }, 0, 0, 0, VER_NT_WORKSTATION };  
    DWORDLONG        const dwlConditionMask = VerSetConditionMask( 0, VER_PRODUCT_TYPE, VER_EQUAL );  
  
    return !VerifyVersionInfoW( &osvi, VER_PRODUCT_TYPE, dwlConditionMask );  
}  
  
#endif // NTDDI_VERSION  
  
#endif // defined(__midl)  
  
#endif /* WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP) */  
  
#pragma endregion  
  
FORCEINLINE int GetWinOSVersion(){
	bool bIs64 = []()->bool{
		typedef void (WINAPI *pFnGetNativeSystemInfo)(SYSTEM_INFO* lpSystemInfo);
		pFnGetNativeSystemInfo pFun = (pFnGetNativeSystemInfo)GetProcAddress(GetModuleHandleA("kernel32.dll"), "GetNativeSystemInfo");  
		SYSTEM_INFO si = { 0 };  
		if (pFun != NULL)
		{
			pFun(&si);
		}
		else
		{
			GetSystemInfo(&si);
		}
		if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 || si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64)
		{
			return true;
		}
		return false;
	}();

	do{
		//if (IsWindowsServer()){
		//	return 14;//winserver
		//}
		if (IsWindows10OrGreater()){
			return bIs64 ? 13 : 12;//win1064 : win1032
		}
		if (IsWindows8Point1OrGreater()){
			return bIs64 ? 11 : 10;//win8164 : win8132
		}
		if (IsWindows8OrGreater()){
			return bIs64 ? 9 : 8;//win864 : win832
		}
		if (IsWindows7SP1OrGreater() || IsWindows7OrGreater()){
			return bIs64 ? 7 : 6;//win764 : win732
		}
		if (IsWindowsVistaSP2OrGreater() || IsWindowsVistaSP1OrGreater() || IsWindowsVistaOrGreater()){
			return bIs64 ? 5 : 4;//vista64 : vista32
		}
		if (IsWindowsXPSP3OrGreater()){
			return 2;//xp sp3 32
		}
		if (IsWindowsXPSP2OrGreater() || IsWindowsXPSP1OrGreater() || IsWindowsXPOrGreater()){
			return bIs64 ? 3 : 1;//xp64 : xp
		}
	} while (false);
	return 0;//unknown
}

#endif // _VERSIONHELPERS_H_INCLUDED_  