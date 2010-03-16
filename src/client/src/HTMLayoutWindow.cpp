#include "stdafx.h"
#include "HTMLayoutWindow.h"

#include "Tools.h"

using namespace htmlayout;
using namespace std;
using namespace Tools;

//----------
// interface
//----------

HTMLayoutWindow::HTMLayoutWindow(const wchar_t * resourceId)
	: resourceId (resourceId)
{
}

void HTMLayoutWindow::ConnectBehavior
		( const char * path
		, UINT         command
		, EventType    event
		)
{
	dom::element root(dom::element::root_element(hwnd_));
	vector<dom::element> elements;
	root.find_all(elements, path);
	foreach (dom::element element, elements)
	{
		EventRecord record = { element, command, event };
		eventRecords.push_back(record);
	}
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
		, HANDLE_BEHAVIOR_EVENT
		);

	HtmlResource resource = LoadHtmlResource(resourceId);
	if (!HTMLayoutLoadHtml(hwnd_, resource.data, resource.size))
		throw exception("Failed to load interface.");

	RegisterEventHandlers();

	SignalCreated();

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

BOOL HTMLayoutWindow::OnBehavior(BEHAVIOR_EVENT_PARAMS * params)
{
	const HELEMENT element = params->heTarget;
	const UINT     command = params->cmd;
	foreach (const EventRecord & record, eventRecords)
	{
		if (record.element == element && record.command == command)
		{
			(this->*record.event)();
			return TRUE;
		}
	}
	return FALSE;
}

BOOL HTMLayoutWindow::OnLoadData(NMHL_LOAD_DATA * params)
{
	HtmlResource resource(LoadHtmlResource(params->uri));
	params->outData     = resource.data;
	params->outDataSize = resource.size;
	return LOAD_OK;
}

bool HTMLayoutWindow::ProcessHtmLayout(WndMsg & msg)
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
	( HELEMENT element
	, UINT     event
	, LPVOID   params
	)
{
	switch (event)
	{
	case HANDLE_BEHAVIOR_EVENT:
		return OnBehavior(reinterpret_cast<BEHAVIOR_EVENT_PARAMS*>(params));
	};
	return FALSE;
}

BOOL CALLBACK HTMLayoutWindow::ProcessHTMLayoutEvent
	( LPVOID   tag
	, HELEMENT element
	, UINT     event
	, LPVOID   params
	)
{
	return reinterpret_cast<HTMLayoutWindow*>(tag)->
		ProcessHTMLayoutEvent(element, event, params);
}

LRESULT HTMLayoutWindow::ProcessHTMLayoutNotify
	( UINT   uMsg
	, WPARAM wParam
	, LPARAM lParam
	)
{
	NMHDR*  phdr = reinterpret_cast<NMHDR*>(lParam);
	switch(phdr->code)
	{
	case HLN_LOAD_DATA:
		return OnLoadData(reinterpret_cast<NMHL_LOAD_DATA*>(lParam));
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
