#pragma once
#include "IProfileView.h"

class MockProfileView : public IProfileView
{
public:

	bool isShown;

	MoveButtonState moveButtonState;

	std::wstring dbPath;
	std::wstring dbSize;
	std::wstring moveErrorMessage;
	std::wstring username;

	signal SignalClose;
	signal SignalMoveToCard;
	signal SignalMoveToDevice;

public:

	MockProfileView();

public:

	virtual void ConnectClose(slot_type OnClose);

	virtual void ConnectMoveToCard(slot_type OnMoveToCard);

	virtual void ConnectMoveToDevice(slot_type OnMoveToDevice);

	virtual void Hide();

	virtual void SetDbPath(const wchar_t * path);

	virtual void SetDbSize(const wchar_t * size);

	virtual void SetMoveButtonState(MoveButtonState state);

	virtual void SetMoveErrorMessage(const wchar_t * message);

	virtual void SetUsername(const wchar_t * username);

	virtual void Show();
};
