#pragma once

class ILastUserModel;
class INoteListView;
class IUserModel;

class UserLoader
{
private:

	IUserModel     & userModel;
	ILastUserModel & lastUserModel;
	INoteListView  & noteListView;

public:

	UserLoader
		( IUserModel     & userModel
		, ILastUserModel & lastUserModel
		, INoteListView  & noteListView
		);

	void OnCreated();
};
