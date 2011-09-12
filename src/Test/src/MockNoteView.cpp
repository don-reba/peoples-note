#include "stdafx.h"
#include "MockNoteView.h"

#include <algorithm>

using namespace std;

MockNoteView::MockNoteView()
	: enableChrome (false)
	, isDirty      (false)
	, isMaximized  (false)
	, isShown      (false)
{
}

void MockNoteView::GetBody(wstring & html)
{
	html = body;
}

void MockNoteView::GetNote(Note & note)
{
	note = this->note;
}

wstring MockNoteView::GetSavePath
	( const wstring & title
	, const wstring & fileName
	, const wstring & directory
	)
{
	savePathTitle     = title;
	savePathFileName  = fileName;
	savePathDirectory = directory;
	return savePath;
}

Guid MockNoteView::GetSelecteAttachmentGuid()
{
	return selectedAttachment;
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
	( const Note                   & note
	, const wstring                & titleText
	, const wstring                & subtitleText
	, const wstring                & bodyHtml
	, const AttachmentViewInfoList & attachments
	, const bool                     enableChrome
	)
{
	this->note         = note;
	this->body         = bodyHtml;
	this->subtitle     = subtitleText;
	this->title        = titleText;
	this->attachments  = attachments;
	this->enableChrome = enableChrome;
}

void MockNoteView::SetWindowTitle(const wstring & text)
{
	windowTitle = text;
}

void MockNoteView::Show()
{
	isShown = true;
}

