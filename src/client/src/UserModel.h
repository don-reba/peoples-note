#pragma once
#include "IUserModel.h"

#include "CredentialsModel.h"

class IDataStore;

class UserModel : public IUserModel
{
private:

	IDataStore & dataStore;

	std::wstring folder;

	NotebookList notebooks;
	NoteList     notes;

	signal SignalLoaded;

// interface

public:

	UserModel(IDataStore & dataStore, std::wstring folder);

	int GetNotebookCount();

	int GetVersion();

// IUserModel implementation

public:

	virtual void AddImageResource
		( std::string  hash
		, const Blob & data
		, Guid         note
		);

	virtual void AddNote
		( const Note          & note
		, const std::wstring  & body
		, const std::wstring  & bodyText
		, const Notebook      & notebook
		);

	virtual void AddNotebook(const Notebook & notebook);

	virtual void BeginTransaction();

	virtual void ConnectLoaded(slot_type OnLoaded);

	virtual void EndTransaction();

	virtual bool Exists(const std::wstring & username);

	virtual Credentials GetCredentials();

	virtual Notebook GetDefaultNotebook();

	virtual Notebook GetLastUsedNotebook();

	virtual void GetImageResource(std::string hash, Blob & blob);

	virtual Note GetNote(Guid guid);

	virtual void GetNoteBody(Guid guid, std::wstring & body);

	virtual void GetNoteThumbnail
		( const Guid & guid
		, Thumbnail & thumbnail
		);

	virtual const NotebookList & GetNotebooks();

	virtual const NoteList & GetNotesByNotebook(const Notebook & notebook);

	virtual const NoteList & GetNotesBySearch(std::wstring search);

	virtual void Load(const std::wstring & username);

	virtual void LoadAs
		( const std::wstring & oldUsername
		, const std::wstring & newUsername
		);

	virtual void LoadOrCreate(const std::wstring & username);

	virtual void MakeNotebookDefault(const Notebook & notebook);

	virtual void MakeNotebookLastUsed(const Notebook & notebook);

	virtual void SetCredentials
		( const std::wstring & username
		, const std::wstring & password
		);
	
	virtual void SetNoteThumbnail
		( const Guid      & guid
		, const Thumbnail & thumbnail
		);

	virtual void Unload();

// utility functions

private:

	void SetProperty(std::wstring key, std::wstring value);

	void Create(std::wstring path);

	std::wstring CreatePathFromName(std::wstring name);

	void CreateTable(const char * sql);

	std::wstring GetProperty(std::wstring name);

	void Initialize(std::wstring name);

	void SetPragma(const char * sql);

	bool TryLoad(std::wstring path);

	void Update();
};
