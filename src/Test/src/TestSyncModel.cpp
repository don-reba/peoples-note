#include "stdafx.h"
#include "SyncModel.h"

#include "EnNoteTranslator.h"
#include "MockEnService.h"
#include "MockMessagePump.h"
#include "MockLogger.h"
#include "MockUserModel.h"
#include "SignalCheck.h"

#include <boost/ref.hpp>

using namespace boost;
using namespace std;

//-----------------------
// auxilliary definitions
//-----------------------

struct SyncModelFixture
{
	EnNoteTranslator enNoteTranslator;
	MockEnService    enService;
	MockMessagePump  messagePump;
	MockLogger       logger;
	MockUserModel    userModel;

	SyncModel syncModel;

	SyncModelFixture()
		: syncModel
			( enNoteTranslator
			, enService
			, messagePump
			, userModel
			, logger
			)
	{
		enService.userStore->authenticationResult.IsGood = true;
	}
};

//-----------
// test cases
//-----------

BOOST_FIXTURE_TEST_CASE(SyncModel_Test, SyncModelFixture)
{
	BOOST_CHECK(!messagePump.wokeUp);

	SignalCheck signalSyncCompleteCheck;
	syncModel.ConnectSyncComplete(ref(signalSyncCompleteCheck));
	syncModel.BeginSync(L"username", L"password", Guid("guid"));

	::Sleep(20);
	BOOST_CHECK(!signalSyncCompleteCheck);
	
	BOOST_CHECK(messagePump.wokeUp);

	syncModel.ProcessMessages();

	BOOST_CHECK(signalSyncCompleteCheck);

	BOOST_CHECK_EQUAL(userModel.loadCount, 1);
}
