#pragma once

class IFlashCard
{
public:

	virtual bool Exists() const = 0;

	virtual bool GetPath(std::wstring & path) const = 0;
};
