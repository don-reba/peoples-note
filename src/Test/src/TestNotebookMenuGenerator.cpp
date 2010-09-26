#include "stdafx.h"
#include "NotebookMenuGenerator.h"

using namespace boost;
using namespace std;

Notebook CreateNotebook(const wchar_t * name, const char * guid)
{
	Notebook notebook;
	notebook.name = name;
	notebook.guid = Guid(guid);
	return notebook;
}

BOOST_AUTO_TEST_CASE(NotebookMenuGenerator_GetMenuHtml)
{
	const int groupSize(2);

	NotebookList notebooks;
	wstring      html;

	notebooks.clear();
	NotebookMenuGenerator::GetMenuHtml(notebooks, groupSize, html);
	BOOST_CHECK_EQUAL(html, L"");

	notebooks.push_back(CreateNotebook(L"n0", "guid0"));
	NotebookMenuGenerator::GetMenuHtml(notebooks, groupSize, html);
	BOOST_CHECK_EQUAL(html, L"<li guid=\"guid0\">n0</li>");

	notebooks.clear();
	notebooks.push_back(CreateNotebook(L"n0", "guid0"));
	notebooks.push_back(CreateNotebook(L"n1", "guid1"));
	NotebookMenuGenerator::GetMenuHtml(notebooks, groupSize, html);
	BOOST_CHECK_EQUAL
		( html
		,
			L"<li>n0<menu>"
				L"<li guid=\"guid0\">n0</li>"
				L"<li guid=\"guid1\">n1</li>"
			L"</menu></li>"
		);

	notebooks.clear();
	notebooks.push_back(CreateNotebook(L"n0", "guid0"));
	notebooks.push_back(CreateNotebook(L"n1", "guid1"));
	notebooks.push_back(CreateNotebook(L"n2", "guid2"));
	NotebookMenuGenerator::GetMenuHtml(notebooks, groupSize, html);
	BOOST_CHECK_EQUAL
		( html
		,
			L"<li>n0<menu>"
				L"<li guid=\"guid0\">n0</li>"
				L"<li guid=\"guid1\">n1</li>"
			L"</menu></li>"
			L"<li>n2<menu>"
				L"<li guid=\"guid2\">n2</li>"
			L"</menu></li>"
		);
}