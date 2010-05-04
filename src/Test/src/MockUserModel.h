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

	struct ImageRecord
	{
		ImageRecord
			( std::string  hash
			, const Blob & data
			, Guid         note
			)
			: hash (hash)
			, data (data)
			, note (note)
		{
		}
		std::string hash;
		Blob        data;
		Guid        note;
	};

	typedef std::map<std::string, Blob> ImageList;

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
	ImageList    images;
	TagList      tags;
	

	std::map<std::string, std::wstring> noteBodies;
	std::map<std::string, Thumbnail>    noteThumbnails;

	std::vector<ImageRecord> addedImages;
	std::vector<NoteRecord>  addedNotes;
	std::vector<Note>        deletedNotes;

	std::set<std::wstring> validUsernames;

	std::wstring loadedAs;
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

	virtual void AddImageResource
		( std::string  hash
		, const Blob & data
		, const Guid & noteGuid
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

	virtual void DeleteNote(const Note & note);

	virtual void EndTransaction();

	virtual bool Exists(const std::wstring & username);

	virtual Credentials GetCredentials();

	virtual Notebook GetDefaultNotebook();

	virtual std::wstring GetFolder() const;

	virtual void GetImageResource(std::string hash, Blob & blob);

	virtual Notebook GetLastUsedNotebook();

	virtual Note GetNote(Guid guid);

	virtual void GetNoteBody(Guid guid, std::wstring & body);

	virtual void GetNoteResources
		( const Note            & note
		, std::vector<Resource> & resources
		);

	virtual void GetNoteThumbnail
		( const Guid & guid
		, Thumbnail  & thumbnail
		);

	virtual const NotebookList & GetNotebooks();

	virtual const NoteList & GetNotesByNotebook(const Notebook & notebook);

	virtual const NoteList & GetNotesBySearch(std::wstring search);

	virtual const TagList & GetTags();

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
