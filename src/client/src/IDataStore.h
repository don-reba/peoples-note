#pragma once

class INote;
class INotebook;

class IDataStore
{
public:

	virtual ~IDataStore() {}

	virtual void LoadOrCreate(std::wstring name) = 0;

	virtual void AddNotebook(const INotebook & notebook) = 0;

	virtual void MakeNotebookDefault(const INotebook & notebook) = 0;

	virtual INotebook & GetLastUsedNotebook() = 0;

	virtual boost::ptr_vector<INotebook> & GetNotebooks() = 0;

	virtual int GetNotebookCount() = 0;

	virtual boost::ptr_vector<INote> & GetNotesByNotebook(const INotebook & notebook) = 0;

	virtual boost::ptr_vector<INote> & GetNotesBySearch(std::wstring search) = 0;
};
