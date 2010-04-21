#pragma once

#include "Note.h"
#include "Notebook.h"

class IEnService
{
public:

	struct CredentialsValidity
	{
		bool         IsGood;
		std::wstring Message;
	};

	struct ServerState
	{
		NotebookList notebooks;
		NoteList     notes;
	};

public:

	virtual CredentialsValidity CheckCredentials
		( const std::wstring & username
		, const std::wstring & password
		) = 0;

	virtual void GetState(ServerState & state) = 0;
};
