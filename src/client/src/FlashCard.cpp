#include "stdafx.h"
#include "FlashCard.h"

#include "resourceppc.h"
#include "Tools.h"

#include <projects.h>

using namespace std;
using namespace Tools;

bool FlashCard::Exists() const
{
	return !GetName().empty();
}

wstring FlashCard::GetPath() const
{
	wstring name(GetName());
	if (name.empty())
		return L"";

	wstring path(L"\\");
	path.append(name);
	path.append(L"\\");
	path.append(LoadStringResource(IDS_DOC_FOLDER));
	return path;
}

wstring FlashCard::GetName()
{
	WIN32_FIND_DATA data = { 0 };
	HANDLE handle(::FindFirstFlashCard(&data));
	if (INVALID_HANDLE_VALUE != handle)
	{
		do
		{
			if (!IsValidData(data))
				continue;
			::FindClose(handle);
			return data.cFileName;
		}
		while (::FindNextFlashCard(handle, &data));
		::FindClose(handle);
	}
	return L"";
}

bool FlashCard::IsValidData(const WIN32_FIND_DATA & data)
{
	return data.cFileName && data.cFileName
		&& (0 != wcsicmp(data.cFileName, L"network"));
}
