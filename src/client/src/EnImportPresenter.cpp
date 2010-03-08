#include "stdafx.h"
#include "EnImportPresenter.h"

#include "IEnIMporter.h"
#include "INoteListModel.h"
#include "INoteListView.h"
#include "IUserModel.h"

#include <fstream>

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

	NoteList notes;
	enImporter.ImportNotes(file, notes);

	const Notebook & notebook = userModel.GetLastUsedNotebook();
	foreach (const Note & note, notes)
		userModel.AddNote(note, notebook);

	noteListModel.SetNotes(userModel.GetNotesByNotebook(notebook));
}
