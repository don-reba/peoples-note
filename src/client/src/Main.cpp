#include "stdafx.h"

#include "LastUserModel.h"
#include "NoteListModel.h"
#include "NoteListPresenter.h"
#include "NoteListView.h"
#include "RegistryKey.h"
#include "UserLoader.h"
#include "UserModel.h"

#include "resourceppc.h"
#include "htmlayout.h"
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

WPARAM RunMessageLoop(HACCEL accelerators)
{
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if (!TranslateAccelerator(msg.hwnd, accelerators, &msg)) 
		{
			if (!HTMLayoutTranslateMessage(&msg))
				TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return msg.wParam;
}


int WINAPI WinMain(HINSTANCE instance,
				   HINSTANCE hPrevInstance,
				   LPTSTR    lpCmdLine,
				   int       nCmdShow)
{
	if (SwitchToPreviousInstance(instance))
		return 0;

	SHInitExtraControls();

	try
	{
		RegistryKey registryKey;

		LastUserModel lastUserModel(registryKey);
		NoteListModel noteListModel;
		UserModel     userModel;

		NoteListView noteListView(instance, nCmdShow);

		UserLoader userLoader
			( userModel
			, lastUserModel
			, noteListView
			);
		NoteListPresenter noteListPresenter
			( noteListModel
			, noteListView
			, userModel
			);

		noteListView.Create();

		HACCEL accelerators = LoadAccelerators
			( instance
			, MAKEINTRESOURCE(IDC_CLIENT)
			);
		return static_cast<int>(RunMessageLoop(accelerators));
	}
	catch(std::exception e)
	{
		MessageBox
			( NULL
			, ConvertToUnicode(e.what()).c_str()
			, L"Error"
			, MB_OK | MB_ICONERROR
			);
		return 1;
	}
}
