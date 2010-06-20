#include "stdafx.h"
#include "Animator.h"

#include <boost/ref.hpp>

using namespace boost;
using namespace std;

struct MockAnimation
{
	int count;

	MockAnimation() : count(0) {}

	void operator () (DWORD) { ++count; }
};

BOOST_AUTO_TEST_CASE(Animator_Test)
{
	Animator animator;

	// test empty animator
	BOOST_CHECK(!animator.IsRunning());

	animator.StepFrame();

	// test one animation
	MockAnimation animation0;

	IAnimator::Connection connection0(animator.Subscribe(ref(animation0)));

	BOOST_CHECK(animator.IsRunning());

	// test two animations with three connections
	MockAnimation animation1;

	IAnimator::Connection connection1(animator.Subscribe(ref(animation0)));
	IAnimator::Connection connection2(animator.Subscribe(ref(animation1)));

	animator.StepFrame();

	BOOST_CHECK_EQUAL(animation0.count, 2);
	BOOST_CHECK_EQUAL(animation1.count, 1);

	// test repeated stepping
	animator.StepFrame();

	BOOST_CHECK_EQUAL(animation0.count, 4);
	BOOST_CHECK_EQUAL(animation1.count, 2);

	// test disconnection
	connection1.Disconnect();
	connection0.Disconnect();
	connection2.Disconnect();

	BOOST_CHECK(!animator.IsRunning());

	animator.StepFrame();

	BOOST_CHECK_EQUAL(animation0.count, 4);
	BOOST_CHECK_EQUAL(animation1.count, 2);
}
