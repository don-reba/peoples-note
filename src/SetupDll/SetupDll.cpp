#include "stdafx.h"
#include "ce_setup.h"

#include <fstream>

const wchar_t * const szTargetFilename = L"AppIcon.png";

struct DpiInfo
{
	const int Dpi;
	const int IconResolution;

	DpiInfo(int dpi, int iconResolution)
		: Dpi            (dpi)
		, IconResolution (iconResolution)
	{
	}
};

// Can only be called once.
int GetDpi(LPCTSTR pszInstallDir)
{
	// based on:
	// http://windowsteamblog.com/windows_phone/b/windowsphone/archive/2009/08/11/using-custom-icons-in-windows-mobile-6-5.aspx
	wchar_t path[MAX_PATH];
	wsprintf(path, L"%s\\%s", pszInstallDir, _T("GetRealDPI.exe"));
	// Launch DPI Detector
	PROCESS_INFORMATION info;
	if (!::CreateProcess(path, NULL, NULL, NULL, NULL, 0, NULL, NULL, NULL, &info))
	{
		::DeleteFile(path);
		return -1;
	}
	::WaitForSingleObject(info.hProcess, 10000);
	// DPI is returned in exit code of detector app
	DWORD systemDpi(0);
	if (!::GetExitCodeProcess(info.hProcess, &systemDpi))
	{
		::CloseHandle(info.hProcess);
		::DeleteFile(path);
		return -1;
	}
	::CloseHandle(info.hProcess);
	::DeleteFile(path);
	return systemDpi;
}

// Can only be called once.
void CreateIcon(int systemDpi, LPCTSTR pszInstallDir)
{
	// based on:
	// http://windowsteamblog.com/windows_phone/b/windowsphone/archive/2009/08/11/using-custom-icons-in-windows-mobile-6-5.aspx
	const int dpiInfosCount(3);
	DpiInfo dpiInfos[dpiInfosCount] =
		{ DpiInfo(96,  45)
		, DpiInfo(128, 60)
		, DpiInfo(192, 90)
		};

	// based on:
	// http://windowsteamblog.com/windows_phone/b/windowsphone/archive/2009/08/11/using-custom-icons-in-windows-mobile-6-5.aspx
	wchar_t dst[MAX_PATH];
	wchar_t src[MAX_PATH];
	wsprintf(dst,_T("%s\\%s"), pszInstallDir, szTargetFilename);
	for (int i(0); i != dpiInfosCount; ++i)
	{
		DpiInfo & dpiInfo(dpiInfos[i]);
		wsprintf(src, _T("%s\\%d.png"), pszInstallDir, dpiInfo.IconResolution);
		if (dpiInfo.Dpi == systemDpi)
			::CopyFile(src, dst, FALSE);
		::DeleteFile(src);
	}
}

// Can only be called once.
void CreateShortcut(HWND hwndParent, LPCTSTR pszInstallDir)
{
	// based on:
	// http://windowsteamblog.com/windows_phone/b/windowsphone/archive/2009/08/11/using-custom-icons-in-windows-mobile-6-5.aspx

	const wchar_t * appName(wcsrchr(pszInstallDir, L'\\') + 1);
	if (!appName)
		appName = pszInstallDir;

	wchar_t shortcutPath[MAX_PATH];
	// CSIDL_PROGRAMS == \Windows\Start Menu\Programs
	::SHGetSpecialFolderPath(hwndParent, shortcutPath, CSIDL_PROGRAMS , false); 
	wsprintf(shortcutPath, _T("%s\\%s.lnk"), shortcutPath, appName);

	wchar_t targetPath[MAX_PATH];
	wsprintf(targetPath, L"\"%s\\pnote.exe\"", pszInstallDir);

	::SHCreateShortcut(shortcutPath, targetPath);
}

void DeleteShortcut(HWND hwndParent, LPCTSTR pszInstallDir)
{
	const wchar_t * appName(wcsrchr(pszInstallDir, L'\\') + 1);
	if (!appName)
		appName = pszInstallDir;

	wchar_t shortcutPath[MAX_PATH];
	// CSIDL_PROGRAMS == \Windows\Start Menu\Programs
	::SHGetSpecialFolderPath(hwndParent, shortcutPath, CSIDL_PROGRAMS , false); 
	wsprintf(shortcutPath, _T("%s\\%s.lnk"), shortcutPath, appName);

	::DeleteFile(shortcutPath);
}

//-------------
// entry points
//
// based on:
// http://msdn.microsoft.com/en-us/library/aa924308.aspx
//-------------

codeINSTALL_INIT Install_Init
	( HWND    hwndParent
	, BOOL    fFirstCall     // is this the first time this function is being called?
	, BOOL    fPreviouslyInstalled
	, LPCTSTR pszInstallDir
	)
{
	return codeINSTALL_INIT_CONTINUE;
}

codeINSTALL_EXIT Install_Exit
	( HWND    hwndParent
	, LPCTSTR pszInstallDir
	, WORD    cFailedDirs
	, WORD    cFailedFiles
	, WORD    cFailedRegKeys
	, WORD    cFailedRegVals
	, WORD    cFailedShortcuts
	)
{
	CreateIcon(GetDpi(pszInstallDir), pszInstallDir);
	CreateShortcut(hwndParent, pszInstallDir);
	return codeINSTALL_EXIT_DONE;
}

codeUNINSTALL_INIT Uninstall_Init
	( HWND    hwndParent
	, LPCTSTR pszInstallDir
	)
{
	DeleteShortcut(hwndParent, pszInstallDir);
	return codeUNINSTALL_INIT_CONTINUE;
}

codeUNINSTALL_EXIT Uninstall_Exit(HWND hwndParent)
{
	return codeUNINSTALL_EXIT_DONE;
}
