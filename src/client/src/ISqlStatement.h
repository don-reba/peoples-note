#pragma once

#include "Blob.h"

class ISqlStatement
{
public:

	virtual bool Execute() = 0;

	virtual void Finalize() = 0;

	virtual void Bind(int index, __int32              n)    = 0;
	virtual void Bind(int index, __int64              n)    = 0;
	virtual void Bind(int index, const std::string  & text) = 0;
	virtual void Bind(int index, const std::wstring & text) = 0;
	virtual void Bind(int index, const Blob         & blob) = 0;

	virtual void Get(int index, bool         & n)    = 0;
	virtual void Get(int index, __int32      & n)    = 0;
	virtual void Get(int index, __int64      & n)    = 0;
	virtual void Get(int index, std::string  & text) = 0;
	virtual void Get(int index, std::wstring & text) = 0;

	virtual bool IsNull(int index) = 0;
};
