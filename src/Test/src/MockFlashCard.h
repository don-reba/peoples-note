#pragma once
#include "IFlashCard.h"

class MockFlashCard : public IFlashCard
{
public:

	std::wstring path;

public:

	virtual bool Exists() const;

	virtual std::wstring GetPath() const;
};
