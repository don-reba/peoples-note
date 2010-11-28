#pragma once

struct RecognitionEntry
{
	std::wstring text;
	int          weight;
	int          x;
	int          y;
	int          w;
	int          h;
	Guid         resource;
};

typedef std::vector<RecognitionEntry> RecognitionEntryList;
