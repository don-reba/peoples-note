#include "stdafx.h"
#include "Timestamp.h"

#include <sstream>

using namespace std;

wstring Timestamp::GetFormattedDateTime() const
{
	LCID locale = LOCALE_USER_DEFAULT;

	vector<wchar_t> dateChars(11);
	::GetDateFormat
		( locale           // Locale
		, 0                // dwFlags
		, &time            // lpDate
		, L"yyyy-MM-Dd"    // lpFormat
		, &dateChars[0]    // lpDateSTr
		, dateChars.size() // cchDate
		);

	vector<wchar_t> timeChars(6);
	::GetTimeFormat
		( locale           // Locale
		, 0                // dwFlags
		, &time            // lpTime
		, L"HH:mm"         // lpFOrmat
		, &timeChars[0]    // lpTimeStr
		, timeChars.size() // cchTime
		);

	wostringstream stream;
	stream << &dateChars[0] << L" " << &timeChars[0];
	return stream.str();
}
