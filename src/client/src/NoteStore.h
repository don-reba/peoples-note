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
		( const Guid           & guid
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

// utility functions

private:

	Timestamp ConvertFromEnTime(__int64 enTime);

	__int64 ConvertToEnTime(Timestamp time);

	void ConvertFromEnNote
		( const Evernote::EDAM::Type::Note & enNote
		, Note                             & note
		);

	void ConvertToEnNote
		( const Note                 & note
		, const std::wstring         & body
		, const Guid                 & notebook
		, Evernote::EDAM::Type::Note & enNote
		);

	void ConvertFromEnNotebook
		( const Evernote::EDAM::Type::Notebook & enNotebook
		, Notebook                             & notebook
		);

	void ConvertToEnNotebook
		( const Notebook                 & notebook
		, Evernote::EDAM::Type::Notebook & enNotebook
		);

	void NoteStore::ConvertFromEnResource
		( const Evernote::EDAM::Type::Resource & enResource
		, Resource                             & resource
		);

	void ConvertFromEnResource
		( const Evernote::EDAM::Type::Resource & enResource
		, const Guid                           & guid
		, Resource                             & resource
		, RecognitionEntryList                 & recognitionEntries
		);

	void ConvertToEnResource
		( const Resource                 & resource
		, Evernote::EDAM::Type::Resource & enResource
		);

	void ConvertFromEnTag
		( const Evernote::EDAM::Type::Tag & enTag
		, Tag                             & tag
		);

	void ConvertToEnTag
		( const Tag                 & tag
		, Evernote::EDAM::Type::Tag & enTag
		);

	void ListEntries
		( Evernote::EDAM::NoteStore::SyncChunk & chunk
		, int                                    globalUpdateCount
		, EnInteropNoteList                    & notes
		, NotebookList                         & notebooks
		, TagList                              & tags
		, const Guid                           & notebookFilter
		);

	void NoteStore::ListEntries
		( Evernote::EDAM::NoteStore::SyncChunk & chunk
		, std::vector<Guid>                    & expungedNotes
		, std::vector<Guid>                    & expungedNotebooks
		, std::vector<Guid>                    & expungedTags
		, std::vector<Guid>                    & resources
		, const Guid                           & notebookFilter
		);
};
