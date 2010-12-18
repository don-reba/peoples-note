#include "stdafx.h"
#include "HTMLayoutWindow.h"

#include "Tools.h"

#include "htmlayout.h"

using namespace htmlayout;
using namespace htmlayout::dom;
using namespace std;
using namespace Tools;

//----------
// interface
//----------

HTMLayoutWindow::HTMLayoutWindow(const wchar_t * resourceId, bool highRes)
	: resourceId    (resourceId)
	, highRes       (highRes)
	, isHtmlDataSet (false)
	, htmlData      (NULL)
	, htmlUri       (NULL)
{
}

void HTMLayoutWindow::ConnectBehavior
		( const char * path
		, UINT         command
		, EventType    event
		)
{
	element root(element::root_element(hwnd_));
	vector<element> elements;
	root.find_all(elements, path);
	foreach (element element, elements)
		ConnectBehavior(element, command, event);
}

void HTMLayoutWindow::ConnectBehavior
	( HELEMENT  element
	, UINT      command
	, EventType event
	)
{
	EventRecord record = { element, command, event };
	eventRecords.push_back(record);
}

void HTMLayoutWindow::ConnectLoadHtmlData(slot_type OnLoadHtmlData)
{
	SignalLoadHtmlData.connect(OnLoadHtmlData);
}

void HTMLayoutWindow::DisconnectBehavior(const char * path)
{
	element root(element::root_element(hwnd_));
	vector<element> elements;
	root.find_all(elements, path);
	foreach (element element, elements)
		DisconnectBehavior(element);
}

void HTMLayoutWindow::DisconnectBehavior(HELEMENT element)
{
	eventRecords.erase(find(eventRecords.begin(), eventRecords.end(), element));
}

HELEMENT HTMLayoutWindow::FindFirstElement(const char * selector)
{
	element root(element::root_element(hwnd_));
	element result(root.find_first(selector));
	if (!result)
	{
		string message(selector);
		message.append(" not found.");
		throw std::exception(message.c_str());
	}
	return result;
}

const wchar_t * HTMLayoutWindow::GetHtmlUri()
{
	return htmlUri;
}

void HTMLayoutWindow::SetHtmlData(const BYTE * data, DWORD size)
{
	htmlData      = data;
	htmlDataSize  = size;
	isHtmlDataSet = true;
}

BYTE * HTMLayoutWindow::GetHtmlData()
{
	return const_cast<BYTE*>(htmlData);
}

DWORD HTMLayoutWindow::GetHtmlDataSize()
{
	return htmlDataSize;
}

void HTMLayoutWindow::SetHtmlUri(const wchar_t * uri)
{
	htmlUri = uri;
}

bool HTMLayoutWindow::UseHtmlData()
{
	bool isHtmlDataSet = this->isHtmlDataSet;
	this->isHtmlDataSet = false;
	return isHtmlDataSet;
}

//------------------------
// window message handlers
//------------------------

void HTMLayoutWindow::OnCreate(Msg<WM_CREATE> & msg)
{
	HTMLayoutSetOption(hwnd_, HTMLAYOUT_FONT_SMOOTHING, highRes ? 0 : 1);

	HTMLayoutSetCallback
		( hwnd_
		, &HTMLayoutWindow::ProcessHTMLayoutNotify
		, this
		);
	HTMLayoutWindowAttachEventHandler
		( hwnd_
		, &HTMLayoutWindow::ProcessHTMLayoutEvent
		, this
		, HANDLE_BEHAVIOR_EVENT|HANDLE_FOCUS|HANDLE_KEY
		);

	HtmlResource resource = LoadHtmlResource(resourceId, highRes);
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

void HTMLayoutWindow::ProcessMessage(WndMsg & msg)
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
	catch (const std::exception & e)
	{
		DEBUGMSG(true, (L"%s\n", ConvertToUnicode(e.what()).c_str()));
		throw e;
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
			(this->*record.event)(params);
			return TRUE;
		}
	}
	return FALSE;
}

BOOL HTMLayoutWindow::OnFocus(FOCUS_PARAMS * params)
{
	return FALSE;
}

BOOL HTMLayoutWindow::OnKey(KEY_PARAMS * params)
{
	return FALSE;
}

BOOL HTMLayoutWindow::OnLoadData(NMHL_LOAD_DATA * params)
{
	htmlUri = params->uri;
	SignalLoadHtmlData();
	if (UseHtmlData())
	{
		if (htmlDataSize > 0)
		{
			params->outData     = const_cast<BYTE*>(htmlData);
			params->outDataSize = htmlDataSize;
			return LOAD_OK;
		}
		else
		{
			return LOAD_DISCARD;
		}
	}
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
	switch (event & ~(HANDLED | SINKING | BUBBLING))
	{
	case HANDLE_BEHAVIOR_EVENT:
		return OnBehavior(reinterpret_cast<BEHAVIOR_EVENT_PARAMS*>(params));
	case HANDLE_FOCUS:
		return OnFocus(reinterpret_cast<FOCUS_PARAMS*>(params));
	case HANDLE_KEY:
		return OnKey(reinterpret_cast<KEY_PARAMS*>(params));
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
