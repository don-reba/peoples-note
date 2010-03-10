#pragma once

#include "INoteView.h"

class MockNoteView : public INoteView
{
public:

	std::wstring body;

public:

	virtual void SetBody(std::wstring text);
};
