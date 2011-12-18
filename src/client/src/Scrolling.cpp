#include "stdafx.h"

#include "Rect.h"

using namespace htmlayout::dom;
using namespace htmlayout;

void UpdateWindow(element e)
{
	HWND hwnd(0);
	if (HLDOM_OK == ::HTMLayoutGetElementHwnd(e, &hwnd, TRUE))
		::HTMLayoutUpdateWindow(hwnd);
}

void ScrollHorizontally(element e, element bar, int pos)
{
	POINT scrollPos;
	Rect  viewRect;
	SIZE  contentSize;
	e.get_scroll_info(scrollPos, viewRect, contentSize);

	Rect eRect(e.get_location(SCROLLABLE_AREA));
	if (eRect.GetWidth() == 0 || eRect.GetHeight() == 0)
		return;

	int contentWidth(contentSize.cx - eRect.GetWidth());

	scrollPos.x = max(min(pos, contentWidth), 0);
	e.set_scroll_pos(scrollPos, false);
	scrollPos.y = 0;

	if (contentWidth > 0)
	{
		element slider(bar.find_first(".slider"));

		Rect scrollRect(bar.get_location(ROOT_RELATIVE|CONTENT_BOX));
		Rect sliderRect(slider.get_location(CONTAINER_RELATIVE|BORDER_BOX));

		__int64 scrollDistance(scrollRect.GetWidth() - sliderRect.GetWidth());
		if (scrollDistance > 0L)
			scrollPos.x =-static_cast<int>(scrollPos.x * scrollDistance / contentWidth);
	}

	bar.set_scroll_pos(scrollPos, false);
}

void ScrollVertically(element e, element bar, int pos)
{
	POINT scrollPos;
	Rect  viewRect;
	SIZE  contentSize;
	e.get_scroll_info(scrollPos, viewRect, contentSize);

	Rect eRect(e.get_location(SCROLLABLE_AREA));
	if (eRect.GetWidth() == 0 || eRect.GetHeight() == 0)
		return;

	int contentHeight(contentSize.cy - eRect.GetHeight());

	scrollPos.y = max(min(pos, contentHeight), 0);
	e.set_scroll_pos(scrollPos, false);
	scrollPos.x = 0;

	if (contentHeight > 0)
	{
		element slider(bar.find_first(".slider"));

		Rect scrollRect(bar.get_location(ROOT_RELATIVE|CONTENT_BOX));
		Rect sliderRect(slider.get_location(CONTAINER_RELATIVE|BORDER_BOX));

		__int64 scrollDistance(scrollRect.GetHeight() - sliderRect.GetHeight());
		if (scrollDistance > 0L)
			scrollPos.y = static_cast<LONG>(-scrollPos.y * scrollDistance / contentHeight);
	}

	bar.set_scroll_pos(scrollPos, false);
}

void UpdateHorizontalScrollbar(element e, element bar)
{
	UpdateWindow(e);

	POINT scrollPos;
	RECT  viewRect;
	SIZE  contentSize;
	e.get_scroll_info(scrollPos, viewRect, contentSize);
	if (contentSize.cx <= 0)
		return;

	RECT listRect(e.get_location(SCROLLABLE_AREA));
	__int64 scrollableWidth(listRect.right - listRect.left);
	if (scrollableWidth <= 0L)
		return;

	element slider(bar.find_first(".slider"));

	if (contentSize.cx <= scrollableWidth)
	{
		slider.set_style_attribute("display", L"none");
		return;
	}
	else
	{
		slider.set_style_attribute("display", L"block");
	}

	RECT scrollRect(bar.get_location(ROOT_RELATIVE|CONTENT_BOX));
	__int64 scrollWidth(scrollRect.right - scrollRect.left);
	if (scrollWidth <= 0L)
		return;

	__int64 sliderWidth(scrollWidth * scrollableWidth / contentSize.cx);

	wchar_t text[16];
	_itow_s(static_cast<int>(sliderWidth), text, 16, 10);
	slider.set_style_attribute("width", text);
}

void UpdateVerticalScrollbar   (element e, element bar)
{
	UpdateWindow(e);

	POINT scrollPos;
	RECT  viewRect;
	SIZE  contentSize;
	e.get_scroll_info(scrollPos, viewRect, contentSize);
	if (contentSize.cy <= 0)
		return;

	RECT listRect(e.get_location(SCROLLABLE_AREA));
	__int64 scrollableHeight(listRect.bottom - listRect.top);
	if (scrollableHeight <= 0L)
		return;

	element slider(bar.find_first(".slider"));

	if (contentSize.cy <= scrollableHeight)
	{
		slider.set_style_attribute("display", L"none");
		return;
	}
	else
	{
		slider.set_style_attribute("display", L"block");
	}

	RECT scrollRect(bar.get_location(ROOT_RELATIVE|CONTENT_BOX));
	__int64 scrollHeight(scrollRect.bottom - scrollRect.top);
	if (scrollHeight <= 0L)
		return;

	__int64 sliderHeight(scrollHeight * scrollableHeight / contentSize.cy);

	wchar_t text[16];
	_itow_s(static_cast<int>(sliderHeight), text, 16, 10);
	slider.set_style_attribute("height", text);
}
