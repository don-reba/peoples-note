#include "stdafx.h"

#include "DataStore.h"
#include "LastUserModel.h"
#include "NoteListModel.h"
#include "NoteListPresenter.h"
#include "NoteListView.h"
#include "RegistryKey.h"
#include "SearchPresenter.h"
#include "UserLoader.h"
#include "UserModel.h"

#include "resourceppc.h"
#include "htmlayout.h"
#include "Tools.h"

#include <sstream>
#include <vector>

using namespace std;
using namespace Tools;

wstring GetDocumentPath()
{
	wchar_t folder[MAX_PATH];
	folder[0] = L'\0';
	::SHGetSpecialFolderPath
		( NULL           // hwndOwner
		, folder         // lpszPath
		, CSIDL_PERSONAL // nFolder
		, TRUE           // fCreate
		);
	wstringstream stream;
	stream << folder << L'\\' << LoadStringResource(IDS_DOC_FOLDER);
	return stream.str();
}

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
		DataStore   dataStore(GetDocumentPath());

		LastUserModel lastUserModel(registryKey);
		NoteListModel noteListModel;
		UserModel     userModel(dataStore);

		NoteListView noteListView(instance, nCmdShow);

		NoteListPresenter noteListPresenter
			( noteListModel
			, noteListView
			, userModel
			);
		SearchPresenter searchPresenter
			( noteListModel
			, userModel
			, noteListView
			);
		UserLoader userLoader
			( userModel
			, lastUserModel
			, noteListView
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
