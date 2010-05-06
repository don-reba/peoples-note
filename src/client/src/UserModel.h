#pragma once
#include "IUserModel.h"

#include "CredentialsModel.h"

class IDataStore;

class UserModel : public IUserModel
{
private:

	IDataStore & dataStore;

	std::wstring folder;

	signal SignalLoaded;

// interface

public:

	UserModel(IDataStore & dataStore, std::wstring folder);

	int GetNotebookCount();

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

	virtual void AddResource(const Resource & resource);

	virtual void AddTag(const Tag & tag);

	virtual void BeginTransaction();

	virtual void ConnectLoaded(slot_type OnLoaded);

	virtual void DeleteNote(const Note & note);

	virtual void DeleteNotebook(const Notebook & notebook);

	virtual void DeleteTag(const Tag & tag);

	virtual void EndTransaction();

	virtual bool Exists(const std::wstring & username);

	virtual void GetCredentials(Credentials & credentials);

	virtual void GetDefaultNotebook(Notebook & notebook);

	virtual std::wstring GetFolder() const;

	virtual void GetLastUsedNotebook(Notebook & notebook);

	virtual Note GetNote(Guid guid);

	virtual void GetNoteBody(Guid guid, std::wstring & body);

	virtual void GetNoteThumbnail
		( const Guid & guid
		, Thumbnail  & thumbnail
		);

	virtual void GetNotebooks(NotebookList & notebooks);

	virtual void GetNotesByNotebook
		( const Notebook & notebook
		, NoteList       & notes
		);

	virtual void GetNotesBySearch
		( const std::wstring & search
		, NoteList           & notes
		);

	virtual void GetResource
		( const std::string & hash
		, Blob              & blob
		);

	virtual void GetResource
		( const Guid & guid
		, Resource   & resource
		);

	virtual void GetTags(TagList & tags);

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
