#include "stdafx.h"
#include "SyncPresenter.h"

#include "INoteListView.h"
#include "ISyncModel.h"
#include "ILastUserModel.h"
#include "IUserModel.h"

using namespace boost;
using namespace std;

SyncPresenter::SyncPresenter
	( INoteListView  & noteListView
	, ISyncModel     & syncModel
	, ILastUserModel & lastUserModel
	, IUserModel     & userModel
	)
	: noteListView  (noteListView)
	, syncModel     (syncModel)
	, lastUserModel (lastUserModel)
	, userModel     (userModel)
{
	noteListView.ConnectSync(bind(&SyncPresenter::OnSync, this));
}

void SyncPresenter::OnSync()
{
	Guid notebookGuid;
	userModel.GetLastUsedNotebook(notebookGuid);
	syncModel.BeginSync
		( lastUserModel.GetUsername()
		, lastUserModel.GetToken()
		, lastUserModel.GetShard()
		, notebookGuid
		);
}
