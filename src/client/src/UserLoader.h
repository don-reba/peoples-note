#pragma once

#include "ILastUserModel.h"
#include "INoteListView.h"
#include "IUserModel.h"

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
