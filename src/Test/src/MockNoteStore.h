#pragma once
#include "INoteStore.h"

class MockNoteStore : public INoteStore
{
public:

	EnInteropNoteList remoteNotes;
	NotebookList      remoteNotebooks;
	TagList           remoteTags;

public:

	virtual void GetNoteBody
		( const Note   & Note
		, std::wstring & content
		);

	virtual void GetNoteResource
		( const Guid & guid
		, Resource   & resource
		);

	virtual void ListEntries
		( EnInteropNoteList & notes
		, NotebookList      & notebooks
		, TagList           & tags
		);

	virtual void CreateNote
		( const Note                  & note
		, const std::vector<Resource> & resources
		);

	virtual void CreateNotebook
		( const Notebook & notebook
		);

	virtual void CreateTag
		( const Tag & tag
		);
};
