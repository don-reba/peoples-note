#pragma once

#include "ITag.h"

class MockTag : public ITag
{
public:

	std::wstring name;

public:

	virtual std::wstring GetName() const;
};
