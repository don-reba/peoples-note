#pragma once

class Tag
{
private:

	std::wstring name;

public:

	Tag::Tag(std::wstring name);

	std::wstring GetName() const;
};

typedef std::vector<Tag> TagList;
