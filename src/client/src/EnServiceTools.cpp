#include "stdafx.h"
#include "EnServiceTools.h"

using namespace EnServiceTools;
using namespace Evernote::EDAM::Error;
using namespace std;
using namespace Thrift;
using namespace Thrift::Transport;

wstring EnServiceTools::CreateExceptionMessage(const TException & e)
{
	wstring message;
	message.append(L"TException(");
	message.append(e.GetMessageW());
	message.append(L")");
	return message;
}

wstring EnServiceTools::CreateNotFoundExceptionMessage(const EDAMNotFoundException & e)
{
	wstring message;
	message.append(L"EDAMNotFoundException(");
	if (e.__isset.identifier)
	{
		message.append(L"identifier = ");
		message.append(e.identifier);
	}
	if (e.__isset.identifier && e.__isset.key)
		message.append(L", ");
	if (e.__isset.key)
	{
		message.append(L"key = ");
		message.append(e.key);
	}
	message.append(L")");
	return message;
}

wstring EnServiceTools::CreateSystemExceptionMessage(const EDAMSystemException & e)
{
	wstring message;
	message.append(L"EDAMSystemException(");
	message.append(L"errorCode = ");
	message.append(GetErrorCodeString(e.errorCode));
	if (e.__isset.message)
	{
		message.append(L", message = ");
		message.append(e.message);
	}
	message.append(L")");
	return message;
}

wstring EnServiceTools::CreateUserExceptionMessage(const EDAMUserException & e)
{
	wstring message;
	message.append(L"EDAMUserException(");
	message.append(L"errorCode = ");
	message.append(GetErrorCodeString(e.errorCode));
	if (e.__isset.parameter)
	{
		message.append(L", parameter = ");
		message.append(e.parameter);
	}
	message.append(L")");
	return message;
}

wstring EnServiceTools::CreateTransportExceptionMessage(const Thrift::Transport::TTransportException & e)
{
	wstring message;
	message.append(L"TTransportException(");
	message.append(L"errorCode = ");
	message.append(GetErrorCodeString(e.GetType()));
	message.append(L", message = ");
	message.append(e.GetMessageW());
	message.append(L")");
	return message;
}

wstring EnServiceTools::GetErrorCodeString(EDAMErrorCode errorCode)
{
	switch (errorCode)
	{
	case BAD_DATA_FORMAT:   return L"BAD_DATA_FORMAT";
	case PERMISSION_DENIED: return L"PERMISSION_DENIED";
	case INTERNAL_ERROR:    return L"INTERNAL_ERROR";
	case DATA_REQUIRED:     return L"DATA_REQUIRED";
	case LIMIT_REACHED:     return L"LIMIT_REACHED";
	case QUOTA_REACHED:     return L"QUOTA_REACHED";
	case INVALID_AUTH:      return L"INVALID_AUTH";
	case AUTH_EXPIRED:      return L"AUTH_EXPIRED";
	case DATA_CONFLICT:     return L"DATA_CONFLICT";
	case ENML_VALIDATION:   return L"ENML_VALIDATION";
	case SHARD_UNAVAILABLE: return L"SHARD_UNAVAILABLE";
	}
	return L"?";
}

wstring EnServiceTools::GetErrorCodeString(TTransportException::ExceptionType type)
{
	switch (type)
	{
	case TTransportException::NotOpen:     return L"NotOpen";     break;
	case TTransportException::AlreadyOpen: return L"AlreadyOpen"; break;
	case TTransportException::TimedOut:    return L"TimedOut";    break;
	case TTransportException::EndOfFile:   return L"EndOfFile";   break;
	case TTransportException::Unknown:     return L"Unknown";     break;
	}
	return L"?";
}
