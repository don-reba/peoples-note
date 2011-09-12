#pragma once
#include "IFlashCard.h"

class FlashCard : public IFlashCard
{
public:

	virtual bool Exists() const;

	virtual std::wstring GetPath() const;

private:

	static std::wstring GetName();

	static bool IsValidData(const WIN32_FIND_DATA & data);
};
