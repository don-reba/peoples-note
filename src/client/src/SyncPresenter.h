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

public:

	SyncPresenter::SyncPresenter
		( INoteListModel & noteListModel
		, INoteListView  & noteListView
		, ISyncModel     & syncModel
		, IUserModel     & userModel
		);

private:

	void OnSync();

	void OnSyncComplete();
};
