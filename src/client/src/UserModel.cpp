#include "stdafx.h"
#include "UserModel.h"

#include "IFlashCard.h"
#include "ISqlBlob.h"
#include "ISqlStatement.h"
#include "Notebook.h"
#include "Tools.h"
#include "Transaction.h"
#include "WinException.h"

#include "SQLite/sqlite3.h"

#include <algorithm>
#include <functional>
#include <iterator>
#include <sstream>

using namespace boost;
using namespace std;

const int dbVersion(7);

//----------
// interface
//----------

UserModel::UserModel
	( IDataStore       & dataStore
	, const wstring    & deviceFolder
	, const IFlashCard & flashCard
	)
	: dataStore    (dataStore)
	, deviceFolder (deviceFolder)
	, flashCard    (flashCard)
{
}

int UserModel::GetNoteCount()
{
	IDataStore::Statement statement = dataStore.MakeStatement
		( "SELECT COUNT(*)"
		"  FROM Notes"
		"  WHERE isDeleted = 0"
		);
	statement->Execute();
	int count(0);
	statement->Get(count);
	return count;
}

int UserModel::GetNotebookCount()
{
	IDataStore::Statement statement = dataStore.MakeStatement
		( "SELECT COUNT(*)"
		"  FROM Notebooks"
		);
	statement->Execute();
	int count(0);
	statement->Get(count);
	return count;
}

int UserModel::GetRecognitionEntryCount()
{
	IDataStore::Statement statement = dataStore.MakeStatement
		( "SELECT COUNT(*)"
		"  FROM Recognition"
		);
	statement->Execute();
	int count(0);
	statement->Get(count);
	return count;
}

int UserModel::GetResourceCount()
{
	IDataStore::Statement statement = dataStore.MakeStatement
		( "SELECT COUNT(*)"
		"  FROM Resources"
		);
	statement->Execute();
	int count(0);
	statement->Get(count);
	return count;
}

//-------------------------
// IUserModel implementaion
//-------------------------

void UserModel::AddNote
	( const Note     & note
	, const wstring  & body
	, const wstring  & bodyText
	, const Guid     & notebook
	)
{
	if (notebook.IsEmpty())
		throw std::exception("Invalid notebook.");

	Transaction transaction(*this);

	IDataStore::Statement existenceCheck = dataStore.MakeStatement
		( "SELECT rowid from Notes WHERE guid = ?"
		);
	existenceCheck->Bind(note.guid);
	if (existenceCheck->Execute())
	{
		ReplaceNote(note, body, bodyText, notebook);
	}
	else
	{
		__int64 rowid(0);
		existenceCheck->Get(rowid);
		{
			IDataStore::Statement updateText = dataStore.MakeStatement
				( "UPDATE NoteText"
				"  SET    title = ?, body = ?"
				"  WHERE  rowid = ?"
				);
			updateText->Bind(note.name);
			updateText->Bind(bodyText);
			updateText->Bind(rowid);
			updateText->Execute();
		}
		{
			IDataStore::Statement updateNote = dataStore.MakeStatement
				( "UPDATE Notes"
				"  SET usn = ?, title = ?, isDirty = ?, body = ?, creationDate = ?,"
				"      modificationDate = ?, subjectDate = ?, altitude = ?, latitude = ?,"
				"      longitude = ?, author = ?, source = ?, sourceUrl = ?, sourceApplication = ?,"
				"      isDeleted = 0, thumbnail = NULL, thumbnailWidth = 0, thumbnailHeight = 0,"
				"      notebook = ?"
				"  WHERE rowid = ?"
				);
			updateNote->Bind(note.usn);
			updateNote->Bind(note.name);
			updateNote->Bind(note.isDirty);
			updateNote->Bind(body);
			updateNote->Bind(note.creationDate.GetTime());
			updateNote->Bind(note.modificationDate.GetTime());
			updateNote->Bind(note.subjectDate.GetTime());
			if (note.Location.IsValid)
			{
				updateNote->Bind(note.Location.Altitude);
				updateNote->Bind(note.Location.Latitude);
				updateNote->Bind(note.Location.Longitude);
			}
			else
			{
				updateNote->BindNull();
				updateNote->BindNull();
				updateNote->BindNull();
			}
			updateNote->Bind(note.Author);
			updateNote->Bind(note.Source);
			updateNote->Bind(note.SourceUrl);
			updateNote->Bind(note.SourceApplication);
			updateNote->Bind(notebook);
			updateNote->Bind(rowid);
			updateNote->Execute();
		}
	}
}

void UserModel::AddNotebook(const Notebook & notebook)
{
	IDataStore::Statement statement = dataStore.MakeStatement
		("INSERT OR REPLACE INTO Notebooks(`guid`, `usn`, `name`, `creationDate`, `modificationDate`,"
		"                                  `isDirty`, `updateCount`, `isDefault`, `isLastUsed`)"
		"  VALUES (?, ?, ?, ?, ?, ?, 0, 0, 0)"
		);
	statement->Bind(notebook.guid);
	statement->Bind(notebook.usn);
	statement->Bind(notebook.name);
	statement->Bind(notebook.CreationDate.GetTime());
	statement->Bind(notebook.ModificationDate.GetTime());
	statement->Bind(notebook.isDirty);
	statement->Execute();
}

void UserModel::AddRecognitionEntry(const RecognitionEntry & entry)
{
	wstring ucText;
	transform
		( entry.Text.begin()
		, entry.Text.end()
		, back_inserter(ucText)
		, towupper
		);

	IDataStore::Statement statement = dataStore.MakeStatement
		( "INSERT INTO Recognition(text, weight, x, y, w, h, resource)"
		"  VALUES (?, ?, ?, ?, ?, ?, ?)"
		);
	statement->Bind(ucText);
	statement->Bind(entry.Weight);
	statement->Bind(entry.X);
	statement->Bind(entry.Y);
	statement->Bind(entry.W);
	statement->Bind(entry.H);
	statement->Bind(entry.Resource);
	statement->Execute();
}

void UserModel::AddResource(const Resource & resource)
{
	IDataStore::Statement statement = dataStore.MakeStatement
		( "INSERT OR REPLACE INTO Resources(`guid`, `hash`, `data`, `mime`, `width`, `height`,"
		"                                   `sourceUrl`, `timestamp`, `altitude`, `latitude`,"
		"                                   `longitude`, `fileName`, `isAttachment`, `note`)"
		"  VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)"
		);
	statement->Bind(resource.Guid);
	statement->Bind(resource.Hash);
	statement->Bind(resource.Data);
	statement->Bind(resource.Mime);
	if (resource.Dimensions.IsValid)
	{
		statement->Bind(resource.Dimensions.Width);
		statement->Bind(resource.Dimensions.Height);
	}
	else
	{
		statement->BindNull();
		statement->BindNull();
	}
	statement->Bind(resource.SourceUrl);
	statement->Bind(resource.Timestamp.GetTime());
	if (resource.Location.IsValid)
	{
		statement->Bind(resource.Location.Altitude);
		statement->Bind(resource.Location.Latitude);
		statement->Bind(resource.Location.Longitude);
	}
	else
	{
		statement->BindNull();
		statement->BindNull();
		statement->BindNull();
	}
	statement->Bind(resource.FileName);
	statement->Bind(resource.IsAttachment);
	statement->Bind(resource.Note);
	statement->Execute();
}

void UserModel::AddTag(const Tag & tag)
{
	wstring searchName;
	transform
		( tag.name.begin()
		, tag.name.end()
		, back_inserter(searchName)
		, towupper
		);

	IDataStore::Statement statement = dataStore.MakeStatement
		( "INSERT OR REPLACE INTO Tags(guid, usn, name, parentGuid, searchName, isDirty)"
		"  VALUES (?, ?, ?, ?, ?, ?)"
		);
	statement->Bind(tag.guid);
	statement->Bind(tag.usn);
	statement->Bind(tag.name);
	statement->Bind(tag.parentGuid);
	statement->Bind(searchName);
	statement->Bind(tag.isDirty);
	statement->Execute();
}

void UserModel::AddTagToNote
	( const Guid & tag
	, const Note & note
	)
{
	IDataStore::Statement statement = dataStore.MakeStatement
		( "INSERT OR REPLACE INTO NoteTags(note, tag)"
			" SELECT ?, guid"
			" FROM   tags"
			" WHERE  guid = ?"
		);
	statement->Bind(note.guid);
	statement->Bind(tag);
	statement->Execute();
}

void UserModel::BeginTransaction()
{
	dataStore.MakeStatement("BEGIN TRANSACTION")->Execute();
}

void UserModel::Compact()
{
	dataStore.MakeStatement("VACUUM")->Execute();
}

void UserModel::DeleteNote(const Guid & note)
{
	if (note.IsLocal())
	{
		ExpungeNote(note);
		return;
	}
	IDataStore::Statement statement = dataStore.MakeStatement
		( "UPDATE Notes"
		"  SET isDeleted = 1, isDirty = 1"
		"  WHERE guid = ?"
		);
	statement->Bind(note);
	statement->Execute();
}

void UserModel::DeleteNoteTags(const Guid & note)
{
	IDataStore::Statement statement = dataStore.MakeStatement
		( "DELETE FROM NoteTags"
		"  WHERE note = ?"
		);
	statement->Bind(note);
	statement->Execute();
}

void UserModel::DeleteNoteThumbnail(const Guid & note)
{
	IDataStore::Statement statement = dataStore.MakeStatement
		( "UPDATE Notes"
		"  SET thumbnail = NULL"
		"  WHERE guid = ?"
		);
	statement->Bind(note);
	statement->Execute();
}

void UserModel::EndTransaction()
{
	dataStore.MakeStatement("END TRANSACTION")->Execute();
}

bool UserModel::Exists(const wstring & username)
{
	wstring path(CreatePathFromName(deviceFolder, username));
	if (INVALID_FILE_ATTRIBUTES != ::GetFileAttributes(path.c_str()))
		return true;
	path = flashCard.GetPath();
	if (path.empty())
		return false;
	path = CreatePathFromName(path, username);
	return INVALID_FILE_ATTRIBUTES != ::GetFileAttributes(path.c_str());
}

void UserModel::ExpungeNote(const Guid & note)
{
	Transaction transaction(*this);
	__int64 rowid(0L);
	{
		IDataStore::Statement statement = dataStore.MakeStatement
			( "SELECT rowid FROM Notes Where guid = ?"
			);
		statement->Bind(note);
		if (statement->Execute())
			return;
		statement->Get(rowid);
	}
	{
		IDataStore::Statement statement = dataStore.MakeStatement
			( "DELETE FROM Notes WHERE rowid = ?"
			);
		statement->Bind(rowid);
		statement->Execute();
	}
	{
		IDataStore::Statement statement = dataStore.MakeStatement
			( "DELETE FROM NoteText WHERE rowid = ?"
			);
		statement->Bind(rowid);
		statement->Execute();
	}
}

void UserModel::ExpungeNotebook(const Guid & notebook)
{
	if (notebook.IsEmpty())
		throw std::exception("Invalid notebook.");

	IDataStore::Statement statement = dataStore.MakeStatement
		( "DELETE FROM Notebooks WHERE guid = ?"
		);
	statement->Bind(notebook);
	statement->Execute();
}

void UserModel::ExpungeTag(const Guid & tag)
{
	IDataStore::Statement statement = dataStore.MakeStatement
		( "DELETE FROM Tags WHERE guid = ?"
		);
	statement->Bind(tag);
	statement->Execute();
}

wstring UserModel::GetPasswordHash()
{
	wstring password;
	GetProperty(L"password", password);
	return password;
}

wstring UserModel::GetUsername()
{
	wstring username;
	GetProperty(L"username", username);
	return username;
}

void UserModel::GetDefaultNotebook(Guid & notebook)
{
	IDataStore::Statement statement = dataStore.MakeStatement
		( "SELECT guid"
		"  FROM Notebooks"
		"  WHERE isDefault = 1"
		"  LIMIT 1"
		);
	if (statement->Execute())
		throw std::exception("No default notebook.");
	statement->Get(notebook);
}

void UserModel::GetDeletedNotes(GuidList & notes)
{
	notes.clear();
	IDataStore::Statement statement = dataStore.MakeStatement
		( "SELECT guid"
		"  FROM Notes"
		"  WHERE isDeleted = 1"
		);
	while (!statement->Execute())
	{
		Guid guid;
		statement->Get(guid);
		notes.push_back(guid);
	}
}

int UserModel::GetDirtyNoteCount(const Guid & notebook)
{
	string str
			( "SELECT Count(*)"
			"  FROM Notes"
			"  WHERE isDirty = 1 {0}"
			);
	Tools::ReplaceAll(str, "{0}", notebook.IsEmpty() ? "" : "AND notebook = ?");
	IDataStore::Statement statement(dataStore.MakeStatement(str.c_str()));
	if (!notebook.IsEmpty())
		statement->Bind(notebook);
	if (statement->Execute())
		throw std::exception("Could not count dirty notes.");
	int count;
	statement->Get(count);
	return count;
}

__int64 UserModel::GetLastSyncEnTime()
{
	__int64 enTime(0);
	GetProperty(L"lastSyncTime", enTime);
	return enTime;
}

void UserModel::GetLastUsedNotebook(Guid & notebook)
{
	Transaction transaction(*this);

	IDataStore::Statement statement = dataStore.MakeStatement
		( "SELECT guid"
		"  FROM Notebooks"
		"  WHERE isLastUsed = 1"
		"  LIMIT 1"
		);
	if (statement->Execute())
		notebook = Guid::GetEmpty();
	else
		statement->Get(notebook);
}

void UserModel::GetLastUsedNotebookOrDefault(Guid & notebook)
{
	GetLastUsedNotebook(notebook);
	if (notebook.IsEmpty())
		GetDefaultNotebook(notebook);
}

DbLocation UserModel::GetLocation()
{
	return dataStore.GetLocation();
}

void UserModel::GetNote(const Guid & guid, Note & note)
{
	IDataStore::Statement statement = dataStore.MakeStatement
		("SELECT usn, title, isDirty, creationDate, modificationDate, subjectDate,"
		"        altitude, latitude, longitude, author, source, sourceUrl, sourceApplication"
		"  FROM Notes"
		"  WHERE guid = ?"
		"  LIMIT 1"
		);
	statement->Bind(guid);
	if (statement->Execute())
		throw std::exception("Note not found.");
	note.guid = guid;
	statement->Get(note.usn);
	statement->Get(note.name);
	statement->Get(note.isDirty);
	statement->Get(note.creationDate);
	statement->Get(note.modificationDate);
	statement->Get(note.subjectDate);
	statement->Get(note.Location.Altitude);
	statement->Get(note.Location.Latitude);
	statement->Get(note.Location.Longitude);
	statement->Get(note.Author);
	statement->Get(note.Source);
	statement->Get(note.SourceUrl);
	statement->Get(note.SourceApplication);

	note.Location.Validate();
}

void UserModel::GetNoteAttachments
		( const Guid     & note
		, AttachmentList & attachments
		)
{
	IDataStore::Statement statement = dataStore.MakeStatement
		( "SELECT guid, mime, fileName"
		"  FROM Resources"
		"  WHERE note = ? AND length(fileName) > 0"
		"  ORDER BY fileName, timestamp"
		);
	statement->Bind(note);
	while (!statement->Execute())
	{
		attachments.push_back(Attachment());
		Attachment & attachment(attachments.back());
		statement->Get(attachment.Guid);
		statement->Get(attachment.Mime);
		statement->Get(attachment.FileName);
	}
}

void UserModel::GetNoteBody
	( const Guid & guid
	, wstring    & body
	)
{
	IDataStore::Statement statement = dataStore.MakeStatement
		( "SELECT body"
		"  FROM Notes"
		"  WHERE guid = ?"
		"  LIMIT 1"
		);
	statement->Bind(guid);
	if (statement->Execute())
		throw std::exception("Note not found.");
	statement->Get(body);
}

void UserModel::GetNoteResources
	( const Guid & note
	, GuidList   & resources
	)
{
	IDataStore::Statement statement = dataStore.MakeStatement
		( "SELECT guid"
		"  FROM Resources"
		"  WHERE note = ?"
		);
	statement->Bind(note);
	while (!statement->Execute())
	{
		Guid guid;
		statement->Get(guid);
		resources.push_back(guid);
	}
}

void UserModel::GetNoteTags
	( const Note & note
	, TagList    & tags
	)
{
	IDataStore::Statement statement = dataStore.MakeStatement
		( "SELECT guid, usn, name, parentGuid, isDirty"
		"  FROM Tags, NoteTags"
		"  WHERE note = ? AND tag = guid"
		"  ORDER BY name"
		);
	statement->Bind(note.guid);
	while (!statement->Execute())
	{
		tags.push_back(Tag());
		Tag & t(tags.back());
		statement->Get(t.guid);
		statement->Get(t.usn);
		statement->Get(t.name);
		statement->Get(t.parentGuid);
		statement->Get(t.isDirty);
	}
}

void UserModel::GetNotebook
	( const Guid & guid
	, Notebook   & notebook
	)
{
	if (guid.IsEmpty())
		throw std::exception("Invalid notebook.");
	IDataStore::Statement statement = dataStore.MakeStatement
		( "SELECT guid, usn, name, creationDate, modificationDate, isDirty"
		"  FROM   Notebooks"
		"  WHERE  guid = ?"
		);
	statement->Bind(guid);
	if (statement->Execute())
		throw std::exception("Notebook not found.");
	statement->Get(notebook.guid);
	statement->Get(notebook.usn);
	statement->Get(notebook.name);
	statement->Get(notebook.CreationDate);
	statement->Get(notebook.ModificationDate);
	statement->Get(notebook.isDirty);
}

int UserModel::GetNotebookUpdateCount(const Guid & notebook)
{
	IDataStore::Statement statement;

	if (notebook.IsEmpty())
	{
		statement = dataStore.MakeStatement
			( "SELECT MIN(updateCount)"
			"  FROM   Notebooks"
			);
		statement->Execute();
	}
	else
	{
		statement = dataStore.MakeStatement
			( "SELECT updateCount"
			"  FROM   Notebooks"
			"  WHERE  guid = ?"
			);
		statement->Bind(notebook);
		if (statement->Execute())
			throw std::exception("Notebook not found.");
	}

	int updateCount(0);
	statement->Get(updateCount);
	return updateCount;
}

void UserModel::GetNoteThumbnail(const Guid & guid, Thumbnail & thumbnail)
{
	IDataStore::Statement statement = dataStore.MakeStatement
		( "SELECT rowid, thumbnailWidth, thumbnailHeight"
		"  FROM Notes"
		"  WHERE guid = ?"
		"  AND   thumbnail IS NOT NULL"
		"  LIMIT 1"
		);
	statement->Bind(guid);
	if (!statement->Execute())
	{
		__int64 row(0);
		statement->Get(row);
		statement->Get(thumbnail.Width);
		statement->Get(thumbnail.Height);

		IDataStore::Blob sqlBlob = dataStore.MakeBlob
			( "Notes"
			, "thumbnail"
			, row
			);
		sqlBlob->Read(thumbnail.Data);
	}
}

void UserModel::GetNotebooks(NotebookList & notebooks)
{
	IDataStore::Statement statement = dataStore.MakeStatement
		( "SELECT guid, usn, name, creationDate, modificationDate, isDirty"
		"  FROM Notebooks"
		"  ORDER BY name"
		);
	while (!statement->Execute())
	{
		notebooks.push_back(Notebook());
		Notebook & n(notebooks.back());
		statement->Get(n.guid);
		statement->Get(n.usn);
		statement->Get(n.name);
		statement->Get(n.CreationDate);
		statement->Get(n.ModificationDate);
		statement->Get(n.isDirty);
	}
}

void UserModel::GetNotes
	( Guid       notebook // leave empty, if not used
	, wstring    search   // leave empty, if not used
	, int        start    // set count to 0, if not used
	, int        count    // set to 0, if not used
	, NoteList & notes
	)
{
	IDataStore::Statement statement;
	if (search.find_first_not_of(L" \t") == wstring::npos)
	{
		string str =
			" SELECT guid, usn, title, isDirty, creationDate, modificationDate, subjectDate,"
			"        altitude, latitude, longitude, author, source, sourceUrl, sourceApplication"
			"  FROM Notes"
			"  WHERE isDeleted = 0 {0}"
			"  ORDER BY modificationDate DESC, creationDate DESC"
			"  {1}";
		Tools::ReplaceAll(str, "{0}", notebook.IsEmpty() ? "" : "AND notebook = ?");
		Tools::ReplaceAll(str, "{1}", (count <= 0) ? "" : "LIMIT ? OFFSET ?");
		statement = dataStore.MakeStatement(str.c_str());
		if (!notebook.IsEmpty())
			statement->Bind(notebook);
		if (count > 0)
		{
			statement->Bind(count);
			statement->Bind(start);
		}
	}
	else
	{
		string str =
			" SELECT guid, usn, title, isDirty, creationDate, modificationDate, subjectDate,"
			"        altitude, latitude, longitude, author, source, sourceUrl, sourceApplication"
			" FROM ( SELECT n.guid"
			"        FROM   Notes AS n JOIN NoteText ON (n.rowid = NoteText.rowid)"
			"        WHERE  n.isDeleted = 0 {0} AND NoteText MATCH ?"
			"        UNION"
			"        SELECT n.guid"
			"        FROM   Notes    AS n"
			"        JOIN   NoteTags AS nt ON (n.guid = nt.note)"
			"        JOIN   Tags     AS t  ON (t.guid = nt.tag)"
			"        WHERE  n.isDeleted = 0 {0} AND t.searchName = ?"
			"        UNION"
			"        SELECT n.guid"
			"        FROM   Notes       AS n"
			"        JOIN   Resources   AS rs ON (n.guid = rs.note)"
			"        JOIN   Recognition AS rc ON (rs.guid = rc.resource)"
			"        WHERE  n.isDeleted = 0 {0} AND rc.text = ?"
			"      ) JOIN Notes USING (guid)"
			" ORDER BY modificationDate DESC, creationDate DESC"
			" {1}";
		Tools::ReplaceAll(str, "{0}", notebook.IsEmpty() ? "" : "AND n.notebook = ?");
		Tools::ReplaceAll(str, "{1}", (count <= 0) ? "" : "LIMIT ? OFFSET ?");
		statement = dataStore.MakeStatement(str.c_str());
		wstring ucSearch;
		transform
			( search.begin()
			, search.end()
			, back_inserter(ucSearch)
			, towupper
			);
		if (!notebook.IsEmpty())
			statement->Bind(notebook);
		statement->Bind(ucSearch);
		if (!notebook.IsEmpty())
			statement->Bind(notebook);
		statement->Bind(ucSearch);
		if (!notebook.IsEmpty())
			statement->Bind(notebook);
		statement->Bind(ucSearch);
		if (count > 0)
		{
			statement->Bind(count);
			statement->Bind(start);
		}
	}
	notes.clear();
	while (!statement->Execute())
	{
		notes.push_back(Note());
		Note & n(notes.back());
		statement->Get(n.guid);
		statement->Get(n.usn);
		statement->Get(n.name);
		statement->Get(n.isDirty);
		statement->Get(n.creationDate);
		statement->Get(n.modificationDate);
		statement->Get(n.subjectDate);
		statement->Get(n.Location.Altitude);
		statement->Get(n.Location.Latitude);
		statement->Get(n.Location.Longitude);
		statement->Get(n.Author);
		statement->Get(n.Source);
		statement->Get(n.SourceUrl);
		statement->Get(n.SourceApplication);
		n.Location.Validate();
	}
}

wstring UserModel::GetPath()
{
	return dataStore.GetPath();
}

void UserModel::GetResource
	( const string & hash
	, Blob         & blob
	)
{
	IDataStore::Statement statement = dataStore.MakeStatement
		( "SELECT rowid"
		"  FROM   Resources"
		"  WHERE  hash = ?"
		"  LIMIT  1"
		);
	statement->Bind(hash);
	if (statement->Execute())
		throw std::exception("Image resource not found.");
	__int64 row(0);
	statement->Get(row);

	IDataStore::Blob sqlBlob = dataStore.MakeBlob
		( "Resources"
		, "data"
		, row
		);
	sqlBlob->Read(blob);
}

void UserModel::GetResource
	( const Guid & guid
	, Resource   & resource
	)
{
	IDataStore::Statement statement = dataStore.MakeStatement
		( "SELECT `rowid`, `hash`, `mime`, `width`, `height`, `sourceUrl`, `timestamp`,"
		"         `altitude`, `latitude`, `longitude`, `fileName`, `isAttachment`, `note`"
		"  FROM   Resources"
		"  WHERE  guid = ?"
		);
	statement->Bind(guid);
	if (statement->Execute())
		throw std::exception("Resource not found.");

	resource.Guid = guid;

	__int64 row(0);
	statement->Get(row);
	statement->Get(resource.Hash);
	statement->Get(resource.Mime);
	statement->Get(resource.Dimensions.Width);
	statement->Get(resource.Dimensions.Height);
	statement->Get(resource.SourceUrl);
	statement->Get(resource.Timestamp);
	statement->Get(resource.Location.Altitude);
	statement->Get(resource.Location.Latitude);
	statement->Get(resource.Location.Longitude);
	statement->Get(resource.FileName);
	statement->Get(resource.IsAttachment);
	statement->Get(resource.Note);

	resource.Dimensions.Validate();
	resource.Location.Validate();

	IDataStore::Blob sqlBlob = dataStore.MakeBlob
		( "Resources"
		, "data"
		, row
		);
	sqlBlob->Read(resource.Data);
}

void UserModel::GetResources(GuidList & resources)
{
	IDataStore::Statement statement = dataStore.MakeStatement
		( "SELECT guid"
		"  FROM Resources"
		);
	while (!statement->Execute())
	{
		Guid guid;
		statement->Get(guid);
		resources.push_back(guid);
	}
}

__int64 UserModel::GetSize()
{
	HANDLE file = ::CreateFile
		( dataStore.GetPath().c_str()        // lpFileName
		, GENERIC_READ                       // dwDesiredAccess
		, FILE_SHARE_READ | FILE_SHARE_WRITE // dwShareMode
		, NULL                               // lpSecurityAttributes
		, OPEN_EXISTING                      // dwCreationDisposition
		, FILE_ATTRIBUTE_NORMAL              // dwFlagsAndAttributes
		, NULL                               // hTemplateFile
		);
	if (file == INVALID_HANDLE_VALUE)
		throw std::exception("Could not access file.");

	ULARGE_INTEGER size;
	size.LowPart = ::GetFileSize(file, &size.HighPart);
	::CloseHandle(file);
	if (size.LowPart == 0xFFFFFFFF && ::GetLastError() != NO_ERROR)
		throw std::exception("Could not retreive file size.");

	return size.QuadPart;
}

int UserModel::GetSyncVersion()
{
	int version(0);
	GetProperty(L"syncVersion", version);
	return version;
}

void UserModel::GetTags(TagList & tags)
{
	IDataStore::Statement statement = dataStore.MakeStatement
		( "SELECT guid, usn, name, parentGuid, isDirty"
		"  FROM Tags"
		"  ORDER BY name"
		);
	while (!statement->Execute())
	{
		tags.push_back(Tag());
		Tag & t(tags.back());
		statement->Get(t.guid);
		statement->Get(t.usn);
		statement->Get(t.name);
		statement->Get(t.parentGuid);
		statement->Get(t.isDirty);
	}
}

int UserModel::GetUpdateCount()
{
	int updateCount(0);
	GetProperty(L"updateCount", updateCount);
	return updateCount;
}

int UserModel::GetVersion()
{
	int version(0);
	GetProperty(L"version", version);
	return version;
}

void UserModel::Load(const wstring & username)
{
	wstring path(CreatePathFromName(deviceFolder.c_str(), username));
	if (!TryLoad(path, DbLocationDevice))
	{
		wstring cardFolder(flashCard.GetPath());
		if (cardFolder.empty())
			throw std::exception("Database could not be loaded.");
		path = CreatePathFromName(cardFolder.c_str(), username);
		if (!TryLoad(path, DbLocationCard))
			throw std::exception("Database could not be loaded.");
	}
	Update();
	SignalLoaded();
}

void UserModel::LoadAs
	( const std::wstring & oldUsername
	, const std::wstring & newUsername
	)
{
	wstring    oldPath  (CreatePathFromName(deviceFolder, oldUsername));
	wstring    newPath  (CreatePathFromName(deviceFolder, newUsername));
	DbLocation location (DbLocationDevice);
	if (INVALID_FILE_ATTRIBUTES == ::GetFileAttributes(oldPath.c_str()))
	{
		wstring cardFolder(flashCard.GetPath());
		if (cardFolder.empty())
			throw std::exception("Database could not be found.");
		oldPath  = CreatePathFromName(cardFolder, oldUsername);
		newPath  = CreatePathFromName(cardFolder, newUsername);
		location = DbLocationCard;
	}
	if (!::MoveFile(oldPath.c_str(), newPath.c_str()))
		throw std::exception("Database could not be renamed.");
	if (!TryLoad(newPath, location))
		throw std::exception("Database could not be loaded.");
	Update();
	SetProperty(L"username", newUsername);
	SignalLoaded();
}

void UserModel::LoadOrCreate(const wstring & username)
{
	wstring    path     (CreatePathFromName(deviceFolder, username));
	DbLocation location (DbLocationDevice);
	if (INVALID_FILE_ATTRIBUTES == ::GetFileAttributes(path.c_str()))
	{
		wstring cardFolder(flashCard.GetPath());
		if (!cardFolder.empty())
		{
			path     = CreatePathFromName(cardFolder, username);
			location = DbLocationCard;
		}
	}
	if (!TryLoad(path, location))
	{
		path = CreatePathFromName(deviceFolder, username);
		::CreateDirectory(deviceFolder.c_str(), NULL);
		Create(path, DbLocationDevice);
		Initialize(username);
	}
	Update();
	SignalLoaded();
}

void UserModel::MakeNotebookDefault(const Guid & notebook)
{
	if (notebook.IsEmpty())
		throw std::exception("Invalid notebook.");

	IDataStore::Statement removeOld = dataStore.MakeStatement
		( "UPDATE Notebooks"
		"  SET isDefault = 0"
		"  WHERE isDefault = 1"
		);
	removeOld->Execute();

	IDataStore::Statement setNew = dataStore.MakeStatement
		( "UPDATE Notebooks"
		"  SET isDefault = 1"
		"  WHERE guid = ?"
		);
	setNew->Bind(notebook);
	setNew->Execute();
}

void UserModel::MakeNotebookLastUsed(const Guid & notebook)
{
	IDataStore::Statement removeOld = dataStore.MakeStatement
		( "UPDATE Notebooks"
		"  SET isLastUsed = 0"
		"  WHERE isLastUsed = 1"
		);
	removeOld->Execute();

	if (!notebook.IsEmpty())
	{
		IDataStore::Statement setNew = dataStore.MakeStatement
			( "UPDATE Notebooks"
			"  SET isLastUsed = 1"
			"  WHERE guid = ?"
			);
		setNew->Bind(notebook);
		setNew->Execute();
	}
}

void UserModel::MoveToCard()
{
	wstring cardFolder(flashCard.GetPath());
	if (cardFolder.empty())
		throw std::exception("No storage card available.");

	wstring username(GetUsername());
	::CreateDirectory(cardFolder.c_str(), NULL);
	Move
		( CreatePathFromName(deviceFolder, username)
		, CreatePathFromName(cardFolder,   username)
		, username
		);
}

void UserModel::MoveToDevice()
{
	wstring cardFolder(flashCard.GetPath());
	if (cardFolder.empty())
		throw std::exception("No storage card available.");

	wstring username(GetUsername());
	::CreateDirectory(deviceFolder.c_str(), NULL);
	Move
		( CreatePathFromName(cardFolder,   username)
		, CreatePathFromName(deviceFolder, username)
		, username
		);
}

void UserModel::ReplaceNote
	( const Note          & note
	, const std::wstring  & body
	, const std::wstring  & bodyText
	, const Guid          & notebook
	)
{
	if (notebook.IsEmpty())
		throw std::exception("Invalid notebook.");

	Transaction transaction(*this);
	{
		IDataStore::Statement insertNote = dataStore.MakeStatement
			("INSERT OR REPLACE INTO Notes(`guid`, `usn`, `title`, `isDirty`, `body`,"
			"                              `creationDate`, `modificationDate`, `subjectDate`,"
			"                              `altitude`, `latitude`, `longitude`, `author`,"
			"                              `source`, `sourceUrl`, `sourceApplication`, `notebook`)"
			"  VALUES (?, ?, ?, ?,  ?, ?, ?, ?,  ?, ?, ?, ?,  ?, ?, ?, ?)"
			);
		insertNote->Bind(note.guid);
		insertNote->Bind(note.usn);
		insertNote->Bind(note.name);
		insertNote->Bind(note.isDirty);
		insertNote->Bind(body);
		insertNote->Bind(note.creationDate.GetTime());
		insertNote->Bind(note.modificationDate.GetTime());
		insertNote->Bind(note.subjectDate.GetTime());
		if (note.Location.IsValid)
		{
			insertNote->Bind(note.Location.Altitude);
			insertNote->Bind(note.Location.Latitude);
			insertNote->Bind(note.Location.Longitude);
		}
		else
		{
			insertNote->BindNull();
			insertNote->BindNull();
			insertNote->BindNull();
		}
		insertNote->Bind(note.Author);
		insertNote->Bind(note.Source);
		insertNote->Bind(note.SourceUrl);
		insertNote->Bind(note.SourceApplication);
		insertNote->Bind(notebook);
		insertNote->Execute();
	}
	{
		__int64 rowid(dataStore.GetLastInsertRowid());

		wstring ucName;
		ucName.reserve(note.name.size());
		transform
			( note.name.begin()
			, note.name.end()
			, back_inserter(ucName)
			, towupper
			);
		wstring ucBody;
		ucBody.reserve(bodyText.size());
		transform
			( bodyText.begin()
			, bodyText.end()
			, back_inserter(ucBody)
			, towupper
			);

		IDataStore::Statement insertText = dataStore.MakeStatement
			( "INSERT OR REPLACE INTO NoteText(rowid, title, body)"
			"  VALUES (?, ?, ?)"
			);
		insertText->Bind(rowid);
		insertText->Bind(ucName);
		insertText->Bind(ucBody);
		insertText->Execute();
	}
}

void UserModel::SetCredentials
	( const wstring & username
	, const wstring & password
	)
{
	SetProperty(L"username", username);
	SetProperty(L"password", password);
}

void UserModel::SetLastSyncEnTime(__int64 enTime)
{
	SetProperty(L"lastSyncTime", enTime);
}

void UserModel::SetNotebookUpdateCount
	( const Guid & notebook
	, int          updateCount
	)
{
	if (notebook.IsEmpty())
		throw std::exception("Invalid notebook.");

	IDataStore::Statement statement = dataStore.MakeStatement
		( "UPDATE Notebooks"
		"  SET    updateCount = ?"
		"  WHERE  guid = ?"
		);
	statement->Bind(updateCount);
	statement->Bind(notebook);
	statement->Execute();
}

void UserModel::SetNoteThumbnail(const Guid & guid, const Thumbnail & thumbnail)
{
	IDataStore::Statement statement = dataStore.MakeStatement
		( "UPDATE Notes"
		"  SET    thumbnail = ?, thumbnailWidth = ?, thumbnailHeight = ?"
		"  WHERE  guid = ?"
		);
	statement->Bind(thumbnail.Data);
	statement->Bind(thumbnail.Width);
	statement->Bind(thumbnail.Height);
	statement->Bind(guid);
	statement->Execute();
}

void UserModel::SetSyncVersion(int version)
{
	SetProperty(L"syncVersion", version);
}

void UserModel::SetUpdateCount(int updateCount)
{
	SetProperty(L"updateCount", updateCount);
}

void UserModel::Unload()
{
	dataStore.Close();
}

void UserModel::UpdateNote
	( const Guid & note
	, const Note & replacement
	)
{
	IDataStore::Statement statement = dataStore.MakeStatement
		( "UPDATE Notes"
		"  SET guid = ?, usn = ?, title = ?, isDirty = ?, creationDate = ?,"
		"      modificationDate = ?, subjectDate = ?, altitude = ?, latitude = ?,"
		"      longitude = ?, author = ?, source = ?, sourceUrl = ?, sourceApplication = ?"
		"  WHERE guid = ?"
		);
	statement->Bind(replacement.guid);
	statement->Bind(replacement.usn);
	statement->Bind(replacement.name);
	statement->Bind(replacement.isDirty);
	statement->Bind(replacement.creationDate.GetTime());
	statement->Bind(replacement.modificationDate.GetTime());
	statement->Bind(replacement.subjectDate.GetTime());
	if (replacement.Location.IsValid)
	{
		statement->Bind(replacement.Location.Altitude);
		statement->Bind(replacement.Location.Latitude);
		statement->Bind(replacement.Location.Longitude);
	}
	else
	{
		statement->BindNull();
		statement->BindNull();
		statement->BindNull();
	}
	statement->Bind(replacement.Author);
	statement->Bind(replacement.Source);
	statement->Bind(replacement.SourceUrl);
	statement->Bind(replacement.SourceApplication);
	statement->Bind(note);
	statement->Execute();
}

void UserModel::UpdateNotebook
	( const Guid     & notebook
	, const Notebook & replacement
	)
{
	if (notebook.IsEmpty())
		throw std::exception("Invalid notebook.");

	IDataStore::Statement statement = dataStore.MakeStatement
		( "UPDATE Notebooks"
		"  SET guid = ?, usn = ?, name = ?, creationDate = ?,"
		"      modificationDate = ?, isDirty = ?"
		"  WHERE guid = ?"
		);
	statement->Bind(replacement.guid);
	statement->Bind(replacement.usn);
	statement->Bind(replacement.name);
	statement->Bind(replacement.CreationDate.GetTime());
	statement->Bind(replacement.ModificationDate.GetTime());
	statement->Bind(replacement.isDirty);
	statement->Bind(notebook);
	statement->Execute();
}

void UserModel::UpdateResource
		( const Guid     & resource
		, const Resource & replacement
		)
{
	IDataStore::Statement statement = dataStore.MakeStatement
		( "UPDATE Resources"
		"  SET hash = ?, data = ?, mime = ?, width = ?, height = ?,"
		"      sourceUrl = ?, timestamp = ?, altitude = ?, latitude = ?,"
		"      longitude = ?, fileName = ?, isAttachment = ?"
		"  WHERE guid = ?"
		);
	statement->Bind(replacement.Hash);
	statement->Bind(replacement.Data);
	statement->Bind(replacement.Mime);
	if (replacement.Dimensions.IsValid)
	{
		statement->Bind(replacement.Dimensions.Width);
		statement->Bind(replacement.Dimensions.Height);
	}
	else
	{
		statement->BindNull();
		statement->BindNull();
	}
	statement->Bind(replacement.SourceUrl);
	statement->Bind(replacement.Timestamp.GetTime());
	if (replacement.Location.IsValid)
	{
		statement->Bind(replacement.Location.Altitude);
		statement->Bind(replacement.Location.Latitude);
		statement->Bind(replacement.Location.Longitude);
	}
	else
	{
		statement->BindNull();
		statement->BindNull();
		statement->BindNull();
	}
	statement->Bind(replacement.FileName);
	statement->Bind(replacement.IsAttachment);
	statement->Bind(resource);
}

void UserModel::UpdateTag
	( const Guid & tag
	, const Tag  & replacement
	)
{
	IDataStore::Statement statement = dataStore.MakeStatement
		( "UPDATE Tags"
		"  SET guid = ?, usn = ?, name = ?, parentGuid = ?, isDirty = ?"
		"  WHERE guid = ?"
		);
	statement->Bind(replacement.guid);
	statement->Bind(replacement.usn);
	statement->Bind(replacement.name);
	statement->Bind(replacement.parentGuid);
	statement->Bind(replacement.isDirty);
	statement->Bind(tag);
	statement->Execute();
}

//------------------
// utility functions
//------------------

void UserModel::Create(const wstring & path, DbLocation location)
{
	int flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
	if (!dataStore.Open(path.c_str(), location, flags))
		throw std::exception("Database could not be created.");
}

wstring UserModel::CreatePathFromName
	( const wstring & folder
	, const wstring & name
	)
{
	wstringstream stream;
	stream << folder << L'\\' << name << L".db";
	return stream.str();
}

void UserModel::Initialize(wstring name)
{
	dataStore.MakeStatement
		( "CREATE TABLE Properties"
			"( key PRIMARY KEY"
			", value NOT NULL"
			")"
		)->Execute();
	SetProperty(L"version",      dbVersion);
	SetProperty(L"syncVersion",  dbVersion);
	SetProperty(L"username",     name);
	SetProperty(L"password",     L"");
	SetProperty(L"lastSyncTime", 0);
	SetProperty(L"updateCount",  0);

	dataStore.MakeStatement
		( "CREATE TABLE Notebooks"
			"( guid PRIMARY KEY"
			", usn"
			", name"
			", creationDate"
			", modificationDate"
			", isDirty"
			", updateCount DEFAULT 0"
			", isDefault  DEFAULT 0"
			", isLastUsed DEFAULT 0"
			")"
		)->Execute();

	dataStore.MakeStatement
		( "CREATE TRIGGER ReplaceNotebook"
		"  BEFORE INSERT ON Notebooks"
		"  BEGIN"
		"  UPDATE Notebooks"
		"  SET    usn = NEW.usn, name = NEW.name, updateCount = NEW.updateCount,"
		"         isDirty = NEW.isDirty, isDefault = NEW.isDefault,"
		"         isLastUsed = NEW.isLastUsed, creationDate = NEW.creationDate,"
		"         modificationDate = NEW.modificationDate"
		"  WHERE  guid = NEW.guid;"
		"  END"
		)->Execute();

	dataStore.MakeStatement
		( "CREATE VIRTUAL TABLE NoteText USING fts3"
			"( title"
			", body"
			")"
		)->Execute();

	dataStore.MakeStatement
		( "CREATE TABLE Notes"
			"( guid PRIMARY KEY"
			", usn"
			", title"
			", isDirty"
			", body"
			", creationDate"
			", modificationDate"
			", subjectDate"
			", altitude"
			", latitude"
			", longitude"
			", author"
			", source"
			", sourceUrl"
			", sourceApplication"
			", isDeleted DEFAULT 0"
			", thumbnail"
			", thumbnailWidth  DEFAULT 0"
			", thumbnailHeight DEFAULT 0"
			", notebook REFERENCES Notebooks(guid) ON DELETE CASCADE ON UPDATE CASCADE"
			")"
		)->Execute();

	dataStore.MakeStatement
		( "CREATE INDEX NotesNotebooks ON Notes(notebook)"
		)->Execute();

	dataStore.MakeStatement
		( "CREATE TABLE Resources"
			"( guid PRIMARY KEY"
			", hash UNIQUE"
			", data"
			", mime"
			", width"
			", height"
			", sourceUrl"
			", timestamp"
			", altitude"
			", latitude"
			", longitude"
			", fileName"
			", isAttachment"
			", note REFERENCES Notes(guid) ON DELETE CASCADE ON UPDATE CASCADE"
			")"
		)->Execute();

	dataStore.MakeStatement
		( "CREATE INDEX ResourcesNote ON Resources(note)"
		)->Execute();

	dataStore.MakeStatement
		( "CREATE TABLE Tags"
			"( guid PRIMARY KEY"
			", usn"
			", name"
			", parentGuid"
			", searchName UNIQUE"
			", isDirty"
			")"
		)->Execute();

	dataStore.MakeStatement
		( "CREATE INDEX TagsSearchName ON Tags(searchName)"
		)->Execute();

	dataStore.MakeStatement
		( "CREATE TABLE NoteTags"
			"( note REFERENCES Notes(guid) ON DELETE CASCADE ON UPDATE CASCADE"
			", tag  REFERENCES Tags(guid)  ON DELETE CASCADE ON UPDATE CASCADE"
			", UNIQUE (note, tag) ON CONFLICT REPLACE"
			")"
		)->Execute();

	dataStore.MakeStatement
		( "CREATE INDEX NoteTagsNote ON NoteTags(note)"
		)->Execute();

	dataStore.MakeStatement
		( "CREATE TABLE Recognition"
			"( text"
			", weight"
			", x"
			", y"
			", w"
			", h"
			", resource REFERENCES Resources(guid) ON DELETE CASCADE ON UPDATE CASCADE"
			")"
		)->Execute();

	dataStore.MakeStatement
		( "CREATE INDEX RecognitionResource ON Recognition(resource)"
		)->Execute();
}

void UserModel::MigrateFrom3To4()
{
	Transaction transaction(*this);
	dataStore.MakeStatement
		( "ALTER TABLE Notes ADD COLUMN isDeleted DEFAULT 0"
		)->Execute();
	SetProperty(L"version", 4);
}

void UserModel::MigrateFrom4To5()
{
	dataStore.MakeStatement("ALTER TABLE Notes ADD COLUMN `modificationDate`")->Execute();
	dataStore.MakeStatement("ALTER TABLE Notes ADD COLUMN `subjectDate`")->Execute();
	dataStore.MakeStatement("ALTER TABLE Notes ADD COLUMN `altitude`")->Execute();
	dataStore.MakeStatement("ALTER TABLE Notes ADD COLUMN `latitude`")->Execute();
	dataStore.MakeStatement("ALTER TABLE Notes ADD COLUMN `longitude`")->Execute();
	dataStore.MakeStatement("ALTER TABLE Notes ADD COLUMN `author`")->Execute();
	dataStore.MakeStatement("ALTER TABLE Notes ADD COLUMN `source`")->Execute();
	dataStore.MakeStatement("ALTER TABLE Notes ADD COLUMN `sourceUrl`")->Execute();
	dataStore.MakeStatement("ALTER TABLE Notes ADD COLUMN `sourceApplication`")->Execute();

	dataStore.MakeStatement("ALTER TABLE Notebooks ADD COLUMN `creationDate`")->Execute();
	dataStore.MakeStatement("ALTER TABLE Notebooks ADD COLUMN `modificationDate`")->Execute();

	dataStore.MakeStatement("ALTER TABLE Tags ADD COLUMN `parentGuid`")->Execute();

	dataStore.MakeStatement("DROP TRIGGER ReplaceNotebook")->Execute();
	dataStore.MakeStatement
		( "CREATE TRIGGER ReplaceNotebook"
		"  BEFORE INSERT ON Notebooks"
		"  BEGIN"
		"  UPDATE Notebooks"
		"  SET    usn = NEW.usn, name = NEW.name, updateCount = NEW.updateCount,"
		"         isDirty = NEW.isDirty, isDefault = NEW.isDefault,"
		"         isLastUsed = NEW.isLastUsed, creationDate = NEW.creationDate,"
		"         modificationDate = NEW.modificationDate"
		"  WHERE  guid = NEW.guid;"
		"  END"
		)->Execute();

	SetProperty(L"version", 5);
}

void UserModel::MigrateFrom5To6()
{
	wstring password;
	GetProperty(L"password", password);
	if (!password.empty())
		SetProperty(L"password", Tools::HashPassword(password));
	SetProperty(L"version", 6);
}

void UserModel::MigrateFrom6To7()
{
	dataStore.MakeStatement("ALTER TABLE Resources ADD COLUMN `width`")->Execute();
	dataStore.MakeStatement("ALTER TABLE Resources ADD COLUMN `height`")->Execute();
	dataStore.MakeStatement("ALTER TABLE Resources ADD COLUMN `sourceUrl`")->Execute();
	dataStore.MakeStatement("ALTER TABLE Resources ADD COLUMN `timestamp`")->Execute();
	dataStore.MakeStatement("ALTER TABLE Resources ADD COLUMN `altitude`")->Execute();
	dataStore.MakeStatement("ALTER TABLE Resources ADD COLUMN `latitude`")->Execute();
	dataStore.MakeStatement("ALTER TABLE Resources ADD COLUMN `longitude`")->Execute();
	dataStore.MakeStatement("ALTER TABLE Resources ADD COLUMN `fileName`")->Execute();
	dataStore.MakeStatement("ALTER TABLE Resources ADD COLUMN `isAttachment`")->Execute();
	SetProperty(L"version", 7);
}

void UserModel::Move
	( const wstring & oldPath
	, const wstring & newPath
	, const wstring & username
	)
{
	Unload();

	BOOL result(::MoveFile(oldPath.c_str(), newPath.c_str()));
	DWORD error(::GetLastError());

	Load(username);

	if (result == FALSE)
		throw WinException(error, L"Kernel32.dll", "Database could not be moved.");
}

void UserModel::SetPragma(const char * sql)
{
	dataStore.MakeStatement(sql)->Execute();
}

bool UserModel::TryLoad(const wstring & path, DbLocation location)
{
	int flags = SQLITE_OPEN_READWRITE;
	return dataStore.Open(path.c_str(), location, flags);
}

void UserModel::Update()
{
	SetPragma("PRAGMA foreign_keys = ON");
	SetPragma("PRAGMA synchronous = NORMAL");
	//SetPragma("PRAGMA locking_mode = EXCLUSIVE");
	Transaction transaction(*this);
	switch (GetVersion())
	{
	case 3: MigrateFrom3To4();
	case 4: MigrateFrom4To5();
	case 5: MigrateFrom5To6();
	case 6: MigrateFrom6To7();
	case 7: break;
	default:
		throw std::exception("Incompatible database version.");
	}
	if (GetNotebookCount() == 0)
	{
		Notebook notebook;
		notebook.name    = L"Notes";
		notebook.isDirty = true;
		notebook.usn     = GetUpdateCount();
		AddNotebook(notebook);
		MakeNotebookDefault(notebook.guid);
		MakeNotebookLastUsed(notebook.guid);
	}
}
