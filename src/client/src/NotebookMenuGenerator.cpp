#include "stdafx.h"
#include "NotebookMenuGenerator.h"

#include "Tools.h"

using namespace std;

//----------
// interface
//----------

void NotebookMenuGenerator::GetMenuHtml
		( const NotebookList & notebooks
		, int                  groupSize
		, wstring            & html
		)
{
	int  depth     (0);
	int  i         (0);
	int  maxDepth  (GetMaxDepth(notebooks.size(), groupSize));
	bool asc       (false);

	html.clear();
	if (notebooks.empty())
		return;

	for (;;)
	{
		if (asc && (i == notebooks.size() || 0 == i % GetLeafCount(groupSize, depth, maxDepth)))
		{
			--depth;
			if (depth < 0)
				return;
			PrintCloseMenu(html);
		}
		else
		{
			const Notebook & notebook(notebooks.at(i));
			if (depth == maxDepth)
			{
				asc = true;
				PrintMenuEntry(html, notebook.name, notebook.guid);
				++i;
			}
			else
			{
				asc = false;
				PrintOpenMenu(html, notebook.name);
				++depth;
			}
		}
	}
}

//------------------
// utility functions
//------------------

int NotebookMenuGenerator::GetLeafCount(int groupSize, int depth, int maxDepth)
{
	return static_cast<int>
		( pow
			( static_cast<double>(groupSize)
			, static_cast<double>(maxDepth + 1  - depth)
			)
		);
}

int NotebookMenuGenerator::GetMaxDepth(int notebookCount, int groupSize)
{
	return static_cast<int>
		( log(static_cast<double>(notebookCount))
		/ log(static_cast<double>(groupSize))
		);
}

void NotebookMenuGenerator::PrintCloseMenu(wstring & html)
{
	html.append(L"</menu></li>");
}
void NotebookMenuGenerator::PrintMenuEntry
	(       wstring & html
	, const wstring & name
	, const Guid    & guid
	)
{
	html.append(L"<li guid=\"");
	html.append(Tools::ConvertToUnicode(guid));
	html.append(L"\">");
	html.append(name);
	html.append(L"</li>");
}

void NotebookMenuGenerator::PrintOpenMenu
	(       wstring & html
	, const wstring & title
	)
{
	html.append(L"<li>");
	html.append(title);
	html.append(L"<menu>");
}
