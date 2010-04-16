#include "stdafx.h"
#include "MockLastUserModel.h"

using namespace std;

wstring MockLastUserModel::GetUsername()
{
	return username;
}

void MockLastUserModel::SetUsername(const wstring & username)
{
	this->username = username;
}
