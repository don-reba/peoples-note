#pragma once

class INoteListView;
class ISyncModel;
class ILastUserModel;

class SyncPresenter
{
private:

	INoteListView  & noteListView;
	ISyncModel     & syncModel;
	ILastUserModel & lastUserModel;

public:

	SyncPresenter::SyncPresenter
		( INoteListView  & noteListView
		, ISyncModel     & syncModel
		, ILastUserModel & lastUserModel
		);

private:

	void OnSync();

	void OnSyncComplete();
};
