#include "stdafx.h"
#include "MockLogger.h"

using namespace std;

void MockLogger::Clear()
{
}

void MockLogger::Flush()
{
}

void MockLogger::ListNotes(const wstring & listTitle, const EnInteropNoteList & notes)
{
}

void MockLogger::ListNotebooks(const wstring & listTitle, const NotebookList & notebooks)
{
}

void MockLogger::ListTags(const wstring & listTitle, const TagList & tags)
{
}

void MockLogger::ListGuids(const wstring & listTitle, const vector<Guid> & guids)
{
}

void MockLogger::BeginSyncStage(const wstring & name)
{
}

void MockLogger::PerformAction
	( const wchar_t * action
	, const Guid    * local
	, const Guid    * remote
	)
{
}

void MockLogger::SyncError(const wstring & message)
{
}
