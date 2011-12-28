#include "stdafx.h"
#include "crackers.h"

BOOL AppendMenu(HMENU hMenu, UINT uFlags, HMENU hPopup, LPCTSTR lpNewItem)
{
	ASSERT((uFlags & MF_POPUP) == MF_POPUP);
	return ::AppendMenu(hMenu, uFlags, reinterpret_cast<UINT>(hPopup), lpNewItem);
}

BOOL Toolbar_GetButtonInfo(HWND hwndCtl, int iID, LPTBBUTTONINFO lptbbi)
{
	WPARAM wParam = iID;
	LPARAM lParam = reinterpret_cast<LPARAM>(lptbbi);
	return SendMessage(hwndCtl, TB_GETBUTTONINFO, wParam, lParam);
}

void CloseWindow(HWND hwnd)
{
	PostMessage(hwnd, WM_CLOSE, 0, 0);
}

HMENU GetWindowMenu(HWND hwnd, int menuId)
{
	return reinterpret_cast<HMENU>(::SendMessage(hwnd, SHCMBM_GETSUBMENU, 0, menuId));
}
