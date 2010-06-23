#pragma once

class IAnimator;

class InstrumentationPresenter
{
private:

	IAnimator & animator;

public:

	InstrumentationPresenter
		( IAnimator & animator
		);

private:

	void OnAnimationCompleted();
};
