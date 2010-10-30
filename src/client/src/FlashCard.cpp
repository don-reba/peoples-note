#include "stdafx.h"
#include "FlashCard.h"

#include "resourceppc.h"
#include "Tools.h"

#include <projects.h>

using namespace std;
using namespace Tools;

bool FlashCard::Exists() const
{
	WIN32_FIND_DATA find = { 0 };
	::CloseHandle(::FindFirstFlashCard(&find));
	return find.cFileName && *find.cFileName;
}

bool FlashCard::GetPath(wstring & path) const
{
	WIN32_FIND_DATA find = { 0 };
	::CloseHandle(::FindFirstFlashCard(&find));
	if (!find.cFileName || !*find.cFileName)
		return false;

	path = L"\\";
	path.append(find.cFileName);
	path.append(L"\\");
	path.append(LoadStringResource(IDS_DOC_FOLDER));

	return true;
}
