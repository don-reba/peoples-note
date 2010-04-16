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
		( const std::wstring & value
		, const std::wstring & defaultData
		) const = 0;

	virtual void SetString
		( const std::wstring & value
		, const std::wstring & data
		) = 0;
};
