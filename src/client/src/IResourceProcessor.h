#pragma once

template <typename T>
class IResourceProcessor
{
public:
	virtual void Add    (const T & remote) = 0;
	virtual void Delete (const T & local)  = 0;
	virtual void Rename (const T & local)  = 0;
	virtual void Upload (const T & local)  = 0;
	virtual void Merge
		( const T & local
		, const T & remote
		) = 0;
};
