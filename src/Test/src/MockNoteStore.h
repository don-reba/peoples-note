#pragma once
#include "INoteStore.h"

#include "Guid.h"
#include "Note.h"
#include "Notebook.h"
#include "Resource.h"
#include "Tag.h"

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

	NoteList     createdNotes;
	NotebookList createdNotebooks;
	ResourceList createdResources;
	TagList      createdTags;

	ResourceList resources;

	Note     replacementNote;
	Notebook replacementNotebook;
	Tag      replacementTag;

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
		, const std::wstring          & body
		, const std::vector<Resource> & resources
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
};
