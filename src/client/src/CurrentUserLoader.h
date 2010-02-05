#pragma once

#include "IAppModel.h"
#include "ILastUserModel.h"
#include "IUserModel.h"

class CurrentUserLoader
{
private:

	IAppModel      & appModel;
	IUserModel     & userModel;
	ILastUserModel & lastUserModel;

public:

	CurrentUserLoader
		( IAppModel      & appModel
		, IUserModel     & userModel
		, ILastUserModel & lastUserModel
		);

	void OnStart();
};
