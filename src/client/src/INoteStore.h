#pragma once

#include "EnInteropNote.h"
#include "Notebook.h"
#include "Resource.h"
#include "Tag.h"

class INoteStore
{
public:

	virtual void GetNoteBody
		( const Note   & Note
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
		, const std::vector<Resource> & resources
		) = 0;

	virtual void CreateNotebook
		( const Notebook & notebook
		) = 0;

	virtual void CreateTag
		( const Tag & tag
		) = 0;
};
