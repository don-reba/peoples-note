#include "stdafx.h"

int _tmain(int argc, _TCHAR* argv[])
{
	// copied from:
	// http://windowsteamblog.com/windows_phone/b/windowsphone/archive/2009/08/11/using-custom-icons-in-windows-mobile-6-5.aspx
    HDC hdc = ::GetDC(NULL);
    INT ans = ::GetDeviceCaps(hdc, LOGPIXELSX);
    ::ReleaseDC(NULL, hdc);
    return ans;
}
