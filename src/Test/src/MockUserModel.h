#pragma once
#include "IUserModel.h"

#include "ICredentialsModel.h"
#include "Note.h"
#include "Notebook.h"

#include <map>
#include <set>

class MockUserModel : public IUserModel
{
	MacroTestEvent(Loaded)

public:

	struct NoteRecord
	{
		NoteRecord
			( const Note         & note
			, const std::wstring & body
			, const std::wstring & bodyText
			, const Guid         & notebook
			)
			: note     (note)
			, body     (body)
			, bodyText (bodyText)
			, notebook (notebook)
		{
		}
		Note         note;
		std::wstring body;
		std::wstring bodyText;
		Guid         notebook;
	};

	enum LoadMethod
	{
		LoadMethodNone,
		LoadMethodLoad,
		LoadMethodLoadAs,
		LoadMethodLoadOrCreate,
	};

// data

public:

	std::wstring password;
	std::wstring username;

	Guid defaultNotebook;
	Guid lastUsedNotebook;
	Guid notebookSelection;

	NotebookList         notebooks;
	NoteList             notes;
	RecognitionEntryList recognitionEntries;
	ResourceList         resources;
	TagList              tags;

	std::map<std::string, std::wstring> noteBodies;
	std::map<std::string, Thumbnail>    noteThumbnails;
	std::multimap<Guid, Guid>           noteTags;

	std::vector<NoteRecord> addedNotes;

	GuidList deletedNotes;
	GuidList expungedNotes;
	GuidList expungedNotebooks;
	GuidList expungedTags;

	std::set<std::wstring> validUsernames;

	std::wstring loadedAs;
	int          loadCount;
	LoadMethod   loadMethod;

	bool isCompacted;
	bool isInTransaction;

	std::wstring searchSelection;

	std::wstring folder;
	DbLocation   location;
	std::wstring path;

	__int64 lastSyncEnTime;
	__int64 size;
	int     updateCount;
	int     syncVersion;
	int     version;

	std::map<std::string, int> notebookUpdateCounts;
	
// interface

public:

	MockUserModel();

// IUserModel implementation

public:

	virtual void AddNote
		( const Note          & note
		, const std::wstring  & body
		, const std::wstring  & bodyText
		, const Guid          & notebook
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

	virtual void Compact();

	virtual void DeleteNote(const Guid & note);

	virtual void DeleteNoteTags(const Guid & note);

	virtual void DeleteNoteThumbnail(const Guid & note);

	virtual void EndTransaction();

	virtual bool Exists(const std::wstring & username);

	virtual void ExpungeNote(const Guid & note);

	virtual void ExpungeNotebook(const Guid & notebook);

	virtual void ExpungeTag(const Guid & tag);

	virtual void GetDefaultNotebook(Guid & notebook);

	virtual void GetDeletedNotes(GuidList & notes);

	virtual int GetDirtyNoteCount(const Guid & notebook);

	virtual std::wstring GetFolder();

	virtual __int64 GetLastSyncEnTime();

	virtual void GetLastUsedNotebook(Guid & notebook);

	virtual void GetLastUsedNotebookOrDefault(Guid & notebook);

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
		( const Guid        & note
		, std::vector<Guid> & resources
		);

	virtual void GetNoteTags
		( const Note & note
		, TagList    & tags
		);

	virtual void GetNotebook
		( const Guid & guid
		, Notebook   & notebook
		);

	virtual void GetNoteThumbnail
		( const Guid & guid
		, Thumbnail  & thumbnail
		);

	virtual void GetNotebooks(NotebookList & notebooks);

	virtual int GetNotebookUpdateCount(const Guid & notebook);

	virtual void GetNotes
		( Guid           notebook // leave empty, if not used
		, std::wstring   search   // leave empty, if not used
		, int            start    // set count to 0, if not used
		, int            count    // set to 0, if not used
		, NoteList     & notes
		);

	virtual std::wstring GetPath();

	virtual std::wstring GetPasswordHash();

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

	virtual std::wstring GetUsername();

	virtual void GetTags(TagList & tags);

	virtual int GetUpdateCount();

	virtual int GetVersion();

	virtual void Load(const std::wstring & username);

	virtual void LoadAs
		( const std::wstring & oldUsername
		, const std::wstring & newUsername
		);

	virtual void LoadOrCreate(const std::wstring & name);

	virtual void MakeNotebookDefault(const Guid & notebook);

	virtual void MakeNotebookLastUsed(const Guid & notebook);

	virtual void MoveToCard();

	virtual void MoveToDevice();

	virtual void ReplaceNote
		( const Note          & note
		, const std::wstring  & body
		, const std::wstring  & bodyText
		, const Guid          & notebook
		);

	virtual void SetCredentials
		( const std::wstring & username
		, const std::wstring & passwordHash
		);

	virtual void SetLastSyncEnTime(__int64 enTime);
	
	virtual void SetNoteThumbnail
		( const Guid      & guid
		, const Thumbnail & thumbnail
		);

	virtual void SetNotebookUpdateCount
		( const Guid & notebook
		, int          updateCount
		);

	virtual void SetUpdateCount(int updateCount);

	virtual void SetSyncVersion(int version);

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
};
