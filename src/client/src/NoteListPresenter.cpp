#include "stdafx.h"
#include "NoteListPresenter.h"

#include "ICredentialsModel.h"
#include "INoteListModel.h"
#include "INoteListView.h"
#include "INoteView.h"
#include "ISyncModel.h"
#include "IUserModel.h"
#include "Notebook.h"
#include "Tools.h"
#include "Transaction.h"
#include "WaitCursor.h"

#include <sstream>

using namespace boost;
using namespace std;
using namespace Tools;

NoteListPresenter::NoteListPresenter
	( INoteListModel   & noteListModel
	, INoteListView    & noteListView
	, INoteView        & noteView
	, IUserModel       & userModel
	, ISyncModel       & syncModel
	, EnNoteTranslator & enNoteTranslator
	)
	: noteListModel    (noteListModel)
	, noteListView     (noteListView)
	, noteView         (noteView)
	, userModel        (userModel)
	, syncModel        (syncModel)
	, enNoteTranslator (enNoteTranslator)
{
	noteListModel.ConnectNoteChanged
		(bind(&NoteListPresenter::OnNoteChanged, this));

	noteListModel.ConnectNoteListChanged
		(bind(&NoteListPresenter::OnNoteListChanged, this));

	noteListView.ConnectNotebookSelected
		(bind(&NoteListPresenter::OnNotebookSelected, this));

	noteListView.ConnectNotebookTitleStateChanged
		(bind(&NoteListPresenter::OnNotebookTitleChanged, this));

	noteListView.ConnectPageDown
		(bind(&NoteListPresenter::OnPageDown, this));

	noteListView.ConnectPageUp
		(bind(&NoteListPresenter::OnPageUp, this));

	noteListView.ConnectSync
		(bind(&NoteListPresenter::OnSyncBegin, this));

	noteListView.ConnectViewStyleChanged
		(bind(&NoteListPresenter::OnViewStyleChanged, this));

	syncModel.ConnectStatusUpdated
		(bind(&NoteListPresenter::OnSyncStatusUpdated, this));

	syncModel.ConnectSyncComplete
		(bind(&NoteListPresenter::OnSyncEnd, this));

	userModel.ConnectLoaded
		(bind(&NoteListPresenter::OnUserLoaded, this));

}

//---------------
// event handlers
//---------------

void NoteListPresenter::OnNoteChanged()
{
	UpdateSyncCounter();
}

void NoteListPresenter::OnNotebookSelected()
{
	Transaction transaction(userModel);
	UpdateActiveNotebook();
	UpdateTitle();
	ResetSearch();
	noteListModel.Reload();
	UpdateSyncCounter();
}

void NoteListPresenter::OnNotebookTitleChanged()
{
	noteListModel.SetNotebookTitleState(noteListView.GetRequestedNotebookTitleState());
	UpdateNotebookTitle();
}

void NoteListPresenter::OnNoteListChanged()
{
	MacroWaitCursor;

	NoteList notes;
	bool hasPreviousPage, hasNextPage;
	noteListModel.GetCurrentPage(notes, hasPreviousPage, hasNextPage);
	noteListView.ClearNotes();
	foreach (const Note & note, notes)
	{
		wstring guid(note.guid);
		noteListView.AddNote(ConvertToHtml(note, guid), guid, note.isDirty);
	}
	if (hasPreviousPage) noteListView.ShowPageUp();
	else                 noteListView.HidePageUp();
	if (hasNextPage) noteListView.ShowPageDown();
	else             noteListView.HidePageDown();
	noteListView.UpdateNotes();
}

void NoteListPresenter::OnPageDown()
{
	noteListModel.SelectNextPage();
}

void NoteListPresenter::OnPageUp()
{
	noteListModel.SelectPreviousPage();
}

void NoteListPresenter::OnSyncBegin()
{
	noteListView.DisableSync();
}

void NoteListPresenter::OnSyncEnd()
{
	Transaction transaction(userModel);

	UpdateTitle();
	noteListModel.Reload();

	NotebookList notebooks;
	userModel.GetNotebooks(notebooks);
	noteListView.SetNotebookMenu(notebooks);

	UpdateSyncCounter();
	noteListView.EnableSync();
}

void NoteListPresenter::OnSyncStatusUpdated()
{
	noteListView.SetProgress(syncModel.GetSyncProgress());
	noteListView.SetStatusText(syncModel.GetStatusText());
}

void NoteListPresenter::OnUserLoaded()
{
	UpdateNotebookTitle();
	UpdateViewStyle();

	Transaction transaction(userModel);

	UpdateTitle();
	UpdateNotebookListView();

	noteListModel.Reload();

	const wstring username(userModel.GetUsername());

	if (username == L"[anonymous]")
	{
		noteListView.SetProfileText(L"Profile");
		noteListView.SetSigninText(L"Sign in");
		noteListView.HideSyncButton();
	}
	else
	{
		noteListView.SetProfileText(username);
		noteListView.SetSigninText(L"Sign out");
		noteListView.ShowSyncButton();
	}

	UpdateSyncCounter();
}

void NoteListPresenter::OnViewStyleChanged()
{
	MacroWaitCursor;
	noteListModel.SetViewStyle(noteListView.GetRequestedViewStyle());
	UpdateViewStyle();
}

//--------------------------------------------------
// utility functions
//
// Note: transactions are managed at a higher level.
//--------------------------------------------------

void NoteListPresenter::ResetSearch()
{
	noteListView.SetSearchText(L"");
	noteListModel.SetQuery(L"");
	noteListView.SetSearchButtonToSearch();
}

void NoteListPresenter::UpdateActiveNotebook()
{
	Notebook notebook;
	userModel.GetNotebook
		( noteListView.GetSelectedNotebookGuid()
		, notebook
		);
	userModel.MakeNotebookLastUsed(notebook.guid);
}

void NoteListPresenter::UpdateNotebookListView()
{
	NotebookList notebooks;
	userModel.GetNotebooks(notebooks);
	noteListView.SetNotebookMenu(notebooks);
}

void NoteListPresenter::UpdateNotebookTitle()
{
	if (noteListModel.GetNotebookTitleState())
		noteListView.ShowNotebookTitle();
	else
		noteListView.HideNotebookTitle();
}

void NoteListPresenter::UpdateSyncCounter()
{
	Notebook notebook;
	userModel.GetLastUsedNotebook(notebook);

	int noteCount(userModel.GetDirtyNoteCount(notebook));

	int totalCount(noteCount);

	wostringstream stream;
	stream << totalCount;
	noteListView.SetSyncText(stream.str());
}

void NoteListPresenter::UpdateTitle()
{
	Notebook notebook;
	userModel.GetLastUsedNotebook(notebook);

	noteListView.SetWindowTitle(notebook.name);
	noteView.SetWindowTitle(notebook.name);
}

void NoteListPresenter::UpdateViewStyle()
{
	NotebookViewStyle viewStyle(noteListModel.GetViewStyle());
	noteListView.SetViewStyle(viewStyle);
	switch (viewStyle)
	{
	case NotebookViewCombined: noteListModel.SetPageSize(20); break;
	case NotebookViewTitles:   noteListModel.SetPageSize(30); break;
	}
}

wstring NoteListPresenter::ConvertToHtml(const Note & note, const wstring & guid)
{
	wostringstream stream;
	stream << L"<img #thumb src=\"thumb:";
	stream << guid;
	stream << L"\"/><text #time>";
	stream << FormatDate(note.modificationDate);
	stream << L"</text><text #title>";
	stream << FormatTitle(note.name);
	stream << L"</text>";
	return stream.str();
}

wstring NoteListPresenter::FormatTitle(const wstring & title)
{
	return EscapeHtml(title);
}

wstring NoteListPresenter::FormatTags(const TagList & tags)
{
	wostringstream stream;
	bool first = true;
	foreach (const Tag & tag, tags)
	{
		if (!first)
			stream << L", ";
		first = false;
		stream << EscapeHtml(tag.name);
	}
	return stream.str();
}

wstring NoteListPresenter::FormatDate(const Timestamp &timestamp)
{
	return EscapeHtml(timestamp.GetFormattedDateTime());
}
