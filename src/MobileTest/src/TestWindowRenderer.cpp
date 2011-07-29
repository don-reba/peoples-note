#include "stdafx.h"
#include "WindowRenderer.h"
/*
#include "Test.h"
#include "Timer.h"
#include "Tools.h"
#include "Thumbnail.h"

#include <sstream>

using namespace htmlayout;

struct mem
{
	DWORD count;

	mem(DWORD count) : count(count) {}
};

template<typename T>
T & operator << (T & stream, mem & mem)
{
	DWORD count = mem.count;
	if (count < 1024)
		return stream << count << " B";
	if (count < 1024 * 1024)
		return stream << count / 1024 << " KB";
	return stream << count / 1024 / 1024 << " MB";
}

AUTO_TEST_CASE(TestRenderThumbnail)
{
	HWND window = ::CreateWindow
		( HTMLayoutClassNameT()   // lpClassName
		, L"WindowRenderer Test"  // lpWindowName
		, 0                       // dwStyle
		, 0                       // x
		, 0                       // y
		, 480                     // nWidth
		, 640                     // nHeight
		, NULL                    // hWndParent
		, NULL                    // hMenu
		, ::GetModuleHandle(NULL) // hInstance
		, NULL                    // lpParam
		);
	TEST_CHECK(window != NULL);

	std::wstringstream out;

	HtmlResource resource = Tools::LoadHtmlResource(L"test.htm", true);
	::HTMLayoutLoadHtml(window, resource.data, resource.size);

	Thumbnail thumbnail;
	thumbnail.Width  = 164;
	thumbnail.Height = 100;

	const int mb = 1000000;
	MEMORYSTATUS sMem;
	::GlobalMemoryStatus(&sMem);

	Timer timer;
	const int repeatCount(500);
	for (int i(0); i != repeatCount; ++i)
		WindowRenderer::RenderThumbnail(window, thumbnail);
	timer.Stop();

	MEMORYSTATUS fMem;
	::GlobalMemoryStatus(&fMem);

	out << L"render time: " << timer.GetElapsedSeconds() / repeatCount << L'\n';
	out << L"physical: " << mem((sMem.dwAvailPhys    - fMem.dwAvailPhys)    / repeatCount) << L'\n';
	out << L"virtual:  " << mem((sMem.dwAvailVirtual - fMem.dwAvailVirtual) / repeatCount) << L'\n';

	TEST_PRINT(out.str().c_str());

	::DestroyWindow(window);
}
*/