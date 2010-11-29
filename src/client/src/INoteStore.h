#pragma once

#include "EnInteropNote.h"
#include "Notebook.h"
#include "RecognitionEntry.h"
#include "Resource.h"
#include "SyncState.h"
#include "Tag.h"

class Guid;

class INoteStore
{
public:

	virtual void CreateNote
		( const Note                  & note
		, const std::wstring          & body
		, const std::vector<Resource> & resources
		, const Guid                  & notebook
		, Note                        & replacement
		) = 0;

	virtual void CreateNotebook
		( const Notebook & notebook
		, Notebook       & replacement
		) = 0;

	virtual void CreateTag
		( const Tag & tag
		, Tag       & replacement
		) = 0;

	virtual void GetDefaultNotebook(Guid & notebook) = 0;

	virtual void GetNoteBody
		( const Note   & note
		, std::wstring & content
		) = 0;

	virtual void GetNoteResource
		( const Guid           & guid
		, Resource             & resource
		, RecognitionEntryList & recognitionEntries
		) = 0;

	virtual void GetNoteTagNames
		( const Note                & note
		, std::vector<std::wstring> & names
		) = 0;

	virtual void GetSyncState(SyncState & syncState) = 0;

	virtual void ListEntries
		( EnInteropNoteList & notes
		, NotebookList      & notebooks
		, TagList           & tags
		, const Guid        & notebookFilter
		) = 0;

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
		) = 0;

	virtual void UpdateNote
		( const Note                  & note
		, const std::wstring          & body
		, const std::vector<Resource> & resources
		, const Guid                  & guid
		, Note                        & replacement
		) = 0;

	virtual void UpdateNotebook
		( const Notebook & notebook
		, Notebook       & replacement
		) = 0;

	virtual void UpdateTag
		( const Tag & tag
		, Tag       & replacement
		) = 0;
};
