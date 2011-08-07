#pragma once
#include "IUserModel.h"

#include "CredentialsModel.h"

class IDataStore;
class IFlashCard;

class UserModel : public IUserModel
{
// events

	MacroEvent(Loaded)

// data

private:

	IDataStore & dataStore;

	const IFlashCard & flashCard;

	const std::wstring deviceFolder;

// interface

public:

	UserModel
		( IDataStore         & dataStore
		, const std::wstring & deviceFolder
		, const IFlashCard   & flash
		);

	int GetNoteCount();

	int GetNotebookCount();

	int GetResourceCount();

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

	virtual void AddRecognitionEntry(const RecognitionEntry & entry);

	virtual void AddResource(const Resource & resource);

	virtual void AddTag(const Tag & tag);

	virtual void AddTagToNote
		( const std::wstring & tagName
		, const Note         & note
		);

	virtual void BeginTransaction();

	virtual void DeleteNote(const Guid & note);

	virtual void DeleteNoteThumbnail(const Guid & note);

	virtual void EndTransaction();

	virtual bool Exists(const std::wstring & username);

	virtual void ExpungeNote(const Guid & note);

	virtual void ExpungeNotebook(const Guid & notebook);

	virtual void ExpungeTag(const Guid & tag);

	virtual void GetCredentials(Credentials & credentials);

	virtual void GetDefaultNotebook(Notebook & notebook);

	virtual void GetDeletedNotes(GuidList & notes);

	virtual int GetDirtyNoteCount(const Notebook & notebook);

	virtual __int64 GetLastSyncEnTime();

	virtual void GetLastUsedNotebook(Notebook & notebook);

	virtual DbLocation GetLocation();

	virtual Note GetNote(Guid guid);

	virtual void GetNoteBody
		( const Guid   & guid
		, std::wstring & body
		);

	virtual void GetNoteResources
		( const Guid        & note
		, std::vector<Guid> & resources
		);

	virtual void GetNoteTags
		( const Note & note
		, TagList    & tags
		);

	virtual void GetNoteThumbnail
		( const Guid & guid
		, Thumbnail  & thumbnail
		);

	virtual void GetNotebook
		( const Guid & guid
		, Notebook   & notebook
		);

	virtual void GetNotebooks(NotebookList & notebooks);

	virtual int GetNotebookUpdateCount(const Guid & notebook);

	virtual void GetNotesByNotebook
		( const Notebook & notebook
		, NoteList       & notes
		);

	virtual void GetNotesBySearch
		( const std::wstring & search
		, NoteList           & notes
		);

	virtual std::wstring GetPath();

	virtual __int64 GetSize();

	virtual void GetResource
		( const std::string & hash
		, Blob              & blob
		);

	virtual void GetResource
		( const Guid & guid
		, Resource   & resource
		);

	virtual void GetTags(TagList & tags);

	virtual int GetUpdateCount();

	virtual void Load(const std::wstring & username);

	virtual void LoadAs
		( const std::wstring & oldUsername
		, const std::wstring & newUsername
		);

	virtual void LoadOrCreate(const std::wstring & username);

	virtual void MakeNotebookDefault(const Guid & notebook);

	virtual void MakeNotebookLastUsed(const Guid & notebook);

	virtual void MoveToCard();

	virtual void MoveToDevice();

	virtual void RemoveNoteTags(const Guid & note);

	virtual void SetCredentials
		( const std::wstring & username
		, const std::wstring & password
		);

	virtual void SetLastSyncEnTime(__int64 enTime);

	virtual void SetNotebookUpdateCount
		( const Guid & notebook
		, int          updateCount
		);

	virtual void SetNoteThumbnail
		( const Guid      & guid
		, const Thumbnail & thumbnail
		);

	virtual void SetUpdateCount(int updateCount);

	virtual void Unload();

	virtual void UpdateNotebook
		( const Notebook & notebook
		, const Notebook & replacement
		);

	virtual void UpdateTag
		( const Tag & tag
		, const Tag & replacement
		);

// utility functions

private:

	void Create
		( const std::wstring & path
		, DbLocation           location
		);

	std::wstring CreatePathFromName
		( const std::wstring & folder
		, const std::wstring & name
		);

	void GetFirstNotebook(Notebook & notebook);

	template<typename T>
	void GetProperty(const std::wstring & name, T & value);

	void Initialize(std::wstring name);

	void MigrateFrom3To4();

	void MigrateFrom4To5();

	void Move
		( const std::wstring & oldPath
		, const std::wstring & newPath
		, const std::wstring & username
		);

	void SetPragma(const char * sql);

	template <typename T>
	void SetProperty(const std::wstring & key, const T & value);

	bool TryLoad
		( const std::wstring & path
		, DbLocation           location
		);

	void Update();
};

template <typename T>
void UserModel::GetProperty(const std::wstring & key, T & value)
{
	IDataStore::Statement statement = dataStore.MakeStatement
		( "SELECT value"
		"  FROM Properties"
		"  WHERE key = ?"
		"  LIMIT 1"
		);
	statement->Bind(1, key);
	if (statement->Execute())
		throw std::exception("Property not found.");
	statement->Get(0, value);
}

template <typename T>
void UserModel::SetProperty(const std::wstring & key, const T & value)
{
	IDataStore::Statement statement = dataStore.MakeStatement
		( "INSERT OR REPLACE INTO Properties VALUES (?, ?)"
		);
	statement->Bind(1, key);
	statement->Bind(2, value);
	statement->Execute();
}
