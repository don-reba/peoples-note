#pragma once

#include "Thumbnail.h"

class WindowRenderer
{
private:

	struct BITMAPINFO_BF
	{
		BITMAPINFOHEADER bmiHeader;
		DWORD bmiColorsR;
		DWORD bmiColorsG;
		DWORD bmiColorsB;

		BITMAPINFO * GetBitmapInfo()
		{
			return reinterpret_cast<BITMAPINFO*>(this);
		}
	};

// interface

public:

	static void RenderThumbnail(HWND window, Thumbnail & thumbnail);

	static void Render(HBITMAP bmp, const RECT & rect, Blob & blob);

// utility functions

private:

	static SIZE ComputeWindowSize(HWND window, SIZE size);

	static void CropImage(WORD * data, SIZE & size, const RECT & rect);

	static RECT CropRect(const RECT & rect, const SIZE & size);

	static void FlipImage(WORD * data, SIZE size);

	static HBITMAP RenderBitmap(HWND window, SIZE size, WORD *& data);

	static void ResizeAndCompress
		( WORD * data
		, SIZE   startSize
		, SIZE   endSize
		, Blob & blob
		);
};
