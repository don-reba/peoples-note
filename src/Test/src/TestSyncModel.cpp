#include "stdafx.h"
#include "SyncModel.h"

#include "EnNoteTranslator.h"
#include "MockEnService.h"
#include "MockMessagePump.h"
#include "MockSyncLogger.h"
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

struct SyncModelFixture
{
	EnNoteTranslator enNoteTranslator;
	MockEnService    enService;
	MockMessagePump  messagePump;
	MockSyncLogger   syncLogger;
	MockUserModel    userModel;

	SyncModel syncModel;

	SyncModelFixture()
		: syncModel
			( enNoteTranslator
			, enService
			, messagePump
			, userModel
			, syncLogger
			)
	{
		enService.userStore->authenticationResult.IsGood = true;
	}
};

BOOST_FIXTURE_TEST_CASE(SyncModel_Test, SyncModelFixture)
{
	BOOST_CHECK(!messagePump.wokeUp);

	SignalCheck check;
	syncModel.ConnectSyncComplete(ref(check));
	syncModel.BeginSync(L"test-usr");

	::Sleep(20);
	BOOST_CHECK(!check.signalled);
	
	BOOST_CHECK(messagePump.wokeUp);

	syncModel.ProcessMessages();
	BOOST_CHECK(check.signalled);

	BOOST_CHECK_EQUAL(userModel.loadCount, 1);
}
