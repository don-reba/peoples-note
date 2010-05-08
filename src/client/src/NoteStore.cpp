#include "stdafx.h"
#include "NoteStore.h"
#include "Tools.h"

#include <algorithm>

using namespace std;
using namespace Tools;

using namespace Thrift;
using namespace Thrift::Protocol;
using namespace Thrift::Transport;

using namespace Evernote;

const wchar_t * const NoteStore::baseUrl
	= L"http://sandbox.evernote.com/edam/note/";

NoteStore::NoteStore
	( const AuthenticationToken & token
	, const wstring             & shardId
	)
	: token     (token)
	, transport ((baseUrl + shardId).c_str())
	, protocol  (transport)
	, noteStore (protocol)
{
	transport.Open();
}

void NoteStore::GetNoteBody
	( const Note & note
	, wstring    & content
	)
{
	content = noteStore.getNoteContent
		( token
		, ConvertToUnicode(note.guid)
		);
}

void NoteStore::GetNoteResource
	( const Guid & guid
	, Resource   & resource
	)
{
}

void NoteStore::ListEntries
	( EnInteropNoteList & notes
	, NotebookList      & notebooks
	, TagList           & tags
	)
{
}

void NoteStore::CreateNote
	( const Note             & note
	, const wstring          & body
	, const vector<Resource> & resources
	, Note                   & replacement
	)
{
	EDAM::Types::Note enNote;
	enNote.__isset.title     = true;
	enNote.__isset.content   = true;
	enNote.__isset.resources = true;

	enNote.title   = note.name;
	enNote.content = body;

	enNote.resources.resize(resources.size());
	for (int i(0); i != resources.size(); ++i)
	{
		EDAM::Types::Resource & resource(enNote.resources.at(i));
		resource.__isset.data = true;
		resource.data.__isset.body = true;
		resource.data.__isset.size = true;

		copy
			( resources.at(i).Data.begin()
			, resources.at(i).Data.end()
			, back_inserter(resource.data.body)
			);
		resource.data.size = resources.at(i).Data.size();
	}

	EDAM::Types::Note enReplacement(noteStore.createNote(token, enNote));
	replacement.guid         = Guid(enReplacement.guid);
	replacement.name         = enReplacement.title;
	replacement.creationDate = static_cast<time_t>(enReplacement.created);
	replacement.usn          = enReplacement.updateSequenceNum;
	replacement.isDirty      = false;
}

void NoteStore::CreateNotebook
	( const Notebook & notebook
	)
{
}

void NoteStore::CreateTag
	( const Tag & tag
	)
{
}
