#pragma once

#include <boost/function.hpp>

class IAnimator
{
public:

	class Connection
	{
	private:

		IAnimator * animator;
		int         id;

	public:

		Connection();

		Connection(IAnimator * animator, int id);

		void Disconnect();
	};

	typedef boost::function<void(DWORD)> Animation;

public:

	virtual void StepFrame() = 0;

	virtual bool IsRunning() = 0;

	virtual Connection Subscribe(Animation OnFrameStep) = 0;

	virtual void Unsubscribe(int connectionId) = 0;
};
