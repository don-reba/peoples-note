#pragma once
#include "IFlashCard.h"

class FlashCard : public IFlashCard
{
public:

	virtual bool Exists() const;

	virtual bool GetPath(std::wstring & path) const;
};
