#pragma once

class ICredentialsModel;
class ICredentialsView;

class CredentialsPresenter
{
private:

	ICredentialsModel & credentialsModel;
	ICredentialsView  & credentialsView;

public:
	
	CredentialsPresenter
		( ICredentialsModel & credentialsModel
		, ICredentialsView  & credentialsView
		);

private:

	void OnCancel();
	void OnCommit();
	void OnOk();
	void OnUpdate();
	void OnViewCreated();

	static std::wstring CreateDisplayUsername(const std::wstring & username);
};
