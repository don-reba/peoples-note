#pragma once
#include "IFlashCard.h"

class MockFlashCard : public IFlashCard
{
public:

	std::wstring path;

public:

	bool GetPath(std::wstring & path) const;
};
