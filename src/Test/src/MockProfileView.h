#pragma once
#include "IProfileView.h"

class MockProfileView : public IProfileView
{
	MacroTestEvent(Close)
	MacroTestEvent(Compact)
	MacroTestEvent(DbMove)

public:

	bool isMoveButtonEnabled;
	bool isShown;

	std::wstring dbPath;
	std::wstring dbSize;
	std::wstring message;
	std::wstring moveButtonText;
	std::wstring username;

public:

	MockProfileView();

public:

	virtual void DisableMoveButton();

	virtual void EnableMoveButton();

	virtual void Hide();

	virtual bool IsShown();

	virtual void SetDbPath(const std::wstring & path);

	virtual void SetDbSize(const std::wstring & size);

	virtual void SetMessage(const std::wstring & message);

	virtual void SetMoveButtonText(const std::wstring & text);

	virtual void SetUsername(const std::wstring & username);

	virtual void Show();
};
