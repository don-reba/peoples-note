#pragma once

class INote;

class INotebook
{
public:

	virtual std::wstring GetName() const = 0;
};
