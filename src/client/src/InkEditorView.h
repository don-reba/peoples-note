#pragma once
#include "IInkEditorView.h"

#include "htmlayout_graphin.h"
#include "window.h"

class InkEditorView : public Window, public IInkEditorView
{
private:

	struct BITMAPINFO_BF
	{
		BITMAPINFOHEADER bmiHeader;
		DWORD bmiColorsR;
		DWORD bmiColorsG;
		DWORD bmiColorsB;

		BITMAPINFO * GetBitmapInfo()
		{
			return reinterpret_cast<BITMAPINFO*>(this);
		}
	};

// data

private:

	SHACTIVATEINFO activateInfo;
	HINSTANCE      instance;
	HWND           menuBar;
	HWND           parent;

	HIMG img;
	HGFX gfx;

	POINT lineStart;
	RECT  drawingBounds;
	bool  isDrawing;
	bool  isDrawingEmpty;

	const wchar_t * penColor;
	int             penWidth;

	signal SignalAccept;
	signal SignalCancel;
	signal SignalPenChanged;

// interface

public:

	InkEditorView(HINSTANCE instance);

	void Create(HWND parent);

// IInkEditorView implementation

public:

	virtual void ConnectAccept(slot_type OnAccept);

	virtual void ConnectCancel(slot_type OnCancel);

	virtual void ConnectPenChanged(slot_type OnPenChanged);

	virtual void GetImage(Blob & blob);

	virtual const wchar_t * GetPenColor();

	virtual int GetPenWidth();

	virtual void Hide();

	virtual void SetPen(int width, COLORREF color);

	virtual void Show();

// window message handlers

private:

	void OnActivate        (Msg<WM_ACTIVATE>    & msg);
	void OnCommand         (Msg<WM_COMMAND>     & msg);
	void OnEraseBackground (Msg<WM_ERASEBKGND>  & msg);
	void OnMouseDown       (Msg<WM_LBUTTONDOWN> & msg);
	void OnMouseMove       (Msg<WM_MOUSEMOVE>   & msg);
	void OnMouseUp         (Msg<WM_LBUTTONUP>   & msg);
	void OnPaint           (Msg<WM_PAINT>       & msg);
	void OnSize            (Msg<WM_SIZE>        & msg);

	virtual void ProcessMessage(WndMsg &msg);

// utility functions

private:

	void AddToDrawingBounds(const POINT & point);

	ATOM RegisterClass(const std::wstring & wndClass);

	void ResizeWindow();
};
