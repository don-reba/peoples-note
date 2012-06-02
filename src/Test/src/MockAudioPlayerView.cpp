#include "stdafx.h"
#include "MockAudioPlayerView.h"

using namespace std;

void MockAudioPlayerView::Hide()
{
	isShown = false;
}

void MockAudioPlayerView::SetFileName(wstring & name)
{
	this->name = name;
}

void MockAudioPlayerView::Show()
{
	isShown = true;
}
