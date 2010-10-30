#pragma once
#include "IFlashCard.h"

class MockFlashCard : public IFlashCard
{
public:

	std::wstring path;

public:

	bool Exists() const;

	bool GetPath(std::wstring & path) const;
};
