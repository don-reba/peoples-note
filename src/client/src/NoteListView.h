#pragma once
#include "INoteListView.h"

#include "HTMLayoutWindow.h"
#include "IAnimator.h"
#include "NoteListGestureProcessor.h"
#include "NoteView.h"

class NoteListView : public HTMLayoutWindow, public INoteListView
{
	MacroEvent(About)
	MacroEvent(ClearSearch)
	MacroEvent(Import)
	MacroEvent(NewInkNote)
	MacroEvent(NewNote)
	MacroEvent(NewPhotoNote)
	MacroEvent(NewVoiceNote)
	MacroEvent(NotebookSelected)
	MacroEvent(NotebookTitle)
	MacroEvent(OpenNote)
	MacroEvent(PageDown)
	MacroEvent(PageUp)
	MacroEvent(Profile)
	MacroEvent(Search)
	MacroEvent(SearchChanged)
	MacroEvent(SignIn)
	MacroEvent(Sync)

private:

	enum SearchButtonState
	{
		SearchButtonSearch,
		SearchButtonClear,
	};

	typedef htmlayout::dom::element element;

private:

	NoteListGestureProcessor gestureProcessor;

	SHACTIVATEINFO activateInfo;
	int            cmdShow;
	HINSTANCE      instance;
	DWORD          sipState;

	int startScrollPos;

	element clickTarget;
	element noteList;
	element listScroll;
	element listSlider;
	element searchBox;
	element status;

	Guid selectedNotebookGuid;

	SearchButtonState searchButtonState;

// interface

public:

	NoteListView
		( HINSTANCE         instance
		, bool              highRes
		, IAnimator       & animator
		, int               cmdShow
		, IHtmlDataLoader & htmlDataLoader
		);

	void Create();

	virtual void RegisterEventHandlers();

// INoteListView implementation

public:

	virtual void AddNote
		( const std::wstring & html
		, const std::wstring & value
		,       bool           isDirty
		);

	virtual void CheckNotebookTitleOption();

	virtual void ClearNotes();

	virtual void DisableSync();

	virtual void EnableSync();

	virtual bool GetEnexPath(std::wstring & path);

	virtual Guid GetSelectedNotebookGuid();

	virtual Guid GetSelectedNoteGuid();
	
	virtual std::wstring GetSearchString();

	virtual void GetThumbSize(SIZE & size);

	virtual void HideNotebookTitle();

	virtual void HidePageDown();

	virtual void HidePageUp();

	virtual void HideSyncButton();

	virtual bool IsNotebookTitleOptionChecked();

	virtual void SetNotebookMenu(const std::wstring & html);

	virtual void SetProfileText(const std::wstring & text);

	virtual void SetProgress(double fraction);

	virtual void SetSearchButtonToClear();

	virtual void SetSearchButtonToSearch();

	virtual void SetSearchText(const std::wstring & text);

	virtual void SetSigninText(const std::wstring & text);

	virtual void SetStatusText(const std::wstring & text);

	virtual void SetSyncText(const std::wstring & text);

	virtual void SetWindowTitle(const std::wstring & text);

	virtual void ShowNotebookTitle();

	virtual void ShowPageDown();

	virtual void ShowPageUp();

	virtual void ShowSyncButton();

	virtual void UncheckNotebookTitleOption();

	virtual void UpdateNotes();

	virtual void UpdateThumbnail(const Guid & note);

// utility functions

private:

	element GetChild(element parent, element descendant);

	int GetScrollPos();

	static bool IsChild(element child, element parent);

	void OnSearch();

	ATOM RegisterClass(std::wstring wndClass);

	void SetScrollPos(int pos);

	void UpdateScrollbar();

// gesture message handlers

private:

	void OnDelayedMouseDown();
	void OnGestureStart();
	void OnGestureStep();

// window message handlers

private:

	void OnActivate       (Msg<WM_ACTIVATE>       & msg);
	void OnCaptureChanged (Msg<WM_CAPTURECHANGED> & msg);
	void OnDestroy        (Msg<WM_DESTROY>        & msg);
	void OnMouseDown      (Msg<WM_LBUTTONDOWN>    & msg);
	void OnMouseUp        (Msg<WM_LBUTTONUP>      & msg);
	void OnMouseMove      (Msg<WM_MOUSEMOVE>      & msg);

	virtual void ProcessMessage(WndMsg &msg);

// HTMLayout message handlers

private:

	virtual BOOL OnFocus (FOCUS_PARAMS * params);
	virtual BOOL OnKey   (KEY_PARAMS   * params);

	void OnMenuAbout         (BEHAVIOR_EVENT_PARAMS * params);
	void OnMenuExit          (BEHAVIOR_EVENT_PARAMS * params);
	void OnMenuImport        (BEHAVIOR_EVENT_PARAMS * params);
	void OnMenuNotebook      (BEHAVIOR_EVENT_PARAMS * params);
	void OnMenuNotebookTitle (BEHAVIOR_EVENT_PARAMS * params);
	void OnMenuProfile       (BEHAVIOR_EVENT_PARAMS * params);
	void OnMenuSignIn        (BEHAVIOR_EVENT_PARAMS * params);
	void OnNewInk            (BEHAVIOR_EVENT_PARAMS * params);
	void OnNewPhoto          (BEHAVIOR_EVENT_PARAMS * params);
	void OnNewText           (BEHAVIOR_EVENT_PARAMS * params);
	void OnNewVoice          (BEHAVIOR_EVENT_PARAMS * params);
	void OnNote              (BEHAVIOR_EVENT_PARAMS * params);
	void OnPageDown          (BEHAVIOR_EVENT_PARAMS * params);
	void OnPageUp            (BEHAVIOR_EVENT_PARAMS * params);
	void OnSearch            (BEHAVIOR_EVENT_PARAMS * params);
	void OnSearchChanged     (BEHAVIOR_EVENT_PARAMS * params);
	void OnStatus            (BEHAVIOR_EVENT_PARAMS * params);
	void OnSync              (BEHAVIOR_EVENT_PARAMS * params);
};
