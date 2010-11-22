#include "stdafx.h"
#include "SyncPresenter.h"

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
	( INoteListView  & noteListView
	, ISyncModel     & syncModel
	, IUserModel     & userModel
	)
	: noteListView  (noteListView)
	, syncModel     (syncModel)
	, userModel     (userModel)
{
	noteListView.ConnectSync(bind(&SyncPresenter::OnSync, this));
}

void SyncPresenter::OnSync()
{
	Credentials credentials;
	userModel.GetCredentials(credentials);

	syncModel.BeginSync(credentials.GetUsername());
}
