#pragma once

#include "Blob.h"
#include "Note.h"

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

	void ImportNotes(const wchar_t * fileName);

	void ReadFile
		( const wchar_t * fileName
		, std::wstring  & contents
		);
};
