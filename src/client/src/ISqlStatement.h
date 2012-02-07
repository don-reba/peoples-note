#pragma once

#include "Blob.h"
#include "Guid.h"
#include "Timestamp.h"

class ISqlStatement
{
public:

	virtual bool Execute() = 0;

	virtual void Bind(__int32              n)    = 0;
	virtual void Bind(__int64              n)    = 0;
	virtual void Bind(double               n)    = 0;
	virtual void Bind(const std::string  & text) = 0;
	virtual void Bind(const std::wstring & text) = 0;
	virtual void Bind(const Blob         & blob) = 0;
	virtual void Bind(const Guid         & guid) = 0;

	virtual void BindNull() = 0;

	virtual void Get(bool         & n)    = 0;
	virtual void Get(__int16      & n)    = 0;
	virtual void Get(__int32      & n)    = 0;
	virtual void Get(__int64      & n)    = 0;
	virtual void Get(double       & n)    = 0;
	virtual void Get(std::string  & text) = 0;
	virtual void Get(std::wstring & text) = 0;
	virtual void Get(Guid         & guid) = 0;
	virtual void Get(Timestamp    & time) = 0;
};
