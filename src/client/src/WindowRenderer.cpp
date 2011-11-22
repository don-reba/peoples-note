#include "stdafx.h"
#include "WindowRenderer.h"

#include "CComPtr.h"
#include "ImagingException.h"

#include <imaging.h>
#include <initguid.h>
#include <imgguids.h>

#include <boost/scope_exit.hpp>

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
	BOOST_SCOPE_EXIT((&bmp))
	{
		::DeleteObject(bmp);
	} BOOST_SCOPE_EXIT_END

	if (!data)
		throw std::exception("Thumbnail render failed.");
	FlipImage(data, windowSize);
	ResizeAndCompress(data, windowSize, size, thumbnail.Data);
	
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

void WindowRenderer::RescaleImage(const Blob & srcBlob, Blob & dstBlob, SIZE maxSize)
{
	if (srcBlob.empty())
		return;

	HRESULT result;

	// get the image factory
	CComPtr<IImagingFactory> imagingFactory;
	result = ::CoCreateInstance
		( CLSID_ImagingFactory
		, NULL
		, CLSCTX_INPROC_SERVER
		, IID_IImagingFactory
		, reinterpret_cast<void**>(&imagingFactory.Ptr())
		);
	if (FAILED(result))
		throw ImagingException(result);

	// get an image from the data
	CComPtr<IImage> image;
	result = imagingFactory->CreateImageFromBuffer
		( &srcBlob[0]
		, srcBlob.size()
		, BufferDisposalFlagNone
		, &image.Ptr()
		);
	if (FAILED(result))
		return; // probably not an image

	// get image information
	ImageInfo info;
	result = image->GetImageInfo(&info);
	if (FAILED(result))
		return;

	SIZE startSize = { info.Width, info.Height };
	if (startSize.cx <= maxSize.cx && startSize.cy <= maxSize.cy)
		return;

	// resize the image
	CComPtr<IBitmapImage> bitmap;
	imagingFactory->CreateBitmapFromImage
		( image
		, maxSize.cx
		, maxSize.cy
		, PixelFormat24bppRGB
		, InterpolationHintBilinear
		, &bitmap.Ptr()
		);

	// cast  image from IBitmapImage to IImage
	CComPtr<IImage> image2;
	result = bitmap.QueryInterface(image2);
	if (FAILED(result))
		throw ImagingException(result);

	SIZE endSize;
	if (startSize.cx * maxSize.cy < startSize.cy * maxSize.cx)
	{
		// bounded by height
		endSize.cx = maxSize.cy * startSize.cx / startSize.cy;
		endSize.cy = maxSize.cy;
	}
	else
	{
		// bounded by width
		endSize.cx = maxSize.cx;
		endSize.cy = maxSize.cx * startSize.cy / startSize.cx;
	}
	ResizeAndCompress(imagingFactory, image, startSize, endSize, dstBlob);
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
	BOOST_SCOPE_EXIT((&window)(&windowDc))
	{
		::ReleaseDC(window, windowDc);
	} BOOST_SCOPE_EXIT_END

	HDC dc(::CreateCompatibleDC(windowDc));
	BOOST_SCOPE_EXIT((&dc))
	{
		::DeleteDC(dc);
	} BOOST_SCOPE_EXIT_END

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

	// get the image factory
	CComPtr<IImagingFactory> imagingFactory;
	result = ::CoCreateInstance
		( CLSID_ImagingFactory
		, NULL
		, CLSCTX_INPROC_SERVER
		, IID_IImagingFactory
		, reinterpret_cast<void**>(&imagingFactory.Ptr())
		);
	if (FAILED(result))
		throw ImagingException(result);

	// prepare a bitmap header
	BitmapData bitmapData = { 0 };
	bitmapData.Width       = startSize.cx;
	bitmapData.Height      = startSize.cy;
	bitmapData.Stride      = ((startSize.cx * 2 + 3) & ~3);
	bitmapData.PixelFormat = PixelFormat16bppRGB565;
	bitmapData.Scan0       = data;

	// get an IBitmapImage using our data
	CComPtr<IBitmapImage> bitmap;
	result = imagingFactory->CreateBitmapFromBuffer
		( &bitmapData  // bitmapData
		, &bitmap.Ptr() // bitmap
		);
	if (FAILED(result))
		throw ImagingException(result);

	// cast  image from IBitmapImage to IImage
	CComPtr<IImage> image;
	result = bitmap.QueryInterface(image);
	if (FAILED(result))
		throw ImagingException(result);

	ResizeAndCompress(imagingFactory, image, startSize, endSize, blob);
}

void WindowRenderer::ResizeAndCompress
	( IImagingFactory * imagingFactory
	, IImage          * image
	, SIZE              startSize
	, SIZE              endSize
	, Blob            & blob
	)
{
	HRESULT result;

	// list installed image encoders
	ImageCodecInfo * infos(NULL);
	UINT             infoCount(0);
	BOOST_SCOPE_EXIT((&infos))
	{
		CoTaskMemFree(infos);
	} BOOST_SCOPE_EXIT_END
	result = imagingFactory->GetInstalledEncoders(&infoCount, &infos);
	if (FAILED(result))
		throw ImagingException(result);

	// get JPEG encoder CLSID
	CLSID codecId = { 0 };
	for (int i(0); i != infoCount; ++i)
	{
		ImageCodecInfo * info = infos + i;
		if (wcscmp(info->FormatDescription, L"JPEG") == 0)
			codecId = info->Clsid;
	}

	// create a new data stream
	CComPtr<IStream> stream;
	result = ::CreateStreamOnHGlobal
		( NULL          // hGlobal
		, TRUE          // fDeleteOnRelease
		, &stream.Ptr() // ppstm
		);
	if (FAILED(result))
		throw ImagingException(result);

	// connect the stream to the encoder
	CComPtr<IImageEncoder> encoder;
	result = imagingFactory->CreateImageEncoderToStream
		( &codecId       // clsid
		, stream         // stream
		, &encoder.Ptr() // encoder
		);
	if (FAILED(result))
		throw ImagingException(result);
	encoder->InitEncoder(stream);

	// create a new image sink
	CComPtr<IImageSink> sink;
	result = encoder->GetEncodeSink(&sink.Ptr());
	if (FAILED(result))
		throw ImagingException(result);

	// create the thumbnail
	if (startSize.cx == endSize.cx && startSize.cy == endSize.cy)
	{
		result = image->PushIntoSink(sink);
		if (FAILED(result))
			throw ImagingException(result);
	}
	else
	{
		CComPtr<IImage> thumb;
		result = image->GetThumbnail(endSize.cx, endSize.cy, &thumb.Ptr());
		if (FAILED(result))
			throw ImagingException(result);

		result = thumb->PushIntoSink(sink);
		if (FAILED(result))
			throw ImagingException(result);
	}

	// read thumbnail data from the stream into a blob
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
