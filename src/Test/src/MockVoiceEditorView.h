#pragma once
#include "IVoiceEditorView.h"

class MockVoiceEditorView : public IVoiceEditorView
{
	MacroTestEvent(Cancel)
	MacroTestEvent(Play)
	MacroTestEvent(Record)
	MacroTestEvent(Stop)

public:

	bool isShown;

public:

	MockVoiceEditorView();

public:

	virtual void Hide();

	virtual void Show();
};
