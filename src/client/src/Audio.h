#pragma once
#include "IAudio.h"

class Audio : public IAudio
{
public:

	Audio();

public:

	virtual void Play(Blob & blob);

	virtual void Record(Blob & blob);

	virtual void Stop();

private:

	static void Read
		( Blob   & blob
		, size_t   begin
		, size_t   end
		, void   * destination
		);
};
