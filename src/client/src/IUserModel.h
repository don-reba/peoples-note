#pragma once

#include "ISignalProvider.h"

class ICredentialsModel;
class INote;
class INotebook;

class IUserModel : public ISignalProvider
{
public:

	virtual ~IUserModel() {}

	virtual void ConnectLoaded(slot_type OnLoaded) = 0;

	virtual void CreateDefaultUser() = 0;
	
	virtual void SetCredentials(const ICredentialsModel & credentials) = 0;

	virtual INotebook & GetLastUsedNotebook() = 0;

	virtual boost::ptr_vector<INotebook> & GetNotebooks() = 0;

	virtual boost::ptr_vector<INote> & GetNotesByNotebook(INotebook & notebook) = 0;

	virtual boost::ptr_vector<INote> & GetNotesBySearch(std::wstring search) = 0;

	virtual void Load() = 0;
};
