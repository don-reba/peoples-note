#pragma once

#include "IUserModel.h"
#include "MockCredentialsModel.h"
#include "MockNotebook.h"
#include "MockNote.h"

class MockUserModel : public IUserModel
{
private:
	
	signal SignalLoaded;

public:

	MockCredentialsModel    credentialsModel;
	MockNotebook            lastUsedNotebook;
	std::vector<INotebook*> notebooks;
	std::vector<INote*>     notes;

	bool isDefault;
	bool isLoaded;

	mutable std::wstring notebookSelection;
	mutable std::wstring searchSelection;

public:

	MockUserModel();

	void Loaded();

	virtual void ConnectLoaded(slot_type OnLoaded);

	virtual void CreateDefaultUser();
	
	virtual void SetCredentials(const ICredentialsModel & credentials);

	virtual INotebook & GetLastUsedNotebook();

	virtual std::vector<INotebook*> GetNotebooks() const;

	virtual std::vector<INote*> GetNotesByNotebook(INotebook & notebook) const;

	virtual std::vector<INote*> GetNotesBySearch(std::wstring search) const;

	virtual void Load();
};
