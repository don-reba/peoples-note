#pragma once
#include "IVoiceEditorView.h"

class MockVoiceEditorView : public IVoiceEditorView
{
	MacroTestEvent(Cancel)

public:

	bool isShown;

public:

	MockVoiceEditorView();

public:

	virtual void Hide();

	virtual void Show();
};
