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

	void OnModelNeedsSet();
	void OnViewCreated();
	void OnOk();
	void OnCancel();

private:

	static void AreCredentialsValid
		( const std::wstring & username
		, const std::wstring & password
		);
};
