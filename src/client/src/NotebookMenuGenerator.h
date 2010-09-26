#pragma once

#include "Guid.h"
#include "Notebook.h"

class NotebookMenuGenerator
{
// interface

public:

	static void GetMenuHtml
		( const NotebookList & notebooks
		, int                  groupSize
		, std::wstring       & html
		);

// utility functions

private:

	static int GetLeafCount(int groupSize, int depth, int maxDepth);

	static int GetMaxDepth(int notebookCount, int groupSize);

	static void PrintCloseMenu(std::wstring & html);

	static void PrintMenuEntry
		(       std::wstring & html
		, const std::wstring & name
		, const Guid         & guid
		);

	static void PrintOpenMenu
		(       std::wstring & html
		, const std::wstring & name
		);
};
