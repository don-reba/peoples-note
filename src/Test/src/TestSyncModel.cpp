#include "stdafx.h"
#include "SyncModel.h"

#include <boost/ref.hpp>

using namespace boost;
using namespace std;

struct SignalCheck
{
	bool signalled;
	SignalCheck() : signalled(false) {}
	void operator () () { signalled = true; }
};

BOOST_AUTO_TEST_CASE(SyncModel_Test)
{
	SignalCheck check;

	SyncModel syncModel;
	syncModel.ConnectSyncComplete(boost::ref(check));
	syncModel.BeginSync();

	::Sleep(10);
	BOOST_CHECK(!check.signalled);

	syncModel.ProcessMessages();
	BOOST_CHECK(check.signalled);
}
