#pragma once

class IAudio
{
public:

	virtual void Play() = 0;

	virtual void Record() = 0;

	virtual void Stop() = 0;
};
