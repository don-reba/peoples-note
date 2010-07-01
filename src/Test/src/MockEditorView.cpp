#include "stdafx.h"
#include "MockEditorView.h"

using namespace std;

MockEditorView::MockEditorView()
	: isShown (false)
{
}

void MockEditorView::ConnectAccept(slot_type OnAccept)
{
	SignalAccept.connect(OnAccept);
}

void MockEditorView::ConnectCancel(slot_type OnCancel)
{
	SignalCancel.connect(OnCancel);
}

void MockEditorView::GetBody(wstring & html)
{
	html = body;
}

void MockEditorView::GetTitle(std::wstring & text)
{
	text = title;
}

void MockEditorView::Hide()
{
	isShown = false;
}

void MockEditorView::SetBody(const wstring & html)
{
	body = html;
}

void MockEditorView::SetTitle(const wstring & text)
{
	title = text;
}

void MockEditorView::Show()
{
	isShown = true;
}
