#include "stdafx.h"
#include "EnImportPresenter.h"

#include "IEnIMporter.h"
#include "INoteListView.h"
#include "IUserModel.h"

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

	ptr_vector<INote> notes;
	enImporter.ImportNotes(path, notes);

	const INotebook & notebook = userModel.GetLastUsedNotebook();
	foreach (const INote & note, notes)
		userModel.AddNote(note, notebook);
}
