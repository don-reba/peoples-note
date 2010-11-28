#include "stdafx.h"
#include "EnRecognitionParser.h"

#include "RapidXML/rapidxml.hpp"

using namespace rapidxml;
using namespace std;

void EnRecognitionParser::Parse
	( const wstring        & xml
	, RecognitionEntryList & entries
	, const Guid           & resource
	)
{
		typedef xml_document<wchar_t> XmlDocument;
		auto_ptr<XmlDocument> doc(new XmlDocument());

		vector<wchar_t> xmlCopy;
		xmlCopy.reserve(xml.size() + 1);
		copy(xml.begin(), xml.end(), ::back_inserter(xmlCopy));
		xmlCopy.push_back(L'\0');
		doc->parse<0>(&xmlCopy[0]);

		xml_node<wchar_t> * recoIndex(doc->first_node(L"recoIndex"));
		if (!recoIndex)
			return;

		xml_node<wchar_t> * item(recoIndex->first_node(L"item"));
		while (item)
		{
			RecognitionEntry entry = { };
			entry.resource = resource;

			xml_attribute<wchar_t> * a(item->first_attribute());
			while (a)
			{
				wstring name  (a->name(),  a->name_size());
				wstring value (a->value(), a->value_size());
				if      (name == L"x") entry.x = _wtoi(value.c_str());
				else if (name == L"y") entry.y = _wtoi(value.c_str());
				else if (name == L"w") entry.w = _wtoi(value.c_str());
				else if (name == L"h") entry.h = _wtoi(value.c_str());
				a = a->next_attribute();
			}

			xml_node<wchar_t> * t(item->first_node(L"t"));
			while (t)
			{
				entry.text.assign(t->value(), t->value_size());

				xml_attribute<wchar_t> * a(t->first_attribute());
				while (a)
				{
					wstring name  (a->name(),  a->name_size());
					wstring value (a->value(), a->value_size());
					if (name == L"w")
						entry.weight = _wtoi(value.c_str());
					a = a->next_attribute();
				}

				entries.push_back(entry);

				t = t->next_sibling(L"t");
			}
			item = item->next_sibling(L"item");
		}
}