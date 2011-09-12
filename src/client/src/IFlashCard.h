#pragma once

class IFlashCard
{
public:

	virtual bool Exists() const = 0;

	virtual std::wstring GetPath() const = 0;
};
