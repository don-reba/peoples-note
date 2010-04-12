#pragma once

// A simple COM interface wrapper with automatic release, but no reference counting.

template <typename T>
class CComPtr
{
private:

	T * ptr;

public:

	CComPtr() : ptr (NULL) {}

	~CComPtr() { if (ptr) ptr->Release(); }

	template <typename S>
	HRESULT QueryInterface(CComPtr<S> & object)
	{
		S * s;
		HRESULT result = ptr->QueryInterface(__uuidof(S), reinterpret_cast<void**>(&s));
		if (S_OK == result)
			object.Ptr() = s;
		return result;
	}

	operator       T *& ()       { return ptr; }
	operator const T *& () const { return ptr; }

	      T *& Ptr()       { return ptr; }
	const T *& Ptr() const { return ptr; }

	T * operator -> () const { return ptr; }
};
