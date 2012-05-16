#pragma once
#include "ISignalProvider.h"

class IVoiceEditorView : public ISignalProvider
{
	MacroIEvent(Cancel)
	MacroIEvent(Hide)
	MacroIEvent(Play)
	MacroIEvent(Show)
	MacroIEvent(Stop)
	MacroIEvent(Record)

public:

	enum ButtonId
	{
		NoButton     = 0,
		PlayButton   = 1,
		RecordButton = 2,
		StopButton   = 4,
	};

public:

	virtual void Hide() = 0;

	virtual void Show() = 0;

	virtual void SetButtons(int buttons) = 0;
};