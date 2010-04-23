#include "stdafx.h"
#include "SyncModel.h"

#include "MockEnService.h"
#include "MockMessagePump.h"
#include "MockUserModel.h"

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
	MockEnService   enService;
	MockMessagePump messagePump;
	MockUserModel   userModel;
	SyncModel syncModel(enService, messagePump);

	BOOST_CHECK(!messagePump.wokeUp);

	SignalCheck check;
	syncModel.ConnectSyncComplete(boost::ref(check));
	syncModel.BeginSync(userModel);

	::Sleep(10);
	BOOST_CHECK(!check.signalled);
	BOOST_CHECK(messagePump.wokeUp);

	syncModel.ProcessMessages();
	BOOST_CHECK(check.signalled);
}
