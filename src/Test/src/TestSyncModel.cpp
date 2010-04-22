#include "stdafx.h"
#include "SyncModel.h"

#include "MockEnService.h"
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
	MockEnService enService;
	MockUserModel userModel;
	SyncModel syncModel(enService);

	SignalCheck check;
	syncModel.ConnectSyncComplete(boost::ref(check));
	syncModel.BeginSync(userModel);

	::Sleep(10);
	BOOST_CHECK(!check.signalled);

	syncModel.ProcessMessages();
	BOOST_CHECK(check.signalled);
}
