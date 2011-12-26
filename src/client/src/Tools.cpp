#include "stdafx.h"

#include "Tools.h"

#include "MD5/md5.h"

#include <algorithm>
#include <sstream>

using namespace Tools;
using namespace std;


//-----------------
// Helper functions
//-----------------

// Based on Raymond Chen's implementation.
// http://blogs.msdn.com/oldnewthing/archive/2004/01/30/65013.aspx
LPCWSTR GetStringResource(HINSTANCE instance, WORD id)
{
	// Convert the string ID into a bundle number
	LPWSTR resourceName = MAKEINTRESOURCE(id / 16 + 1);

	HRSRC hrsrc = ::FindResource(instance, resourceName, RT_STRING);
	if (!hrsrc)
		throw exception("Resource not found.");

	HGLOBAL hglob = ::LoadResource(instance, hrsrc);
	if (!hglob)
		throw exception("Resource could not be loaded.");

	LPCWSTR resource = reinterpret_cast<LPCWSTR>(::LockResource(hglob));
	if (!resource)
		throw exception("Resource could not be locked.");

	// walk the string table
	for (int i = 0; i < (id & 0xF); i++)
		resource += 1 + (UINT)*resource;

	return resource;
}

bool IsBase64Whitespace(wchar_t c)
{
	switch (c)
	{
	case L' ': case L'\n': case L'\r': case L'\t':
		return true;
	}
	return false;
}

#ifndef RT_HTML
#define RT_HTML MAKEINTRESOURCE(23)
#endif // RT_HTML

//---------------------
// Tools implementation
//---------------------

string Tools::ConvertToAnsi(const wstring & str)
{
	UINT  codePage = CP_ACP;
	DWORD flags    = 0;
	int resultSize = WideCharToMultiByte
		( codePage     // CodePage
		, flags        // dwFlags
		, str.c_str()  // lpWideCharStr
		, str.length() // cchWideChar
		, NULL         // lpMultiByteStr
		, 0            // cbMultiByte
		, NULL         // lpDefaultChar
		, NULL         // lpUsedDefaultChar
		);
	vector<char> result(resultSize + 1);
	WideCharToMultiByte
		( codePage     // CodePage
		, flags        // dwFlags
		, str.c_str()  // lpWideCharStr
		, str.length() // cchWideChar
		, &result[0]   // lpMultiByteStr
		, resultSize   // cbMultiByte
		, NULL         // lpDefaultChar
		, NULL         // lpUsedDefaultChar
		);
	return &result[0];
}

const unsigned char * Tools::ConvertToUtf8
	( const wstring         & str
	, vector<unsigned char> & result
	)
{
	UINT  codePage = CP_UTF8;
	DWORD flags    = 0;
	int resultSize = WideCharToMultiByte
		( codePage     // CodePage
		, flags        // dwFlags
		, str.c_str()  // lpWideCharStr
		, str.length() // cchWideChar
		, NULL         // lpMultiByteStr
		, 0            // cbMultiByte
		, NULL         // lpDefaultChar
		, NULL         // lpUsedDefaultChar
		);
	result.resize(0);
	result.resize(resultSize + 1);
	LPSTR resultStr = reinterpret_cast<char*>(&result[0]);
	WideCharToMultiByte
		( codePage     // CodePage
		, flags        // dwFlags
		, str.c_str()  // lpWideCharStr
		, str.length() // cchWideChar
		, resultStr    // lpMultiByteStr
		, resultSize   // cbMultiByte
		, NULL         // lpDefaultChar
		, NULL         // lpUsedDefaultChar
		);
	result.resize(result.size() - 1); // cut terminating zero
	return result.empty()
		? reinterpret_cast<unsigned char *>("")
		: &result[0]
		;
}

wstring Tools::ConvertToUnicode(const string & str)
{
	UINT  codePage = CP_ACP;
	DWORD flags    = 0;
	int resultSize = MultiByteToWideChar
		( codePage     // CodePage
		, flags        // dwFlags
		, str.c_str()  // lpMultiByteStr
		, str.length() // cbMultiByte
		, NULL         // lpWideCharStr
		, 0            // cchWideChar
		);
	vector<wchar_t> result(resultSize + 1);
	MultiByteToWideChar
		( codePage     // CodePage
		, flags        // dwFlags
		, str.c_str()  // lpMultiByteStr
		, str.length() // cbMultiByte
		, &result[0]   // lpWideCharStr
		, resultSize   // cchWideChar
		);
	return &result[0];
}

void Tools::ConvertToUnicode
	( const unsigned char * str
	, wstring             & result
	)
{
	if (!str)
	{
		result.clear();
		return;
	}

	LPCSTR cStr = reinterpret_cast<LPCSTR>(str);
	int length = strlen(cStr);

	UINT  codePage = CP_UTF8;
	DWORD flags    = 0;
	int resultSize = MultiByteToWideChar
		( codePage // CodePage
		, flags    // dwFlags
		, cStr     // lpMultiByteStr
		, length   // cbMultiByte
		, NULL     // lpWideCharStr
		, 0        // cchWideChar
		);
	result.resize(resultSize + 1);
	MultiByteToWideChar
		( codePage   // CodePage
		, flags      // dwFlags
		, cStr       // lpMultiByteStr
		, length     // cbMultiByte
		, &result[0] // lpWideCharStr
		, resultSize // cchWideChar
		);
	result.resize(resultSize);
}

void Tools::DecodeBase64(const wchar_t * text, Blob & data)
{
	if (!text)
		throw std::exception("Invalid input.");

	// verify and measure input
	int size(0);
	for (const wchar_t * t(text); *t;)
	{
		if (IsBase64Whitespace(*t))
			++t;
		else
		{
			for (int i(0); i != 4; ++i)
			{
				if (!*t || *t < 43 || *t >= 43 + 90)
					throw std::exception("Invalid input.");
				++size;
				++t;
			}
		}
	}

	// calculate output size
	if (size == 0)
		return;
	if (size % 4 != 0)
		throw std::exception("Invalid input.");
	size = size / 4 * 3;

	// parse input
	const BYTE lookup[] =
		{ 62, 0 , 0 , 0 , 63, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 0
		, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 1 , 2 , 3 , 4 , 5 , 6 , 7 , 8 , 9
		, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25
		, 0 , 0 , 0 , 0 , 0 , 0 , 26, 27, 28, 29, 30, 31, 32, 33, 34, 35
		, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51
		};
	data.clear();
	data.reserve(size);
	while (*text)
	{
		if (IsBase64Whitespace(*text))
			++text;
		else
		{
			BYTE n0 = lookup[*text - 43]; ++text;
			BYTE n1 = lookup[*text - 43]; ++text;
			BYTE n2 = lookup[*text - 43]; ++text;
			BYTE n3 = lookup[*text - 43]; ++text;
			data.push_back((n0 << 2) | (n1 >> 4));
			data.push_back((n1 << 4) | (n2 >> 2));
			data.push_back((n2 << 6) | (n3 >> 0));
		}
	}

	// remove padding
	--text;
	while (IsBase64Whitespace(*text))
		--text;
	if (text[0] == L'=')
		--size;
	if (text[-1] == L'=')
		--size;
	data.resize(size);
}

bool Tools::EndsWith(const wstring & sequence, const wchar_t * fragment)
{
	if (!fragment)
		return false;
	return EndsWith(sequence, fragment, fragment + wcslen(fragment));
}

wstring Tools::EscapeHtml(const wstring & str)
{
	wstring result;
	result.reserve(str.size());
	foreach (wchar_t c, str)
	{
		switch (c)
		{
		case L'<': result.append(L"&lt;");   break;
		case L'>': result.append(L"&gt;");   break;
		case L'&': result.append(L"&amp;");  break;
		case L'"': result.append(L"&quot;"); break;
		default:   result.push_back(c);
		}
	}
	return result;
}

#ifdef _DEBUG
wstring Tools::GetMessageName(int id)
{
	switch (id)
	{
	case 0x0000: return L"WM_NULL";
	case 0x0001: return L"WM_CREATE";
	case 0x0002: return L"WM_DESTROY";
	case 0x0003: return L"WM_MOVE";
	case 0x0004: return L"SW_ERASE";
	case 0x0005: return L"WM_SIZE";
	case 0x0006: return L"WM_ACTIVATE";
	case 0x0007: return L"WM_SETFOCUS";
	case 0x0008: return L"WM_KILLFOCUS";
	case 0x000A: return L"WM_ENABLE";
	case 0x000B: return L"WM_SETREDRAW";
	case 0x000C: return L"WM_SETTEXT";
	case 0x000D: return L"WM_GETTEXT";
	case 0x000E: return L"WM_GETTEXTLENGTH";
	case 0x000F: return L"WM_PAINT";
	case 0x0010: return L"WM_CLOSE";
	case 0x0012: return L"WM_QUIT";
	case 0x0014: return L"WM_ERASEBKGND";
	case 0x0015: return L"WM_SYSCOLORCHANGE";
	case 0x0018: return L"WM_SHOWWINDOW";
	case 0x001A: return L"WM_SETTINGCHANGE";
	case 0x001D: return L"WM_FONTCHANGE";
	case 0x001F: return L"WM_CANCELMODE";
	case 0x0020: return L"WM_SETCURSOR";
	case 0x0028: return L"WM_NEXTDLGCTL";
	case 0x002B: return L"WM_DRAWITEM";
	case 0x002C: return L"WM_MEASUREITEM";
	case 0x002D: return L"WM_DELETEITEM";
	case 0x002E: return L"WM_VKEYTOITEM";
	case 0x002F: return L"WM_CHARTOITEM";
	case 0x0030: return L"WM_SETFONT";
	case 0x0031: return L"WM_GETFONT";
	case 0x0037: return L"WM_QUERYDRAGICON";
	case 0x0039: return L"WM_COMPAREITEM";
	case 0x0047: return L"WM_WINDOWPOSCHANGED";
	case 0x004A: return L"WM_COPYDATA";
	case 0x004E: return L"WM_NOTIFY";
	case 0x0050: return L"WM_INPUTLANGCHANGEREQUEST";
	case 0x0051: return L"WM_INPUTLANGCHANGE";
	case 0x0053: return L"WM_HELP";
	case 0x007B: return L"WM_CONTEXTMENU";
	case 0x007D: return L"WM_STYLECHANGED";
	case 0x007F: return L"WM_GETICON";
	case 0x0080: return L"WM_SETICON";
	case 0x0087: return L"WM_GETDLGCODE";
	case 0x0100: return L"WM_KEYDOWN";
	case 0x0101: return L"WM_KEYUP";
	case 0x0102: return L"WM_CHAR";
	case 0x0103: return L"WM_DEADCHAR";
	case 0x0104: return L"WM_SYSKEYDOWN";
	case 0x0105: return L"WM_SYSKEYUP";
	case 0x0106: return L"WM_SYSCHAR";
	case 0x0107: return L"WM_SYSDEADCHAR";
	case 0x0108: return L"WM_KEYLAST";
	case 0x010C: return L"WM_IM_INFO";
	case 0x010D: return L"WM_IME_STARTCOMPOSITION";
	case 0x010E: return L"WM_IME_ENDCOMPOSITION";
	case 0x010F: return L"WM_IME_KEYLAST";
	case 0x0110: return L"WM_INITDIALOG";
	case 0x0111: return L"WM_COMMAND";
	case 0x0112: return L"WM_SYSCOMMAND";
	case 0x0113: return L"WM_TIMER";
	case 0x0114: return L"WM_HSCROLL";
	case 0x0115: return L"WM_VSCROLL";
	case 0x0117: return L"WM_INITMENUPOPUP";
	case 0x0120: return L"WM_MENUCHAR";
	case 0x0132: return L"WM_CTLCOLORMSGBOX";
	case 0x0133: return L"WM_CTLCOLOREDIT";
	case 0x0134: return L"WM_CTLCOLORLISTBOX";
	case 0x0135: return L"WM_CTLCOLORBTN";
	case 0x0136: return L"WM_CTLCOLORDLG";
	case 0x0137: return L"WM_CTLCOLORSCROLLBAR";
	case 0x0138: return L"WM_CTLCOLORSTATIC";
	case 0x0200: return L"WM_MOUSEMOVE";
	case 0x0201: return L"WM_LBUTTONDOWN";
	case 0x0202: return L"WM_LBUTTONUP";
	case 0x0203: return L"WM_LBUTTONDBLCLK";
	case 0x0204: return L"WM_RBUTTONDOWN";
	case 0x0205: return L"WM_RBUTTONUP";
	case 0x0206: return L"WM_RBUTTONDBLCLK";
	case 0x0207: return L"WM_MBUTTONDOWN";
	case 0x0208: return L"WM_MBUTTONUP";
	case 0x0209: return L"WM_MBUTTONDBLCLK";
	case 0x020A: return L"WM_MOUSEWHEEL";
	case 0x020B: return L"WM_XBUTTONDOWN";
	case 0x020C: return L"WM_XBUTTONUP";
	case 0x020D: return L"WM_XBUTTONDBLCLK";
	case 0x0211: return L"WM_ENTERMENULOOP";
	case 0x0212: return L"WM_EXITMENULOOP";
	case 0x0215: return L"WM_CAPTURECHANGED";
	case 0x0281: return L"WM_IME_SETCONTEXT";
	case 0x0282: return L"WM_IME_NOTIFY";
	case 0x0283: return L"WM_IME_CONTROL";
	case 0x0284: return L"WM_IME_COMPOSITIONFULL";
	case 0x0285: return L"WM_IME_SELECT";
	case 0x0286: return L"WM_IME_CHAR";
	case 0x0287: return L"WM_IME_SYSTEM";
	case 0x0288: return L"WM_IME_REQUEST";
	case 0x0290: return L"WM_IME_KEYDOWN";
	case 0x0291: return L"WM_IME_KEYUP";
	case 0x0300: return L"WM_CUT";
	case 0x0301: return L"WM_COPY";
	case 0x0302: return L"WM_PASTE";
	case 0x0303: return L"WM_CLEAR";
	case 0x0304: return L"WM_UNDO";
	case 0x0305: return L"WM_RENDERFORMAT";
	case 0x0306: return L"WM_RENDERALLFORMATS";
	case 0x0307: return L"WM_DESTROYCLIPBOARD";
	case 0x030F: return L"WM_QUERYNEWPALETTE";
	case 0x0311: return L"WM_PALETTECHANGED";
	case 0x0312: return L"WM_HOTKEY";
	case 0x03FD: return L"WM_DBNOTIFICATION";
	case 0x03FE: return L"WM_NETCONNECT";
	case 0x03FF: return L"WM_HIBERNATE";
	case 0x0400: return L"WM_USER";
	case 0x0401: return L"DM_SETDEFID";
	case 0x0402: return L"DM_RESERVED0x2";
	case 0x8000: return L"WM_APP";
	}
	vector<wchar_t> number(14);
	swprintf(&number[0], L"0x%X", id);
	return &number[0];
}

wstring Tools::GetVkName(UINT vk)
{
	switch (vk)
	{
	case 0x01:  return L"VK_LBUTTON";
	case 0x02:  return L"VK_RBUTTON";
	case 0x03:  return L"VK_CANCEL";
	case 0x04:  return L"VK_MBUTTON";
	case 0x05:  return L"VK_XBUTTON1";
	case 0x06:  return L"VK_XBUTTON2";
	case 0x08:  return L"VK_BACK";
	case 0x09:  return L"VK_TAB";
	case 0x0C:  return L"VK_CLEAR";
	case 0x0D:  return L"VK_RETURN";
	case 0x10:  return L"VK_SHIFT";
	case 0x11:  return L"VK_CONTROL";
	case 0x12:  return L"VK_MENU";
	case 0x13:  return L"VK_PAUSE";
	case 0x14:  return L"VK_CAPITAL";
	case 0x15:  return L"VK_KANA";
	case 0x17:  return L"VK_JUNJA";
	case 0x18:  return L"VK_FINAL";
	case 0x19:  return L"VK_KANJI";
	case 0x1B:  return L"VK_ESCAPE";
	case 0x1c:  return L"VK_CONVERT";
	case 0x1d:  return L"VK_NOCONVERT";
	case 0x20:  return L"VK_SPACE";
	case 0x21:  return L"VK_PRIOR";
	case 0x22:  return L"VK_NEXT";
	case 0x23:  return L"VK_END";
	case 0x24:  return L"VK_HOME";
	case 0x25:  return L"VK_LEFT";
	case 0x26:  return L"VK_UP";
	case 0x27:  return L"VK_RIGHT";
	case 0x28:  return L"VK_DOWN";
	case 0x29:  return L"VK_SELECT";
	case 0x2A:  return L"VK_PRINT";
	case 0x2B:  return L"VK_EXECUTE";
	case 0x2C:  return L"VK_SNAPSHOT";
	case 0x2D:  return L"VK_INSERT";
	case 0x2E:  return L"VK_DELETE";
	case 0x2F:  return L"VK_HELP";
	case 0x5B:  return L"VK_LWIN";
	case 0x5C:  return L"VK_RWIN";
	case 0x5D:  return L"VK_APPS";
	case 0x5F:  return L"VK_SLEEP";
	case 0x60:  return L"VK_NUMPAD0";
	case 0x61:  return L"VK_NUMPAD1";
	case 0x62:  return L"VK_NUMPAD2";
	case 0x63:  return L"VK_NUMPAD3";
	case 0x64:  return L"VK_NUMPAD4";
	case 0x65:  return L"VK_NUMPAD5";
	case 0x66:  return L"VK_NUMPAD6";
	case 0x67:  return L"VK_NUMPAD7";
	case 0x68:  return L"VK_NUMPAD8";
	case 0x69:  return L"VK_NUMPAD9";
	case 0x6A:  return L"VK_MULTIPLY";
	case 0x6B:  return L"VK_ADD";
	case 0x6C:  return L"VK_SEPARATOR";
	case 0x6D:  return L"VK_SUBTRACT";
	case 0x6E:  return L"VK_DECIMAL";
	case 0x6F:  return L"VK_DIVIDE";
	case 0x70:  return L"VK_F1";
	case 0x71:  return L"VK_F2";
	case 0x72:  return L"VK_F3";
	case 0x73:  return L"VK_F4";
	case 0x74:  return L"VK_F5";
	case 0x75:  return L"VK_F6";
	case 0x76:  return L"VK_F7";
	case 0x77:  return L"VK_F8";
	case 0x78:  return L"VK_F9";
	case 0x79:  return L"VK_F10";
	case 0x7A:  return L"VK_F11";
	case 0x7B:  return L"VK_F12";
	case 0x7C:  return L"VK_F13";
	case 0x7D:  return L"VK_F14";
	case 0x7E:  return L"VK_F15";
	case 0x7F:  return L"VK_F16";
	case 0x80:  return L"VK_F17";
	case 0x81:  return L"VK_F18";
	case 0x82:  return L"VK_F19";
	case 0x83:  return L"VK_F20";
	case 0x84:  return L"VK_F21";
	case 0x85:  return L"VK_F22";
	case 0x86:  return L"VK_F23";
	case 0x87:  return L"VK_F24";
	case 0x90:  return L"VK_NUMLOCK";
	case 0x91:  return L"VK_SCROLL";
	case 0xA0:  return L"VK_LSHIFT";
	case 0xA1:  return L"VK_RSHIFT";
	case 0xA2:  return L"VK_LCONTROL";
	case 0xA3:  return L"VK_RCONTROL";
	case 0xA4:  return L"VK_LMENU";
	case 0xA5:  return L"VK_RMENU";
	case 0xA6:  return L"VK_BROWSER_BACK";
	case 0xA7:  return L"VK_BROWSER_FORWARD";
	case 0xA8:  return L"VK_BROWSER_REFRESH";
	case 0xA9:  return L"VK_BROWSER_STOP";
	case 0xAA:  return L"VK_BROWSER_SEARCH";
	case 0xAB:  return L"VK_BROWSER_FAVORITES";
	case 0xAC:  return L"VK_BROWSER_HOME";
	case 0xAD:  return L"VK_VOLUME_MUTE";
	case 0xAE:  return L"VK_VOLUME_DOWN";
	case 0xAF:  return L"VK_VOLUME_UP";
	case 0xB0:  return L"VK_MEDIA_NEXT_TRACK";
	case 0xB1:  return L"VK_MEDIA_PREV_TRACK";
	case 0xB2:  return L"VK_MEDIA_STOP";
	case 0xB3:  return L"VK_MEDIA_PLAY_PAUSE";
	case 0xB4:  return L"VK_LAUNCH_MAIL";
	case 0xB5:  return L"VK_LAUNCH_MEDIA_SELECT";
	case 0xB6:  return L"VK_LAUNCH_APP1";
	case 0xB7:  return L"VK_LAUNCH_APP2";
	case 0xBA:  return L"VK_SEMICOLON";
	case 0xBB:  return L"VK_EQUAL";
	case 0xBC:  return L"VK_COMMA";
	case 0xBD:  return L"VK_HYPHEN";
	case 0xBE:  return L"VK_PERIOD";
	case 0xBF:  return L"VK_SLASH";
	case 0xC0:  return L"VK_BACKQUOTE";
	case 0xDB:  return L"VK_LBRACKET";
	case 0xDC:  return L"VK_BACKSLASH";
	case 0xDD:  return L"VK_RBRACKET";
	case 0xDE:  return L"VK_APOSTROPHE";
	case 0xDF:  return L"VK_OFF";
	case 0xE2:  return L"VK_EXTEND_BSLASH";
	case 0xE5:  return L"VK_PROCESSKEY";
	case 0xF6:  return L"VK_ATTN";
	case 0xF7:  return L"VK_CRSEL";
	case 0xF8:  return L"VK_EXSEL";
	case 0xF9:  return L"VK_EREOF";
	case 0xFA:  return L"VK_PLAY";
	case 0xFB:  return L"VK_ZOOM";
	case 0xFC:  return L"VK_NONAME";
	case 0xFD:  return L"VK_PA1";
	case 0xFE:  return L"VK_OEM_CLEAR";
	}
	wstring name(L"VK_");
	if (vk >= '0' && vk <= '9') return name.push_back(L'0' + (vk - '0')), name;
	if (vk >= 'A' && vk <= 'Z') return name.push_back(L'A' + (vk - 'A')), name;
	vector<wchar_t> number(14);
	swprintf(&number[0], L"0x%X", vk);
	return name + &number[0];
}

#endif // _DEBUG

string Tools::HashWithMD5(const Blob & data)
{
	md5_state_t state;
	md5_byte_t  digest[16];

	md5_init(&state);
	if (!data.empty())
		md5_append(&state, &data[0], data.size());
	md5_finish(&state, digest);

	char digits[] =
		{ '0', '1', '2', '3', '4', '5', '6', '7'
		, '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
		};
	string hex;
	hex.reserve(16 * 2 + 1);
	for (int i(0); i != 16; ++i)
	{
		hex.push_back(digits[(digest[i] & 0xF0) >> 4]);
		hex.push_back(digits[(digest[i] & 0x0F) >> 0]);
	}

	return hex;
}

wstring Tools::HashPassword(const wstring & password)
{
	Blob blob(password.size() * 2);
	for (int i(0); i != password.size(); ++i)
	{
		wchar_t c(password.at(i));
		blob.at(i + i + 0) = c >> 8;
		blob.at(i + i + 1) = c && 0xFF;
	}
	return ConvertToUnicode(HashWithMD5(blob));
}

HtmlResource Tools::LoadHtmlResource(LPCWSTR id, bool highRes)
{
	wstring newId(id);
	std::replace(newId.begin(), newId.end(), L'-', L'_');
	if (!highRes)
		newId.insert(0, L"LR_");

	HINSTANCE instance = GetModuleHandle(NULL);

	HRSRC hrsrc = ::FindResource(instance, newId.c_str(), RT_HTML);
	if(!hrsrc)
		throw exception("Resource not found.");

	HGLOBAL hglob = ::LoadResource(instance, hrsrc);
	if(!hglob)
		throw exception("Resource could not be loaded.");

	HtmlResource resource;

	resource.data = static_cast<PBYTE>(::LockResource(hglob));
	if (!resource.data)
		throw exception("Resource could not be locked.");

	resource.size = ::SizeofResource(instance, hrsrc);
	if (0 == resource.size)
		throw exception("Resource size could not be found");

	return resource;
}

wstring Tools::LoadStringResource(int id)
{
	HINSTANCE instance = GetModuleHandle(NULL);
	LPCWSTR resource = GetStringResource(instance, id);
	vector<wchar_t> str(1 + *resource);
	CopyMemory(&str[0], resource + 1, *resource * sizeof(wchar_t));
	return &str[0];
}

wstring Tools::MakeSizeString(__int64 size)
{
	wstringstream stream;
	if (size < 0)
	{
		stream << L"-";
		size = -size;
	}
	if (size < 0x400L)
		stream << size << L" B";
	else if(size < 0x100000L)
		stream << (size / 0x400L) << L" KB";
	else if (size < 0x40000000L)
		stream << (size / 0x100000L) << L" MB";
	else
		stream << (size / 0x40000000L) << L" GB";
	return stream.str();
}

void Tools::ReadBinaryFile
	( const wchar_t * fileName
	, Blob          & data
	)
{
	HANDLE file = ::CreateFile
		( fileName              // lpFileName
		, GENERIC_READ          // dwDesiredAccess
		, FILE_SHARE_READ       // dwShareMode
		, NULL                  // lpSecurityAttributes
		, OPEN_EXISTING         // dwCreationDisposition
		, FILE_ATTRIBUTE_NORMAL // dwFlagsAndAttributes
		, NULL                  // hTemplateFile
		);
	if (file == INVALID_HANDLE_VALUE)
		return;

	DWORD size(::GetFileSize(file, NULL));
	if (!size)
		return;
	data.resize(size);

	DWORD bytesRead(0);
	::ReadFile(file, &data[0], size, &bytesRead, NULL);
	if (bytesRead != size)
		return;
}

void Tools::ReadUtf8File
	( const wchar_t * fileName
	, std::wstring  & contents
	)
{
	vector<unsigned char> utf8;
	ReadBinaryFile(fileName, utf8);
	ConvertToUnicode(&utf8[0], contents);
}

void Tools::ReplaceAll
	(       wstring & str
	, const wstring & target
	, const wstring & replacement
	)
{
	if (target.empty() || target == replacement)
		return;
	size_t i(str.find(target));
	while(i != wstring::npos)
	{
		str.replace(i, target.length(), replacement);
		i = str.find(target, i + replacement.length());
	}
}

bool Tools::StartsWith(const wstring & sequence, const wchar_t * fragment)
{
	if (!fragment)
		return NULL;
	return StartsWith(sequence, fragment, fragment + wcslen(fragment));
}

void Tools::UnixTimeToFileTime
	(     time_t     unixTime
	, OUT FILETIME & fileTime
	)
{
	// Note that LONGLONG is a 64-bit value
	LONGLONG temp;
	temp = Int32x32To64(unixTime, 10000000) + 116444736000000000;
	fileTime.dwLowDateTime  = static_cast<DWORD>(temp);
	fileTime.dwHighDateTime = temp >> 32;
}

void Tools::UnixTimeToLocalSystemTime
	(     time_t       unixTime
	, OUT SYSTEMTIME & systemTime
	)
{
	FILETIME fileTime;
	UnixTimeToFileTime(unixTime, fileTime);
	FILETIME localTime;
	::FileTimeToLocalFileTime(&fileTime, &localTime);
	::FileTimeToSystemTime(&localTime, &systemTime);
}

time_t Tools::FileTimeToUnixTime
	( const FILETIME & fileTime
	)
{
	LONGLONG temp;
	temp = fileTime.dwHighDateTime;
	temp = (temp << 32) | fileTime.dwLowDateTime;
	return static_cast<time_t>(temp / 10000000 - 11644473600);
}

time_t Tools::SystemTimeToUnixTime
	( const SYSTEMTIME & systemTime
	)
{
	FILETIME fileTime;
	::SystemTimeToFileTime(&systemTime, &fileTime);
	return FileTimeToUnixTime(fileTime);
}
