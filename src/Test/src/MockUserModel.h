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
	boost::ptr_vector<INotebook> notebooks;
	boost::ptr_vector<INote>     notes;

	typedef std::pair<std::wstring, std::wstring> AddedNote;
	std::vector<AddedNote> addedNotes;

	bool isDefault;
	bool isLoaded;

	mutable std::wstring notebookSelection;
	mutable std::wstring searchSelection;

public:

	MockUserModel();

	void Loaded();

	virtual void AddNote(const INote & note, const INotebook & notebook);

	virtual void ConnectLoaded(slot_type OnLoaded);

	virtual void CreateDefaultUser();
	
	virtual void SetCredentials(const ICredentialsModel & credentials);

	virtual INotebook & GetLastUsedNotebook();

	virtual boost::ptr_vector<INotebook> & GetNotebooks();

	virtual boost::ptr_vector<INote> & GetNotesByNotebook(INotebook & notebook);

	virtual boost::ptr_vector<INote> & GetNotesBySearch(std::wstring search);

	virtual void Load();
};
