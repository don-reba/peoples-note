#pragma once
#include "IEditorView.h"

#include "Note.h"

class MockEditorView : public IEditorView
{
public:

	Note         note;
	std::wstring body;

	bool isShown;

	signal SignalAccept;
	signal SignalCancel;

public:

	MockEditorView();

public:

	virtual void ConnectAccept(slot_type OnAccept);

	virtual void ConnectCancel(slot_type OnCancel);

	virtual void GetBody(std::wstring & html);

	virtual void GetNote(Note & note);

	virtual void GetTitle(std::wstring & text);

	virtual void Hide();

	virtual void SetNote
		( const Note         & note
		, const std::wstring & bodyHtml
		);

	virtual void Show();
};