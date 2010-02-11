#pragma once

#include "HtmlResource.h"

namespace Tools
{
	std::string ConvertToAnsi(std::wstring str);

	std::vector<unsigned char> ConvertToUtf8(std::wstring str);

	std::wstring ConvertToUnicode(std::string str);

	HtmlResource LoadHtmlResource(int id);

	std::wstring LoadStringResource(int id);
}
