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

	virtual std::wstring GetString
		( std::wstring value
		, std::wstring defaultData
		) const = 0;
};