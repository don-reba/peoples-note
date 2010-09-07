#include "stdafx.h"
#include "ProfilePresenter.h"

#include "IProfileView.h"
#include "INoteListView.h"

using namespace boost;

ProfilePresenter::ProfilePresenter
	( IProfileView  & profileView
	, INoteListView & noteListView
	)
	: profileView  (profileView)
	, noteListView (noteListView)
{
	noteListView.ConnectProfile(bind(&ProfilePresenter::OnProfile, this));
	profileView.ConnectClose(bind(&ProfilePresenter::OnClose, this));
}

void ProfilePresenter::OnProfile()
{
	profileView.Show();
}

void ProfilePresenter::OnClose()
{
	profileView.Hide();
}
