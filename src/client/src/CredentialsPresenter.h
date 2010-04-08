#pragma once

class ICredentialsModel;
class ICredentialsView;
class IEnService;

class CredentialsPresenter
{
private:

	ICredentialsModel & credentialsModel;
	ICredentialsView  & credentialsView;
	IEnService        & enService;

public:
	
	CredentialsPresenter
		( ICredentialsModel & credentialsModel
		, ICredentialsView  & credentialsView
		, IEnService        & enService
		);

private:

	void OnModelUpdating();
	void OnViewCreated();
	void OnOk();
	void OnCancel();
};
