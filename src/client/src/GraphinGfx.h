#pragma once

struct graphin_graphics;
struct graphin_image;

typedef graphin_graphics* HGFX;
typedef graphin_image*    HIMG;

class GraphinGfx
{
private:

	HGFX gfx;
	HIMG img;

public:

	GraphinGfx(int width, int height);

	~GraphinGfx();

	void BitBlt
		( HDC dcDst
		, int xDst
		, int yDst
		, int xSrc
		, int ySrc
		, int width
		, int height
		);

	void DrawLine(int x1, int y1, int x2, int y2);

	void DrawRectangle(int x, int y, int width, int height);

	void SetFillColor(COLORREF color);

	void SetLineColor(COLORREF color);

	void SetLineWidth(int width);
};
