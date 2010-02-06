#include "stdafx.h"
#include "NoteListPresenter.h"

#include <sstream>

using namespace boost;
using namespace std;

NoteListPresenter::NoteListPresenter
	( INoteListModel & noteListModel
	, INoteListView  & noteListView
	)
	: noteListModel (noteListModel)
	, noteListView  (noteListView)
{
	noteListModel.ConnectReset(bind(&NoteListPresenter::OnResetNoteList, this));
}

void NoteListPresenter::OnResetNoteList()
{
	const vector<const INote*> & notes(noteListModel.GetNotes());
	noteListView.ClearNotes();
	foreach (const INote * note, notes)
		noteListView.AddNote(ConvertToHtml(note));
	noteListView.Update();
}

wstring NoteListPresenter::ConvertToHtml(const INote * note) const
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

wstring NoteListPresenter::FormatTitle(const wstring & title) const
{
	return EscapeHtml(title);
}

wstring NoteListPresenter::FormatTags(const vector<const ITag*> &tags) const
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

wstring NoteListPresenter::FormatDate(const ITimestamp &timestamp) const
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

wstring NoteListPresenter::EscapeHtml(const std::wstring & str) const
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
