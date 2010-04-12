#include "stdafx.h"
#include "WindowRenderer.h"

#include "CComPtr.h"

#include <imaging.h>
#include <initguid.h>
#include <imgguids.h>

using namespace htmlayout;
using namespace htmlayout::dom;
using namespace std;

void WindowRenderer::Render(HWND window, Thumbnail & thumbnail)
{
	SIZE size = { thumbnail.width, thumbnail.height };
	SIZE windowSize(ComputeWindowSize(window, size));

	WORD * data(NULL);
	HBITMAP bmp(RenderBitmap(window, windowSize, data));
	FlipImage(data, windowSize);
	ResizeAndCompress(data, windowSize, size, thumbnail.data);
	::DeleteObject(bmp);
}

SIZE WindowRenderer::ComputeWindowSize(HWND window, SIZE size)
{
	RECT client;
	::GetClientRect(window, &client);

	SIZE windowSize;
	windowSize.cx = client.right;
	windowSize.cy = min(client.bottom, client.right * size.cy / size.cx);
	return windowSize;
}

void WindowRenderer::FlipImage(WORD * data, SIZE size)
{
	vector<WORD> line(size.cx);
	WORD * line1(data);
	WORD * line2(data + size.cx * (size.cy - 1));
	size_t lineSize(size.cx * 2);
	for (int y(0); y != size.cy / 2; ++y)
	{
		memcpy(&line[0], line1,    lineSize);
		memcpy(line1,    line2,    lineSize);
		memcpy(line2,    &line[0], lineSize);
		line1 += size.cx;
		line2 -= size.cx;
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

	HDC dc(::CreateCompatibleDC(::GetDC(window)));
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

	CComPtr<IImage> image2;
	result = image.QueryInterface(image2);

	CComPtr<IBasicBitmapOps> basicBitmapOps;
	image.QueryInterface(basicBitmapOps);

	CComPtr<IImage> thumb;
	result = image2->GetThumbnail(endSize.cx, endSize.cy, &thumb.Ptr());

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

	CComPtr<IImageEncoder> encoder;
	result = imageFactory->CreateImageEncoderToStream
		( &codecInfo->Clsid // clsid
		, stream            // stream
		, &encoder.Ptr()    // encoder
		);
	encoder->InitEncoder(stream);

	CComPtr<IImageSink> sink;
	result = encoder->GetEncodeSink(&sink.Ptr());

	result = thumb->PushIntoSink(sink);

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
