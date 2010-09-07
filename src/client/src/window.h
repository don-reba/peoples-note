//--------------------------------------------------------------------------
// SWL - Small Window Library
// The code is based on http://rsdn.ru/Forum/Message.aspx?mid=253425&only=1.
// The idea is not to create a wrapper around WinAPI,
// but to make use of WinAPI enjoyable.
//--------------------------------------------------------------------------

#pragma once

#include <map>

//-------------------------------------------------------------
// base window class definition and implementation
//-------------------------------------------------------------

struct WndMsg
{
	WndMsg(UINT id, LPARAM lprm, WPARAM wprm)
		: id_      (id)
		, lprm_    (lprm)
		, wprm_    (wprm)
		, result_  (0)
		, handled_ (false)
	{}
	const UINT id_;
	LPARAM     lprm_;
	WPARAM     wprm_;
	LRESULT    result_;
	bool       handled_;
};

struct Window
{
	Window() : hwnd_(NULL) {}
	virtual ~Window() { hwnd_ = NULL; };
	virtual void ProcessMessage(WndMsg &) {}
protected:
	// window message pump
	template <typename T>
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		// prevent processing until we get the object pointer
		static bool initialized(false);
		if (WM_CREATE == uMsg)
		{
			LPCREATESTRUCT cs(reinterpret_cast<LPCREATESTRUCT>(lParam));
			reinterpret_cast<Window*>(cs->lpCreateParams)->hwnd_ = hWnd;
			SetWindowLong
				( hWnd
				, GWL_USERDATA
				, reinterpret_cast<LONG>(cs->lpCreateParams)
				);
			initialized = true;
		}
		if (!initialized)
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
		// retrieve the object pointer
		Window *obj(reinterpret_cast<Window*>(GetWindowLong(hWnd, GWL_USERDATA)));
		// process the message
		WndMsg msg(uMsg, lParam, wParam);
		obj->ProcessMessage(msg); // virtual
		// reset initialized state on exit
		if (WM_DESTROY == uMsg)
			initialized = false;
		return msg.handled_ ? msg.result_ : DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	// dialog message pump
	template <typename T>
	static INT_PTR CALLBACK DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		// prevent processing until we get the object pointer
		static bool initialized(false);
		if (WM_INITDIALOG == uMsg)
		{
			reinterpret_cast<Window*>(lParam)->hwnd_ = hWnd;
			SetWindowLong(hWnd, GWL_USERDATA, lParam);
			initialized = true;
		}
		if (!initialized)
			return FALSE;
		// retrieve the object pointer
		Window *obj(reinterpret_cast<Window*>(GetWindowLong(hWnd, GWL_USERDATA)));
		// process the message
		WndMsg msg(uMsg, lParam, wParam);
		obj->ProcessMessage(msg); // virtual
		// reset initialized state on exit
		if (WM_DESTROY == uMsg)
			initialized = false;
		if (msg.handled_)
			SetWindowLong(hWnd, DWL_MSGRESULT, msg.result_);
		return msg.handled_ ? msg.result_ : FALSE;
	}
public:
	HWND hwnd_;
};

struct Msg_
{
	Msg_(WndMsg &msg)
		:lprm_(msg.lprm_)
		,wprm_(msg.wprm_)
		,result_(msg.result_)
		,handled_(msg.handled_)
	{}
	LPARAM  &lprm_;
	WPARAM  &wprm_;
	LRESULT &result_;
	bool    &handled_;
};

template <UINT id>
struct Msg : Msg_
{
	Msg(WndMsg &msg)
		:Msg_(msg)
	{}
};

class Handler
{
	struct SortedHandlers
	{
		typedef std::map<UINT, Handler*> MapType;
		typedef MapType::iterator        MapTypeIter;
		MapType handlers_;
	public:
		template <size_t N>
		SortedHandlers(Handler(&mmp)[N])
		{
			for (int i(0); i != N; ++i)
				handlers_.insert(MapType::value_type(mmp[i].id_, &mmp[i]));
		}
		Handler* GetHandler(UINT id)
		{
			MapTypeIter iter(handlers_.find(id));
			return (handlers_.end() == iter) ? 0 : iter->second;
		}
	};
	struct CallProxyBase
	{
		virtual void Call(WndMsg&, Window*) = 0;
		virtual ~CallProxyBase() {}
	};
	template <UINT id, typename T>
	struct CallProxy : CallProxyBase
	{
		typedef void (T::*MfnType)(Msg<id>&);
		MfnType fn_;
		CallProxy(MfnType fn)
			:fn_(fn)
		{}
		void Call(WndMsg &wnd_msg, Window *wnd)
		{
			Msg<id> msg(wnd_msg);
			(static_cast<T*>(wnd)->*fn_)(msg);
		}
	};
	CallProxyBase *proxy_;
public:
	const UINT id_;
	template <UINT id, typename T>
	Handler(void (T::*fn)(Msg<id>&))
		:id_(id)
	{
		static CallProxy<id, T> proxy(fn);
		proxy_ = &proxy;
	}
	template <class T, UINT N>
	static bool Call(Handler(&mmp)[N], T *wnd, WndMsg &msg)
	{
		static SortedHandlers handlers(mmp);
		if (Handler *handler = handlers.GetHandler(msg.id_))
		{
			handler->proxy_->Call(msg, wnd);
			return msg.handled_;
		}
		return false;
	}
};

//---------------------
// some common messages
//---------------------

enum ActiveState
{
	Active      = WA_ACTIVE,
	ClickActive = WA_CLICKACTIVE,
	Inactive    = WA_INACTIVE
};

template <>
struct Msg<WM_ACTIVATE> : Msg_
{
	Msg(WndMsg &msg) : Msg_(msg) {}
	ActiveState GetActiveState() {
		return static_cast<ActiveState>(LOWORD(wprm_));
	}
	bool IsMinimized() {
		return HIWORD(wprm_) != 0;
	}
	HWND GetPreviousWindow() {
		return reinterpret_cast<HWND>(lprm_);
	}
};

template <>
struct Msg<WM_CAPTURECHANGED> : Msg_
{
	Msg(WndMsg &msg) : Msg_(msg) {}
	HWND Reciever() const {
		return reinterpret_cast<HWND>(lprm_);
	}
};

template <>
struct Msg<WM_CLOSE> : Msg_
{
	Msg(WndMsg &msg) : Msg_(msg) {}
};

template <>
struct Msg<WM_COMMAND> : Msg_
{
	Msg(WndMsg &msg) :Msg_(msg) {}
	int CtrlId() const {
		return LOWORD(wprm_);
	}
	UINT NotifyCode() const {
		return HIWORD(wprm_);
	}
	HWND CtrlHwnd() const {
		return reinterpret_cast<HWND>(lprm_);
	}
};

template <>
struct Msg<WM_CREATE> : Msg_
{
	Msg(WndMsg &msg) : Msg_(msg) {}
	CREATESTRUCT& GetCreateStruct() const {
		return *reinterpret_cast<CREATESTRUCT*>(lprm_);
	}
	template<typename T>
	T GetCreateParams() const {
		return reinterpret_cast<T>(CS().lpCreateParams);
	}
};

template <>
struct Msg<WM_ENABLE> : Msg_
{
	Msg(WndMsg &msg) : Msg_(msg) {}
	bool IsEnabled() const {
		return FALSE != wprm_;
	}
};

template <>
struct Msg<WM_ERASEBKGND> : Msg_
{
	Msg(WndMsg &msg) : Msg_(msg) {}
	HDC DC() const {
		return reinterpret_cast<HDC>(wprm_);
	}
};

template <>
struct Msg<WM_INITDIALOG> : Msg_
{
	Msg(WndMsg &msg) : Msg_(msg) {}
	HWND FocusHwnd() const {
		return reinterpret_cast<HWND>(wprm_);
	}
	template<typename T>
	T IniData() const {
		return reinterpret_cast<T>(lprm_);
	}
};

template <>
struct Msg<WM_KEYDOWN> : Msg_
{
	Msg(WndMsg &msg) : Msg_(msg) {}
	UINT Flags() const {
		return HIWORD(lprm_);
	}
	int RepeatCount() const {
		return static_cast<short>(LOWORD(lprm_));
	}
	UINT VKey() const {
		return static_cast<UINT>(wprm_);
	}
};

template <>
struct Msg<WM_KEYUP> : Msg_
{
	Msg(WndMsg &msg) : Msg_(msg) {}
	UINT Flags() const {
		return HIWORD(lprm_);
	}
	int RepeatCount() const {
		return static_cast<short>(LOWORD(lprm_));
	}
	UINT VKey() const {
		return static_cast<UINT>(wprm_);
	}
};

template <>
struct Msg<WM_LBUTTONDOWN> : Msg_
{
	Msg(WndMsg &msg) : Msg_(msg) {}
	UINT Flags() const {
		return static_cast<UINT>(wprm_);
	}
	POINT Position() const {
		POINT position = { (short)LOWORD(lprm_), (short)HIWORD(lprm_) };
		return position;
	}
};

template <>
struct Msg<WM_LBUTTONUP> : Msg_
{
	Msg(WndMsg &msg) : Msg_(msg) {}
	UINT Flags() const {
		return static_cast<UINT>(wprm_);
	}
	POINT Position() const {
		POINT position = { (short)LOWORD(lprm_), (short)HIWORD(lprm_) };
		return position;
	}
};

template <>
struct Msg<WM_MOUSEMOVE> : Msg_
{
	Msg(WndMsg &msg) : Msg_(msg) {}
	UINT Flags() const {
		return static_cast<UINT>(wprm_);
	}
	POINT Position() const {
		POINT position = { (short)LOWORD(lprm_), (short)HIWORD(lprm_) };
		return position;
	}
};

template <>
struct Msg<WM_MOVE> : Msg_
{
	Msg(WndMsg &msg) : Msg_(msg) {}
	POINT Position() const {
		POINT position = { (short)LOWORD(lprm_), (short)HIWORD(lprm_) };
		return position;
	}
};

template <>
struct Msg<WM_MOUSEWHEEL> : Msg_
{
	Msg(WndMsg &msg) : Msg_(msg) {}
	UINT Flags() const {
		return LOWORD(wprm_);
	}
	POINT Position() const {
		POINT position = { LOWORD(lprm_), HIWORD(lprm_) };
		return position;
	}
	int WheelDelta() const {
		return static_cast<short>(HIWORD(wprm_));
	}
};

template <>
struct Msg<WM_NOTIFY> : Msg_
{
	Msg(WndMsg &msg) :Msg_(msg) {}
	int CtrlId() const {
		return static_cast<int>(wprm_);
	}
	NMHDR& nmhdr() const {
		return *reinterpret_cast<NMHDR*>(lprm_);
	}
};

template <>
struct Msg<WM_PAINT> : Msg_
{
	Msg(WndMsg &msg)
		:Msg_(msg)
		,painting_(false)
	{}
	~Msg()
	{
		EndPaint();
	}
	HDC BeginPaint(HWND hwnd) {
		hwnd_ = hwnd;
		painting_ = true;
		return ::BeginPaint(hwnd, &ps_);
	}
	void EndPaint() {
		if (painting_)
		{
			::EndPaint(hwnd_, &ps_);
			painting_ = false;
		}
	}
	PAINTSTRUCT& PS() {
		return ps_;
	}
private:
	HWND        hwnd_;
	bool        painting_;
	PAINTSTRUCT ps_;
};

template <>
struct Msg<WM_SETCURSOR> : Msg_
{
	Msg(WndMsg &msg) :Msg_(msg) {}
	HWND Hwnd() const {
		return reinterpret_cast<HWND>(wprm_);
	}
	UINT HitTestCode() const {
		return LOWORD(lprm_);
	}
	UINT MouseMsg() const {
		return HIWORD(lprm_);
	}
};

template <>
struct Msg<WM_SETTINGCHANGE> : Msg_
{
	Msg(WndMsg &msg) :Msg_(msg) {}
	UINT Flag() {
		return wprm_;
	}	
	LPCTSTR SectionName() {
		return reinterpret_cast<LPCTSTR>(lprm_);
	}
};

template <>
struct Msg<WM_SIZE> : Msg_
{
	Msg(WndMsg &msg) : Msg_(msg) {}
	UINT State() const {
		return static_cast<UINT>(wprm_);
	}
	SIZE Size() const {
		SIZE size = { LOWORD(lprm_), HIWORD(lprm_) };
		return size;
	}
};

template <>
struct Msg<WM_SHOWWINDOW> : Msg_
{
	Msg(WndMsg &msg) : Msg_(msg) {}
	bool IsShown() const {
		return wprm_ == TRUE;
	}
	UINT Status() const {
		return static_cast<UINT>(lprm_);
	}
};

template <>
struct Msg<WM_TIMER> : Msg_
{
	Msg(WndMsg &msg) : Msg_(msg) {}
	UINT TimerId() const {
		return static_cast<UINT>(wprm_);
	}
	TIMERPROC TimerProc() {
		return reinterpret_cast<TIMERPROC>(lprm_);
	}
};
