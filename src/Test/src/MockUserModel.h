#pragma once

#include "IUserModel.h"
#include "MockCredentialsModel.h"
#include "Note.h"
#include "Notebook.h"

#include <map>
#include <set>

class MockUserModel : public IUserModel
{
public:

	struct NoteRecord
	{
		NoteRecord
			( const Note         & note
			, const std::wstring & body
			, const std::wstring & bodyText
			, const Notebook     & notebook
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
		Notebook     notebook;
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

	Credentials credentials;

	Notebook defaultNotebook;
	Notebook lastUsedNotebook;

	NotebookList notebooks;
	NoteList     notes;
	TagList      tags;
	ResourceList resources;
	

	std::map<std::string, std::wstring> noteBodies;
	std::map<std::string, Thumbnail>    noteThumbnails;

	std::vector<NoteRecord> addedNotes;

	NoteList     deletedNotes;
	NotebookList deletedNotebooks;
	TagList      deletedTags;

	std::set<std::wstring> validUsernames;

	std::wstring loadedAs;
	int          loadCount;
	LoadMethod   loadMethod;

	bool isInTransaction;

	mutable std::wstring notebookSelection;
	mutable std::wstring searchSelection;

	std::wstring folder;
	
	signal SignalLoaded;

// interface

public:

	MockUserModel();

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

	virtual int GetDirtyNoteCount(const Notebook & notebook);

	virtual std::wstring GetFolder() const;

	virtual void GetLastUsedNotebook(Notebook & notebook);

	virtual Note GetNote(Guid guid);

	virtual void GetNoteBody(Guid guid, std::wstring & body);

	virtual void GetNotebook
		( const Guid & guid
		, Notebook   & notebook
		);

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

	virtual void LoadOrCreate(const std::wstring & name);

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
};
