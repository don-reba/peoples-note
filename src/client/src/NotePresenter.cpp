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
	noteListView.ConnectOpenNote   (bind(&NotePresenter::OnOpenNote,       this));
	noteView.ConnectClose          (bind(&NotePresenter::OnCloseNote,      this));
	noteView.ConnectToggleMaximize (bind(&NotePresenter::OnToggleMaximize, this));
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

	Note note;
	noteView.GetNote(note);
	note.modificationDate = Timestamp::GetCurrentTime();
	note.isDirty = true;

	{
		Transaction transaction(userModel);

		Notebook notebook;
		userModel.GetLastUsedNotebook(notebook);
		userModel.AddNote(note, bodyXml, bodyText, notebook);

		userModel.DeleteNoteThumbnail(note.guid);
		noteListView.UpdateThumbnail(note.guid);
	}

	noteListModel.Reload();
}

void NotePresenter::OnOpenNote()
{
	Transaction transaction(userModel);

	Guid guid(noteListView.GetSelectedNoteGuid());

	wstring body;
	userModel.GetNoteBody(guid, body);
	Note note;
	userModel.GetNote(guid, note);

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

	//wstring attachment =
	//	L"<div><img src='audio-attachment.png' />Placeholder 1 with a very long title that will never fit onto a single line</div>"
	//	L"<div><img src='audio-attachment.png' />Placeholder 2</div>";

	noteView.SetNote(note, note.name, subtitle, html, L"", true);
	noteView.Show();
}

void NotePresenter::OnToggleMaximize()
{
	if (noteView.IsMaximized())
		noteView.RestoreWindow();
	else
		noteView.MaximizeWindow();
}
