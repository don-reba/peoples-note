#pragma once

class INote;

class INotebook
{
public:

	virtual ~INotebook() {}

	virtual std::wstring GetName() const = 0;
};
