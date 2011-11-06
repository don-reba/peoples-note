#include "stdafx.h"
#include "Scrollbar.h"

#include "Rect.h"

#include "behaviors/behavior_aux.h"
#include "htmlayout.h"

using namespace htmlayout;
using namespace htmlayout::dom;

struct ScrollbarH : behavior
{
	ScrollbarH()
		: behavior
			( HANDLE_BEHAVIOR_EVENT
			, "horizontal-touch-scrollbar"
			)
	{
	}

	virtual BOOL on_event(HELEMENT he, HELEMENT target, BEHAVIOR_EVENTS type, UINT_PTR reason)
	{
		switch (type)
		{
		case TOUCH_SCROLL_POS:    HandlePos(target, he, reason); return TRUE;
		case TOUCH_SCROLL_UPDATE: HandleUpdate(target, he);      return TRUE;
		}
		return FALSE;
	}

private:

	static void HandlePos(element bar, element e, int pos)
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

	static void HandleUpdate(element bar, element e)
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

	static void UpdateWindow(element e)
	{
		HWND hwnd(0);
		if (HLDOM_OK == ::HTMLayoutGetElementHwnd(e, &hwnd, TRUE))
			::HTMLayoutUpdateWindow(hwnd);
	}
};

ScrollbarH scrollbarH;
