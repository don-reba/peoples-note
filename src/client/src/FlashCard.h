#pragma once
#include "IFlashCard.h"

class FlashCard : public IFlashCard
{
public:

	virtual bool GetPath(std::wstring & path) const;
};
