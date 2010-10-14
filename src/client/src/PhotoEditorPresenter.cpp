#include "stdafx.h"
#include "PhotoEditorPresenter.h"

#include "INoteListModel.h"
#include "INoteListView.h"
#include "IPhotoEditorView.h"
#include "IUserModel.h"
#include "Note.h"
#include "Notebook.h"
#include "Tools.h"
#include "Transaction.h"

using namespace boost;
using namespace std;
using namespace Tools;

PhotoEditorPresenter::PhotoEditorPresenter
	( INoteListModel   & noteListModel
	, INoteListView    & noteListView
	, IPhotoEditorView & photoEditorView
	, IUserModel       & userModel
	)
	: noteListModel   (noteListModel)
	, noteListView    (noteListView)
	, photoEditorView (photoEditorView)
	, userModel       (userModel)
{
	noteListView.ConnectNewPhotoNote (bind(&PhotoEditorPresenter::OnNewPhotoNote, *this));
	photoEditorView.ConnectCancel    (bind(&PhotoEditorPresenter::OnCancel,       *this));
	photoEditorView.ConnectOk        (bind(&PhotoEditorPresenter::OnOk,           *this));
	photoEditorView.ConnectCapture   (bind(&PhotoEditorPresenter::OnCapture,      *this));
}

void PhotoEditorPresenter::OnCancel()
{
	photoEditorView.Hide();
}

void PhotoEditorPresenter::OnCapture()
{
	// create note 
	Note note;
	note.name         = photoEditorView.GetTitle();
	note.creationDate = Timestamp::GetCurrentTime();
	note.isDirty      = true;
	note.usn          = 0;

	// create resource with hash
	Resource image;
	if (!ReadFile(photoEditorView.GetImagePath(), image.Data))
	{
		photoEditorView.Hide();
		return;
	}
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

	photoEditorView.Hide();

	noteListModel.Reload();
}

void PhotoEditorPresenter::OnNewPhotoNote()
{
	photoEditorView.Show();
}

void PhotoEditorPresenter::OnOk()
{
	photoEditorView.InitiateCapture();
}
