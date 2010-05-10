#include "stdafx.h"
#include "MockSyncLogger.h"

using namespace std;

void MockSyncLogger::ListLocalNotes(const EnInteropNoteList & notes)
{
}

void MockSyncLogger::ListRemoteNotes(const EnInteropNoteList & notes)
{
}

void MockSyncLogger::ListLocalNotebooks(const NotebookList & notebooks)
{
}

void MockSyncLogger::ListRemoteNotebooks(const NotebookList & notebooks)
{
}

void MockSyncLogger::ListLocalTags(const TagList & tags)
{
}

void MockSyncLogger::ListRemoteTags(const TagList & tags)
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
