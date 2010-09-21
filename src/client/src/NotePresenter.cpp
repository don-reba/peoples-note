#include "stdafx.h"
#include "NotePresenter.h"

#include "EnNoteTranslator.h"
#include "Guid.h"
#include "INoteListModel.h"
#include "INoteListView.h"
#include "INoteView.h"
#include "IUserModel.h"
#include "Tools.h"
#include "Transaction.h"

using namespace boost;
using namespace std;
using namespace Tools;

//----------
// interface
//----------

NotePresenter::NotePresenter
	( INoteListModel   & noteListModel
	, INoteListView    & noteListView
	, INoteView        & noteView
	, IUserModel       & userModel
	, EnNoteTranslator & enNoteTranslator
	)
	: noteListModel    (noteListModel)
	, noteListView     (noteListView)
	, noteView         (noteView)
	, userModel        (userModel)
	, enNoteTranslator (enNoteTranslator)
{
	noteListView.ConnectOpenNote (bind(&NotePresenter::OnOpenNote,    this));
	noteView.ConnectClose        (bind(&NotePresenter::OnCloseNote,   this));
}

//---------------
// event handlers
//---------------

void NotePresenter::OnCloseNote()
{
	if (!noteView.IsDirty())
		return;

	wstring bodyHtml;
	noteView.GetBody(bodyHtml);

	wstring bodyXml;
	enNoteTranslator.ConvertToXml(bodyHtml, bodyXml);

	wstring bodyText;
	enNoteTranslator.ConvertToText(bodyXml, bodyText);

	Transaction transaction(userModel);

	Note note;
	noteView.GetNote(note);
	note.isDirty = true;

	Notebook notebook;
	userModel.GetLastUsedNotebook(notebook);
	userModel.AddNote(note, bodyXml, bodyText, notebook);

	noteView.SetNote(note, L"", L"", bodyHtml);

	Thumbnail thumbnail;
	thumbnail.Width  = 164;
	thumbnail.Height = 100;
	noteView.Render(thumbnail);
	userModel.SetNoteThumbnail(note.guid, thumbnail);

	noteListView.UpdateThumbnail(note.guid);

	NoteList notes;
	userModel.GetNotesByNotebook(notebook, notes);
	noteListModel.SetNotes(notes);
}

void NotePresenter::OnOpenNote()
{
	Transaction transaction(userModel);

	Guid guid(noteListView.GetSelectedNoteGuid());

	wstring body;
	userModel.GetNoteBody(guid, body);
	Note note(userModel.GetNote(guid));

	wstring subtitle(L"created on ");
	subtitle.append(note.creationDate.GetFormattedDateTime());

	TagList tags;
	userModel.GetNoteTags(note, tags);
	if (!tags.empty())
	{
		subtitle.append(L"\ntags: ");
		subtitle.append(tags[0].name);
		for (int i = 1; i != tags.size(); ++i)
		{
			subtitle.append(L", ");
			subtitle.append(tags[i].name);
		}
	}

	wstring html;
	enNoteTranslator.ConvertToHtml(body, html);

	noteView.SetNote(note, note.name, subtitle, html);
	noteView.Show();
}
