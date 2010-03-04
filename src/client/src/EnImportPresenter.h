#pragma once

class IEnImporter;
class INoteListView;
class IUserModel;

class EnImportPresenter
{
private:

	IEnImporter   & enImporter;
	INoteListView & noteListView;
	IUserModel    & userModel;

public:

	EnImportPresenter
		( IEnImporter   & enImporter
		, INoteListView & noteListView
		, IUserModel    & userModel
		);

private:

	void OnImport();
};
