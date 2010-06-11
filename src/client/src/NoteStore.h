#pragma once

#include "INoteStore.h"

#include "AuthenticationToken.h"

#include "Thrift/Thrift.h"
#include "Evernote/EDAM/NoteStore.h"

class NoteStore : public INoteStore
{
private:

	static const wchar_t * const baseUrl;

	AuthenticationToken token;

	Thrift::Protocol::TBinaryProtocol protocol;
	Thrift::Transport::THttpTransport transport;

	Evernote::EDAM::NoteStore::NoteStore::Client noteStore;

// interface

public:

	NoteStore
		( const AuthenticationToken & token
		, const std::wstring        & shardId
		);

// INoteStore implementation

public:

	virtual void GetNoteBody
		( const Note   & note
		, std::wstring & content
		);

	virtual void GetNoteResource
		( const Guid & guid
		, Resource   & resource
		);

	virtual void GetNoteTagNames
		( const Note                & note
		, std::vector<std::wstring> & names
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

// utility functions

private:

	void ListEntries
		( Evernote::EDAM::NoteStore::SyncChunk & chunk
		, EnInteropNoteList                    & notes
		, NotebookList                         & notebooks
		, TagList                              & tags
		);
};
