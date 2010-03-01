#pragma once

class ITag
{
public:

	virtual ~ITag() {}

	virtual std::wstring GetName() const = 0;
};
