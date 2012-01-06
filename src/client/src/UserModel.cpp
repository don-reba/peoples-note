#include "stdafx.h"
#include "UserModel.h"

#include "IDataStore.h"
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
	statement->Get(0, count);
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
	statement->Get(0, count);
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
	statement->Get(0, count);
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
	statement->Get(0, count);
	return count;
}

//--------------------------
// IUuserModel implementaion
//--------------------------

void UserModel::AddNote
	( const Note     & note
	, const wstring  & body
	, const wstring  & bodyText
	, const Notebook & notebook
	)
{
	Transaction transaction(*this);

	IDataStore::Statement existenceCheck = dataStore.MakeStatement
		( "SELECT rowid from Notes WHERE guid = ?"
		);
	existenceCheck->Bind(1, note.guid);
	if (existenceCheck->Execute())
	{
		ReplaceNote(note, body, bodyText, notebook);
	}
	else
	{
		__int64 rowid(0);
		existenceCheck->Get(0, rowid);
		{
			IDataStore::Statement updateText = dataStore.MakeStatement
				( "UPDATE NoteText"
				"  SET    title = ?, body = ?"
				"  WHERE  rowid = ?"
				);
			updateText->Bind(1, note.name);
			updateText->Bind(2, bodyText);
			updateText->Bind(3, rowid);
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
			updateNote->Bind(1, note.usn);
			updateNote->Bind(2, note.name);
			updateNote->Bind(3, note.isDirty);
			updateNote->Bind(4, body);
			updateNote->Bind(5, note.creationDate.GetTime());
			updateNote->Bind(6, note.modificationDate.GetTime());
			updateNote->Bind(7, note.subjectDate.GetTime());
			if (note.Location.IsValid)
			{
				updateNote->Bind(8,  note.Location.Altitude);
				updateNote->Bind(9,  note.Location.Latitude);
				updateNote->Bind(10, note.Location.Longitude);
			}
			else
			{
				updateNote->BindNull(8);
				updateNote->BindNull(9);
				updateNote->BindNull(10);
			}
			updateNote->Bind(11, note.Author);
			updateNote->Bind(12, note.Source);
			updateNote->Bind(13, note.SourceUrl);
			updateNote->Bind(14, note.SourceApplication);
			updateNote->Bind(15, notebook.guid);
			updateNote->Bind(16, rowid);
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
	statement->Bind(1, notebook.guid);
	statement->Bind(2, notebook.usn);
	statement->Bind(3, notebook.name);
	statement->Bind(4, notebook.CreationDate.GetTime());
	statement->Bind(5, notebook.ModificationDate.GetTime());
	statement->Bind(6, notebook.isDirty);
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
	statement->Bind(1, ucText);
	statement->Bind(2, entry.Weight);
	statement->Bind(3, entry.X);
	statement->Bind(4, entry.Y);
	statement->Bind(5, entry.W);
	statement->Bind(6, entry.H);
	statement->Bind(7, entry.Resource);
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
	statement->Bind(1, resource.Guid);
	statement->Bind(2, resource.Hash);
	statement->Bind(3, resource.Data);
	statement->Bind(4, resource.Mime);
	if (resource.Dimensions.IsValid)
	{
		statement->Bind(5, resource.Dimensions.Width);
		statement->Bind(6, resource.Dimensions.Height);
	}
	else
	{
		statement->BindNull(5);
		statement->BindNull(6);
	}
	statement->Bind(7, resource.SourceUrl);
	statement->Bind(8, resource.Timestamp.GetTime());
	if (resource.Location.IsValid)
	{
		statement->Bind(9,  resource.Location.Altitude);
		statement->Bind(10, resource.Location.Latitude);
		statement->Bind(11, resource.Location.Longitude);
	}
	else
	{
		statement->BindNull(9);
		statement->BindNull(10);
		statement->BindNull(11);
	}
	statement->Bind(12, resource.FileName);
	statement->Bind(13, resource.IsAttachment);
	statement->Bind(14, resource.Note);
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
	statement->Bind(1, tag.guid);
	statement->Bind(2, tag.usn);
	statement->Bind(3, tag.name);
	statement->Bind(4, tag.parentGuid);
	statement->Bind(5, searchName);
	statement->Bind(6, tag.isDirty);
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
	statement->Bind(1, note.guid);
	statement->Bind(2, tag);
	statement->Execute();
}

void UserModel::BeginTransaction()
{
	IDataStore::Statement statement = dataStore.MakeStatement
		( "BEGIN TRANSACTION"
		);
	statement->Execute();
}

void UserModel::DeleteNote(const Guid & note)
{
	IDataStore::Statement statement = dataStore.MakeStatement
		( "UPDATE Notes"
		"  SET isDeleted = 1, isDirty = 1"
		"  WHERE guid = ?"
		);
	statement->Bind(1, note);
	statement->Execute();
}

void UserModel::DeleteNoteTags(const Guid & note)
{
	IDataStore::Statement statement = dataStore.MakeStatement
		( "DELETE FROM NoteTags"
		"  WHERE note = ?"
		);
	statement->Bind(1, note);
	statement->Execute();
}

void UserModel::DeleteNoteThumbnail(const Guid & note)
{
	IDataStore::Statement statement = dataStore.MakeStatement
		( "UPDATE Notes"
		"  SET thumbnail = NULL"
		"  WHERE guid = ?"
		);
	statement->Bind(1, note);
	statement->Execute();
}

void UserModel::EndTransaction()
{
	IDataStore::Statement statement = dataStore.MakeStatement
		( "END TRANSACTION"
		);
	statement->Execute();
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
		statement->Bind(1, note);
		if (statement->Execute())
			return;
		statement->Get(0, rowid);
	}
	{
		IDataStore::Statement statement = dataStore.MakeStatement
			( "DELETE FROM Notes WHERE rowid = ?"
			);
		statement->Bind(1, rowid);
		statement->Execute();
	}
	{
		IDataStore::Statement statement = dataStore.MakeStatement
			( "DELETE FROM NoteText WHERE rowid = ?"
			);
		statement->Bind(1, rowid);
		statement->Execute();
	}
}

void UserModel::ExpungeNotebook(const Guid & notebook)
{
	Transaction transaction(*this);

	bool isLastUsed;
	{
		IDataStore::Statement statement = dataStore.MakeStatement
			( "SELECT isLastUsed FROM Notebooks WHERE guid = ? LIMIT 1"
			);
		statement->Bind(1, notebook);
		if (statement->Execute())
			return;
		statement->Get(0, isLastUsed);
	}

	{
		IDataStore::Statement statement = dataStore.MakeStatement
			( "DELETE FROM Notebooks WHERE guid = ?"
			);
		statement->Bind(1, notebook);
		statement->Execute();
	}

	if (isLastUsed)
	{
		Notebook notebook;
		GetFirstNotebook(notebook);
		MakeNotebookLastUsed(notebook.guid);
	}
}

void UserModel::ExpungeTag(const Guid & tag)
{
	IDataStore::Statement statement = dataStore.MakeStatement
		( "DELETE FROM Tags WHERE guid = ?"
		);
	statement->Bind(1, tag);
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

void UserModel::GetDefaultNotebook(Notebook & notebook)
{
	IDataStore::Statement statement = dataStore.MakeStatement
		( "SELECT guid, usn, name, creationDate, modificationDate, isDirty"
		"  FROM Notebooks"
		"  WHERE isDefault = 1"
		"  LIMIT 1"
		);
	if (statement->Execute())
		throw std::exception("No default notebook.");
	statement->Get(0, notebook.guid);
	statement->Get(1, notebook.usn);
	statement->Get(2, notebook.name);
	statement->Get(4, notebook.CreationDate);
	statement->Get(5, notebook.ModificationDate);
	statement->Get(6, notebook.isDirty);
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
		statement->Get(0, guid);
		notes.push_back(guid);
	}
}

int UserModel::GetDirtyNoteCount(const Notebook & notebook)
{
	IDataStore::Statement statement = dataStore.MakeStatement
		( "SELECT Count(*)"
		"  FROM Notes"
		"  WHERE isDirty = 1 AND notebook = ?"
		);
	statement->Bind(1, notebook.guid);
	if (statement->Execute())
		throw std::exception("Could not count dirty notes.");
	int count;
	statement->Get(0, count);
	return count;
}

__int64 UserModel::GetLastSyncEnTime()
{
	__int64 enTime(0);
	GetProperty(L"lastSyncTime", enTime);
	return enTime;
}

void UserModel::GetLastUsedNotebook(Notebook & notebook)
{
	Transaction transaction(*this);

	IDataStore::Statement statement = dataStore.MakeStatement
		( "SELECT guid, usn, name, creationDate, modificationDate, isDirty"
		"  FROM Notebooks"
		"  WHERE isLastUsed = 1"
		"  LIMIT 1"
		);
	if (statement->Execute())
	{
		GetFirstNotebook(notebook);
		MakeNotebookLastUsed(notebook.guid);
		return;
	}
	statement->Get(0, notebook.guid);
	statement->Get(1, notebook.usn);
	statement->Get(2, notebook.name);
	statement->Get(3, notebook.CreationDate);
	statement->Get(4, notebook.ModificationDate);
	statement->Get(5, notebook.isDirty);
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
	statement->Bind(1, guid);
	if (statement->Execute())
		throw std::exception("Note not found.");
	note.guid = guid;
	statement->Get(0,  note.usn);
	statement->Get(1,  note.name);
	statement->Get(2,  note.isDirty);
	statement->Get(3,  note.creationDate);
	statement->Get(4,  note.modificationDate);
	statement->Get(5,  note.subjectDate);
	statement->Get(6,  note.Location.Altitude);
	statement->Get(7,  note.Location.Latitude);
	statement->Get(8,  note.Location.Longitude);
	statement->Get(9,  note.Author);
	statement->Get(10, note.Source);
	statement->Get(11, note.SourceUrl);
	statement->Get(12, note.SourceApplication);

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
	statement->Bind(1, note);
	while (!statement->Execute())
	{
		attachments.push_back(Attachment());
		Attachment & attachment(attachments.back());
		statement->Get(0, attachment.Guid);
		statement->Get(1, attachment.Mime);
		statement->Get(2, attachment.FileName);
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
	statement->Bind(1, guid);
	if (statement->Execute())
		throw std::exception("Note not found.");
	statement->Get(0, body);
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
	statement->Bind(1, note);
	while (!statement->Execute())
	{
		Guid guid;
		statement->Get(0, guid);
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
	statement->Bind(1, note.guid);
	while (!statement->Execute())
	{
		tags.push_back(Tag());
		Tag & t(tags.back());
		statement->Get(0, t.guid);
		statement->Get(1, t.usn);
		statement->Get(2, t.name);
		statement->Get(3, t.parentGuid);
		statement->Get(4, t.isDirty);
	}
}

void UserModel::GetNotebook
	( const Guid & guid
	, Notebook   & notebook
	)
{
	IDataStore::Statement statement = dataStore.MakeStatement
		( "SELECT guid, usn, name, creationDate, modificationDate, isDirty"
		"  FROM   Notebooks"
		"  WHERE  guid = ?"
		);
	statement->Bind(1, guid);
	if (statement->Execute())
		throw std::exception("Notebook not found.");
	statement->Get(0, notebook.guid);
	statement->Get(1, notebook.usn);
	statement->Get(2, notebook.name);
	statement->Get(3, notebook.CreationDate);
	statement->Get(4, notebook.ModificationDate);
	statement->Get(5, notebook.isDirty);
}

int UserModel::GetNotebookUpdateCount(const Guid & notebook)
{
	IDataStore::Statement statement = dataStore.MakeStatement
		( "SELECT updateCount"
		"  FROM   Notebooks"
		"  WHERE  guid = ?"
		);
	statement->Bind(1, notebook);
	if (statement->Execute())
		throw std::exception("Notebook not found.");
	int updateCount(0);
	statement->Get(0, updateCount);
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
	statement->Bind(1, guid);
	if (!statement->Execute())
	{
		__int64 row(0);
		statement->Get(0, row);
		statement->Get(1, thumbnail.Width);
		statement->Get(2, thumbnail.Height);

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
		statement->Get(0, n.guid);
		statement->Get(1, n.usn);
		statement->Get(2, n.name);
		statement->Get(3, n.CreationDate);
		statement->Get(4, n.ModificationDate);
		statement->Get(5, n.isDirty);
	}
}

void UserModel::GetNotesByNotebook
	( const Guid & notebook
	, NoteList   & notes
	)
{
	GetNotesByNotebook(notebook, 0, 0, notes);
}

void UserModel::GetNotesByNotebook
	( const Guid & notebook
	, int          start
	, int          count
	, NoteList   & notes
	)
{
	notes.clear();
	IDataStore::Statement statement;
	if (count > 0)
	{
		statement = dataStore.MakeStatement
			("SELECT guid, usn, title, isDirty, creationDate, modificationDate, subjectDate,"
			"        altitude, latitude, longitude, author, source, sourceUrl, sourceApplication"
			"  FROM Notes"
			"  WHERE isDeleted = 0 AND notebook = ?"
			"  ORDER BY modificationDate DESC, creationDate DESC"
			"  LIMIT ? OFFSET ?"
			);
		statement->Bind(1, notebook);
		statement->Bind(2, count);
		statement->Bind(3, start);
	}
	else
	{
		statement = dataStore.MakeStatement
			("SELECT guid, usn, title, isDirty, creationDate, modificationDate, subjectDate,"
			"        altitude, latitude, longitude, author, source, sourceUrl, sourceApplication"
			"  FROM Notes"
			"  WHERE isDeleted = 0 AND notebook = ?"
			"  ORDER BY modificationDate DESC, creationDate DESC"
			);
		statement->Bind(1, notebook);
	}
	while (!statement->Execute())
	{
		notes.push_back(Note());
		Note & n(notes.back());
		statement->Get(0,  n.guid);
		statement->Get(1,  n.usn);
		statement->Get(2,  n.name);
		statement->Get(3,  n.isDirty);
		statement->Get(4,  n.creationDate);
		statement->Get(5,  n.modificationDate);
		statement->Get(6,  n.subjectDate);
		statement->Get(7,  n.Location.Altitude);
		statement->Get(8,  n.Location.Latitude);
		statement->Get(9,  n.Location.Longitude);
		statement->Get(10, n.Author);
		statement->Get(11, n.Source);
		statement->Get(12, n.SourceUrl);
		statement->Get(13, n.SourceApplication);
		n.Location.Validate();
	}
}

void UserModel::GetNotesBySearch
	( const Guid    & notebook
	, const wstring & search
	, int             start
	, int             count
	, NoteList      & notes
	)
{
	if (search.find_first_not_of(L" \t") == wstring::npos)
	{
		GetNotesByNotebook(notebook, start, count, notes);
		return;
	}
	wstring ucSearch;
	transform
		( search.begin()
		, search.end()
		, back_inserter(ucSearch)
		, towupper
		);
	notes.clear();
	IDataStore::Statement statement;
	if (count > 0)
	{
		statement = dataStore.MakeStatement
			("SELECT guid, usn, title, isDirty, creationDate, modificationDate, subjectDate,"
			"        altitude, latitude, longitude, author, source, sourceUrl, sourceApplication"
			" FROM ( SELECT n.guid"
			"        FROM   Notes AS n JOIN NoteText ON (n.rowid = NoteText.rowid)"
			"        WHERE  n.isDeleted = 0 AND n.notebook = ? AND NoteText MATCH ?"
			"        UNION"
			"        SELECT n.guid"
			"        FROM   Notes    AS n"
			"        JOIN   NoteTags AS nt ON (n.guid = nt.note)"
			"        JOIN   Tags     AS t  ON (t.guid = nt.tag)"
			"        WHERE  n.isDeleted = 0 AND n.notebook = ? AND t.searchName = ?"
			"        UNION"
			"        SELECT n.guid"
			"        FROM   Notes       AS n"
			"        JOIN   Resources   AS rs ON (n.guid = rs.note)"
			"        JOIN   Recognition AS rc ON (rs.guid = rc.resource)"
			"        WHERE  n.isDeleted = 0 AND n.notebook = ? AND rc.text = ?"
			"      ) JOIN Notes USING (guid)"
			" ORDER BY modificationDate DESC, creationDate DESC"
			" LIMIT ? OFFSET ?"
			);
		statement->Bind(1, notebook);
		statement->Bind(2, ucSearch);
		statement->Bind(3, notebook);
		statement->Bind(4, ucSearch);
		statement->Bind(5, notebook);
		statement->Bind(6, ucSearch);
		statement->Bind(7, count);
		statement->Bind(8, start);
	}
	else
	{
		statement = dataStore.MakeStatement
			("SELECT guid, usn, title, isDirty, creationDate, modificationDate, subjectDate,"
			"        altitude, latitude, longitude, author, source, sourceUrl, sourceApplication"
			" FROM ( SELECT n.guid"
			"        FROM   Notes AS n JOIN NoteText ON (n.rowid = NoteText.rowid)"
			"        WHERE  n.isDeleted = 0 AND n.notebook = ? AND NoteText MATCH ?"
			"        UNION"
			"        SELECT n.guid"
			"        FROM   Notes    AS n"
			"        JOIN   NoteTags AS nt ON (n.guid = nt.note)"
			"        JOIN   Tags     AS t  ON (t.guid = nt.tag)"
			"        WHERE  n.isDeleted = 0 AND n.notebook = ? AND t.searchName = ?"
			"        UNION"
			"        SELECT n.guid"
			"        FROM   Notes       AS n"
			"        JOIN   Resources   AS rs ON (n.guid = rs.note)"
			"        JOIN   Recognition AS rc ON (rs.guid = rc.resource)"
			"        WHERE  n.isDeleted = 0 AND n.notebook = ? AND rc.text = ?"
			"      ) JOIN Notes USING (guid)"
			" ORDER BY modificationDate DESC, creationDate DESC"
			);
		statement->Bind(1, notebook);
		statement->Bind(2, ucSearch);
		statement->Bind(3, notebook);
		statement->Bind(4, ucSearch);
		statement->Bind(5, notebook);
		statement->Bind(6, ucSearch);
	}
	while (!statement->Execute())
	{
		notes.push_back(Note());
		Note & n(notes.back());
		statement->Get(0,  n.guid);
		statement->Get(1,  n.usn);
		statement->Get(2,  n.name);
		statement->Get(3,  n.isDirty);
		statement->Get(4,  n.creationDate);
		statement->Get(5,  n.modificationDate);
		statement->Get(6,  n.subjectDate);
		statement->Get(7,  n.Location.Altitude);
		statement->Get(8,  n.Location.Latitude);
		statement->Get(9,  n.Location.Longitude);
		statement->Get(10, n.Author);
		statement->Get(11, n.Source);
		statement->Get(12, n.SourceUrl);
		statement->Get(13, n.SourceApplication);
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
	statement->Bind(1, hash);
	if (statement->Execute())
		throw std::exception("Image resource not found.");
	__int64 row(0);
	statement->Get(0, row);

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
	statement->Bind(1, guid);
	if (statement->Execute())
		throw std::exception("Resource not found.");

	resource.Guid = guid;

	__int64 row(0);
	statement->Get(0,  row);
	statement->Get(1,  resource.Hash);
	statement->Get(2,  resource.Mime);
	statement->Get(3,  resource.Dimensions.Width);
	statement->Get(4,  resource.Dimensions.Height);
	statement->Get(5,  resource.SourceUrl);
	statement->Get(6,  resource.Timestamp);
	statement->Get(7,  resource.Location.Altitude);
	statement->Get(8,  resource.Location.Latitude);
	statement->Get(9,  resource.Location.Longitude);
	statement->Get(10, resource.FileName);
	statement->Get(11, resource.IsAttachment);
	statement->Get(12, resource.Note);

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
		statement->Get(0, guid);
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
		statement->Get(0, t.guid);
		statement->Get(1, t.usn);
		statement->Get(2, t.name);
		statement->Get(3, t.parentGuid);
		statement->Get(4, t.isDirty);
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
	setNew->Bind(1, notebook);
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

	IDataStore::Statement setNew = dataStore.MakeStatement
		( "UPDATE Notebooks"
		"  SET isLastUsed = 1"
		"  WHERE guid = ?"
		);
	setNew->Bind(1, notebook);
	setNew->Execute();
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
	, const Notebook      & notebook
	)
{
	Transaction transaction(*this);
	{
		IDataStore::Statement insertNote = dataStore.MakeStatement
			("INSERT OR REPLACE INTO Notes(`guid`, `usn`, `title`, `isDirty`, `body`,"
			"                              `creationDate`, `modificationDate`, `subjectDate`,"
			"                              `altitude`, `latitude`, `longitude`, `author`,"
			"                              `source`, `sourceUrl`, `sourceApplication`, `notebook`)"
			"  VALUES (?, ?, ?, ?,  ?, ?, ?, ?,  ?, ?, ?, ?,  ?, ?, ?, ?)"
			);
		insertNote->Bind(1, note.guid);
		insertNote->Bind(2, note.usn);
		insertNote->Bind(3, note.name);
		insertNote->Bind(4, note.isDirty);
		insertNote->Bind(5, body);
		insertNote->Bind(6, note.creationDate.GetTime());
		insertNote->Bind(7, note.modificationDate.GetTime());
		insertNote->Bind(8, note.subjectDate.GetTime());
		if (note.Location.IsValid)
		{
			insertNote->Bind(9,  note.Location.Altitude);
			insertNote->Bind(10, note.Location.Latitude);
			insertNote->Bind(11, note.Location.Longitude);
		}
		else
		{
			insertNote->BindNull(9);
			insertNote->BindNull(10);
			insertNote->BindNull(11);
		}
		insertNote->Bind(12, note.Author);
		insertNote->Bind(13, note.Source);
		insertNote->Bind(14, note.SourceUrl);
		insertNote->Bind(15, note.SourceApplication);
		insertNote->Bind(16, notebook.guid);
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
		insertText->Bind(1, rowid);
		insertText->Bind(2, ucName);
		insertText->Bind(3, ucBody);
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
	IDataStore::Statement statement = dataStore.MakeStatement
		( "UPDATE Notebooks"
		"  SET    updateCount = ?"
		"  WHERE  guid = ?"
		);
	statement->Bind(1, updateCount);
	statement->Bind(2, notebook);
	statement->Execute();
}

void UserModel::SetNoteThumbnail(const Guid & guid, const Thumbnail & thumbnail)
{
	IDataStore::Statement statement = dataStore.MakeStatement
		( "UPDATE Notes"
		"  SET    thumbnail = ?, thumbnailWidth = ?, thumbnailHeight = ?"
		"  WHERE  guid = ?"
		);
	statement->Bind(1, thumbnail.Data);
	statement->Bind(2, thumbnail.Width);
	statement->Bind(3, thumbnail.Height);
	statement->Bind(4, guid);
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
	statement->Bind(1, replacement.guid);
	statement->Bind(2, replacement.usn);
	statement->Bind(3, replacement.name);
	statement->Bind(4, replacement.isDirty);
	statement->Bind(5, replacement.creationDate.GetTime());
	statement->Bind(6, replacement.modificationDate.GetTime());
	statement->Bind(7, replacement.subjectDate.GetTime());
	if (replacement.Location.IsValid)
	{
		statement->Bind(8,  replacement.Location.Altitude);
		statement->Bind(9,  replacement.Location.Latitude);
		statement->Bind(10, replacement.Location.Longitude);
	}
	else
	{
		statement->BindNull(8);
		statement->BindNull(9);
		statement->BindNull(10);
	}
	statement->Bind(11, replacement.Author);
	statement->Bind(12, replacement.Source);
	statement->Bind(13, replacement.SourceUrl);
	statement->Bind(14, replacement.SourceApplication);
	statement->Bind(15, note);
	statement->Execute();
}

void UserModel::UpdateNotebook
	( const Guid     & notebook
	, const Notebook & replacement
	)
{
	IDataStore::Statement statement = dataStore.MakeStatement
		( "UPDATE Notebooks"
		"  SET guid = ?, usn = ?, name = ?, creationDate = ?,"
		"      modificationDate = ?, isDirty = ?"
		"  WHERE guid = ?"
		);
	statement->Bind(1, replacement.guid);
	statement->Bind(2, replacement.usn);
	statement->Bind(3, replacement.name);
	statement->Bind(4, replacement.CreationDate.GetTime());
	statement->Bind(5, replacement.ModificationDate.GetTime());
	statement->Bind(6, replacement.isDirty);
	statement->Bind(7, notebook);
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
	statement->Bind(1, replacement.Hash);
	statement->Bind(2, replacement.Data);
	statement->Bind(3, replacement.Mime);
	if (replacement.Dimensions.IsValid)
	{
		statement->Bind(4, replacement.Dimensions.Width);
		statement->Bind(5, replacement.Dimensions.Height);
	}
	else
	{
		statement->BindNull(4);
		statement->BindNull(5);
	}
	statement->Bind(6, replacement.SourceUrl);
	statement->Bind(7, replacement.Timestamp.GetTime());
	if (replacement.Location.IsValid)
	{
		statement->Bind(8,  replacement.Location.Altitude);
		statement->Bind(9, replacement.Location.Latitude);
		statement->Bind(10, replacement.Location.Longitude);
	}
	else
	{
		statement->BindNull(8);
		statement->BindNull(9);
		statement->BindNull(10);
	}
	statement->Bind(11, replacement.FileName);
	statement->Bind(12, replacement.IsAttachment);
	statement->Bind(13, resource);
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
	statement->Bind(1, replacement.guid);
	statement->Bind(2, replacement.usn);
	statement->Bind(3, replacement.name);
	statement->Bind(4, replacement.parentGuid);
	statement->Bind(5, replacement.isDirty);
	statement->Bind(6, tag);
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

void UserModel::GetFirstNotebook(Notebook & notebook)
{
	Transaction transaction(*this);

	IDataStore::Statement statement = dataStore.MakeStatement
		( "SELECT   guid, usn, name, creationDate, modificationDate, isDirty"
		"  FROM     Notebooks"
		"  ORDER BY name"
		"  LIMIT    1"
		);
	if (statement->Execute())
	{
		notebook.guid    = Guid();
		notebook.name    = L"Notes";
		notebook.isDirty = true;
		notebook.usn     = GetUpdateCount();
		AddNotebook(notebook);
		MakeNotebookDefault(notebook.guid);
		MakeNotebookLastUsed(notebook.guid);
	}
	statement->Get(0, notebook.guid);
	statement->Get(1, notebook.usn);
	statement->Get(2, notebook.name);
	statement->Get(3, notebook.CreationDate);
	statement->Get(4, notebook.ModificationDate);
	statement->Get(5, notebook.isDirty);
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
