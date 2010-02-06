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
	noteListModel.SetNotes(userModel.GetLastUsedNotebook().GetNotes());
	
	noteListView.ClearNotebooks();
	const vector<INotebook*> notebooks = userModel.GetNotebooks();
	foreach (const INotebook * notebook, notebooks)
		noteListView.AddNotebook(notebook->GetName());
	noteListView.UpdateNotebooks();
}

wstring NoteListPresenter::ConvertToHtml(const INote * note)
{
	wostringstream stream;
	stream << L"<option class=\"note\"><table><tr><td rowspan=\"3\">";
	stream << L"<div id=\"thumb\" /></td><td>";
	stream << FormatTitle(note->GetTitle());
	stream << L"</td></tr><tr><td>";
	stream << FormatTags(note->GetTags());
	stream << L"</td></tr><tr><td>";
	stream << FormatDate(note->GetCreateDate());
	stream << L"</td></tr></table></option>";
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

	// TODO: Move to GetFormattedDateTime implementation

	//const SYSTEMTIME & time   = timestamp.GeTimeString();
	//LCID               locale = LOCALE_USER_DEFAULT;

	//vector<wchar_t> dateChars(11);
	//GetDateFormat
	//	( locale           // Locale
	//	, 0                // dwFlags
	//	, &time            // lpDate
	//	, "yyyy-MM-Dd"     // lpFormat
	//	, &dateChars       // lpDateSTr
	//	, dateChars.size() // cchDate
	//	);

	//vector<wchar_t> timeChars(6);
	//GetTimeFormat
	//	( locale           // Locale
	//	, 0                // dwFlags
	//	, &time            // lpTime
	//	, L"HH:mm"         // lpFOrmat
	//	, &timeChars[0]    // lpTimeStr
	//	, timeChars.size() // cchTime
	//	);

	//wostringstream stream;
	//stream << dateChars << " " << timeChars;
	//return stream.str();
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