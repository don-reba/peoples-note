#include "stdafx.h"
#include "Scrollbar.h"

#include "Rect.h"

#include "behaviors/behavior_aux.h"
#include "htmlayout.h"

using namespace htmlayout;
using namespace htmlayout::dom;

struct ScrollbarV : behavior
{
	ScrollbarV()
		: behavior
			( HANDLE_BEHAVIOR_EVENT
			, "vertical-touch-scrollbar"
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

	static void HandleUpdate(element bar, element e)
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

	static void UpdateWindow(element e)
	{
		HWND hwnd(0);
		if (HLDOM_OK == ::HTMLayoutGetElementHwnd(e, &hwnd, TRUE))
			::HTMLayoutUpdateWindow(hwnd);
	}
};

ScrollbarV scrollbarV;
