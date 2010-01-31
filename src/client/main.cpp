#include "stdafx.h"
#include "main.h"
#include "htmlayout.h"


#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE g_hInst;			// current instance
HWND      g_hWndMenuBar;		// menu bar handle

// Forward declarations of functions included in this code module:
ATOM    yRegisterClass(HINSTANCE, LPTSTR);
BOOL    InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

BOOL             GetHtmlResource(int id, /*out*/PBYTE& pb, /*out*/DWORD& cb);
LRESULT CALLBACK HTMLayoutNotifyHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, LPVOID vParam);


int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPTSTR    lpCmdLine,
                   int       nCmdShow)
{
	MSG msg;

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	HACCEL hAccelTable;
	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CLIENT));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
		{
			if (!HTMLayoutTranslateMessage(&msg))
				TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance, LPTSTR szWindowClass)
{
	WNDCLASS wc;

	wc.style         = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc   = WndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = hInstance;
	wc.hIcon         = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CLIENT));
	wc.hCursor       = 0;
	wc.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = 0;
	wc.lpszClassName = szWindowClass;

	return RegisterClass(&wc);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    HWND hWnd;
    TCHAR szTitle[MAX_LOADSTRING];		// title bar text
    TCHAR szWindowClass[MAX_LOADSTRING];	// main window class name

    g_hInst = hInstance; // Store instance handle in our global variable

#if defined(WIN32_PLATFORM_PSPC) || defined(WIN32_PLATFORM_WFSP)
    // SHInitExtraControls should be called once during your application's initialization to initialize any
    // of the device specific controls such as CAPEDIT and SIPPREF.
    SHInitExtraControls();
#endif // WIN32_PLATFORM_PSPC || WIN32_PLATFORM_WFSP

    LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING); 
    LoadString(hInstance, IDC_CLIENT, szWindowClass, MAX_LOADSTRING);

#if defined(WIN32_PLATFORM_PSPC) || defined(WIN32_PLATFORM_WFSP)
    //If it is already running, then focus on the window, and exit
    hWnd = FindWindow(szWindowClass, szTitle);	
    if (hWnd) 
    {
        // set focus to foremost child window
        // The "| 0x00000001" is used to bring any owned windows to the foreground and
        // activate them.
        SetForegroundWindow((HWND)((ULONG) hWnd | 0x00000001));
        return 0;
    } 
#endif // WIN32_PLATFORM_PSPC || WIN32_PLATFORM_WFSP

    if (!MyRegisterClass(hInstance, szWindowClass))
    {
    	return FALSE;
    }

    hWnd = CreateWindow(szWindowClass, szTitle, WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);

    if (!hWnd)
    {
        return FALSE;
    }

#ifdef WIN32_PLATFORM_PSPC
    // When the main window is created using CW_USEDEFAULT the height of the menubar (if one
    // is created is not taken into account). So we resize the window after creating it
    // if a menubar is present
    if (g_hWndMenuBar)
    {
        RECT rc;
        RECT rcMenuBar;

        GetWindowRect(hWnd, &rc);
        GetWindowRect(g_hWndMenuBar, &rcMenuBar);
        rc.bottom -= (rcMenuBar.bottom - rcMenuBar.top);
		
        MoveWindow(hWnd, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, FALSE);
    }
#endif // WIN32_PLATFORM_PSPC

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);


    return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int wmId, wmEvent;

	// HTMLayout +
	// HTMLayout could be created as sepearate window
	// using CreateWindow API.
	// But in this case we are attaching HTMLayout functionality
	// to the existing window delegating windows message handling to
	// HTMLayoutProcND function.

	LRESULT lResult;
	BOOL    bHandled;

	lResult = HTMLayoutProcND(hWnd,message,wParam,lParam, &bHandled);
	if(bHandled)
		return lResult;

	// HTMLayout -

    static SHACTIVATEINFO s_sai;
	
    switch (message) 
    {
        case WM_COMMAND:
            wmId    = LOWORD(wParam); 
            wmEvent = HIWORD(wParam); 
            // Parse the menu selections:
            switch (wmId)
            {
                case IDM_HELP_ABOUT:
                    // TODO
                    break;
#ifdef WIN32_PLATFORM_PSPC
                case IDM_OK:
                    SendMessage (hWnd, WM_CLOSE, 0, 0);				
                    break;
#endif // WIN32_PLATFORM_PSPC
                default:
                    return DefWindowProc(hWnd, message, wParam, lParam);
            }
            break;
        case WM_CREATE:
			{
#ifdef SHELL_AYGSHELL
				SHMENUBARINFO mbi;

				memset(&mbi, 0, sizeof(SHMENUBARINFO));
				mbi.cbSize     = sizeof(SHMENUBARINFO);
				mbi.hwndParent = hWnd;
				mbi.nToolBarId = IDR_MENU;
				mbi.hInstRes   = g_hInst;

				if (!SHCreateMenuBar(&mbi)) 
				{
					g_hWndMenuBar = NULL;
				}
				else
				{
					g_hWndMenuBar = mbi.hwndMB;
				}

				// Initialize the shell activate info structure
				memset(&s_sai, 0, sizeof (s_sai));
				s_sai.cbSize = sizeof (s_sai);
#endif // SHELL_AYGSHELL
				HTMLayoutSetCallback(hWnd, &HTMLayoutNotifyHandler,0);

				PBYTE pb; DWORD cb;
				if(GetHtmlResource(IDH_MAIN, pb,cb))
				{
					HTMLayoutLoadHtml(hWnd,pb,cb);
				}
			}
			break;
		case WM_BEHAVIOR_NOTIFY: //see behaviors/notifications.h
			break;
        case WM_DESTROY:
#ifdef SHELL_AYGSHELL
            CommandBar_Destroy(g_hWndMenuBar);
#endif // SHELL_AYGSHELL
            PostQuitMessage(0);
            break;

        case WM_ACTIVATE:
            // Notify shell of our activate message
            SHHandleWMActivate(hWnd, wParam, lParam, &s_sai, FALSE);
            break;
        case WM_SETTINGCHANGE:
            SHHandleWMSettingChange(hWnd, wParam, lParam, &s_sai);
            break;

        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

#ifndef RT_HTML
#define RT_HTML         MAKEINTRESOURCE(23)
#endif

BOOL GetHtmlResource(int id, /*out*/PBYTE& pb, /*out*/DWORD& cb)
{
	// Find specified resource and check if ok

	HRSRC hrsrc = ::FindResource(g_hInst, MAKEINTRESOURCE(id), RT_HTML);

	if(!hrsrc)
		return false;

	// Load specified resource and check if ok

	HGLOBAL hgres = ::LoadResource(g_hInst, hrsrc);
	if(!hgres) return FALSE;

	// Retrieve resource data and check if ok

	pb = (PBYTE)::LockResource(hgres); if (!pb) return FALSE;
	cb = ::SizeofResource(g_hInst, hrsrc); if (!cb) return FALSE;

	return TRUE;
}

LRESULT OnAttachBehavior(LPNMHL_ATTACH_BEHAVIOR lpAB );

// HTMLayout specific notification handler.
LRESULT CALLBACK HTMLayoutNotifyHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, LPVOID vParam)
{
	// all HTMLayout notification are comming here.
	NMHDR*  phdr = (NMHDR*)lParam;

	switch(phdr->code)
	{
		case HLN_CREATE_CONTROL:    break; //return OnCreateControl((LPNMHL_CREATE_CONTROL) lParam);
		case HLN_CONTROL_CREATED:   break; //return OnControlCreated((LPNMHL_CREATE_CONTROL) lParam);
		case HLN_DESTROY_CONTROL:   break; //return OnDestroyControl((LPNMHL_DESTROY_CONTROL) lParam);
		case HLN_LOAD_DATA:         break; //return OnLoadData((LPNMHL_LOAD_DATA) lParam);
		case HLN_DATA_LOADED:       break; //return OnDataLoaded((LPNMHL_DATA_LOADED)lParam);
		case HLN_DOCUMENT_COMPLETE: break; //return OnDocumentComplete();
		case HLN_ATTACH_BEHAVIOR:   return OnAttachBehavior((LPNMHL_ATTACH_BEHAVIOR)lParam );
	}
	return 0;
}

LRESULT OnAttachBehavior(LPNMHL_ATTACH_BEHAVIOR lpab )
{
	// attach custom behaviors
	htmlayout::event_handler *pb = htmlayout::behavior::find(lpab->behaviorName, lpab->element);
	//assert(pb);
	if(pb)
	{
		lpab->elementTag  = pb;
		lpab->elementProc = htmlayout::behavior::element_proc;
		lpab->elementEvents = pb->subscribed_to;
	}
	return 0;
}