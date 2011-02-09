#pragma once

#include "Blob.h"
#include "ISignalProvider.h"
#include "Guid.h"

class INoteListView : public ISignalProvider
{
public:

	virtual ~INoteListView() {}

	virtual void AddNote
		( const std::wstring & html
		, const std::wstring & value
		,       bool           isDirty
		) = 0;

	virtual void CheckNotebookTitleOption() = 0;

	virtual void ConnectAbout(slot_type OnAbout) = 0;

	virtual void ConnectClearSearch(slot_type OnClearSearch) = 0;

	virtual void ConnectImport(slot_type OnImport) = 0;

	virtual void ConnectNewInkNote(slot_type OnNewInkNote) = 0;

	virtual void ConnectNewNote(slot_type OnNewNote) = 0;

	virtual void ConnectNewPhotoNote(slot_type OnNewPhoto) = 0;

	virtual void ConnectNotebookSelected(slot_type OnNotebookSelected) = 0;

	virtual void ConnectNotebookTitle(slot_type OnNotebookTitle) = 0;

	virtual void ConnectOpenNote(slot_type OnOpenNote) = 0;

	virtual void ConnectPageDown(slot_type OnPageDown) = 0;

	virtual void ConnectPageUp(slot_type OnPageUp) = 0;

	virtual void ConnectProfile(slot_type OnProfile) = 0;

	virtual void ConnectSearch(slot_type OnSearch) = 0;

	virtual void ConnectSearchChanged(slot_type OnSearchChanged) = 0;

	virtual void ConnectSignIn(slot_type OnSignin) = 0;

	virtual void ConnectSync(slot_type OnSync) = 0;

	virtual void ClearNotes() = 0;

	virtual void DisableSync() = 0;

	virtual void EnableSync() = 0;

	virtual bool GetEnexPath(std::wstring & path) = 0;

	virtual Guid GetSelectedNotebookGuid() = 0;

	virtual Guid GetSelectedNoteGuid() = 0;

	virtual void GetThumbSize(SIZE & size) = 0;

	virtual std::wstring GetSearchString() = 0;

	virtual void HideNotebookTitle() = 0;

	virtual void HidePageDown() = 0;

	virtual void HidePageUp() = 0;

	virtual void HideSyncButton() = 0;

	virtual bool IsNotebookTitleOptionChecked() = 0;

	virtual void SetNotebookMenu(const std::wstring & html) = 0;

	virtual void SetProfileText(const std::wstring & text) = 0;

	virtual void SetProgress(double fraction) = 0;

	virtual void SetSearchButtonToClear() = 0;

	virtual void SetSearchButtonToSearch() = 0;

	virtual void SetSearchText(const std::wstring & text) = 0;

	virtual void SetSigninText(const std::wstring & text) = 0;

	virtual void SetStatusText(const std::wstring & text) = 0;

	virtual void SetSyncText(const std::wstring & text) = 0;

	virtual void SetWindowTitle(const std::wstring & text) = 0;

	virtual void ShowNotebookTitle() = 0;

	virtual void ShowPageDown() = 0;

	virtual void ShowPageUp() = 0;

	virtual void ShowSyncButton() = 0;

	virtual void UncheckNotebookTitleOption() = 0;

	virtual void UpdateNotes() = 0;

	virtual void UpdateThumbnail(const Guid & note) = 0;
};
