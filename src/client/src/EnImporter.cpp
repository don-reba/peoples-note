#include "stdafx.h"
#include "EnImporter.h"

#include "Note.h"
#include "Tools.h"

#include "RapidXML/rapidxml.hpp"

#include <iostream>
#include <iterator>

using namespace boost;
using namespace rapidxml;
using namespace std;
using namespace Tools;

void EnImporter::ImportNotes
	( const std::wstring & text
	, NoteList           & notes
	, NoteBodyList       & bodies
	, ResourceList       & resources
	)
{
	if (text.empty())
		return;
	try
	{
		typedef xml_document<wchar_t> XmlDocument;
		auto_ptr<XmlDocument> doc(new XmlDocument());

		vector<wchar_t> textCopy(text.begin(), text.end());
		textCopy.reserve(textCopy.size() + 1);
		textCopy.push_back(L'\0');
		doc->parse<0>(&textCopy[0]);

		xml_node<wchar_t> * node(doc->first_node());
		while (node && 0 != wcscmp(node->name(), L"en-export"))
			node = node->next_sibling();
		if (!node)
			return;
		node = node->first_node();

		while (node)
		{
			Guid    guid;
			wstring body;
			wstring title;
			wstring created;

			xml_node<wchar_t> * noteNode(node->first_node());
			while (noteNode)
			{
				if (0 == wcscmp(noteNode->name(), L"content"))
				{
					if (noteNode->value_size() > 0)
						body = noteNode->value();
					else
					{
						xml_node<wchar_t> * contentNode
							= noteNode->first_node();
						if (contentNode)
							body = contentNode->value();
					}
				}
				if (0 == wcscmp(noteNode->name(), L"resource"))
				{
					xml_node<wchar_t> * resourceNode(noteNode->first_node());
					resources.push_back(Resource());
					while (resourceNode)
					{
						if (0 == wcscmp(resourceNode->name(), L"data"))
						{
							resources.back().Note = guid;
							Tools::DecodeBase64
								( resourceNode->value()
								, resources.back().Data
								);
							resources.back().Hash = HashWithMD5(resources.back().Data);
						}
						else if (0 == wcscmp(resourceNode->name(), L"mime"))
						{
							resources.back().Mime = resourceNode->value();
						}
						resourceNode = resourceNode->next_sibling();
					}
				}
				if (0 == wcscmp(noteNode->name(), L"title"))
					title = noteNode->value();
				if (0 == wcscmp(noteNode->name(), L"created"))
					created = noteNode->value();
				noteNode = noteNode->next_sibling();
			}

			Timestamp timestamp = Timestamp(ParseTime(created));
			notes.push_back(Note());
			notes.back().guid         = guid;
			notes.back().name         = title;
			notes.back().creationDate = timestamp;
			notes.back().usn          = -1;
			notes.back().isDirty      = true;
			bodies.push_back(body);

			node = node->next_sibling();
		}
	}
	catch (rapidxml::parse_error & e)
	{
		const wchar_t * fragment(e.where<wchar_t>());
		cout << fragment << endl;
	}
}
time_t EnImporter::ParseTime(wstring time)
{
	if (time.size() != 16 || time[8] != L'T' || time[15] != L'Z')
		return 0;
	SYSTEMTIME systemTime = { };
	systemTime.wYear
		= static_cast<WORD>
		( GetDigit(time[0]) * 1000
		+ GetDigit(time[1]) * 100
		+ GetDigit(time[2]) * 10
		+ GetDigit(time[3]) * 1
		);
	systemTime.wMonth
		= static_cast<WORD>
		( GetDigit(time[4]) * 10
		+ GetDigit(time[5]) * 1
		);
	systemTime.wDay
		= static_cast<WORD>
		( GetDigit(time[6]) * 10
		+ GetDigit(time[7]) * 1
		);
	systemTime.wHour
		= static_cast<WORD>
		( GetDigit(time[9])  * 10
		+ GetDigit(time[10]) * 1
		);
	systemTime.wMinute
		= static_cast<WORD>
		( GetDigit(time[11]) * 10
		+ GetDigit(time[12]) * 1
		);
	systemTime.wSecond
		= static_cast<WORD>
		( GetDigit(time[13]) * 10
		+ GetDigit(time[14]) * 1
		);
	return SystemTimeToUnixTime(systemTime);
}

int EnImporter::GetDigit(wchar_t c)
{
	if (c < L'0') return 0;
	if (c > L'9') return 9;
	return c - L'0';
}
