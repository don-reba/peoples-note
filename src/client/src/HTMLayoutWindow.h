#pragma once

#include "ISignalProvider.h"
#include "window.h"

#include "htmlayout.h"

#include <map>

class HTMLayoutWindow : public Window, public ISignalProvider
{
// data types

private:

	typedef void (HTMLayoutWindow::*EventType)();

	struct EventTarget
	{
		std::string path;
		UINT        command;
		EventType   event;
	};

	struct EventRecord
	{
		HELEMENT  element;
		UINT      command;
		EventType event;
	};

	typedef boost::shared_ptr<htmlayout::event_handler> EventHandlerPtr;

	struct EventHandler
	{
		std::string     path;
		EventHandlerPtr handler;
	};

// data

private:

	const wchar_t * const resourceId;

	std::vector<EventTarget>  eventTargets;
	std::vector<EventRecord>  eventRecords;
	std::vector<EventHandler> eventHandlers;

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

	void ConnectBehavior
		( const char      * path
		, EventHandlerPtr   handler
		);

// utility functions

private:

	void AttachBehaviors();

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
