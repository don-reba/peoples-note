#pragma once

#include "ISignalProvider.h"
#include "window.h"

#include <boost/ptr_container/ptr_vector.hpp>
#include <map>

struct htmlayout_dom_element;
typedef htmlayout_dom_element* HELEMENT;
struct BEHAVIOR_EVENT_PARAMS;
struct NMHL_LOAD_DATA;

class HTMLayoutWindow : public Window, public ISignalProvider
{
// data types

private:

	typedef void (HTMLayoutWindow::*EventType)();

	struct EventRecord
	{
		HELEMENT  element;
		UINT      command;
		EventType event;
	};

// data

private:

	const wchar_t * const resourceId;

	std::vector<EventRecord>  eventRecords;

protected:

	signal SignalCreated;

// interface

protected:

	HTMLayoutWindow(const wchar_t * resourceId);

	template <typename T>
	void ConnectBehavior
		( const char * path
		, UINT         command
		, void (T::*event)()
		)
	{
		ConnectBehavior(path, command, static_cast<EventType>(event));
	}

	void ConnectBehavior
		( const char * path
		, UINT         command
		, EventType    event
		);

	virtual void RegisterEventHandlers() {}

// window message handlers

private:

	void OnCreate  (Msg<WM_CREATE>  & msg);
	void OnDestroy (Msg<WM_DESTROY> & msg);

protected:

	virtual void ProcessMessage(WndMsg &msg);

// HTMLayout message handlers

protected:

	virtual BOOL OnBehavior(BEHAVIOR_EVENT_PARAMS * params);

	virtual BOOL OnLoadData(NMHL_LOAD_DATA * params);

private:

	bool ProcessHtmLayout(WndMsg & msg);

	BOOL ProcessHTMLayoutEvent
		( HELEMENT element
		, UINT     event
		, LPVOID   params
		);

	static BOOL CALLBACK ProcessHTMLayoutEvent
		( LPVOID   tag
		, HELEMENT element
		, UINT     event
		, LPVOID   params
		);

	LRESULT ProcessHTMLayoutNotify
		( UINT   uMsg
		, WPARAM wParam
		, LPARAM lParam
		);

	static LRESULT CALLBACK ProcessHTMLayoutNotify
		( UINT   uMsg
		, WPARAM wParam
		, LPARAM lParam
		, LPVOID vParam
		);
};
