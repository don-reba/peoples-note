#include "stdafx.h"
#include "ProfilePresenter.h"

#include "Credentials.h"
#include "IFlashCard.h"
#include "IProfileView.h"
#include "INoteListView.h"
#include "IUserModel.h"
#include "Tools.h"

using namespace boost;
using namespace std;
using namespace Tools;

ProfilePresenter::ProfilePresenter
	( IFlashCard    & flashCard
	, IProfileView  & profileView
	, INoteListView & noteListView
	, IUserModel    & userModel
	)
	: flashCard    (flashCard)
	, profileView  (profileView)
	, noteListView (noteListView)
	, userModel    (userModel)
{
	noteListView.ConnectProfile (bind(&ProfilePresenter::OnProfile, this));
	profileView.ConnectClose    (bind(&ProfilePresenter::OnClose,   this));
	profileView.ConnectDbMove   (bind(&ProfilePresenter::OnDbMove,  this));
	userModel.ConnectLoaded     (bind(&ProfilePresenter::OnLoaded,  this));
}

//---------------
// event handlers
//---------------

void ProfilePresenter::OnClose()
{
	profileView.Hide();
}

void ProfilePresenter::OnDbMove()
{
	try
	{
		profileView.DisableMoveButton();
		profileView.SetMoveErrorMessage(L"");
		switch (userModel.GetLocation())
		{
		case DbLocationCard:   userModel.MoveToDevice(); break;
		case DbLocationDevice: userModel.MoveToCard();   break;
		case DbLocationNone:   userModel.MoveToDevice(); break;
		}
	}
	catch (std::exception & e)
	{
		profileView.SetMoveErrorMessage(ConvertToUnicode(e.what()).c_str());
	}
}

void ProfilePresenter::OnLoaded()
{
	if (profileView.IsShown())
	{
		SetDbPath(userModel.GetPath());
		SetMoveButtonText(userModel.GetLocation());
		profileView.EnableMoveButton();
	}
}

void ProfilePresenter::OnProfile()
{
	profileView.Show();

	Credentials credentials;
	userModel.GetCredentials(credentials);

	SetDbPath(userModel.GetPath());
	SetDbSize(userModel.GetSize());
	SetMoveButtonText(userModel.GetLocation());
	SetUsername(credentials.GetUsername());

	if (!flashCard.Exists())
		profileView.DisableMoveButton();
}

//------------------
// utility functions
//------------------

void ProfilePresenter::SetDbPath(const wstring & path)
{
	profileView.SetDbPath(path);
}

void ProfilePresenter::SetDbSize(__int64 size)
{
	profileView.SetDbSize(MakeSizeString(size));
}

void ProfilePresenter::SetMoveButtonText(DbLocation location)
{
	switch (userModel.GetLocation())
	{
	case DbLocationCard:
		profileView.SetMoveButtonText(L"Move to device memory");
		break;
	case DbLocationDevice:
		profileView.SetMoveButtonText(L"Move to storage card");
		break;
	}
}

void ProfilePresenter::SetUsername(const wstring & username)
{
	profileView.SetUsername
		( username == L"[anonymous]"
		? L"Profile"
		: username
		);
}
