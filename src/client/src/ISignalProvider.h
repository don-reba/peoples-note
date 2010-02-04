#pragma once

class ISignalProvider
{
public:
	typedef boost::signals2::signal<void()>            signal;
	typedef boost::signals2::signal<void()>::slot_type slot_type;
};
