#pragma once

class IFlashCard
{
public:

	virtual bool GetPath(std::wstring & path) const = 0;
};
