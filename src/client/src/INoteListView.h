#pragma once

#include "Blob.h"
#include "ISignalProvider.h"
#include "Guid.h"

class INoteListView : public ISignalProvider
{
public:

	typedef boost::signals2::signal
		< void(size_t, Blob *&)
		> DataSignal;
	typedef boost::signals2::signal
		< void(size_t, Blob *&)
		>::slot_type DataSlot;

public:

	virtual ~INoteListView() {}

	virtual void AddNote(std::wstring html, std::wstring value) = 0;

	virtual void AddNotebook(std::wstring html) = 0;

	virtual void ConnectImport(slot_type OnImport) = 0;

	virtual void ConnectLoadThumbnail(DataSlot OnLoadThumbnail) = 0;

	virtual void ConnectOpenNote(slot_type OnOpenNote) = 0;

	virtual void ConnectSearch(slot_type OnSearch) = 0;

	virtual void ConnectSignIn(slot_type OnSignin) = 0;

	virtual void ClearNotes() = 0;

	virtual void ClearNotebooks() = 0;

	virtual bool GetEnexPath(std::wstring & path) = 0;

	virtual Guid GetSelectedNoteGuid() = 0;

	virtual std::wstring GetSearchString() = 0;

	virtual void SignIn() = 0;

	virtual void UpdateNotes() = 0;

	virtual void UpdateNotebooks() = 0;
};
