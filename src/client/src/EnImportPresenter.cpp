#include "stdafx.h"
#include "EnImportPresenter.h"

#include "EnNoteTranslator.h"
#include "IEnImporter.h"
#include "INoteListModel.h"
#include "INoteListView.h"
#include "IUserModel.h"
#include "Tools.h"
#include "Transaction.h"

#include <fstream>
#include <iterator>

using namespace boost;
using namespace std;
using namespace Tools;

EnImportPresenter::EnImportPresenter
	( EnNoteTranslator & enNoteTranslator
	, IEnImporter      & enImporter
	, INoteListModel   & noteListModel
	, INoteListView    & noteListView
	, IUserModel       & userModel
	)
	: enImporter       (enImporter)
	, enNoteTranslator (enNoteTranslator)
	, noteListModel    (noteListModel)
	, noteListView     (noteListView)
	, userModel        (userModel)
{
	noteListView.ConnectImport(bind(&EnImportPresenter::OnImport, *this));
}

void EnImportPresenter::OnImport()
{
	wstring path;
	if (!noteListView.GetEnexPath(path))
		return;
	ImportNotes(path.c_str());
}

void EnImportPresenter::ImportNotes(const wchar_t * fileName)
{
	wstring fileContents;
	ReadUtf8File(fileName, fileContents);
	if (fileContents.empty())
		return;

	Transaction transaction(userModel);

	Guid targetNotebook;
	userModel.GetLastUsedNotebookOrDefault(targetNotebook);

	NoteList             notes;
	NoteBodyList         bodies;
	ResourceList         resources;
	RecognitionEntryList recognitionEntries;

	enImporter.ImportNotes
		( fileContents
		, notes
		, bodies
		, resources
		, recognitionEntries
		);
	assert(notes.size() == bodies.size());

	for (int i(0); i != notes.size(); ++i)
	{
		const Note    & note = notes.at(i);
		const wstring & body = bodies.at(i);

		wstring bodyText;
		enNoteTranslator.ConvertToText(body, bodyText);

		userModel.AddNote(note, body, bodyText, targetNotebook);
	}

	foreach (const Resource & resource, resources)
		userModel.AddResource(resource);

	foreach (const RecognitionEntry & entry, recognitionEntries)
		userModel.AddRecognitionEntry(entry);

	noteListModel.Reload();
}
