#pragma once

class IAnimator;
class INoteListView;

class InstrumentationPresenter
{
private:

	IAnimator     & animator;
	INoteListView & noteListView;

public:

	InstrumentationPresenter
		( IAnimator     & animator
		, INoteListView & noteListView
		);

private:

	void OnAnimationCompleted();
};
