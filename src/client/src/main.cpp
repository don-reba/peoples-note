#include "stdafx.h"

#include "htmlayout.h"
#include "main.h"
#include "NoteListView.h"
#include "resourceppc.h"
#include "Tools.h"

#include <vector>

using namespace Tools;

bool SwitchToPreviousInstance(HINSTANCE instance)
{
	std::wstring title       = LoadStringResource(IDS_APP_TITLE);
	std::wstring windowClass = LoadStringResource(IDC_CLIENT);
	HWND hwnd = FindWindow(windowClass.c_str(), title.c_str());	
	if (hwnd)
	{
		HWND activeOwnedWindow = (HWND)((ULONG)hwnd | 1);
		SetForegroundWindow(activeOwnedWindow);
		return true;
	}
	return false;
}

int WINAPI WinMain(HINSTANCE hInstance,
				   HINSTANCE hPrevInstance,
				   LPTSTR    lpCmdLine,
				   int       nCmdShow)
{
	SHInitExtraControls();

	if (SwitchToPreviousInstance(hInstance))
		return 0;

	try
	{
		NoteListView noteListView(hInstance, nCmdShow);

		HACCEL hAccelTable;
		hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CLIENT));

		MSG msg;
		while (GetMessage(&msg, NULL, 0, 0)) 
		{
			if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
			{
				if (!HTMLayoutTranslateMessage(&msg))
					TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}

		return (int) msg.wParam;
	}
	catch(std::exception e)
	{
		MessageBox(NULL, ConvertToUnicode(e.what()).c_str(), L"Error", MB_OK | MB_ICONERROR);
		return 1;
	}
}
