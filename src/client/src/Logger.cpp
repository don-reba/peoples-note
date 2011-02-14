#include "stdafx.h"
#include "Logger.h"

#include "Tools.h"

using namespace std;
using namespace Tools;

Logger::Logger(const wstring & documentPath)
	: path(documentPath + L"\\sync log.txt")
{
	stream.open(path.c_str(), wofstream::out | wofstream::app);
}

//---------------------------
// ILogger implementation
//---------------------------

void Logger::Clear()
{
	stream.close();
	stream.open(path.c_str(), wofstream::out | wofstream::trunc);
}

void Logger::Flush()
{
	stream.flush();
}

void Logger::ListNotes(const wstring & listTitle, const EnInteropNoteList & notes)
{
	if (notes.empty())
		return;
	WriteListHeader(listTitle);
	foreach (const EnInteropNote & note, notes)
		WriteListEntry(note.name, note.guid);
}

void Logger::ListNotebooks(const wstring & listTitle, const NotebookList & notebooks)
{
	if (notebooks.empty())
		return;
	WriteListHeader(listTitle);
	foreach (const Notebook & notebook, notebooks)
		WriteListEntry(notebook.name, notebook.guid);
}

void Logger::ListTags(const wstring & listTitle, const TagList & tags)
{
	if (tags.empty())
		return;
	WriteListHeader(listTitle);
	foreach (const Tag & tag, tags)
		WriteListEntry(tag.name, tag.guid);
}

void Logger::ListGuids(const wstring & listTitle, const vector<Guid> & guids)
{
	if (guids.empty())
		return;
	WriteListHeader(listTitle);
	foreach (const Guid & guid, guids)
		WriteListEntry(L"", guid);
}

void Logger::BeginSyncStage(const wstring & name)
{
	stream << L"Sync " << name << L":\n";
}

void Logger::PerformAction
	( const wchar_t * action
	, const Guid    * local
	, const Guid    * remote
	)
{
	if (local)
	{
		if (remote)
			WriteOperation(action, *local, *remote);
		else
			WriteOperation(action, *local);
	}
	else
	{
		if (remote)
			WriteOperation(action, *remote);
		else
			WriteOperation(action, Guid("NA"));
	}
}

void Logger::SyncError(const std::wstring & message)
{
	stream << L"Error: " << message << L"\n";
}

//------------------
// utility functions
//------------------

void Logger::WriteListHeader(const wstring & text)
{
	stream << text << ":\n";
}

void Logger::WriteListEntry(const wstring & name, const Guid & guid)
{
	stream << ConvertToUnicode(guid) << L"\n";
}

void Logger::WriteOperation(const wchar_t * name, const Guid & guid)
{
	stream << name << L"( " << ConvertToUnicode(guid) << L" )\n";
}

void Logger::WriteOperation
	( const wchar_t * name
	, const Guid    & guid1
	, const Guid    & guid2
	)
{
	stream
		<< name << L"( "
		<< ConvertToUnicode(guid1)
		<< L", "
		<< ConvertToUnicode(guid2)
		<< L" )\n";
}
