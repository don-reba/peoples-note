#pragma once

class SignalCheck
{
private:

	bool signalled;

public:

	SignalCheck() : signalled(false) {}

	void operator () () { signalled = true; }

	operator bool() const { return signalled; }
};
