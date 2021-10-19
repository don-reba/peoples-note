#include "stdafx.h"

#include "AboutPresenter.h"
#include "AboutView.h"
#include "Animator.h"
#include "AudioAttachmentPresenter.h"
#include "AudioPlayer.h"
#include "AudioRecorder.h"
#include "CredentialsPresenter.h"
#include "CredentialsModel.h"
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
#include "NoteTagListModel.h"
#include "NoteTagListPresenter.h"
#include "NoteTagListView.h"
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
#include "VoiceEditorModel.h"
#include "VoiceEditorPresenter.h"
#include "VoiceEditorView.h"

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
		const wstring deviceDocumentPath (GetDeviceDocumentPath());
		const bool    highRes            (IsHighRes());

		Animator         animator;
		AudioPlayer      audioPlayer;
		AudioRecorder    audioRecorder;
		File             file;
		FlashCard        flashCard;
		RegistryKey      registryKey(L"Software\\People's Note");
		DataStore        dataStore;
		DataStore        mediaDataStore;
		DataStore        syncDataStore;
		EnImporter       enImporter;
		EnNoteTranslator enNoteTranslator;
		MessagePump      messagePump;
		Logger           logger(deviceDocumentPath);

		EnService enService(logger);

		CredentialsModel credentialsModel;
		InkEditorModel   inkEditorModel(registryKey);
		LastUserModel    lastUserModel(registryKey);
		NoteTagListModel noteTagListModel;
		PhotoEditorModel photoEditorModel(registryKey);
		UserModel        userModel      (dataStore,      deviceDocumentPath, flashCard);
		UserModel        mediaUserModel (mediaDataStore, deviceDocumentPath, flashCard);
		UserModel        syncUserModel  (syncDataStore,  deviceDocumentPath, flashCard);
		VoiceEditorModel attachmentPlayerModel;
		VoiceEditorModel voiceEditorModel;

		NoteListModel noteListModel(userModel, registryKey);
		SyncModel     syncModel(enNoteTranslator, enService, messagePump, syncUserModel, logger);

		HtmlDataLoader htmlDataLoader
			( highRes
			, enNoteTranslator
			, userModel
			);

		AboutView       aboutView            (instance, highRes, htmlDataLoader);
		CredentialsView credentialsView      (instance, highRes, htmlDataLoader);
		EditorView      editorView           (instance, highRes, htmlDataLoader);
		InkEditorView   inkEditorView        (instance);
		NoteView        noteView             (instance, highRes, animator, htmlDataLoader);
		NoteListView    noteListView         (instance, highRes, animator, nCmdShow, htmlDataLoader);
		NoteTagListView noteTagListView      (instance, highRes, animator, htmlDataLoader);
		PhotoEditorView photoEditorView      (instance, highRes, htmlDataLoader);
		ProfileView     profileView          (instance, highRes, htmlDataLoader);
		VoiceEditorView voiceEditorView      (instance, highRes, htmlDataLoader);
		VoiceEditorView attachmentPlayerView (instance, highRes, htmlDataLoader);

		htmlDataLoader.AttachViews(noteListView, noteView);

		AboutPresenter aboutPresenter
			( aboutView
			, noteListView
			);
		AudioAttachmentPresenter audioAttachmentPresenter
			( audioPlayer
			, audioRecorder
			, credentialsModel
			, noteView
			, mediaUserModel
			, attachmentPlayerModel
			, attachmentPlayerView
			);
		CredentialsPresenter credentialsPresenter
			( credentialsModel
			, credentialsView
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
			( deviceDocumentPath
			, noteListModel
			, noteListView
			, noteTagListModel
			, noteView
			, userModel
			, enNoteTranslator
			);
		NoteTagListPresenter
			( noteTagListModel
			, noteTagListView
			, noteView
			, userModel
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
			, lastUserModel
			, userModel
			);
		UserLoader userLoader
			( credentialsModel
			, lastUserModel
			);
		UserSignInPresenter userSignInPresenter
			( credentialsModel
			, enService
			, noteListView
			, userModel
			);
		VoiceEditorPresenter voiceEditorPresenter
			( audioPlayer
			, audioRecorder
			, noteListView
			, voiceEditorModel
			, voiceEditorView
			, userModel
			);

		noteListView.Create();
		aboutView.Create            (noteListView.hwnd_);
		credentialsView.Create      (noteListView.hwnd_);
		editorView.Create           (noteListView.hwnd_);
		inkEditorView.Create        (noteListView.hwnd_);
		noteTagListView.Create      (noteListView.hwnd_);
		noteView.Create             (noteListView.hwnd_);
		photoEditorView.Create      (noteListView.hwnd_);
		profileView.Create          (noteListView.hwnd_);
		voiceEditorView.Create      (noteListView.hwnd_);
		attachmentPlayerView.Create (noteListView.hwnd_);

		userLoader.Run();
		int result(RunMessageLoop(animator, syncModel));

		syncModel.StopSync();

		CoUninitialize();

		return result;
	}
	catch(const std::exception & e)
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
