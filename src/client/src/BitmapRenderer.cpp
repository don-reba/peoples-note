#include "stdafx.h"
#include "BitmapRenderer.h"

#include "Tools.h"

#include "htmlayout.h"

using namespace htmlayout::dom;
using namespace Tools;

BitmapRenderer::BitmapRenderer
	( HINSTANCE instance
	)
	: instance      (instance)
	, event_handler (HANDLE_DRAW)
{
}

void BitmapRenderer::ConnectLoadBitmap
	( BitmapSlot OnLoadBitmap
	)
{
	SignalLoadBitmap.connect(OnLoadBitmap);
}

BOOL BitmapRenderer::handle_draw
	( HELEMENT      he
	, DRAW_PARAMS & params
	)
{
	const wchar_t * src(element(he).get_attribute("src"));
	if (!StartsWith(src, L"bmp:"))
		return FALSE;

	if (params.cmd != DRAW_CONTENT)
		return TRUE;

	HBITMAP bmp(NULL);
	SignalLoadBitmap(_wtoi(src + 4), bmp);
	if (!bmp)
		return FALSE;

	HDC dc(::CreateCompatibleDC(params.hdc));

	HGDIOBJ obj = ::SelectObject(dc, bmp);

	BITMAP info2;
	::GetObject(bmp, sizeof(info2), &info2);

	::BitBlt
		( params.hdc
		, params.area.left
		, params.area.top
		, info2.bmWidth
		, info2.bmHeight
		, dc
		, 0
		, 0
		, SRCCOPY
		);

	::DeleteDC(dc);

	return TRUE;
}
