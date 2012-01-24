#include "stdafx.h"

#include "HtmlEditor.h"
#include "htmlayout.h"
#include "Test.h"

using namespace std;

struct EditorViewFixture
{
	HtmlEditor editor;
};

FIXTURE_TEST_CASE(TestEditorViewSetNote_List, EditorViewFixture)
{
	editor.SetHtml(L"<input type=\"checkbox\" />Line 1<div><input type=\"checkbox\" />Line 2</div>");

	wstring result;
	editor.GetHtml(result);

	wstring expected(L"\r\n  <input type=\"checkbox\" />Line 1<div><input type=\"checkbox\" />Line 2</div>");
	TEST_CHECK_EQUAL(result, expected);
}
