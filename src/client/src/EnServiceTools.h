#include <Evernote\EDAM\Error.h>

namespace EnServiceTools
{
	std::wstring CreateExceptionMessage
		( const Thrift::TException & e
		);

	std::wstring CreateNotFoundExceptionMessage
		( const Evernote::EDAM::Error::EDAMNotFoundException & e
		);

	std::wstring CreateSystemExceptionMessage
		( const Evernote::EDAM::Error::EDAMSystemException & e
		);

	std::wstring CreateTransportExceptionMessage
		( const Thrift::Transport::TTransportException & e
		);

	std::wstring CreateUserExceptionMessage
		( const Evernote::EDAM::Error::EDAMUserException & e
		);

	std::wstring GetErrorCodeString
		( Evernote::EDAM::Error::EDAMErrorCode errorCode
		);

	std::wstring GetErrorCodeString
		( Thrift::Transport::TTransportException::ExceptionType type
		);
}
