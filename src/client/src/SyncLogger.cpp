#include "stdafx.h"
#include "SyncLogger.h"

#include "Tools.h"

using namespace std;
using namespace Tools;

SyncLogger::SyncLogger(const wstring & documentPath)
	: stream((documentPath + L"\\sync log.txt").c_str())
{
}

//---------------------------
// ISyncLogger implementation
//---------------------------

void SyncLogger::ListNotes(const wstring & listTitle, const EnInteropNoteList & notes)
{
	if (notes.empty())
		return;
	WriteListHeader(listTitle);
	foreach (const EnInteropNote & note, notes)
		WriteListEntry(note.name, note.guid);
}

void SyncLogger::ListNotebooks(const wstring & listTitle, const NotebookList & notebooks)
{
	if (notebooks.empty())
		return;
	WriteListHeader(listTitle);
	foreach (const Notebook & notebook, notebooks)
		WriteListEntry(notebook.name, notebook.guid);
}

void SyncLogger::ListTags(const wstring & listTitle, const TagList & tags)
{
	if (tags.empty())
		return;
	WriteListHeader(listTitle);
	foreach (const Tag & tag, tags)
		WriteListEntry(tag.name, tag.guid);
}

void SyncLogger::ListGuids(const wstring & listTitle, const vector<Guid> & guids)
{
	if (guids.empty())
		return;
	WriteListHeader(listTitle);
	foreach (const Guid & guid, guids)
		WriteListEntry(L"", guid);
}

void SyncLogger::BeginSyncStage(const wstring & name)
{
	stream << L"Sync " << name << L":\n";
}

void SyncLogger::Add(const Guid & remote)
{
	WriteOperation(L"Add", remote);
}

void SyncLogger::Delete(const Guid & local)
{
	WriteOperation(L"Delete", local);
}

void SyncLogger::Rename(const Guid & local)
{
	WriteOperation(L"Rename", local);
}

void SyncLogger::Upload(const Guid & local)
{
	WriteOperation(L"Upload", local);
}

void SyncLogger::Merge(const Guid & local, const Guid & remote)
{
	WriteOperation(L"Merge", local, remote);
}

void SyncLogger::Error(const std::wstring & message)
{
	stream << L"Error: " << message << L"\n";
}

//------------------
// utility functions
//------------------

void SyncLogger::WriteListHeader(const wstring & text)
{
	stream << text << ":\n";
}

void SyncLogger::WriteListEntry(const wstring & name, const Guid & guid)
{
	stream << ConvertToUnicode(guid) << L" " << name << L"\n";
}

void SyncLogger::WriteOperation(const wstring & name, const Guid & guid)
{
	stream << name << L"( " << ConvertToUnicode(guid) << L" )\n";
}

void SyncLogger::WriteOperation
	( const wstring & name
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
