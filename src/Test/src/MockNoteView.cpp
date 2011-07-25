#include "stdafx.h"
#include "MockNoteView.h"

#include <algorithm>

using namespace std;

MockNoteView::MockNoteView()
	: isDirty     (false)
	, isMaximized (false)
	, isShown     (false)
{
}

void MockNoteView::ConnectClose(slot_type OnClose)
{
	SignalClose.connect(OnClose);
}

void MockNoteView::ConnectEdit(slot_type OnEdit)
{
	SignalEdit.connect(OnEdit);
}

void MockNoteView::ConnectToggleMaximize(slot_type OnToggleMaximize)
{
	SignalToggleMaximize.connect(OnToggleMaximize);
}

void MockNoteView::GetBody(wstring & html)
{
	html = body;
}

void MockNoteView::GetNote(Note & note)
{
	note = this->note;
}

void MockNoteView::GetTitle(std::wstring & text)
{
	text = title;
}

void MockNoteView::Hide()
{
	isShown = false;
}

bool MockNoteView::IsDirty()
{
	return isDirty;
}

bool MockNoteView::IsMaximized()
{
	return isMaximized;
}

void MockNoteView::MaximizeWindow()
{
	isMaximized = true;
}

void MockNoteView::Render(Thumbnail & thumbnail)
{
	renderSize.cx = thumbnail.Width;
	renderSize.cy = thumbnail.Height;
}

void MockNoteView::RestoreWindow()
{
	isMaximized = false;
}

void MockNoteView::SetNote
	( const Note    & note
	, const wstring & titleText
	, const wstring & subtitleText
	, const wstring & bodyHtml
	, const wstring & attachment
	)
{
	this->note       = note;
	this->body       = bodyHtml;
	this->subtitle   = subtitleText;
	this->title      = titleText;
	this->attachment = attachment;
}

void MockNoteView::SetWindowTitle(const wstring & text)
{
	windowTitle = text;
}

void MockNoteView::Show()
{
	isShown = true;
}

