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
	syncModel.BeginSync();
}

void SyncPresenter::OnSyncComplete()
{
	noteListModel.SetNotes(userModel.GetNotesByNotebook(userModel.GetLastUsedNotebook()));
}
