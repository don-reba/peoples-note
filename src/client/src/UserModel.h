#pragma once
#include "IUserModel.h"

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

	int GetRecognitionEntryCount();

	int GetResourceCount();

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
		( const Guid & tag
		, const Note & note
		);

	virtual void BeginTransaction();

	virtual void DeleteNote(const Guid & note);

	virtual void DeleteNoteTags(const Guid & note);

	virtual void DeleteNoteThumbnail(const Guid & note);

	virtual void EndTransaction();

	virtual bool Exists(const std::wstring & username);

	virtual void ExpungeNote(const Guid & note);

	virtual void ExpungeNotebook(const Guid & notebook);

	virtual void ExpungeTag(const Guid & tag);

	virtual std::wstring GetPasswordHash();

	virtual std::wstring GetUsername();

	virtual void GetDefaultNotebook(Notebook & notebook);

	virtual void GetDeletedNotes(GuidList & notes);

	virtual int GetDirtyNoteCount(const Notebook & notebook);

	virtual __int64 GetLastSyncEnTime();

	virtual void GetLastUsedNotebook(Notebook & notebook);

	virtual DbLocation GetLocation();

	virtual void GetNote(const Guid & guid, Note & note);

	virtual void GetNoteAttachments
		( const Guid     & note
		, AttachmentList & attachments
		);

	virtual void GetNoteBody
		( const Guid   & guid
		, std::wstring & body
		);

	virtual void GetNoteResources
		( const Guid & note
		, GuidList   & resources
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
		( const Guid & notebook
		, NoteList   & notes
		);

	virtual void GetNotesByNotebook
		( const Guid & notebook
		, int          start
		, int          count
		, NoteList   & notes
		);

	virtual void GetNotesBySearch
		( const Guid         & notebook
		, const std::wstring & search
		, int                  start
		, int                  count
		, NoteList           & notes
		);

	virtual std::wstring GetPath();

	virtual void GetResource
		( const std::string & hash
		, Blob              & blob
		);

	virtual void GetResource
		( const Guid & guid
		, Resource   & resource
		);

	virtual void GetResources(GuidList & resources);

	virtual __int64 GetSize();

	virtual int GetSyncVersion();

	virtual void GetTags(TagList & tags);

	virtual int GetUpdateCount();

	virtual int GetVersion();

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

	virtual void ReplaceNote
		( const Note          & note
		, const std::wstring  & body
		, const std::wstring  & bodyText
		, const Notebook      & notebook
		);

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

	virtual void SetSyncVersion(int version);

	virtual void SetUpdateCount(int updateCount);

	virtual void Unload();

	virtual void UpdateNote
		( const Guid & note
		, const Note & replacement
		);

	virtual void UpdateNotebook
		( const Guid     & notebook
		, const Notebook & replacement
		);

	virtual void UpdateResource
		( const Guid     & resource
		, const Resource & replacement
		);

	virtual void UpdateTag
		( const Guid & tag
		, const Tag  & replacement
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

	void MigrateFrom5To6();

	void MigrateFrom6To7();

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
	if (!statement->Execute())
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
