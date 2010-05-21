#include "stdafx.h"
#include "NoteListPresenter.h"

#include "ICredentialsModel.h"
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

	noteListView.ConnectLoadThumbnail
		(bind(&NoteListPresenter::OnLoadThumbnail, this, _1, _2));

	noteListView.ConnectNotebookSelected
		(bind(&NoteListPresenter::OnNotebookSelected, this));

	noteListView.ConnectSync
		(bind(&NoteListPresenter::OnSyncBegin, this));

	syncModel.ConnectNotesChanged
		(bind(&NoteListPresenter::OnNotesChanged, this));

	syncModel.ConnectNotebooksChanged
		(bind(&NoteListPresenter::OnNotebooksChanged, this));

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

void NoteListPresenter::OnLoadThumbnail(const Guid & guid, Blob *& blob)
{
	Transaction transaction(userModel);

	const SIZE size = { 164, 100 };
	userModel.GetNoteThumbnail(guid, thumbnail);
	if (thumbnail.Width != size.cx || thumbnail.Height != size.cy)
	{
		wstring body;
		userModel.GetNoteBody(guid, body);

		wstring html;
		enNoteTranslator.ConvertToHtml(body, html);

		noteView.SetTitle(L"");
		noteView.SetSubtitle(L"");
		noteView.SetBody(html);

		thumbnail.Width  = size.cx;
		thumbnail.Height = size.cy;
		noteView.Render(thumbnail);
		userModel.SetNoteThumbnail(guid, thumbnail);
	}
	blob = &thumbnail.Data;
}

void NoteListPresenter::OnNotesChanged()
{
	Transaction transaction(userModel);
	UpdateSyncCounter();
	UpdateNoteList();
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
	UpdateNoteList();
	UpdateSyncCounter();
}

void NoteListPresenter::OnNoteListChanged()
{
	const NoteList & notes = noteListModel.GetNotes();
	noteListView.ClearNotes();
	foreach (const Note & note, notes)
	{
		wstring guid(ConvertToUnicode(note.guid));
		noteListView.AddNote(ConvertToHtml(note, guid), guid);
	}
	noteListView.UpdateNotes();
	UpdateSyncCounter();
}

void NoteListPresenter::OnSyncBegin()
{
	noteListView.DisableSync();
}

void NoteListPresenter::OnSyncEnd()
{
	noteListView.EnableSync();
}

void NoteListPresenter::OnUserLoaded()
{
	Transaction transaction(userModel);

	UpdateTitle();
	UpdateNotebookListView();

	UpdateNoteList();

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
	noteListView.ClearNotebooks();
	NotebookList notebooks;
	userModel.GetNotebooks(notebooks);
	foreach (const Notebook & notebook, notebooks)
	{
		wstring guid(ConvertToUnicode(notebook.guid));
		noteListView.AddNotebook(notebook.name, guid);
	}
	noteListView.UpdateNotebooks();
}

void NoteListPresenter::UpdateNoteList()
{
	Notebook notebook;
	userModel.GetLastUsedNotebook(notebook);
	NoteList notes;
	userModel.GetNotesByNotebook(notebook, notes);
	noteListModel.SetNotes(notes);
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
	stream << L"<table><tr><td rowspan=\"3\">";
	stream << L"<div id=\"thumb\"><img width=\"164\" height=\"100\" src=\"thumb:";
	stream << guid;
	stream << L"\"/></div></td><td>";
	stream << FormatTitle(note.name);
	stream << L"</td></tr><tr><td>";
	stream << FormatTags(note.tags);
	stream << L"</td></tr><tr><td>";
	stream << FormatDate(note.creationDate);
	stream << L"</td></tr></table>";
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
