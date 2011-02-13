#pragma once

#include "ISignalProvider.h"

class IAboutView : public ISignalProvider
{
	MacroIEvent(Close);

public:

	virtual void Hide() = 0;

	virtual void Show() = 0;
};
