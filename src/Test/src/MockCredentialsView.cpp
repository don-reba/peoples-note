#include "stdafx.h"
#include "MockCredentialsView.h"

using namespace std;

MockCredentialsView::MockCredentialsView()
	: isOpen            (false)
	, isPasswordFocused (false)
	, isUsernameFocused (false)
{
}

void MockCredentialsView::Close()
{
	isOpen = false;
}

void MockCredentialsView::ConnectCancel(slot_type OnCancel)
{
	SignalCancel.connect(OnCancel);
}

void MockCredentialsView::ConnectCreated(slot_type OnCreated)
{
	SignalCreated.connect(OnCreated);
}

void MockCredentialsView::ConnectOk(slot_type OnOk)
{
	SignalOk.connect(OnOk);
}

wstring MockCredentialsView::GetPassword() const
{
	return password;
}

wstring MockCredentialsView::GetUsername() const
{
	return username;
}

void MockCredentialsView::Open()
{
	isOpen = true;
}

void MockCredentialsView::SetFocusToPassword()
{
	isPasswordFocused = true;
	isUsernameFocused = false;
}

void MockCredentialsView::SetFocusToUsername()
{
	isPasswordFocused = false;
	isUsernameFocused = true;
}

void MockCredentialsView::SetMessage(const wstring & message)
{
	this->message = message;
}

void MockCredentialsView::SetPassword(const std::wstring & password)
{
	this->password = password;
}

void MockCredentialsView::SetUsername(const std::wstring & username)
{
	this->username = username;
}
