#include "stdafx.h"
#include "NoteListPresenter.h"

#include <sstream>

using namespace boost;
using namespace std;

NoteListPresenter::NoteListPresenter
	( INoteListModel & noteListModel
	, INoteListView  & noteListView
	, IUserModel     & userModel
	)
	: noteListModel (noteListModel)
	, noteListView  (noteListView)
	, userModel     (userModel)
{
	noteListModel.ConnectChanged(bind(&NoteListPresenter::OnNoteListChanged, this));
	userModel.ConnectLoaded(bind(&NoteListPresenter::OnUserLoaded, this));
}

void NoteListPresenter::OnNoteListChanged()
{
	vector<INote*> notes = noteListModel.GetNotes();
	noteListView.ClearNotes();
	foreach (const INote * note, notes)
		noteListView.AddNote(ConvertToHtml(note));
	noteListView.UpdateNotes();
}

void NoteListPresenter::OnUserLoaded()
{
	noteListView.ClearNotebooks();
	const vector<INotebook*> notebooks = userModel.GetNotebooks();
	foreach (const INotebook * notebook, notebooks)
		noteListView.AddNotebook(notebook->GetName());
	noteListView.UpdateNotebooks();

	noteListModel.SetNotes(userModel.GetLastUsedNotebook().GetNotes());
}

wstring NoteListPresenter::ConvertToHtml(const INote * note)
{
	wostringstream stream;
	stream << L"<table><tr><td rowspan=\"3\">";
	stream << L"<div id=\"thumb\" /></td><td>";
	stream << FormatTitle(note->GetTitle());
	stream << L"</td></tr><tr><td>";
	stream << FormatTags(note->GetTags());
	stream << L"</td></tr><tr><td>";
	stream << FormatDate(note->GetCreateDate());
	stream << L"</td></tr></table>";
	return stream.str();
}

wstring NoteListPresenter::FormatTitle(const wstring & title)
{
	return EscapeHtml(title);
}

wstring NoteListPresenter::FormatTags(const vector<const ITag*> &tags)
{
	wostringstream stream;
	bool first = true;
	foreach (const ITag * tag, tags)
	{
		if (!first)
			stream << L", ";
		first = false;
		stream << EscapeHtml(tag->GetName());
	}
	return stream.str();
}

wstring NoteListPresenter::FormatDate(const ITimestamp &timestamp)
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
