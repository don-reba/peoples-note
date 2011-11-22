#pragma once

#include "Thumbnail.h"

interface IImagingFactory;
interface IImage;

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

	static void Render(HBITMAP bmp, Blob & blob);

	// Rescale the image to maxSize, if it is larger.
	// Leave dstBlob umodified in case of failure.
	static void RescaleImage(const Blob & srcBlob, Blob & dstBlob, SIZE maxSize);

// utility functions

private:

	static SIZE ComputeWindowSize(HWND window, SIZE size);

	static void CropImage(WORD * data, SIZE & size, const RECT & rect);

	static void FlipImage(WORD * data, SIZE size);

	static HBITMAP RenderBitmap(HWND window, SIZE size, WORD *& data);

	static void ResizeAndCompress
		( WORD * data
		, SIZE   startSize
		, SIZE   endSize
		, Blob & blob
		);

	static void ResizeAndCompress
		( IImagingFactory * imagingFactory
		, IImage          * image
		, SIZE              startSize
		, SIZE              endSize
		, Blob            & blob
		);
};
