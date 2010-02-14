#include "stdafx.h"
#include "HTMLayoutWindow.h"
#include "Tools.h"

using namespace std;
using namespace Tools;

HTMLayoutWindow::HTMLayoutWindow(int resourceId)
	: resourceId (resourceId)
{
}

//------------------------
// window message handlers
//------------------------

void HTMLayoutWindow::OnCreate(Msg<WM_CREATE> & msg)
{
	HTMLayoutSetCallback
		( hwnd_
		, &HTMLayoutWindow::ProcessHTMLayoutNotify
		, this
		);
	HTMLayoutWindowAttachEventHandler
		( hwnd_
		, &HTMLayoutWindow::ProcessHTMLayoutEvent
		, this
		, HANDLE_ALL
		);

	HtmlResource resource = LoadHtmlResource(resourceId);
	if (!HTMLayoutLoadHtml(hwnd_, resource.data, resource.size))
		throw exception("Failed to load interface.");

	msg.handled_ = true;
}

void HTMLayoutWindow::OnDestroy(Msg<WM_DESTROY> & msg)
{
	HTMLayoutWindowDetachEventHandler
		( hwnd_
		, &HTMLayoutWindow::ProcessHTMLayoutEvent
		, this
		);
	msg.handled_ = true;
}

void HTMLayoutWindow::ProcessMessage(WndMsg &msg)
{
	static Handler mmp[] =
	{
		&HTMLayoutWindow::OnCreate,
		&HTMLayoutWindow::OnDestroy,
	};
	try
	{
		if (!ProcessHtmLayout(msg) && !Handler::Call(mmp, this, msg))
			__super::ProcessMessage(msg);
	}
	catch(exception e)
	{
		MessageBox
			( hwnd_
			, ConvertToUnicode(e.what()).c_str()
			, L"Error"
			, MB_OK | MB_ICONERROR
			);
	}
}

//---------------------------
// HTMLayout message handlers
//---------------------------

LRESULT HTMLayoutWindow::OnAttachBehavior(LPNMHL_ATTACH_BEHAVIOR lpab)
{
	// attach custom behaviors
	htmlayout::event_handler * handler = htmlayout::behavior::find
		( lpab->behaviorName
		, lpab->element
		);
	if(handler)
	{
		lpab->elementTag    = handler;
		lpab->elementProc   = htmlayout::behavior::element_proc;
		lpab->elementEvents = handler->subscribed_to;
	}
	return 0;
}

bool HTMLayoutWindow::ProcessHtmLayout(WndMsg &msg)
{
	LRESULT result;
	BOOL    handled;
	result = HTMLayoutProcND(hwnd_, msg.id_, msg.wprm_, msg.lprm_, &handled);
	if (handled)
	{
		msg.result_  = result;
		msg.handled_ = true;
	}
	return handled != FALSE;
}

BOOL HTMLayoutWindow::ProcessHTMLayoutEvent
	( LPVOID   tag
	, HELEMENT element
	, UINT     event
	)
{
	return FALSE;
}

BOOL CALLBACK HTMLayoutWindow::ProcessHTMLayoutEvent
	( LPVOID   tag
	, HELEMENT element
	, UINT     event
	, LPVOID   params
	)
{
	return reinterpret_cast<HTMLayoutWindow*>(params)->
		ProcessHTMLayoutEvent(tag, element, event);
}

LRESULT HTMLayoutWindow::ProcessHTMLayoutNotify
	( UINT   uMsg
	, WPARAM wParam
	, LPARAM lParam
	)
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

LRESULT CALLBACK HTMLayoutWindow::ProcessHTMLayoutNotify
	( UINT   uMsg
	, WPARAM wParam
	, LPARAM lParam
	, LPVOID vParam
	)
{
	return reinterpret_cast<HTMLayoutWindow*>(vParam)
		->ProcessHTMLayoutNotify(uMsg, wParam, lParam);
}
