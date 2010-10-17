#include "stdafx.h"
#include "WindowRenderer.h"

#include "CComPtr.h"
#include "ImagingException.h"

#include <imaging.h>
#include <initguid.h>
#include <imgguids.h>

using namespace htmlayout;
using namespace htmlayout::dom;
using namespace std;

//----------
// interface
//----------

void WindowRenderer::RenderThumbnail(HWND window, Thumbnail & thumbnail)
{
	SIZE size = { thumbnail.Width, thumbnail.Height };
	SIZE windowSize(ComputeWindowSize(window, size));

	WORD * data(NULL);
	HBITMAP bmp(RenderBitmap(window, windowSize, data));
	FlipImage(data, windowSize);
	ResizeAndCompress(data, windowSize, size, thumbnail.Data);
	::DeleteObject(bmp);
}

void WindowRenderer::Render(HBITMAP bmp, Blob & blob)
{
	 BITMAP bmpObject;
	::GetObject(bmp, sizeof(bmpObject), &bmpObject);

	if (bmpObject.bmBits == NULL)
		throw std::exception("Bitmap data unavailable.");

	if (bmpObject.bmBitsPixel != 16)
		throw std::exception("Invalid bitmap format.");

	SIZE size = { bmpObject.bmWidth, bmpObject.bmHeight };

	WORD * bits(static_cast<WORD*>(bmpObject.bmBits));

	FlipImage(bits, size);
	ResizeAndCompress(bits, size, size, blob);
}

//------------------
// utility functions
//------------------

SIZE WindowRenderer::ComputeWindowSize(HWND window, SIZE size)
{
	RECT client;
	::GetClientRect(window, &client);

	SIZE windowSize;
	windowSize.cx = client.right;
	windowSize.cy = min(client.bottom, client.right * size.cy / size.cx);
	return windowSize;
}

void WindowRenderer::CropImage(WORD * data, SIZE & size, const RECT & rect)
{
	assert(rect.left <= rect.right);
	assert(rect.top  <= rect.bottom);

	vector<WORD> line(rect.right - rect.left);
	const int srcScanline((size.cx     + 1) & ~1);
	const int dstScanline((line.size() + 1) & ~1);
	WORD * line1(data + size.cx * rect.top + rect.left);
	WORD * line2(data);
	for (int y(rect.top); y != rect.bottom; ++y)
	{
		memcpy(&line[0], line1,    2 * line.size());
		memcpy(line2,    &line[0], 2 * line.size());
		line1 += srcScanline;
		line2 += dstScanline;
	}

	size.cx = rect.right - rect.left;
	size.cy = rect.bottom - rect.top;
}

void WindowRenderer::FlipImage(WORD * data, SIZE size)
{
	const int scanline((size.cx + 1) & ~1);
	vector<WORD> line(scanline);
	WORD * line1(data);
	WORD * line2(data + scanline * (size.cy - 1));
	const size_t lineSize(scanline * 2);
	for (int y(0); y != size.cy / 2; ++y)
	{
		memcpy(&line[0], line1,    lineSize);
		memcpy(line1,    line2,    lineSize);
		memcpy(line2,    &line[0], lineSize);
		line1 += scanline;
		line2 -= scanline;
	}
}

HBITMAP WindowRenderer::RenderBitmap(HWND window, SIZE size, WORD *& data)
{
 	BITMAPINFO_BF info = { 0 };
	info.bmiHeader.biSize         = sizeof(BITMAPINFOHEADER);
	info.bmiHeader.biWidth        = size.cx;
	info.bmiHeader.biHeight       = size.cy;
	info.bmiHeader.biPlanes       = 1;
	info.bmiHeader.biBitCount     = 16;
	info.bmiHeader.biCompression  = BI_BITFIELDS;
	info.bmiHeader.biSizeImage    = ((size.cx * 2 + 3) & ~3) * size.cy;
	info.bmiHeader.biClrUsed      = 1;
	info.bmiHeader.biClrImportant = 0;
	info.bmiColorsR               = 0xf800;
	info.bmiColorsG               = 0x07e0;
	info.bmiColorsB               = 0x001f;

	HDC windowDc(::GetDC(window));
	HDC dc(::CreateCompatibleDC(windowDc));
	HBITMAP bmp = ::CreateDIBSection
		( dc                              // hdc
		, info.GetBitmapInfo()            // pbmi
		, DIB_RGB_COLORS                  // iUsage
		, reinterpret_cast<void**>(&data) // ppvBits
		, NULL                            // hSection
		, 0                               // dwOffset
		);
	if (bmp)
	{
		RECT rect = { 0, 0, size.cx, size.cy };
		if (!HTMLayoutRender(window, bmp, rect))
			throw std::exception("Note rendering failed.");
	}
	::DeleteDC(dc);
	::ReleaseDC(window, windowDc);

	return bmp;
}

void WindowRenderer::ResizeAndCompress
	( WORD * data
	, SIZE   startSize
	, SIZE   endSize
	, Blob & blob
	)
{
	HRESULT result;

	IImagingFactory * imageFactory(NULL);
	result = ::CoCreateInstance
		( CLSID_ImagingFactory
		, NULL
		, CLSCTX_INPROC_SERVER
		, IID_IImagingFactory
		, reinterpret_cast<void**>(&imageFactory)
		);

	BitmapData bitmapData = { 0 };
	bitmapData.Width       = startSize.cx;
	bitmapData.Height      = startSize.cy;
	bitmapData.Stride      = ((startSize.cx * 2 + 3) & ~3);
	bitmapData.PixelFormat = PixelFormat16bppRGB565;
	bitmapData.Scan0       = data;

	CComPtr<IBitmapImage> image;
	result = imageFactory->CreateBitmapFromBuffer
		( &bitmapData  // bitmapData
		, &image.Ptr() // bitmap
		);
	if (FAILED(result))
		throw ImagingException(result);

	CComPtr<IImage> image2;
	result = image.QueryInterface(image2);

	ImageCodecInfo * infos(NULL);
	UINT             infoCount(0);
	result = imageFactory->GetInstalledEncoders(&infoCount, &infos);
	ImageCodecInfo * codecInfo(NULL);
	for (int i(0); i != infoCount; ++i)
	{
		ImageCodecInfo * info = infos + i;
		if (wcscmp(info->FormatDescription, L"JPEG") == 0)
			codecInfo = info;
	}
	
	CComPtr<IStream> stream;
	result = ::CreateStreamOnHGlobal
		( NULL          // hGlobal
		, TRUE          // fDeleteOnRelease
		, &stream.Ptr() // ppstm
		);
	if (FAILED(result))
		throw ImagingException(result);

	CComPtr<IImageEncoder> encoder;
	result = imageFactory->CreateImageEncoderToStream
		( &codecInfo->Clsid // clsid
		, stream            // stream
		, &encoder.Ptr()    // encoder
		);
	if (FAILED(result))
		throw ImagingException(result);
	encoder->InitEncoder(stream);

	CComPtr<IImageSink> sink;
	result = encoder->GetEncodeSink(&sink.Ptr());
	if (FAILED(result))
		throw ImagingException(result);

	if (startSize.cx == endSize.cx && startSize.cy == endSize.cy)
	{
		result = image2->PushIntoSink(sink);
		if (FAILED(result))
			throw ImagingException(result);
	}
	else
	{
		CComPtr<IImage> thumb;
		result = image2->GetThumbnail(endSize.cx, endSize.cy, &thumb.Ptr());
		if (FAILED(result))
			throw ImagingException(result);

		result = thumb->PushIntoSink(sink);
		if (FAILED(result))
			throw ImagingException(result);
	}

	LARGE_INTEGER  zero = { 0 };
	ULARGE_INTEGER pos  = { 0 };
	stream->Seek(zero, STREAM_SEEK_CUR, &pos);
	stream->Seek(zero, STREAM_SEEK_SET, NULL);
	if (pos.HighPart > 0 || pos.LowPart == 0)
		return;
	blob.resize(pos.LowPart);
	ULONG bytesRead = 0;
	stream->Read(&blob[0], pos.LowPart, &bytesRead);
	if (bytesRead < pos.LowPart)
		blob.resize(0);
}
