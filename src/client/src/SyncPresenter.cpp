#include "stdafx.h"
#include "SyncPresenter.h"

#include "INoteListModel.h"
#include "INoteListView.h"
#include "ISyncModel.h"
#include "IUserModel.h"

using namespace boost;

SyncPresenter::SyncPresenter
	( INoteListModel & noteListModel
	, INoteListView  & noteListView
	, ISyncModel     & syncModel
	, IUserModel     & userModel
	, IUserModel     & syncUserModel
	)
	: noteListModel (noteListModel)
	, noteListView  (noteListView)
	, syncModel     (syncModel)
	, userModel     (userModel)
	, syncUserModel (syncUserModel)
{
	noteListView.ConnectSync(bind(&SyncPresenter::OnSync, this));
	syncModel.ConnectSyncComplete(bind(&SyncPresenter::OnSyncComplete, this));
}

void SyncPresenter::OnSync()
{
	Credentials credentials;
	userModel.GetCredentials(credentials);

	syncUserModel.Unload();
	syncUserModel.Load(credentials.GetUsername());
	syncModel.BeginSync(syncUserModel);
}

void SyncPresenter::OnSyncComplete()
{
	Notebook notebook;
	userModel.GetLastUsedNotebook(notebook);
	NoteList notes;
	userModel.GetNotesByNotebook(notebook, notes);
	noteListModel.SetNotes(notes);
}
