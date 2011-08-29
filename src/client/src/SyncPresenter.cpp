#include "stdafx.h"
#include "SyncPresenter.h"

#include "INoteListView.h"
#include "ISyncModel.h"
#include "ILastUserModel.h"

using namespace boost;
using namespace std;

SyncPresenter::SyncPresenter
	( INoteListView  & noteListView
	, ISyncModel     & syncModel
	, ILastUserModel & lastUserModel
	)
	: noteListView  (noteListView)
	, syncModel     (syncModel)
	, lastUserModel (lastUserModel)
{
	noteListView.ConnectSync(bind(&SyncPresenter::OnSync, this));
}

void SyncPresenter::OnSync()
{
	syncModel.BeginSync
		( lastUserModel.GetUsername()
		, lastUserModel.GetPassword()
		);
}
