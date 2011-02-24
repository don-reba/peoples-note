#pragma once
#include "ISignalProvider.h"

class IVoiceEditorView : public ISignalProvider
{
	MacroIEvent(Cancel)
	MacroIEvent(Play)
	MacroIEvent(Stop)
	MacroIEvent(Record)

public:

	virtual void Hide() = 0;

	virtual void Show() = 0;
};