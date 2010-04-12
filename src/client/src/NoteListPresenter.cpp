#include "stdafx.h"
#include "NoteListPresenter.h"

#include "NoteView.h"
#include "Notebook.h"
#include "Tools.h"

#include <sstream>

using namespace boost;
using namespace std;
using namespace Tools;

NoteListPresenter::NoteListPresenter
	( INoteListModel & noteListModel
	, INoteListView  & noteListView
	, INoteView      & noteView
	, IUserModel     & userModel
	)
	: noteListModel (noteListModel)
	, noteListView  (noteListView)
	, noteView      (noteView)
	, userModel     (userModel)
{
	noteListView.ConnectLoadThumbnail(bind(&NoteListPresenter::OnLoadThumbnail, this, _1, _2));
	noteListModel.ConnectChanged(bind(&NoteListPresenter::OnNoteListChanged, this));
	userModel.ConnectLoaded(bind(&NoteListPresenter::OnUserLoaded, this));
}

//---------------
// event handlers
//---------------

void NoteListPresenter::OnLoadThumbnail(const Guid & guid, Blob *& blob)
{
	const SIZE size = { 164, 100 };
	userModel.GetNoteThumbnail(guid, thumbnail);
	if (thumbnail.Width != size.cx || thumbnail.Height != size.cy)
	{
		wstring body;
		userModel.GetNoteBody(guid, body);

		noteView.SetTitle(L"");
		noteView.SetSubtitle(L"");
		noteView.SetBody(body);

		thumbnail.Width  = size.cx;
		thumbnail.Height = size.cy;
		noteView.Render(thumbnail);
		userModel.SetNoteThumbnail(guid, thumbnail);
	}
	blob = &thumbnail.Data;
}

void NoteListPresenter::OnNoteListChanged()
{
	const NoteList & notes = noteListModel.GetNotes();
	noteListView.ClearNotes();
	foreach (const Note & note, notes)
	{
		wstring guid(ConvertToUnicode(note.GetGuid()));
		noteListView.AddNote(ConvertToHtml(note, guid), guid);
	}
	noteListView.UpdateNotes();
}

void NoteListPresenter::OnUserLoaded()
{
	noteListView.ClearNotebooks();
	const NotebookList & notebooks = userModel.GetNotebooks();
	foreach (const Notebook & notebook, notebooks)
		noteListView.AddNotebook(notebook.GetName());
	noteListView.UpdateNotebooks();

	noteListModel.SetNotes(userModel.GetNotesByNotebook(userModel.GetLastUsedNotebook()));
}

//------------------
// utility functions
//------------------

wstring NoteListPresenter::ConvertToHtml(const Note & note, const wstring & guid)
{
	wostringstream stream;
	stream << L"<table><tr><td rowspan=\"3\">";
	stream << L"<div id=\"thumb\"><img width=\"164\" height=\"100\" src=\"thumb:";
	stream << guid;
	stream << L"\"/></div></td><td>";
	stream << FormatTitle(note.GetTitle());
	stream << L"</td></tr><tr><td>";
	stream << FormatTags(note.GetTags());
	stream << L"</td></tr><tr><td>";
	stream << FormatDate(note.GetCreationDate());
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
		stream << EscapeHtml(tag.GetName());
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
