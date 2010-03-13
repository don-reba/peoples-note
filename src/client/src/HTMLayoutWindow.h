#pragma once

#include "ISignalProvider.h"
#include "window.h"

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

// data

private:

	const wchar_t * const resourceId;

	std::vector<EventTarget> eventTargets;
	std::vector<EventRecord> eventRecords;

protected:

	signal SignalCreated;

// interface

protected:

	HTMLayoutWindow(const wchar_t * resourceId);

	template <typename T>
	void ConnectBehavior
		( std::string path
		, UINT        command
		, void (T::*event)()
		)
	{
		EventTarget target = { path, command, static_cast<EventType>(event) };
		eventTargets.push_back(target);
	}

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
