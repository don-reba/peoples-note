#pragma once

#include "INoteView.h"

class MockNoteView : public INoteView
{
public:

	std::wstring body;
	std::wstring title;

public:

	virtual void SetBody(std::wstring html);

	virtual void SetTitle(std::wstring text);
};
