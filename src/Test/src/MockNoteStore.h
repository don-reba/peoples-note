#pragma once
#include "INoteStore.h"

#include "Guid.h"
#include "Note.h"
#include "Notebook.h"
#include "Resource.h"
#include "RecognitionEntry.h"
#include "SyncState.h"
#include "Tag.h"

#include <map>

class MockNoteStore : public INoteStore
{
public:

	typedef std::map<std::string, std::wstring>                NoteBodyMap;
	typedef std::vector<std::pair<std::string, std::wstring> > NoteTagMap;

public:

	EnInteropNoteList    remoteNotes;
	NotebookList         remoteNotebooks;
	RecognitionEntryList recognitionEntries;
	TagList              remoteTags;

	NoteBodyMap noteBodies;
	NoteTagMap  noteTags;

	NoteList     createdNotes;
	NotebookList createdNotebooks;
	ResourceList createdResources;
	TagList      createdTags;

	GuidList deletedNotes;

	GuidList expungedNotes;
	GuidList expungedNotebooks;
	GuidList expungedTags;

	ResourceList resources;

	Note     replacementNote;
	Notebook replacementNotebook;
	Tag      replacementTag;

	Guid defaultNotebook;

	Guid notebookFilter;

	SyncState syncState;

public:

	virtual void CreateNote
		( const Note                  & note
		, const std::wstring          & body
		, const std::vector<Resource> & resources
		, const Guid                  & notebook
		, Note                        & replacement
		);

	virtual void CreateNotebook
		( const Notebook & notebook
		, Notebook       & replacement
		);

	virtual void CreateTag
		( const Tag & tag
		, Tag       & replacement
		);

	virtual void DeleteNote(const Guid & guid);

	virtual void GetDefaultNotebook(Guid & notebook);

	virtual void GetNote(const Guid & guid, Note & note);

	virtual void GetNoteBody
		( const Note   & note
		, std::wstring & content
		);

	virtual void GetNoteResource
		( const Guid           & note
		, Resource             & resource
		, RecognitionEntryList & recognitionEntries
		);

	virtual void GetNoteTagNames
		( const Note                & note
		, std::vector<std::wstring> & names
		);

	virtual void GetNotebook(const Guid & guid, Notebook & notebook);

	virtual void GetResource(const Guid & guid, Resource & resource);

	virtual void GetSyncState(SyncState & syncState);

	virtual void GetTag(const Guid & guid, Tag & tag);

	virtual void ListEntries
		( EnInteropNoteList & notes
		, NotebookList      & notebooks
		, TagList           & tags
		, const Guid        & notebookFilter
		);

	virtual void ListEntries
		( int                 globalUpdateCount
		, int                 notebookUpdateCount
		, EnInteropNoteList & notes
		, NotebookList      & notebooks
		, TagList           & tags
		, std::vector<Guid> & expungedNotes
		, std::vector<Guid> & expungedNotebooks
		, std::vector<Guid> & expungedTags
		, std::vector<Guid> & resources
		, const Guid        & notebookFilter
		);

	virtual void UpdateNote
		( const Note                  & note
		, const std::wstring          & body
		, const std::vector<Resource> & resources
		, const Guid                  & notebook
		, Note                        & replacement
		);

	virtual void UpdateNotebook
		( const Notebook & notebook
		, Notebook       & replacement
		);

	virtual void UpdateTag
		( const Tag & tag
		, Tag       & replacement
		);
};
