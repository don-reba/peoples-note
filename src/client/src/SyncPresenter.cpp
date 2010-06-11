#include "stdafx.h"
#include "SyncPresenter.h"

#include "INoteListModel.h"
#include "INoteListView.h"
#include "ISyncModel.h"
#include "IUserModel.h"
#include "Tools.h"

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

	noteListView.ClearNotebooks();
	NotebookList notebooks;
	userModel.GetNotebooks(notebooks);
	foreach (const Notebook & notebook, notebooks)
	{
		wstring guid(ConvertToUnicode(notebook.guid));
		noteListView.AddNotebook(notebook.name, guid);
	}
	noteListView.UpdateNotebooks();
}
