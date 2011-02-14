#include "stdafx.h"

#include "AboutPresenter.h"
#include "AboutView.h"
#include "Animator.h"
#include "CredentialsPresenter.h"
#include "CredentialsView.h"
#include "DataStore.h"
#include "EditorPresenter.h"
#include "EditorView.h"
#include "EnImportPresenter.h"
#include "EnImporter.h"
#include "EnNoteTranslator.h"
#include "EnService.h"
#include "File.h"
#include "FlashCard.h"
#include "HtmlDataLoader.h"
#include "HtmlLayoutPresenter.h"
#include "InkEditorModel.h"
#include "InkEditorPresenter.h"
#include "InkEditorView.h"
#include "InstrumentationPresenter.h"
#include "LastUserModel.h"
#include "MessagePump.h"
#include "NoteListModel.h"
#include "NoteListPresenter.h"
#include "NoteListView.h"
#include "NotePresenter.h"
#include "PhotoEditorModel.h"
#include "PhotoEditorView.h"
#include "PhotoEditorPresenter.h"
#include "ProfilePresenter.h"
#include "ProfileView.h"
#include "RegistryKey.h"
#include "SearchPresenter.h"
#include "Logger.h"
#include "SyncModel.h"
#include "SyncPresenter.h"
#include "UserLoader.h"
#include "UserModel.h"
#include "UserSignInPresenter.h"

#include "htmlayout.h"
#include "resourceppc.h"
#include "Tools.h"

#include <vector>

using namespace std;
using namespace Tools;

wstring GetDeviceDocumentPath()
{
	vector<wchar_t> folder(MAX_PATH);
	::SHGetSpecialFolderPath
		( NULL           // hwndOwner
		, &folder[0]     // lpszPath
		, CSIDL_PERSONAL // nFolder
		, TRUE           // fCreate
		);
	wstring path(&folder[0]);
	path.append(L"\\");
	path.append(LoadStringResource(IDS_DOC_FOLDER));
	return path;
}

bool IsHighRes()
{
	if (480 > ::GetDeviceCaps(NULL, HORZRES))
		return false;
	if (480 > ::GetDeviceCaps(NULL, VERTRES))
		return false;
	return true;
}

bool SwitchToPreviousInstance(HINSTANCE instance)
{
	std::wstring windowClass(LoadStringResource(IDC_CLIENT));
	HWND hwnd = FindWindow(windowClass.c_str(), NULL);
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
		wstring deviceDocumentPath (GetDeviceDocumentPath());
		bool    highRes            (IsHighRes());

		Animator         animator;
		File             file;
		FlashCard        flashCard;
		RegistryKey      registryKey(L"Software\\People's Note");
		DataStore        dataStore;
		DataStore        syncDataStore;
		EnImporter       enImporter;
		EnNoteTranslator enNoteTranslator;
		MessagePump      messagePump;
		Logger           logger(deviceDocumentPath);
		
		EnService enService(logger);

		CredentialsModel newCredentials;
		InkEditorModel   inkEditorModel(registryKey);
		LastUserModel    lastUserModel(registryKey);
		PhotoEditorModel photoEditorModel(registryKey);
		UserModel        userModel     (dataStore,     deviceDocumentPath, flashCard);
		UserModel        syncUserModel (syncDataStore, deviceDocumentPath, flashCard);

		NoteListModel noteListModel(20, userModel, registryKey);
		SyncModel     syncModel(enNoteTranslator, enService, messagePump, syncUserModel, logger);

		AboutView       aboutView       (instance, highRes);
		CredentialsView credentialsView (instance, highRes);
		EditorView      editorView      (instance, highRes);
		InkEditorView   inkEditorView   (instance);
		NoteView        noteView        (instance, highRes, animator);
		NoteListView    noteListView    (instance, highRes, animator, nCmdShow);
		PhotoEditorView photoEditorView (instance, highRes);
		ProfileView     profileView     (instance, highRes);

		HtmlDataLoader htmlDataLoader
			( highRes
			, enNoteTranslator
			, noteListView
			, noteView
			, userModel
			);

		AboutPresenter aboutPresenter
			( aboutView
			, noteListView
			);
		CredentialsPresenter credentialsPresenter
			( newCredentials
			, credentialsView
			, enService
			);
		EditorPresenter editorPresenter
			( editorView
			, noteListModel
			, noteListView
			, noteView
			, userModel
			, enNoteTranslator
			);
		EnImportPresenter enImportPresenter
			( enNoteTranslator
			, enImporter
			, noteListModel
			, noteListView
			, userModel
			);
		HtmlLayoutPresenter
			( aboutView
			, credentialsView
			, editorView
			, noteView
			, noteListView
			, photoEditorView
			, profileView
			, htmlDataLoader
			);
		InkEditorPresenter
			( inkEditorModel
			, inkEditorView
			, noteListModel
			, noteListView
			, userModel
			);
		InstrumentationPresenter instrumentationPresenter
			( animator
			, noteListView
			);
		NoteListPresenter noteListPresenter
			( noteListModel
			, noteListView
			, noteView
			, userModel
			, syncModel
			, enNoteTranslator
			);
		NotePresenter notePresenter
			( noteListModel
			, noteListView
			, noteView
			, userModel
			, enNoteTranslator
			);
		PhotoEditorPresenter
			( file
			, noteListModel
			, noteListView
			, photoEditorModel
			, photoEditorView
			, userModel
			);
		ProfilePresenter profilePresenter
			( flashCard
			, profileView
			, noteListView
			, userModel
			);
		SearchPresenter searchPresenter
			( noteListModel
			, userModel
			, noteListView
			);
		SyncPresenter syncPresenter
			( noteListView
			, syncModel
			, userModel
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
		noteView.Create        (noteListView.hwnd_);
		editorView.Create      (noteListView.hwnd_);
		inkEditorView.Create   (noteListView.hwnd_);
		photoEditorView.Create (noteListView.hwnd_);
		credentialsView.Create (noteListView.hwnd_);
		aboutView.Create       (noteListView.hwnd_);
		profileView.Create     (noteListView.hwnd_);

		userLoader.Run();
		int result(RunMessageLoop(animator, syncModel));
		userLoader.Save();

		syncModel.StopSync();

		CoUninitialize();

		return result;
	}
	catch(std::exception e)
	{
#ifdef _DEBUG
		MessageBox
			( NULL
			, ConvertToUnicode(e.what()).c_str()
			, L"Error"
			, MB_OK | MB_ICONERROR
			);
#endif // _DEBUG
		return 1;
	}
}
