#include "stdafx.h"
#include "Logger.h"

#include "EnServiceTools.h"
#include "Tools.h"

using namespace std;
using namespace Tools;

Logger::Logger(const wstring & documentPath)
	: path(documentPath + L"\\log.txt")
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

ExceptionMessage Logger::GetExceptionMessage()
try
{
	// this function is meant to be called from a catch block
	// rethrow the exception to catch it again
	throw;
}
catch (const Evernote::EDAM::Error::EDAMNotFoundException & e)
{
	return ExceptionMessage
		( L"Tried to sync, but something went wrong."
		, EnServiceTools::CreateNotFoundExceptionMessage(e).c_str()
		);
}
catch (const Evernote::EDAM::Error::EDAMSystemException & e)
{
	return ExceptionMessage
		( L"Tried to sync, but something went wrong."
		, EnServiceTools::CreateSystemExceptionMessage(e).c_str()
		);
}
catch (const Evernote::EDAM::Error::EDAMUserException & e)
{
	return ExceptionMessage
		( L"Tried to sync, but something went wrong."
		, EnServiceTools::CreateUserExceptionMessage(e).c_str()
		);
}
catch (const Thrift::Transport::TTransportException & e)
{
	return ExceptionMessage
		( L"Encountered a network error."
		, EnServiceTools::CreateTransportExceptionMessage(e).c_str()
		);
}
catch (const Thrift::TException & e)
{
	return ExceptionMessage
		( L"Tried to sync, but something went wrong."
		, EnServiceTools::CreateExceptionMessage(e).c_str()
		);
}
catch (const std::exception & e)
{
	wstring message;
	message.append(L"exception(");
	message.append(ConvertToUnicode(e.what()));
	message.append(L")");
	return ExceptionMessage
		( L"Tried to sync, but something went wrong."
		, message.c_str()
		);
}
catch (...)
{
	return ExceptionMessage
		( L"Tried to sync, but something went wrong."
		, L"Unknown exception."
		);
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

void Logger::AuthorizationError
	( const std::wstring & username
	, const std::wstring & message
	)
{
	stream << L"Auth error: '" << username << "' - " << message << endl;
}

void Logger::SyncError(const std::wstring & message)
{
	stream << L"Sync error: " << message << endl;
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
