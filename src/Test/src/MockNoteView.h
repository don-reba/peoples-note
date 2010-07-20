#pragma once

#include "INoteView.h"

#include "Note.h"

class MockNoteView : public INoteView
{
public:

	std::wstring body;
	std::wstring subtitle;
	std::wstring title;
	std::wstring windowTitle;
	bool         isDirty;
	bool         isShown;

	Note note;

	SIZE renderSize;

	signal SignalClose;
	signal SignalEdit;

public:

	MockNoteView();

	virtual void ConnectClose(slot_type OnClose);

	virtual void ConnectEdit(slot_type OnEdit);

	virtual void GetBody(std::wstring & html);

	virtual void GetNote(Note & note);

	virtual void GetTitle(std::wstring & text);

	virtual void Hide();

	virtual bool IsDirty();

	virtual void Render(Thumbnail & thumbnail);

	virtual void SetNote
		( const Note         & note
		, const std::wstring & titleText
		, const std::wstring & subtitleText
		, const std::wstring & bodyHtml
		);

	virtual void SetWindowTitle(const std::wstring & text);

	virtual void Show();
};
