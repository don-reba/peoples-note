#pragma once

#include "INoteView.h"

class MockNoteView : public INoteView
{
public:

	std::wstring body;
	std::wstring title;
	bool         isShown;

public:

	virtual void Hide();

	virtual void SetBody(std::wstring html);

	virtual void SetTitle(std::wstring text);

	virtual void Show();
};
