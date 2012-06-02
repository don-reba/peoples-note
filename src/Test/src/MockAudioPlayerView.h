#pragma once
#include "IAudioPlayerView.h"

class MockAudioPlayerView : public IAudioPlayerView
{
	MacroTestEvent(Cancel)
	MacroTestEvent(Hide)
	MacroTestEvent(Play)
	MacroTestEvent(Show)
	MacroTestEvent(Stop)

public:

	bool         isShown;
	std::wstring name;

public:

	virtual void Hide();

	virtual void SetFileName(std::wstring & name);

	virtual void Show();
};
