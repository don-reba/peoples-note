#pragma once

class ISqlStatement
{
public:

	virtual bool Execute() = 0;

	virtual void Bind(int index, __int32      n)    = 0;
	virtual void Bind(int index, __int64      n)    = 0;
	virtual void Bind(int index, std::string  text) = 0;
	virtual void Bind(int index, std::wstring text) = 0;

	virtual void Get(int index, __int32      & n)    = 0;
	virtual void Get(int index, __int64      & n)    = 0;
	virtual void Get(int index, std::string  & text) = 0;
	virtual void Get(int index, std::wstring & text) = 0;
};