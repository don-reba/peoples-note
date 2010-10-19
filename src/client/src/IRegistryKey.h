#pragma once

//enum RegistryHive
//{
//	RHClassesRoot,
//	RHCurrentUser,
//	RHLocalMachine,
//	RHUsers
//};

class IRegistryKey
{
public:

	virtual ~IRegistryKey() {}

	virtual std::wstring GetString
		( const wchar_t * value
		, const wchar_t * defaultData
		) const = 0;

	virtual void SetString
		( const wchar_t * value
		, const wchar_t * data
		) = 0;
};
