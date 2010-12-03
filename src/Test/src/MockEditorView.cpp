#include "stdafx.h"
#include "MockEditorView.h"

using namespace std;

MockEditorView::MockEditorView()
	: isShown (false)
{
}

void MockEditorView::ConnectAccept(slot_type OnAccept)
{
	SignalAccept.connect(OnAccept);
}

void MockEditorView::ConnectCancel(slot_type OnCancel)
{
	SignalCancel.connect(OnCancel);
}

void MockEditorView::ConnectDeleteNote(slot_type OnDeleteNote)
{
	SignalDeleteNote.connect(OnDeleteNote);
}

void MockEditorView::GetBody(wstring & html)
{
	html = body;
}

void MockEditorView::GetNote(Note & note)
{
	note = this->note;
}

void MockEditorView::GetTitle(std::wstring & text)
{
	text = note.name;
}

void MockEditorView::Hide()
{
	isShown = false;
}

void MockEditorView::SetNote
	( const Note         & note
	, const std::wstring & bodyHtml
	)
{
	this->note = note;
	this->body = bodyHtml;
}

void MockEditorView::Show()
{
	isShown = true;
}
