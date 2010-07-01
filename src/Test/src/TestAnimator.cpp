#include "stdafx.h"
#include "Animator.h"

#include <boost/ref.hpp>

using namespace boost;
using namespace std;

struct MockAnimation
{
	IAnimator              & animator;
	IAnimator::AnimationId   id;

	int  count;
	bool deleteSelf;

	MockAnimation
		( IAnimator              & animator
		, IAnimator::AnimationId   id
		)
		: animator   (animator)
		, count      (0)
		, deleteSelf (false)
		, id         (id)
	{
	}

	void operator () (DWORD)
	{
		++count;
		if (deleteSelf)
			animator.Unsubscribe(id);
	}
};

BOOST_AUTO_TEST_CASE(Animator_Test)
{
	Animator animator;

	// test empty animator
	BOOST_CHECK(!animator.IsRunning());

	animator.StepFrame();

	// test one animation
	MockAnimation animation0(animator, IAnimator::AnimationTest0);
	animator.Subscribe(animation0.id, ref(animation0));

	BOOST_CHECK(animator.IsRunning());

	// test two animations
	MockAnimation animation1(animator, IAnimator::AnimationTest1);
	animator.Subscribe(animation1.id, ref(animation1));

	animator.StepFrame();

	BOOST_CHECK_EQUAL(animation0.count, 1);
	BOOST_CHECK_EQUAL(animation1.count, 1);

	// test repeated stepping
	animation0.deleteSelf = true;

	animator.StepFrame();

	BOOST_CHECK_EQUAL(animation0.count, 2);
	BOOST_CHECK_EQUAL(animation1.count, 2);

	// test disconnection
	animator.Unsubscribe(IAnimator::AnimationTest1);

	BOOST_CHECK(!animator.IsRunning());

	animator.StepFrame();

	BOOST_CHECK_EQUAL(animation0.count, 2);
	BOOST_CHECK_EQUAL(animation1.count, 2);
}
