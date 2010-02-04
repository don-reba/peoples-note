#include <string>
#include <windows.h>

namespace Tools
{
	std::wstring LoadStringResource(HINSTANCE instance, UINT id);
	std::wstring ConvertToUnicode(std::string str);
}
