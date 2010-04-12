#pragma once

#include "IUserModel.h"
#include "MockCredentialsModel.h"
#include "Note.h"
#include "Notebook.h"

#include <map>

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

// data

public:

	MockCredentialsModel credentialsModel;

	Notebook defaultNotebook;
	Notebook lastUsedNotebook;

	NotebookList notebooks;
	NoteList     notes;
	ImageList    images;
	

	std::map<std::string, std::wstring> noteBodies;
	std::map<std::string, Thumbnail>    noteThumbnails;

	std::vector<ImageRecord> addedImages;
	std::vector<NoteRecord>  addedNotes;

	bool isDefault;
	bool isLoaded;

	mutable std::wstring notebookSelection;
	mutable std::wstring searchSelection;
	
	signal SignalLoaded;

// interface

public:

	MockUserModel();

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

	virtual void ConnectLoaded(slot_type OnLoaded);

	virtual ICredentialsModel & GetCredentials();

	virtual void CreateDefaultUser();

	virtual Notebook GetDefaultNotebook();

	virtual Notebook GetLastUsedNotebook();

	virtual void GetImageResource(std::string hash, Blob & blob);

	virtual Note GetNote(Guid guid);

	virtual void GetNoteBody(Guid guid, std::wstring & body);

	virtual void GetNoteThumbnail(const Guid & guid, Thumbnail & thumbnail);

	virtual const NotebookList & GetNotebooks();

	virtual const NoteList & GetNotesByNotebook(const Notebook & notebook);

	virtual const NoteList & GetNotesBySearch(std::wstring search);

	virtual void Load();

	virtual void MakeNotebookDefault(const Notebook & notebook);

	virtual void MakeNotebookLastUsed(const Notebook & notebook);
	
	virtual void SetCredentials(const ICredentialsModel & credentials);

	virtual void SetNoteThumbnail(const Guid & guid, const Thumbnail & thumbnail);
};
