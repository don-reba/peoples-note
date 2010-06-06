#include "stdafx.h"
#include "AboutPresenter.h"

#include "IAboutView.h"
#include "INoteListView.h"

using namespace boost;

AboutPresenter::AboutPresenter
	( IAboutView    & aboutView
	, INoteListView & noteListView
	)
	: aboutView    (aboutView)
	, noteListView (noteListView)
{
	noteListView.ConnectAbout(bind(&AboutPresenter::OnAbout, this));
	aboutView.ConnectClose(bind(&AboutPresenter::OnClose, this));
}

void AboutPresenter::OnAbout()
{
	aboutView.Show();
}

void AboutPresenter::OnClose()
{
	aboutView.Hide();
}
