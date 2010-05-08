#pragma once

#include "EnInteropNote.h"
#include "Notebook.h"
#include "Resource.h"
#include "Tag.h"

class INoteStore
{
public:

	virtual void GetNoteBody
		( const Note   & note
		, std::wstring & content
		) = 0;

	virtual void GetNoteResource
		( const Guid & guid
		, Resource   & resource
		) = 0;

	virtual void ListEntries
		( EnInteropNoteList & notes
		, NotebookList      & notebooks
		, TagList           & tags
		) = 0;

	virtual void CreateNote
		( const Note                  & note
		, const std::wstring          & body
		, const std::vector<Resource> & resources
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
};
