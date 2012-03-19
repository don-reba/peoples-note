#include "stdafx.h"
#include "ProfilePresenter.h"

#include "IFlashCard.h"
#include "IProfileView.h"
#include "INoteListView.h"
#include "IUserModel.h"
#include "Tools.h"
#include "WaitCursor.h"

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
	profileView.ConnectCompact  (bind(&ProfilePresenter::OnCompact, this));
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

void ProfilePresenter::OnCompact()
{
	MacroWaitCursor;
	profileView.SetMessage(L"This might take a while...");
	userModel.Compact();
	profileView.SetMessage(L"");
	SetDbSize(userModel.GetSize());
}

void ProfilePresenter::OnDbMove()
{
	MacroWaitCursor;

	try
	{
		profileView.DisableMoveButton();
		profileView.SetMessage(L"");
		switch (userModel.GetLocation())
		{
		case DbLocationCard:   userModel.MoveToDevice(); break;
		case DbLocationDevice: userModel.MoveToCard();   break;
		case DbLocationNone:   userModel.MoveToDevice(); break;
		}
	}
	catch (std::exception & e)
	{
		profileView.SetMessage(ConvertToUnicode(e.what()).c_str());
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

	SetDbPath(userModel.GetPath());
	SetDbSize(userModel.GetSize());
	SetMoveButtonText(userModel.GetLocation());
	SetUsername(userModel.GetUsername());

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
