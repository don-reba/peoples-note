#pragma once

#include "ILastUserModel.h"
#include "INoteListView.h"
#include "IUserModel.h"

class CurrentUserLoader
{
private:

	IUserModel     & userModel;
	ILastUserModel & lastUserModel;
	INoteListView  & noteListView;

public:

	CurrentUserLoader
		( IUserModel     & userModel
		, ILastUserModel & lastUserModel
		, INoteListView  & noteListView
		);

	void OnCreated();
};
