#include "stdafx.h"
#include "GraphinGfx.h"

#include "htmlayout_graphin.h"

using namespace htmlayout;

GraphinGfx::GraphinGfx(int width, int height)
	: gfx (NULL)
	, img (NULL)
{
	gapi()->imageCreate(width, height, &img);
	gapi()->gCreate(img, &gfx);
}

GraphinGfx::~GraphinGfx()
{
	if (img)
		gapi()->imageRelease(img);
	if (gfx)
		gapi()->gRelease(gfx);
}

void GraphinGfx::BitBlt
	( HDC dcDst
	, int xDst
	, int yDst
	, int xSrc
	, int ySrc
	, int width
	, int height
	)
{
	gapi()->imageBlit
		(dcDst, xDst, yDst, img, xSrc, ySrc, width, height, false);
}

void GraphinGfx::DrawLine(int x1, int y1, int x2, int y2)
{
	gapi()->gLine
		( gfx
		, static_cast<POS>(x1)
		, static_cast<POS>(y1)
		, static_cast<POS>(x2)
		, static_cast<POS>(y2)
		);
}

void GraphinGfx::DrawRectangle(int x, int y, int width, int height)
{
	gapi()->gRectangle
		( gfx
		, static_cast<POS>(x)
		, static_cast<POS>(y)
		, static_cast<POS>(width)
		, static_cast<POS>(height)
		);
}

void GraphinGfx::SetFillColor(COLORREF color)
{
	gapi()->gFillColor(gfx, color);
}

void GraphinGfx::SetLineColor(COLORREF color)
{
	gapi()->gLineColor(gfx, color);
}

void GraphinGfx::SetLineWidth(int width)
{
	gapi()->gLineWidth(gfx, static_cast<DIM>(width));
}
