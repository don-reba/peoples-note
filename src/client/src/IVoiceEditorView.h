#pragma once
#include "ISignalProvider.h"

class IVoiceEditorView : public ISignalProvider
{
	MacroIEvent(Cancel)

public:

	virtual void Hide() = 0;

	virtual void Show() = 0;
};