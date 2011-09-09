#pragma once

#include "Blob.h"
#include "Guid.h"
#include "Location.h"
#include "Timestamp.h"

class Resource
{
public:

	class Size
	{
	public:

		__int16 Width;
		__int16 Height;
		bool    IsValid;

	public:

		Size() : IsValid(false) {}

		void Validate() { IsValid = Width || Height; }
	};

public:

	std::string  Hash;
	std::wstring Mime;
	Blob         Data;
	::Guid       Guid;
	::Guid       Note;
	Size         Dimensions;
	std::wstring SourceUrl;
	Timestamp    Timestamp;
	Location     Location;
	std::wstring FileName;
	bool         IsAttachment;
};

typedef std::vector<Resource> ResourceList;
