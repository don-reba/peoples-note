#include "stdafx.h"

#include "CredentialsPresenter.h"
#include "ICredentialsModel.h"
#include "ICredentialsView.h"
#include "IEnService.h"

using namespace boost;
using namespace std;

CredentialsPresenter::CredentialsPresenter
	( ICredentialsModel & credentialsModel
	, ICredentialsView  & credentialsView
	, IEnService        & enService
	)
	: credentialsModel (credentialsModel)
	, credentialsView  (credentialsView)
	, enService        (enService)
{
	credentialsModel.ConnectNeedsSet(bind(&CredentialsPresenter::OnModelNeedsSet, this));
	credentialsView.ConnectCreated  (bind(&CredentialsPresenter::OnViewCreated,   this));
	credentialsView.ConnectOk       (bind(&CredentialsPresenter::OnOk,            this));
	credentialsView.ConnectCancel   (bind(&CredentialsPresenter::OnCancel,        this));
}

void CredentialsPresenter::OnModelNeedsSet()
{
	credentialsView.Open();
}

void CredentialsPresenter::OnViewCreated()
{
	credentialsView.SetUsername(credentialsModel.GetUsername());
	credentialsView.SetPassword(credentialsModel.GetPassword());
}

void CredentialsPresenter::OnOk()
{
	wstring username(credentialsModel.GetUsername());
	wstring password(credentialsModel.GetPassword());

	IEnService::CredentialsValidity validity = enService.CheckCredentials
		( credentialsModel.GetUsername()
		, credentialsModel.GetPassword()
		);
	if (validity.IsGood)
	{
		credentialsModel.SetCredentials
			( credentialsView.GetUsername()
			, credentialsView.GetPassword()
			);
		credentialsView.Close();
	}
	else
	{
		wstring emptyPassword(L"");
		credentialsView.SetPassword(emptyPassword);
		credentialsView.SetMessage(validity.Message);
	}
}

void CredentialsPresenter::OnCancel()
{
	credentialsView.Close();
}
