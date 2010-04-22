#pragma once

class INoteListModel;
class INoteListView;
class ISyncModel;
class IUserModel;

class SyncPresenter
{
private:

	INoteListModel & noteListModel;
	INoteListView  & noteListView;
	ISyncModel     & syncModel;
	IUserModel     & userModel;
	IUserModel     & syncUserModel;

public:

	SyncPresenter::SyncPresenter
		( INoteListModel & noteListModel
		, INoteListView  & noteListView
		, ISyncModel     & syncModel
		, IUserModel     & userModel
		, IUserModel     & syncUserModel
		);

private:

	void OnSync();

	void OnSyncComplete();
};
