#pragma once

#include "Guid.h"

class INote;

class INotebook
{
public:

	virtual ~INotebook() {}

	virtual std::wstring GetName() const = 0;

	virtual Guid GetGuid() const = 0;
};
