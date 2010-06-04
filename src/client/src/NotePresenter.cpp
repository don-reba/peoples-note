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
	noteView.ConnectClose(bind(&NotePresenter::OnCloseNote, this));
	noteView.ConnectLoadingData(bind(&NotePresenter::OnLoadingData, this, _1, _2));
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

	Transaction transaction(userModel);
	note.isDirty = true;
	Notebook notebook;
	userModel.GetLastUsedNotebook(notebook);
	userModel.AddNote(note, bodyXml, L"", notebook);

	Thumbnail thumbnail;
	thumbnail.Width  = 164;
	thumbnail.Height = 100;
	noteView.Render(thumbnail);
	userModel.SetNoteThumbnail(note.guid, thumbnail);

	noteListView.UpdateThumbnail(note.guid);
}

void NotePresenter::OnLoadingData
	( const wchar_t * uri
	, Blob          & blob
	)
{
	const wchar_t * colonPosition = wcschr(uri, L':');
	if (!colonPosition)
		return;
	const wchar_t * dotPosition = wcschr(colonPosition + 1, L'.');
	if (!dotPosition)
		return;
	wstring hash(colonPosition + 1, dotPosition - colonPosition - 1);
	Transaction transaction(userModel);
	userModel.GetResource(ConvertToAnsi(hash), blob);
}

void NotePresenter::OnOpenNote()
{
	Transaction transaction(userModel);

	Guid guid(noteListView.GetSelectedNoteGuid());

	wstring body;
	userModel.GetNoteBody(guid, body);
	note = userModel.GetNote(guid);

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

	noteView.SetBody(html);
	noteView.SetSubtitle(subtitle);
	noteView.SetTitle(note.name);
	noteView.Show();
}
