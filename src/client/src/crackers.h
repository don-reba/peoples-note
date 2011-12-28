#pragma once

BOOL Toolbar_GetButtonInfo(HWND hwndCtl, int iID, LPTBBUTTONINFO lptbbi);

BOOL AppendMenu(HMENU hMenu, UINT uFlags, HMENU hPopup, LPCTSTR lpNewItem);

void CloseWindow(HWND hwnd);

HMENU GetWindowMenu(HWND hwnd, int menuId);
