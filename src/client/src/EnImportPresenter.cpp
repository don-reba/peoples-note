#include "stdafx.h"
#include "EnImportPresenter.h"

#include "IEnIMporter.h"
#include "INoteListModel.h"
#include "INoteListView.h"
#include "IUserModel.h"

#include "MD5/md5.h"

#include <fstream>
#include <sstream>

using namespace boost;
using namespace std;

EnImportPresenter::EnImportPresenter
	( IEnImporter    & enImporter
	, INoteListModel & noteListModel
	, INoteListView  & noteListView
	, IUserModel     & userModel
	)
	: enImporter    (enImporter)
	, noteListModel (noteListModel)
	, noteListView  (noteListView)
	, userModel     (userModel)
{
	noteListView.ConnectImport(bind(&EnImportPresenter::OnImport, *this));
}

void EnImportPresenter::OnImport()
{
	wstring path;
	if (!noteListView.GetEnexPath(path))
		return;
	
	wifstream file;
	file.open(path.c_str());
	if (!file.is_open())
		throw std::exception("File could not be opened.");

	NoteList               notes;
	NoteBodyList           bodies;
	IEnImporter::ImageList images;
	enImporter.ImportNotes(file, notes, bodies, images);
	assert(notes.size() == bodies.size());

	const Notebook & notebook = userModel.GetLastUsedNotebook();
	for (int i(0); i != notes.size(); ++i)
	{
		const Note    & note     = notes.at(i);
		const wstring & body     = bodies.at(i);
		wstring         bodyText = L""; // TODO: produce body text
		userModel.AddNote(note, body, bodyText, notebook);
	}

	foreach (const IEnImporter::Image & image, images)
	{
		userModel.AddImageResource
			( Hash(image.blob)
			, image.blob
			, image.noteGuid
			);
	}

	noteListModel.SetNotes(userModel.GetNotesByNotebook(notebook));
}

string EnImportPresenter::Hash(const Blob & blob)
{
	md5_state_t state;
	md5_byte_t  digest[16];

	md5_init(&state);
	md5_append(&state, &blob[0], blob.size());
	md5_finish(&state, digest);

	char hex[16 * 2 + 1];
	for (int i(0); i != 16; ++i)
		sprintf(hex + 2 * i, "%02x", digest[i]);

	return hex;
}
