#pragma once
#include "IUserModel.h"

#include "CredentialsModel.h"

class IDataStore;

class UserModel : public IUserModel
{
private:

	IDataStore & dataStore;

	std::wstring folder;

	CredentialsModel credentialsModel;

	NotebookList notebooks;
	NoteList     notes;

	signal SignalLoaded;

// interface

public:

	UserModel(IDataStore & dataStore, std::wstring folder);

	int GetNotebookCount();

	std::wstring GetUser();

	int GetVersion();

// IUserModel implementation

public:

	virtual void AddNote
		( const Note          & note
		, const std::wstring  & body
		, const std::wstring  & bodyText
		, const Notebook      & notebook
		);

	virtual void AddNotebook(const Notebook & notebook);

	virtual void ConnectLoaded(slot_type OnLoaded);

	virtual void CreateDefaultUser();

	virtual Notebook GetDefaultNotebook();

	virtual Notebook GetLastUsedNotebook();

	virtual std::wstring GetNoteBody(Guid guid);

	virtual const NotebookList & GetNotebooks();

	virtual const NoteList & GetNotesByNotebook(const Notebook & notebook);

	virtual const NoteList & GetNotesBySearch(std::wstring search);

	virtual void Load();

	virtual void MakeNotebookDefault(const Notebook & notebook);

	virtual void MakeNotebookLastUsed(const Notebook & notebook);
	
	virtual void SetCredentials(const ICredentialsModel & credentials);

// utility functions

private:

	void AddProperty(std::wstring key, std::wstring value);

	void Create(std::wstring path);

	std::wstring CreatePathFromName(std::wstring name);

	void CreateTable(const char * sql);

	std::wstring GetProperty(std::wstring name);

	void Initialize(std::wstring name);

	void LoadOrCreate(std::wstring name);

	void SetPragma(const char * sql);

	bool TryLoad(std::wstring path);
};
