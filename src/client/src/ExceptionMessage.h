#pragma once

struct ExceptionMessage
{
	const std::wstring Title;
	const std::wstring Message;

	ExceptionMessage
		( const std::wstring & title
		, const std::wstring & message
		)
		: Title   (title)
		, Message (message)
	{
	}
};