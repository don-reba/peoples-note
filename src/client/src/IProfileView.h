#pragma once

#include "ISignalProvider.h"

class IProfileView : public ISignalProvider
{
	MacroIEvent(Close)
	MacroIEvent(Compact)
	MacroIEvent(DbMove)

public:

	virtual void DisableMoveButton() = 0;

	virtual void EnableMoveButton() = 0;

	virtual void Hide() = 0;

	virtual bool IsShown() = 0;

	virtual void SetDbPath(const std::wstring & path) = 0;

	virtual void SetDbSize(const std::wstring & size) = 0;

	virtual void SetMessage(const std::wstring & message) = 0;

	virtual void SetMoveButtonText(const std::wstring & text) = 0;

	virtual void SetUsername(const std::wstring & username) = 0;

	virtual void Show() = 0;
};
