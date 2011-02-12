#include "stdafx.h"
#include "EditorPresenter.h"

#include "EnNoteTranslator.h"
#include "IEditorView.h"
#include "INoteListModel.h"
#include "INoteListView.h"
#include "INoteView.h"
#include "IUserModel.h"
#include "Transaction.h"

using namespace boost;
using namespace std;

EditorPresenter::EditorPresenter
	( IEditorView      & editorView
	, INoteListModel   & noteListModel
	, INoteListView    & noteListView
	, INoteView        & noteView
	, IUserModel       & userModel
	, EnNoteTranslator & enNoteTranslator
	)
	: editorView       (editorView)
	, enNoteTranslator (enNoteTranslator)
	, noteListModel    (noteListModel)
	, noteListView     (noteListView)
	, noteView         (noteView)
	, userModel        (userModel)
{
	editorView.ConnectAccept     (bind(&EditorPresenter::OnAccept,    this));
	editorView.ConnectCancel     (bind(&EditorPresenter::OnCancel,    this));
	editorView.ConnectDeleteNote (bind(&EditorPresenter::OnDeleteNote, this));
	noteListView.ConnectNewNote  (bind(&EditorPresenter::OnNewNote,   this));
	noteView.ConnectEdit         (bind(&EditorPresenter::OnEditNote,  this));
}

void EditorPresenter::OnAccept()
{
	wstring bodyHtml;
	editorView.GetBody(bodyHtml);

	wstring bodyXml;
	enNoteTranslator.ConvertToXml(bodyHtml, bodyXml);

	wstring bodyText;
	enNoteTranslator.ConvertToText(bodyXml, bodyText);

	Transaction transaction(userModel);

	Note note;
	editorView.GetNote(note);
	editorView.GetTitle(note.name);
	note.isDirty = true;

	Notebook notebook;
	userModel.GetLastUsedNotebook(notebook);
	userModel.AddNote(note, bodyXml, bodyText, notebook);

	noteView.SetNote(note, L"", L"", bodyHtml);

	SIZE thumbnailSize;
	noteListView.GetThumbSize(thumbnailSize);

	Thumbnail thumbnail;
	thumbnail.Width  = thumbnailSize.cx;
	thumbnail.Height = thumbnailSize.cy;
	noteView.Render(thumbnail);
	userModel.SetNoteThumbnail(note.guid, thumbnail);

	noteListView.UpdateThumbnail(note.guid);

	editorView.Hide();

	noteListModel.Reload();
}

void EditorPresenter::OnCancel()
{
	editorView.Hide();
}

void EditorPresenter::OnDeleteNote()
{
	Note note;
	editorView.GetNote(note);
	userModel.DeleteNote(note.guid);
	
	editorView.Hide();

	noteListModel.Reload();
}

void EditorPresenter::OnEditNote()
{
	editorView.Show();
	noteView.Hide();

	Note note;
	noteView.GetNote(note);

	wstring body;
	noteView.GetBody(body);

	editorView.SetNote(note, body);
}

void EditorPresenter::OnNewNote()
{
	editorView.Show();
	
	Notebook notebook;
	userModel.GetLastUsedNotebook(notebook);

	Note note;
	note.name         = L"New note in ";
	note.name.append(notebook.name);
	note.creationDate = Timestamp::GetCurrentTime();
	note.isDirty      = true;
	note.usn          = 0;

	wstring body(L"<div type=\"en-note\" />");

	editorView.SetNote(note, body);
}
