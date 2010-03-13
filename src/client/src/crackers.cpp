#include "stdafx.h"
#include "crackers.h"

BOOL Toolbar_GetButtonInfo(HWND hwndCtl, int iID, LPTBBUTTONINFO lptbbi)
{
	WPARAM wParam = iID;
	LPARAM lParam = reinterpret_cast<LPARAM>(lptbbi);
	return SendMessage(hwndCtl, TB_GETBUTTONINFO, wParam, lParam);
}

void CloseWindow(HWND hwnd)
{
	SendMessage(hwnd, WM_CLOSE, 0, 0);
}
