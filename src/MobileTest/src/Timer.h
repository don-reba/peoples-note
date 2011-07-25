#pragma once

class Timer
{
private:

	__int64 s;
	__int64 f;

public:

	Timer();

	void Stop();

	float GetElapsedSeconds() const;
};
