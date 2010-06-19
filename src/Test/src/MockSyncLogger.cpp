#include "stdafx.h"
#include "MockSyncLogger.h"

using namespace std;

void MockSyncLogger::ListNotes(const wstring & listTitle, const EnInteropNoteList & notes)
{
}

void MockSyncLogger::ListNotebooks(const wstring & listTitle, const NotebookList & notebooks)
{
}

void MockSyncLogger::ListTags(const wstring & listTitle, const TagList & tags)
{
}

void MockSyncLogger::ListGuids(const wstring & listTitle, const vector<Guid> & guids)
{
}

void MockSyncLogger::BeginSyncStage(const wstring & name)
{
}

void MockSyncLogger::Add(const Guid & remote)
{
}

void MockSyncLogger::Delete(const Guid & local)
{
}

void MockSyncLogger::Rename(const Guid & local)
{
}

void MockSyncLogger::Upload(const Guid & local)
{
}

void MockSyncLogger::Merge(const Guid & local, const Guid & remote)
{
}

void MockSyncLogger::Error(const wstring & message)
{
}
