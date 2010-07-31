#pragma once

#include "Blob.h"
#include "ISignalProvider.h"
#include "Guid.h"

class INoteListView : public ISignalProvider
{
public:

	virtual ~INoteListView() {}

	virtual void AddNote(std::wstring html, std::wstring value) = 0;

	virtual void AddNotebook(std::wstring html, std::wstring value) = 0;

	virtual void ConnectAbout(slot_type OnAbout) = 0;

	virtual void ConnectImport(slot_type OnImport) = 0;

	virtual void ConnectNewNote(slot_type OnNewNote) = 0;

	virtual void ConnectNotebookSelected(slot_type OnNotebookSelected) = 0;

	virtual void ConnectOpenNote(slot_type OnOpenNote) = 0;

	virtual void ConnectPageDown(slot_type OnPageDown) = 0;

	virtual void ConnectPageUp(slot_type OnPageUp) = 0;

	virtual void ConnectSearch(slot_type OnSearch) = 0;

	virtual void ConnectSignIn(slot_type OnSignin) = 0;

	virtual void ConnectSync(slot_type OnSync) = 0;

	virtual void ClearNotes() = 0;

	virtual void ClearNotebooks() = 0;

	virtual void DisableSync() = 0;

	virtual void EnableSync() = 0;

	virtual bool GetEnexPath(std::wstring & path) = 0;

	virtual Guid GetSelectedNotebookGuid() = 0;

	virtual Guid GetSelectedNoteGuid() = 0;

	virtual std::wstring GetSearchString() = 0;

	virtual void HidePageDown() = 0;

	virtual void HidePageUp() = 0;

	virtual void HideSyncButton() = 0;

	virtual void SetProfileText(const std::wstring & text) = 0;

	virtual void SetSigninText(const std::wstring & text) = 0;

	virtual void SetStatusText(const std::wstring & text) = 0;

	virtual void SetSyncText(const std::wstring & text) = 0;

	virtual void SetWindowTitle(const std::wstring & text) = 0;

	virtual void ShowPageDown() = 0;

	virtual void ShowPageUp() = 0;

	virtual void ShowSyncButton() = 0;

	virtual void UpdateNotes() = 0;

	virtual void UpdateNotebooks() = 0;

	virtual void UpdateThumbnail(const Guid & guid) = 0;
};
