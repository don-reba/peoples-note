#pragma once

#include "INoteView.h"

#include "Note.h"

class MockNoteView : public INoteView
{
public:

	std::wstring attachment;
	std::wstring body;
	std::wstring subtitle;
	std::wstring title;
	std::wstring windowTitle;
	bool         enableChrome;
	bool         isDirty;
	bool         isMaximized;
	bool         isShown;

	Note note;

	SIZE renderSize;

	signal SignalClose;
	signal SignalEdit;
	signal SignalToggleMaximize;

public:

	MockNoteView();

	virtual void ConnectClose(slot_type OnClose);

	virtual void ConnectEdit(slot_type OnEdit);

	virtual void ConnectToggleMaximize(slot_type OnToggleMaximize);

	virtual void GetBody(std::wstring & html);

	virtual void GetNote(Note & note);

	virtual void GetTitle(std::wstring & text);

	virtual void Hide();

	virtual bool IsDirty();

	virtual bool IsMaximized();

	virtual void MaximizeWindow();

	virtual void Render(Thumbnail & thumbnail);

	virtual void RestoreWindow();

	virtual void SetNote
		( const Note         & note
		, const std::wstring & titleText
		, const std::wstring & subtitleText
		, const std::wstring & bodyHtml
		, const std::wstring & attachment
		, const bool           enableChrome
		);

	virtual void SetWindowTitle(const std::wstring & text);

	virtual void Show();
};
