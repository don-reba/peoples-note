#include "stdafx.h"
#include "EditorPresenter.h"

#include "IEditorView.h"
#include "INoteListView.h"
#include "INoteView.h"
#include "IUserModel.h"
#include "EnNoteTranslator.h"
#include "Transaction.h"

using namespace boost;
using namespace std;

EditorPresenter::EditorPresenter
	( IEditorView   & editorView
	, INoteListView & noteListView
	, INoteView     & noteView
	, IUserModel    & userModel
	, EnNoteTranslator & enNoteTranslator
	)
	: editorView       (editorView)
	, enNoteTranslator (enNoteTranslator)
	, noteListView     (noteListView)
	, noteView         (noteView)
	, userModel        (userModel)
{
	editorView.ConnectAccept    (bind(&EditorPresenter::OnAccept,   this));
	editorView.ConnectCancel    (bind(&EditorPresenter::OnCancel,   this));
	noteListView.ConnectNewNote (bind(&EditorPresenter::OnNewNote,  this));
	noteView.ConnectEdit        (bind(&EditorPresenter::OnEditNote, this));
}

void EditorPresenter::OnAccept()
{
	editorView.Hide();

	wstring bodyHtml;
	editorView.GetBody(bodyHtml);

	wstring bodyXml;
	enNoteTranslator.ConvertToXml(bodyHtml, bodyXml);

	Transaction transaction(userModel);
	Guid guid(noteListView.GetSelectedNoteGuid());
	Note note(userModel.GetNote(guid));
	editorView.GetTitle(note.name);
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

	editorView.SetTitle(L"");
	editorView.SetBody(L"");
}

void EditorPresenter::OnCancel()
{
	editorView.Hide();
	editorView.SetTitle(L"");
	editorView.SetBody(L"");
}

void EditorPresenter::OnEditNote()
{
	wstring bodyHtml;
	noteView.GetBody(bodyHtml);

	wstring title;
	noteView.GetTitle(title);

	editorView.SetBody(bodyHtml);
	editorView.SetTitle(title);

	editorView.Show();
	noteView.Hide();
}

void EditorPresenter::OnNewNote()
{
	editorView.SetTitle(L"");
	editorView.SetBody(L"");
	editorView.Show();
}
