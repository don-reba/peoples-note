#pragma once

class HtmlEditor
{
private:

	HWND hwnd;

public:

	HtmlEditor();

	~HtmlEditor();

	void SetHtml(const wchar_t * html);

	void GetHtml(std::wstring & html);

private:

	static LRESULT CALLBACK ProcessHTMLayoutNotify
		(UINT uMsg, WPARAM wParam, LPARAM lParam, LPVOID vParam);

	static LRESULT CALLBACK WndProc
		(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};
