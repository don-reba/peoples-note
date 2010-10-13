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
	ReadFile(fileName, fileContents);
	if (fileContents.empty())
		return;

	Transaction transaction(userModel);

	Notebook notebook;
	userModel.GetLastUsedNotebook(notebook);

	NoteList     notes;
	NoteBodyList bodies;
	ResourceList resources;

	enImporter.ImportNotes(fileContents, notes, bodies, resources);
	assert(notes.size() == bodies.size());

	for (int i(0); i != notes.size(); ++i)
	{
		const Note    & note = notes.at(i);
		const wstring & body = bodies.at(i);

		wstring bodyText;
		enNoteTranslator.ConvertToText(body, bodyText);

		userModel.AddNote(note, body, bodyText, notebook);
	}

	foreach (const Resource & resource, resources)
		userModel.AddResource(resource);

	noteListModel.Reload();
}

void EnImportPresenter::ReadFile
	( const wchar_t * fileName
	, std::wstring  & contents
	)
{
	basic_ifstream<unsigned char> stream(fileName, ios_base::in | ios_base::binary);
	vector<unsigned char> text
		( (istreambuf_iterator<unsigned char>(stream))
		, istreambuf_iterator<unsigned char>()
		);
	text.reserve(text.size() + 1);
	text.push_back(L'\0');
	contents = ConvertToUnicode(&text[0]);
}
