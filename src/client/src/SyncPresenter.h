#pragma once

class INoteListView;
class ISyncModel;
class ILastUserModel;
class IUserModel;

class SyncPresenter
{
private:

	INoteListView  & noteListView;
	ISyncModel     & syncModel;
	ILastUserModel & lastUserModel;
	IUserModel     & userModel;

public:

	SyncPresenter::SyncPresenter
		( INoteListView  & noteListView
		, ISyncModel     & syncModel
		, ILastUserModel & lastUserModel
		, IUserModel     & userModel
		);

private:

	void OnSync();

	void OnSyncComplete();
};
