#pragma once

struct RecognitionEntry
{
	std::wstring Text;
	int          Weight;
	int          X;
	int          Y;
	int          W;
	int          H;
	Guid         Resource;
};

typedef std::vector<RecognitionEntry> RecognitionEntryList;
