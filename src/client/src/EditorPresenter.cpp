#include "stdafx.h"
#include "EditorPresenter.h"

#include "EnNoteTranslator.h"
#include "IEditorView.h"
#include "INoteListModel.h"
#include "INoteListView.h"
#include "INoteView.h"
#include "IUserModel.h"
#include "Transaction.h"
#include "WaitCursor.h"

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
	editorView.ConnectAccept     (bind(&EditorPresenter::OnAccept,     this));
	editorView.ConnectCancel     (bind(&EditorPresenter::OnCancel,     this));
	editorView.ConnectDeleteNote (bind(&EditorPresenter::OnDeleteNote, this));
	noteListView.ConnectNewNote  (bind(&EditorPresenter::OnNewNote,    this));
	noteView.ConnectEdit         (bind(&EditorPresenter::OnEditNote,   this));
}

void EditorPresenter::OnAccept()
{
	MacroWaitCursor;

	wstring bodyHtml;
	editorView.GetBody(bodyHtml);

	wstring bodyXml;
	enNoteTranslator.ConvertToXml(bodyHtml, bodyXml);

	wstring bodyText;
	enNoteTranslator.ConvertToText(bodyXml, bodyText);

	Note note;
	editorView.GetNote(note);
	editorView.GetTitle(note.name);
	note.modificationDate = Timestamp::GetCurrentTime();
	note.isDirty = true;

	editorView.Hide();

	{
		Transaction transaction(userModel);

		Notebook notebook;
		userModel.GetLastUsedNotebook(notebook);
		userModel.AddNote(note, bodyXml, bodyText, notebook);

		userModel.DeleteNoteThumbnail(note.guid);
		noteListView.UpdateThumbnail(note.guid);
	}

	noteListModel.Reload();
	noteListModel.NotifyOfNoteChange();
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
	MacroWaitCursor;

	editorView.Show();

	Note note;
	noteView.GetNote(note);

	wstring body;
	noteView.GetBody(body);

	noteView.Hide();

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

	wstring body(L"<p>&nbsp;</p>");

	editorView.SetNote(note, body);
}
