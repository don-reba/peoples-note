#pragma once
#include "ISignalProvider.h"

class IAudioPlayerView : public ISignalProvider
{
	MacroIEvent(Cancel)
	MacroIEvent(Hide)
	MacroIEvent(Play)
	MacroIEvent(Show)
	MacroIEvent(Stop)

public:

	virtual void Hide() = 0;

	virtual void Show() = 0;
};
