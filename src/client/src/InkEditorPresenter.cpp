#include "stdafx.h"
#include "InkEditorPresenter.h"

#include "IInkEditorView.h"
#include "INoteListModel.h"
#include "INoteListView.h"
#include "IUserModel.h"
#include "Resource.h"
#include "Timestamp.h"
#include "Tools.h"
#include "Transaction.h"

using namespace boost;
using namespace std;
using namespace Tools;

InkEditorPresenter::InkEditorPresenter
	( IInkEditorView & inkEditorView
	, INoteListModel & noteListModel
	, INoteListView  & noteListView
	, IUserModel     & userModel
	)
	: inkEditorView (inkEditorView)
	, noteListModel (noteListModel)
	, noteListView  (noteListView)
	, userModel     (userModel)
{
	inkEditorView.ConnectAccept    (bind(&InkEditorPresenter::OnAccept,     this));
	inkEditorView.ConnectCancel    (bind(&InkEditorPresenter::OnCancel,     this));
	noteListView.ConnectNewInkNote (bind(&InkEditorPresenter::OnNewInkNote, this));
}

void InkEditorPresenter::OnAccept()
{
	// create note 
	Note note;
	note.name         = L"Ink note";
	note.creationDate = Timestamp::GetCurrentTime();
	note.isDirty      = true;
	note.usn          = 0;

	// create resource with hash
	Resource image;
	inkEditorView.GetImage(image.Data);
	image.Hash = HashWithMD5(image.Data);
	image.Note = note.guid;

	// create note body referencing resource by hash
	wstring body =
		L"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
		L"<!DOCTYPE en-note SYSTEM \"http://xml.evernote.com/pub/enml2.dtd\">\n"
		L"<en-note><en-media type=\"image/jpeg\" hash=\"";
	body.append(ConvertToUnicode(image.Hash));
	body.append(L"\"/></en-note>");

	// add both to the database
	{
		Transaction transaction(userModel);
		Notebook notebook;
		userModel.GetLastUsedNotebook(notebook);
		userModel.AddNote(note, body, L"", notebook);
		userModel.AddResource(image);
	}

	inkEditorView.Hide();

	noteListModel.Reload();
}

void InkEditorPresenter::OnCancel()
{
	inkEditorView.Hide();
}

void InkEditorPresenter::OnNewInkNote()
{
	inkEditorView.Show();
}
