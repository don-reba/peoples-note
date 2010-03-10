#pragma once

#include "HTMLayoutWindow.h"
#include "INoteView.h"

class NoteView : public HTMLayoutWindow, public INoteView
{
private:

	HINSTANCE instance;

// interface

public:

	NoteView(HINSTANCE instance);

	void Create(HWND parent);

	void Hide();

	void Show();

// INoteView implementation

public:

	virtual void SetBody(std::wstring html);

	virtual void SetTitle(std::wstring text);
};
