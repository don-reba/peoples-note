#include "stdafx.h"
#include "MockNoteListModel.h"

using namespace boost;
using namespace std;

MockNoteListModel::MockNoteListModel()
	: hasNextPage          (false)
	, hasPreviousPage      (false)
	, isReloaded           (false)
	, notebookTitleState   (false)
	, nextPageSelected     (false)
	, previousPageSelected (false)
{
}

void MockNoteListModel::ConnectChanged(slot_type OnReset)
{
	SignalChanged.connect(OnReset);
}


void MockNoteListModel::GetCurrentPage
	( NoteList & notes
	, bool     & hasPreviousPage
	, bool     & hasNextPage
	)
{
	notes.assign(this->notes.begin(), this->notes.end());
	hasPreviousPage = this->hasPreviousPage;
	hasNextPage     = this->hasNextPage;
}

bool MockNoteListModel::GetNotebookTitleState()
{
	return notebookTitleState;
}

NotebookViewStyle MockNoteListModel::GetViewStyle()
{
	return viewStyle;
}

void MockNoteListModel::Reload()
{
	isReloaded = true;
}

void MockNoteListModel::SelectNextPage()
{
	nextPageSelected = true;
}

void MockNoteListModel::SelectPreviousPage()
{
	previousPageSelected = true;
}

void MockNoteListModel::SetNotebookTitleState(bool isEnabled)
{
	notebookTitleState = isEnabled;
}

void MockNoteListModel::SetQuery(const wstring & query)
{
	this->query = query;
}

void MockNoteListModel::SetViewStyle(NotebookViewStyle style)
{
	this->viewStyle = style;
}
