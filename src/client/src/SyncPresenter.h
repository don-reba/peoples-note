#pragma once

#include "Notebook.h"

class INoteListView;
class ISyncModel;
class IUserModel;

class SyncPresenter
{
private:

	INoteListView  & noteListView;
	ISyncModel     & syncModel;
	IUserModel     & userModel;

public:

	SyncPresenter::SyncPresenter
		( INoteListView  & noteListView
		, ISyncModel     & syncModel
		, IUserModel     & userModel
		);

private:

	void OnSync();

	void OnSyncComplete();
};
