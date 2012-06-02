#include "stdafx.h"
#include "MockAUdioPlayer.h"

using namespace boost;

void MockAudioPlayer::Play(shared_ptr<ISqlBlob> & data)
{
	isPlaying = true;
	this->data = data;
}

void MockAudioPlayer::Stop()
{
	isPlaying = false;
}