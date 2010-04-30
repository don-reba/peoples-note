#include "stdafx.h"
#include "NotePresenter.h"

#include "Guid.h"
#include "Tools.h"
#include "Transaction.h"

using namespace boost;
using namespace std;
using namespace Tools;

//----------
// interface
//----------

NotePresenter::NotePresenter
	( INoteListView    & noteListView
	, INoteView        & noteView
	, IUserModel       & userModel
	, EnNoteTranslator & enNoteTranslator
	)
	: noteListView     (noteListView)
	, noteView         (noteView)
	, userModel        (userModel)
	, enNoteTranslator (enNoteTranslator)
{
	noteListView.ConnectOpenNote(bind(&NotePresenter::OnOpenNote, this));
	noteView.ConnectLoadingData(bind(&NotePresenter::OnLoadingData, this, _1, _2));
}

//---------------
// event handlers
//---------------

void NotePresenter::OnLoadingData
	( const wchar_t * uri
	, Blob          & blob
	)
{
	const wchar_t * hash = wcschr(uri, L':');
	if (!hash)
		return;
	++hash;
	Transaction transaction(userModel);
	userModel.GetImageResource(ConvertToAnsi(hash), blob);
}

void NotePresenter::OnOpenNote()
{
	Transaction transaction(userModel);

	Guid guid(noteListView.GetSelectedNoteGuid());

	wstring body;
	userModel.GetNoteBody(guid, body);
	Note note(userModel.GetNote(guid));

	wstring subtitle(L"created on ");
	subtitle.append(note.GetCreationDate().GetFormattedDateTime());

	wstring html;
	enNoteTranslator.ConvertToHtml(body, html);

	noteView.SetBody(html);
	noteView.SetSubtitle(subtitle);
	noteView.SetTitle(note.GetName());
	noteView.Show();
}
