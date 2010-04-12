#pragma once

#include "Blob.h"

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

public:

	void Render(HWND window, SIZE size, Blob & blob);

private:

	static SIZE ComputeWindowSize(HWND window, SIZE size);

	static void FlipImage(WORD * data, SIZE size);

	static HBITMAP RenderBitmap(HWND window, SIZE size, WORD *& data);

	static void ResizeAndCompress
		( WORD * data
		, SIZE   startSize
		, SIZE   endSize
		, Blob & blob
		);
};
