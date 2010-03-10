#pragma once

class INoteView
{
public:

	virtual void SetBody(std::wstring html) = 0;

	virtual void SetTitle(std::wstring text) = 0;
};
