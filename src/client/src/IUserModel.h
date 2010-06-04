#pragma once

#include "Blob.h"
#include "Credentials.h"
#include "ISignalProvider.h"
#include "Guid.h"
#include "Note.h"
#include "Notebook.h"
#include "Resource.h"
#include "Tag.h"
#include "Thumbnail.h"

class ICredentialsModel;

class IUserModel : public ISignalProvider
{
public:

	virtual ~IUserModel() {}

	virtual void AddNote
		( const Note          & note
		, const std::wstring  & body
		, const std::wstring  & bodyText
		, const Notebook      & notebook
		) = 0;

	virtual void AddNotebook(const Notebook & notebook) = 0;

	virtual void AddResource(const Resource & resource) = 0;

	virtual void AddTag(const Tag & tag) = 0;

	virtual void AddTagToNote
		( const std::wstring & tagName
		, const Note         & note
		) = 0;

	virtual void BeginTransaction() = 0;

	virtual void ConnectLoaded(slot_type OnLoaded) = 0;

	virtual void DeleteNote(const Note & note) = 0;

	virtual void DeleteNotebook(const Notebook & notebook) = 0;

	virtual void DeleteTag(const Tag & tag) = 0;

	virtual void EndTransaction() = 0;

	virtual bool Exists(const std::wstring & username) = 0;

	virtual void GetCredentials(Credentials & credentials) = 0;

	virtual void GetDefaultNotebook(Notebook & notebook) = 0;

	virtual int GetDirtyNoteCount(const Notebook & notebook) = 0;

	virtual std::wstring GetFolder() const = 0;

	virtual void GetLastUsedNotebook(Notebook & notebook) = 0;

	// TODO: changed Note to a reference parameter
	virtual Note GetNote(Guid guid) = 0;

	virtual void GetNotebook
		( const Guid & guid
		, Notebook   & notebook
		) = 0;

	virtual void GetNoteBody
		( const Guid   & guid
		, std::wstring & body
		) = 0;

	virtual void GetNoteTags
		( const Note & note
		, TagList    & tags
		) = 0;

	virtual void GetNoteThumbnail
		( const Guid & guid
		, Thumbnail  & thumbnail
		) = 0;

	virtual void GetNotebooks(NotebookList & notebooks) = 0;

	virtual void GetNotesByNotebook
		( const Notebook & notebook
		, NoteList       & notes
		) = 0;

	virtual void GetNotesBySearch
		( const std::wstring & search
		, NoteList           & notes
		) = 0;

	virtual void GetResource
		( const std::string & hash
		, Blob              & blob
		) = 0;

	virtual void GetResource
		( const Guid & guid
		, Resource   & resource
		) = 0;

	virtual void GetTags(TagList & tags) = 0;

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

	virtual void UpdateNotebook
		( const Notebook & notebook
		, const Notebook & replacement
		) = 0;

	virtual void UpdateTag
		( const Tag & tag
		, const Tag & replacement
		) = 0;
};
