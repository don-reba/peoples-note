#include "stdafx.h"
#include "MockCredentialsView.h"

using namespace std;

void MockCredentialsView::ConnectSignIn(ISignalProvider::slot_type OnSignIn)
{
	SignalSignIn.connect(OnSignIn);
}

wstring MockCredentialsView::GetUsername() const
{
	return username;
}

void MockCredentialsView::SetUsername(std::wstring username)
{
	this->username = username;
}

wstring MockCredentialsView::GetPassword() const
{
	return password;
}

void MockCredentialsView::SetPassword(std::wstring password)
{
	this->password = password;
}

void MockCredentialsView::SignIn()
{
	SignalSignIn();
}
