#include "stdafx.h"
#include "Audio.h"

#include "Test.h"
#include "Tools.h"

#include <boost/ref.hpp>
#include <fstream>

using namespace boost;
using namespace std;

AUTO_TEST_CASE(AudioRecord)
{
	Blob data;
	Tools::ReadBinaryFile(L"My Documents\\People's Note\\start.wav", data);

	Audio audio;
	audio.Play(data);
}
