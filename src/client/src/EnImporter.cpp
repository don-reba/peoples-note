#include "stdafx.h"
#include "EnImporter.h"

#include "Note.h"

#include "RapidXML/rapidxml.hpp"

#include <iostream>
#include <iterator>

using namespace boost;
using namespace rapidxml;
using namespace std;

void EnImporter::ImportNotes
	( wistream          & stream
	, ptr_vector<INote> & notes
	)
{
	vector<wchar_t> text
		( (istreambuf_iterator<wchar_t>(stream))
		, istreambuf_iterator<wchar_t>()
		);
	if (text.empty())
		return;
	text.reserve(text.size() + 1);
	text.push_back(L'\0');

	try
	{
		typedef xml_document<wchar_t> XmlDocument;
		auto_ptr<XmlDocument> doc(new XmlDocument());

		doc->parse<0>(&text[0]);

		xml_node<wchar_t> * node(doc->first_node());
		while (node && 0 != wcscmp(node->name(), L"en-export"))
			node = node->next_sibling();
		if (!node)
			return;
		node = node->first_node();

		while (node)
		{
			wstring title;

			xml_node<wchar_t> * noteNode(node->first_node());
			while (noteNode)
			{
				if (0 == wcscmp(noteNode->name(), L"title"))
					title = noteNode->value();
				noteNode = noteNode->next_sibling();
			}

			notes.push_back(new Note(Guid(), title));

			node = node->next_sibling();
		}
	}
	catch (rapidxml::parse_error & e)
	{
		const wchar_t * fragment = e.where<wchar_t>();
		cout << e.what() << endl;
	}
}
