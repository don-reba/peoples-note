#pragma once
#include "IProfileView.h"

class MockProfileView : public IProfileView
{
public:

	bool isMoveButtonEnabled;
	bool isShown;

	std::wstring dbPath;
	std::wstring dbSize;
	std::wstring moveButtonText;
	std::wstring moveErrorMessage;
	std::wstring username;

	signal SignalClose;
	signal SignalDbMove;

public:

	MockProfileView();

public:

	virtual void ConnectClose(slot_type OnClose);

	virtual void ConnectDbMove(slot_type OnDbMove);

	virtual void DisableMoveButton();

	virtual void EnableMoveButton();

	virtual void Hide();

	virtual bool IsShown();

	virtual void SetDbPath(const std::wstring & path);

	virtual void SetDbSize(const std::wstring & size);

	virtual void SetMoveButtonText(const std::wstring & text);

	virtual void SetMoveErrorMessage(const std::wstring & message);

	virtual void SetUsername(const std::wstring & username);

	virtual void Show();
};
