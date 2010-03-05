#pragma once

#include "IDataStore.h"
#include "MockNotebook.h"

class MockDataStore : public IDataStore
{
public:

	std::wstring name;
	MockNotebook defaultNotebook;
	MockNotebook lastUsedNotebook;

	boost::ptr_vector<INotebook> notebooks;
	boost::ptr_vector<INote>     notesByNotebook;
	boost::ptr_vector<INote>     notesBySearch;

	MockNotebook getNotesByNotebookNotebook;
	std::wstring getNotesBySearchSearch;

public:

	virtual void AddNotebook(const INotebook & notebook);

	virtual INotebook & GetLastUsedNotebook();

	virtual int GetNotebookCount();

	virtual boost::ptr_vector<INotebook> & GetNotebooks();

	virtual boost::ptr_vector<INote> & GetNotesByNotebook
		( const INotebook & notebook
		);

	virtual boost::ptr_vector<INote> & GetNotesBySearch
		( std::wstring search
		);

	virtual void LoadOrCreate(std::wstring name);

	virtual void MakeNotebookDefault(const INotebook & notebook);

	virtual void MakeNotebookLastUsed(const INotebook & notebook);
};
