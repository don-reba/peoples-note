#pragma once

#include "ITag.h"

class MockTag : public ITag
{
public:

	std::wstring name;

public:

	MockTag(std::wstring name);

	virtual std::wstring GetName() const;
};
