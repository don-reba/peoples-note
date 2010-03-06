#include "stdafx.h"
#include "EnImportPresenter.h"

#include "IEnIMporter.h"
#include "INoteListView.h"
#include "IUserModel.h"

#include <fstream>

using namespace boost;
using namespace std;

EnImportPresenter::EnImportPresenter
	( IEnImporter   & enImporter
	, INoteListView & noteListView
	, IUserModel    & userModel
	)
	: enImporter   (enImporter)
	, noteListView (noteListView)
	, userModel    (userModel)
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

	ptr_vector<INote> notes;
	enImporter.ImportNotes(file, notes);

	const INotebook & notebook = userModel.GetLastUsedNotebook();
	foreach (const INote & note, notes)
		userModel.AddNote(note, notebook);
}
