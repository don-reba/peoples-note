#include "stdafx.h"
#include "EnImportPresenter.h"

#include "IEnIMporter.h"
#include "INoteListModel.h"
#include "INoteListView.h"
#include "IUserModel.h"
#include "Tools.h"
#include "Transaction.h"

#include <fstream>
#include <sstream>

using namespace boost;
using namespace std;
using namespace Tools;

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

	NoteList notes;
	ImportNotes(file, notes);
	noteListModel.SetNotes(notes);
}

void EnImportPresenter::ImportNotes
	( wistream & file
	, NoteList & notes
	)
{
	Transaction transaction(userModel);
	Notebook notebook;
	userModel.GetLastUsedNotebook(notebook);
	{
		NoteList               notes;
		NoteBodyList           bodies;
		IEnImporter::ImageList images;
		enImporter.ImportNotes(file, notes, bodies, images);
		assert(notes.size() == bodies.size());

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
				( HashWithMD5(image.blob)
				, image.blob
				, image.noteGuid
				);
		}
	}
	userModel.GetNotesByNotebook(notebook, notes);
}
