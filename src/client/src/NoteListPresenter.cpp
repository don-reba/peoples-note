#include "stdafx.h"
#include "NoteListPresenter.h"

#include "ICredentialsModel.h"
#include "INoteListModel.h"
#include "INoteListView.h"
#include "INoteView.h"
#include "ISyncModel.h"
#include "IUserModel.h"
#include "NotebookMenuGenerator.h"
#include "NoteView.h"
#include "Notebook.h"
#include "Tools.h"
#include "Transaction.h"

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
	noteListModel.ConnectChanged
		(bind(&NoteListPresenter::OnNoteListChanged, this));

	noteListView.ConnectNotebookSelected
		(bind(&NoteListPresenter::OnNotebookSelected, this));

	noteListView.ConnectPageDown
		(bind(&NoteListPresenter::OnPageDown, this));

	noteListView.ConnectPageUp
		(bind(&NoteListPresenter::OnPageUp, this));

	noteListView.ConnectSync
		(bind(&NoteListPresenter::OnSyncBegin, this));

	syncModel.ConnectNotesChanged
		(bind(&NoteListPresenter::OnNotesChanged, this));

	syncModel.ConnectNotebooksChanged
		(bind(&NoteListPresenter::OnNotebooksChanged, this));

	syncModel.ConnectStatusUpdated
		(bind(&NoteListPresenter::OnSyncStatusUpdated, this));

	syncModel.ConnectSyncComplete
		(bind(&NoteListPresenter::OnSyncEnd, this));

	syncModel.ConnectTagsChanged
		(bind(&NoteListPresenter::OnTagsChanged, this));

	userModel.ConnectLoaded
		(bind(&NoteListPresenter::OnUserLoaded, this));

}

//---------------
// event handlers
//---------------

void NoteListPresenter::OnNotesChanged()
{
	Transaction transaction(userModel);
	UpdateSyncCounter();
	noteListModel.Reload();
}

void NoteListPresenter::OnNotebooksChanged()
{
	Transaction transaction(userModel);
	UpdateSyncCounter();
	UpdateNotebookListView();
}

void NoteListPresenter::OnTagsChanged()
{
	Transaction transaction(userModel);
	UpdateSyncCounter();
}

void NoteListPresenter::OnNotebookSelected()
{
	Transaction transaction(userModel);
	UpdateActiveNotebook();
	UpdateTitle();
	noteListModel.Reload();
	UpdateSyncCounter();
}

void NoteListPresenter::OnNoteListChanged()
{
	if (noteListModel.HasPreviousNotes())
		noteListView.ShowPageUp();
	else
		noteListView.HidePageUp();

	if (noteListModel.HasNextNotes())
		noteListView.ShowPageDown();
	else
		noteListView.HidePageDown();

	NoteList::const_iterator notesBegin;
	NoteList::const_iterator notesEnd;
	noteListModel.GetCurrentPage(notesBegin, notesEnd);
	noteListView.ClearNotes();
	for (; notesBegin != notesEnd; ++notesBegin)
	{
		const Note & note(*notesBegin);
		wstring guid(ConvertToUnicode(note.guid));
		noteListView.AddNote(ConvertToHtml(note, guid), guid);
	}
	noteListView.UpdateNotes();

	UpdateSyncCounter();
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
	noteListView.EnableSync();
}

void NoteListPresenter::OnSyncStatusUpdated()
{
	noteListView.SetProgress(syncModel.GetSyncProgress());
	noteListView.SetStatusText(syncModel.GetStatusText());
}

void NoteListPresenter::OnUserLoaded()
{
	Transaction transaction(userModel);

	UpdateTitle();
	UpdateNotebookListView();

	noteListModel.Reload();

	Credentials credentials;
	userModel.GetCredentials(credentials);
	wstring username(credentials.GetUsername());

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

//--------------------------------------------------
// utility functions
//
// Note: transactions are managed at a higher level.
//--------------------------------------------------

void NoteListPresenter::UpdateActiveNotebook()
{
	Notebook notebook;
	userModel.GetNotebook
		( noteListView.GetSelectedNotebookGuid()
		, notebook
		);
	userModel.MakeNotebookLastUsed(notebook);
}

void NoteListPresenter::UpdateNotebookListView()
{
	NotebookList notebooks;
	userModel.GetNotebooks(notebooks);

	wstring menuHtml;
	NotebookMenuGenerator::GetMenuHtml(notebooks, 6, menuHtml);
	noteListView.SetNotebookMenu(menuHtml);
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

wstring NoteListPresenter::ConvertToHtml(const Note & note, const wstring & guid)
{
	wostringstream stream;
	stream << L"<img #thumb src=\"thumb:";
	stream << guid;
	stream << L"\"/><text #time>";
	stream << FormatDate(note.creationDate);
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

wstring NoteListPresenter::EscapeHtml(const std::wstring & str)
{
	wostringstream stream;
	foreach (wchar_t c, str)
	{
		switch (c)
		{
		case L'<': stream << L"&lt;";   break;
		case L'>': stream << L"&gt;";   break;
		case L'&': stream << L"&amp;";  break;
		case L'"': stream << L"&quot;"; break;
		default:   stream << c;
		}
	}
	return stream.str();
}
