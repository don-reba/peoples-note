#include "stdafx.h"

#include "Animator.h"
#include "CredentialsPresenter.h"
#include "CredentialsView.h"
#include "DataStore.h"
#include "EnImportPresenter.h"
#include "EnImporter.h"
#include "EnNoteTranslator.h"
#include "EnService.h"
#include "LastUserModel.h"
#include "NoteListModel.h"
#include "NoteListPresenter.h"
#include "NoteListView.h"
#include "NotePresenter.h"
#include "RegistryKey.h"
#include "SearchPresenter.h"
#include "SyncModel.h"
#include "SyncPresenter.h"
#include "UserLoader.h"
#include "UserModel.h"
#include "UserSignInPresenter.h"

#include "htmlayout.h"
#include "resourceppc.h"
#include "Tools.h"

#include <sstream>
#include <vector>

using namespace std;
using namespace Tools;

wstring GetDocumentPath()
{
	vector<wchar_t> folder(MAX_PATH);
	::SHGetSpecialFolderPath
		( NULL           // hwndOwner
		, &folder[0]     // lpszPath
		, CSIDL_PERSONAL // nFolder
		, TRUE           // fCreate
		);
	wstringstream stream;
	stream << &folder[0] << L'\\' << LoadStringResource(IDS_DOC_FOLDER);
	wstring path(stream.str());
	::CreateDirectory(path.c_str(), NULL);
	return path;
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

// Main message loop.
// Switches between two modes: normal and animation.
int RunMessageLoop
	( IAnimator & animator
	, SyncModel & syncModel
	)
{
	MSG msg;
	while (::GetMessage(&msg, NULL, 0, 0)) 
	{
		if (!::HTMLayoutTranslateMessage(&msg))
			::TranslateMessage(&msg);
		::DispatchMessage(&msg);
		syncModel.ProcessMessages();
		while (animator.IsRunning())
		{
			while (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				if (!::HTMLayoutTranslateMessage(&msg))
					::TranslateMessage(&msg);
				::DispatchMessage(&msg);
			}
			if (msg.message == WM_QUIT)
				return static_cast<int>(msg.wParam);
			animator.StepFrame();
		}
	}
	return static_cast<int>(msg.wParam);
}


int WINAPI WinMain(HINSTANCE instance,
				   HINSTANCE hPrevInstance,
				   LPTSTR    lpCmdLine,
				   int       nCmdShow)
{
	if (SwitchToPreviousInstance(instance))
		return 0;

	SHInitExtraControls();
	CoInitializeEx(NULL, COINIT_MULTITHREADED);

	try
	{
		wstring documentPath(GetDocumentPath());

		Animator         animator;
		RegistryKey      registryKey(L"Software\\People's Note");
		DataStore        dataStore;
		DataStore        syncDataStore;
		EnImporter       enImporter;
		EnNoteTranslator enNoteTranslator;
		EnService        enService;

		CredentialsModel newCredentials;
		LastUserModel    lastUserModel(registryKey);
		NoteListModel    noteListModel;
		SyncModel        syncModel(enService);
		UserModel        userModel(dataStore, documentPath);
		UserModel        syncUserModel(syncDataStore, documentPath);

		CredentialsView credentialsView (instance);
		NoteView        noteView        (instance);
		NoteListView    noteListView    (animator, instance, nCmdShow);

		CredentialsPresenter credentialsPresenter
			( newCredentials
			, credentialsView
			, enService
			);
		EnImportPresenter enImportPresenter
			( enImporter
			, noteListModel
			, noteListView
			, userModel
			);
		NoteListPresenter noteListPresenter
			( noteListModel
			, noteListView
			, noteView
			, userModel
			, enNoteTranslator
			);
		NotePresenter notePresenter
			( noteListView
			, noteView
			, userModel
			, enNoteTranslator
			);
		SearchPresenter searchPresenter
			( noteListModel
			, userModel
			, noteListView
			);
		SyncPresenter syncPresenter
			( noteListModel
			, noteListView
			, syncModel
			, userModel
			, syncUserModel
			);
		UserLoader userLoader
			( userModel
			, lastUserModel
			);
		UserSignInPresenter userSignInPresenter
			( newCredentials
			, noteListView
			, userModel
			);

		noteListView.Create();
		noteView.Create(noteListView.hwnd_);
		credentialsView.Create(noteListView.hwnd_);

		userLoader.Run();
		int result(RunMessageLoop(animator, syncModel));
		userLoader.Save();

		syncModel.StopSync();

		CoUninitialize();

		return result;
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
