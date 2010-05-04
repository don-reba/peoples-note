#pragma once

#include "Blob.h"
#include "Credentials.h"
#include "ISignalProvider.h"
#include "Guid.h"
#include "Note.h"
#include "Notebook.h"
#include "Resource.h"
#include "Thumbnail.h"

class ICredentialsModel;

class IUserModel : public ISignalProvider
{
public:

	virtual ~IUserModel() {}

	virtual void AddImageResource
		( std::string  hash
		, const Blob & data
		, const Guid & noteGuid
		) = 0;

	virtual void AddNote
		( const Note          & note
		, const std::wstring  & body
		, const std::wstring  & bodyText
		, const Notebook      & notebook
		) = 0;

	virtual void AddNotebook(const Notebook & notebook) = 0;

	virtual void BeginTransaction() = 0;

	virtual void ConnectLoaded(slot_type OnLoaded) = 0;

	virtual void DeleteNote(const Note & note) = 0;

	virtual void EndTransaction() = 0;

	virtual bool Exists(const std::wstring & username) = 0;

	virtual Credentials GetCredentials() = 0;

	virtual Notebook GetDefaultNotebook() = 0;

	virtual std::wstring GetFolder() const = 0;

	virtual void GetImageResource(std::string hash, Blob & blob) = 0;

	virtual Notebook GetLastUsedNotebook() = 0;

	virtual Note GetNote(Guid guid) = 0;

	virtual void GetNoteBody(Guid guid, std::wstring & body) = 0;

	virtual void GetNoteThumbnail
		( const Guid & guid
		, Thumbnail  & thumbnail
		) = 0;

	virtual const NotebookList & GetNotebooks() = 0;

	virtual const NoteList & GetNotesByNotebook(const Notebook & notebook) = 0;

	virtual const NoteList & GetNotesBySearch(std::wstring search) = 0;

	virtual void GetResource(const Guid & guid, Resource & resource) = 0;

	virtual const TagList & GetTags() = 0;

	virtual void Load(const std::wstring & username) = 0;

	virtual void LoadAs
		( const std::wstring & oldUsername
		, const std::wstring & newUsername
		) = 0;

	virtual void LoadOrCreate(const std::wstring & username) = 0;

	virtual void MakeNotebookDefault(const Notebook & notebook) = 0;

	virtual void MakeNotebookLastUsed(const Notebook & notebook) = 0;

	virtual void SetCredentials
		( const std::wstring & username
		, const std::wstring & password
		) = 0;
	
	virtual void SetNoteThumbnail
		( const Guid      & guid
		, const Thumbnail & thumbnail
		) = 0;

	virtual void Unload() = 0;
};
