#pragma once

#include "Blob.h"

class IEnImporter;
class INoteListModel;
class INoteListView;
class IUserModel;

class EnImportPresenter
{
private:

	IEnImporter    & enImporter;
	INoteListModel & noteListModel;
	INoteListView  & noteListView;
	IUserModel     & userModel;

public:

	EnImportPresenter
		( IEnImporter    & enImporter
		, INoteListModel & noteListModel
		, INoteListView  & noteListView
		, IUserModel     & userModel
		);

private:

	void OnImport();

private:

	static std::string Hash(const Blob & blob);
};
