#include "stdafx.h"
#include "EnNoteTranslator.h"

#include "RapidXml/rapidxml_print.hpp"

#include <iterator>

using namespace rapidxml;
using namespace std;

//----------
// interface
//----------

EnNoteTranslator::EnNoteTranslator()
{
	transforms[L"en-media"] = &EnNoteTranslator::ReplaceMedia;
	transforms[L"en-note"]  = &EnNoteTranslator::ReplaceNote;
	transforms[L"en-todo"]  = &EnNoteTranslator::ReplaceTodo;
}

void EnNoteTranslator::ConvertToHtml
	( wstring   xml
	, wstring & html
	)
{
	typedef xml_document<wchar_t> XmlDocument;
	auto_ptr<XmlDocument> doc(new XmlDocument());
	doc->parse<0>(&xml[0]);

	ProcessNoteXml(doc.get(), doc.get(), transforms);

	html.clear();
	print(std::back_inserter(html), *doc, print_no_indenting);
}

//------------------
// utility functions
//------------------

void EnNoteTranslator::AddXmlAttributes
	( memory_pool<wchar_t> * store
	, xml_node<wchar_t>    * node
	, AttributeMap         & map 
	)
{
	foreach (AttributeMap::const_reference pair, map)
	{
		node->append_attribute(store->allocate_attribute
			( store->allocate_string(pair.first.c_str())
			, store->allocate_string(pair.second.c_str())
			));
	}
}

void EnNoteTranslator::GetXmlAttributes
	( xml_node<wchar_t> * node
	, AttributeMap      & map
	)
{
	xml_attribute<wchar_t> * attribute = node->first_attribute();
	while (attribute)
	{
		map[attribute->name()] = attribute->value();
		attribute = attribute->next_attribute();
	}
}

void EnNoteTranslator::ProcessNoteXml
	( memory_pool<wchar_t> * store
	, xml_node<wchar_t>    * node
	, const TransformMap   & transforms
	)
{
	xml_node<wchar_t> * child(node->first_node());
	while (child)
	{
		xml_node<wchar_t> * sibling = child->next_sibling();

		TransformMap::const_iterator transform = transforms.find(child->name());
		if (transform != transforms.end())
			(*transform->second)(store, node, child);
		ProcessNoteXml(store, child, transforms);

		child = sibling;
	}
}

void EnNoteTranslator::ReplaceMedia
	( memory_pool<wchar_t> * store
	, xml_node<wchar_t>    * parent
	, xml_node<wchar_t>    * child
	)
{
	AttributeMap map;
	GetXmlAttributes(child, map);

	wstring type = map[L"type"];
	if
		(  type != L"image/png"
		&& type != L"image/jpeg"
		&& type != L"image/gif"
		)
	{
		return;
	}
	map.erase(L"type");

	wstring src = L"img:";
	src.append(map[L"hash"]);
	map.erase(L"hash");

	xml_node<wchar_t> * node = store->allocate_node(node_element, L"img");
	node->append_attribute(store->allocate_attribute
		( L"src"
		, store->allocate_string(src.c_str())
		));
	AddXmlAttributes(store, node, map);

	parent->insert_node(child, node);
	parent->remove_node(child);
}

void EnNoteTranslator::ReplaceNote
	( memory_pool<wchar_t> * store
	, xml_node<wchar_t>    * parent
	, xml_node<wchar_t>    * child
	)
{
	child->name(L"div");
}

void EnNoteTranslator::ReplaceTodo
	( memory_pool<wchar_t> * store
	, xml_node<wchar_t>    * parent
	, xml_node<wchar_t>    * child
	)
{
	xml_attribute<wchar_t> * attribute = child->first_attribute(L"checked");
	bool checked(attribute && 0 == wcscmp(attribute->value(), L"true"));

	xml_node<wchar_t> * node(store->allocate_node(node_element, L"input"));
	node->append_attribute(store->allocate_attribute(L"type", L"checkbox"));
	if (checked)
		node->append_attribute(store->allocate_attribute(L"checked"));

	parent->insert_node(child, node);
	parent->remove_node(child);
}
