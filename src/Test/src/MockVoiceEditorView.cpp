#include "stdafx.h"
#include "MockVoiceEditorView.h"

MockVoiceEditorView::MockVoiceEditorView()
	: isShown (false)
{
}

void MockVoiceEditorView::Hide()
{
	isShown = false;
}

void MockVoiceEditorView::Show()
{
	isShown = true;
}
