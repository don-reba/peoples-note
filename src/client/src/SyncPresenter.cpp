#include "stdafx.h"
#include "SyncPresenter.h"

#include "INoteListModel.h"
#include "INoteListView.h"
#include "ISyncModel.h"
#include "IUserModel.h"
#include "NotebookMenuGenerator.h"
#include "Tools.h"

#include <cmath>

using namespace boost;
using namespace std;
using namespace Tools;

SyncPresenter::SyncPresenter
	( INoteListModel & noteListModel
	, INoteListView  & noteListView
	, ISyncModel     & syncModel
	, IUserModel     & userModel
	)
	: noteListModel (noteListModel)
	, noteListView  (noteListView)
	, syncModel     (syncModel)
	, userModel     (userModel)
{
	noteListView.ConnectSync(bind(&SyncPresenter::OnSync, this));
	syncModel.ConnectSyncComplete(bind(&SyncPresenter::OnSyncComplete, this));
}

void SyncPresenter::OnSync()
{
	Credentials credentials;
	userModel.GetCredentials(credentials);

	syncModel.BeginSync(credentials.GetUsername());
}

void SyncPresenter::OnSyncComplete()
{
	Notebook notebook;
	userModel.GetLastUsedNotebook(notebook);

	NoteList notes;
	userModel.GetNotesByNotebook(notebook, notes);
	noteListModel.SetNotes(notes);

	foreach (Note & note, notes)
		noteListView.UpdateThumbnail(note.guid);

	NotebookList notebooks;
	userModel.GetNotebooks(notebooks);

	wstring menuHtml;
	NotebookMenuGenerator::GetMenuHtml(notebooks, 6, menuHtml);
	noteListView.SetNotebookMenu(menuHtml);
}
