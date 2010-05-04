#pragma once
#include "INoteStore.h"

#include "Guid.h"
#include "Resource.h"

#include <map>

class MockNoteStore : public INoteStore
{
public:

	typedef std::map<std::string, std::wstring> NoteBodyMap;

public:

	EnInteropNoteList remoteNotes;
	NotebookList      remoteNotebooks;
	TagList           remoteTags;

	NoteBodyMap noteBodies;

	std::vector<Resource> resources;

public:

	virtual void GetNoteBody
		( const Note   & note
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
