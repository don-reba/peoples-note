#pragma once

#include <string>
#include <windows.h>

namespace Tools
{
	std::string ConvertToAnsi(std::wstring str);

	std::wstring ConvertToUnicode(std::string str);

	std::wstring LoadStringResource(UINT id);
}
