#include "stdafx.h"
#include "EnNoteTranslator.h"

#include "RapidXml/rapidxml_print.hpp"
#include "Tools.h"

#include "windows.h"

#include <algorithm>
#include <iterator>
#include <memory>

#pragma warning(push)
#pragma warning(disable:4996)
#include <boost/xpressive/xpressive.hpp>
#pragma warning(pop)

using namespace boost::xpressive;
using namespace rapidxml;
using namespace std;

//----------
// interface
//----------

EnNoteTranslator::EnNoteTranslator()
{
	editTransforms[L"en-todo"] = &EnNoteTranslator::ReplaceEditTodo;

	htmlTransforms[L"div"]   = &EnNoteTranslator::ReplaceDiv;
	htmlTransforms[L"img"]   = &EnNoteTranslator::ReplaceImg;
	htmlTransforms[L"input"] = &EnNoteTranslator::ReplaceCheckbox;
	htmlTransforms[L"p"]     = &EnNoteTranslator::ReplaceParagraph;

	xmlTransforms[L"en-crypt"] = &EnNoteTranslator::ReplaceCrypt;
	xmlTransforms[L"en-media"] = &EnNoteTranslator::ReplaceMedia;
	xmlTransforms[L"en-note"]  = &EnNoteTranslator::ReplaceNote;
	xmlTransforms[L"en-todo"]  = &EnNoteTranslator::ReplaceTodo;
}

void EnNoteTranslator::ApplyXmlModifications
	( wstring         srcXml
	, wstring       & dstXml
	, std::set<int> & dirtyCheckboxIds)
{
	typedef xml_document<wchar_t> XmlDocument;
	auto_ptr<XmlDocument> doc(new XmlDocument());
	doc->parse<parse_non_destructive>(&srcXml[0]);

	checkboxId = 0;

	this->dirtyCheckboxIds.clear();
	this->dirtyCheckboxIds.insert(dirtyCheckboxIds.begin(), dirtyCheckboxIds.end());

	ProcessNode(doc.get(), doc.get(), editTransforms, false);

	dstXml =
		L"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
		L"<!DOCTYPE en-note SYSTEM \"http://xml.evernote.com/pub/enml2.dtd\">\n";
	rapidxml::print(back_inserter(dstXml), *doc, print_no_indenting | print_no_char_expansion);
}

void EnNoteTranslator::ConvertToHtml
	( wstring   xml
	, wstring & html
	, bool      linkify
	)
{
	typedef xml_document<wchar_t> XmlDocument;
	auto_ptr<XmlDocument> doc(new XmlDocument());
	doc->parse<parse_non_destructive>(&xml[0]);

	checkboxId = 0;

	ProcessNode(doc.get(), doc.get(), xmlTransforms, linkify);

	html.clear();
	rapidxml::print(back_inserter(html), *doc, print_no_indenting | print_no_char_expansion);
	// apostropies and amperstands are escaped in XML, but not in HTML
	Tools::ReplaceAll(html, L"&apos;", L"'");
}

void EnNoteTranslator::ConvertToText
	( wstring   xml
	, wstring & text
	)
{
	typedef xml_document<wchar_t> XmlDocument;
	auto_ptr<XmlDocument> doc(new XmlDocument());
	doc->parse<parse_non_destructive>(&xml[0]);

	text.clear();

	// depth-first iteration
	xml_node<wchar_t> * node(doc.get());
	while (node)
	{
		xml_node<wchar_t> * next(0);

		switch (node->type())
		{
		case node_data:
			{
				// output text
				wstring value(node->value(), node->value_size());
				ClearEntities(value);
				text.append(value);
				text.append(L" ");
				next = node->parent()->next_sibling();
			} break;
		case node_element:
			{
				// skip en-crypt elements
				wstring name(node->name(), node->name_size());
				if (name == L"en-crypt")
					next = node->next_sibling();
				else
					next = node->first_node();
			} break;
		default:
			{
				next = node->first_node();
			}
		}

		// get next node
		while (!next && node->parent())
		{
			next = node->next_sibling();
			if (!next)
				node = node->parent();
		}
		node = next;
	}

	ClearSymbols(text);
}

void EnNoteTranslator::ConvertToXml
	( wstring   html
	, wstring & xml
	)
{
	WrapInDiv(html);

	typedef xml_document<wchar_t> XmlDocument;
	auto_ptr<XmlDocument> doc(new XmlDocument());
	doc->parse<parse_non_destructive>(&html[0]);

	ProcessNode(doc.get(), doc.get(), htmlTransforms, false);
	DeleteNode(doc.get(), doc.get(), doc->first_node());
	SetRootToEnNote(doc.get());

	xml =
		L"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
		L"<!DOCTYPE en-note SYSTEM \"http://xml.evernote.com/pub/enml2.dtd\">\n";
	rapidxml::print(back_inserter(xml), *doc, print_no_indenting | print_no_char_expansion);
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

bool EnNoteTranslator::AttributeNameSortPredicate
	( const wchar_t * name1
	, const wchar_t * name2
	)
{
	return _wcsicmp(name1, name2) < 0;
}

void EnNoteTranslator::ClearEntities(wstring & text)
{
	wstring::size_type s(0), f(0);
	while (f < text.size())
	{
		s = text.find(L'&', f); if (s == wstring::npos) break;
		f = text.find(L';', s); if (f == wstring::npos) break;
		text.replace(s, f - s, f - s, ' ');
	}
}

void EnNoteTranslator::ClearSymbols(wstring & text)
{
	foreach (wchar_t & c, text)
	{
		if (!IsCharAlphaNumeric(c))
			c = L' ';
	}
}

void EnNoteTranslator::DeleteNode
	( memory_pool<wchar_t> * store
	, xml_node<wchar_t>    * parent
	, xml_node<wchar_t>    * child
	)
{
	xml_node<wchar_t> * grandchild(child->first_node());
	while (grandchild)
	{
		xml_node<wchar_t> * sibling(grandchild->next_sibling());
		child->remove_node(grandchild);
		parent->insert_node(child, grandchild);
		grandchild = sibling;
	}
	parent->remove_node(child);
}

void EnNoteTranslator::FilterAttributes
	( xml_node<wchar_t> *  node
	, const wchar_t     ** whitelistBegin
	, const wchar_t     ** whitelistEnd
	)
{
	xml_attribute<wchar_t> * attribute(node->first_attribute());
	while (attribute)
	{
		xml_attribute<wchar_t> * next(attribute->next_attribute());

		wstring name(attribute->name(), attribute->name_size());
		bool exists = binary_search
			( whitelistBegin
			, whitelistEnd
			, name.c_str()
			, AttributeNameSortPredicate
			);
		if (!exists)
			node->remove_attribute(attribute);
		
		attribute = next;
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
		wstring name  (attribute->name(),  attribute->name_size());
		wstring value (attribute->value(), attribute->value_size());
		map[name] = value;
		attribute = attribute->next_attribute();
	}
}

void EnNoteTranslator::ProcessDataNode
	( rapidxml::memory_pool<wchar_t> * store
	, rapidxml::xml_node<wchar_t>    * parent
	, rapidxml::xml_node<wchar_t>    * child
	)
{
	const int phoneNumberMinimumDigits(5);
	if (child->value_size() < phoneNumberMinimumDigits)
		return;

	// Based on the format used by Android's text.util.Linkify.
	// The pattern matches the following:
	// * Optionally, a + sign followed immediately by one or more digits.
	//   Spaces, dots, or dashes may follow.
	// * Optionally, sets of digits in parentheses, separated by spaces,
	//   dots, or dashes.
	// * A string starting and ending with a digit, containing digits,
	//   spaces, dots, and/or dashes.
	wcregex expression = wcregex::compile
		(                            // sdd = space, dot, or dash
		L"(?:\\+\\d+[\\-\\. ]*)?"    // +<digits><sdd>*
		L"(?:\\(\\d+\\)[\\-\\. ]*)?" // (<digits>)<sdd>*
		L"\\d[\\d\\-\\. ]+"          // <digit><digit|sdd>+
		);
	const wchar_t * begin (child->value());
	const wchar_t * end   (child->value() + child->value_size());

	bool hasMatches(false);
	const wchar_t * first(begin);
	foreach_ (const wcmatch & matches, wcregex_iterator(begin, end, expression), wcregex_iterator())
	{
		const wcsub_match & match(*matches.begin());

		// reject numbers with fewer than 5 digits
		if (count_if(match.first, match.second, iswdigit) < phoneNumberMinimumDigits)
			continue;

		hasMatches = true;

		// the regular expression matches numbers ending in non-digits
		// strip trailing non-digits
		typedef reverse_iterator<const wchar_t *> riter;
		const wchar_t * matchBegin (match.first);
		const wchar_t * matchEnd   (find_if(riter(match.second), riter(match.first), iswdigit).base());

		// add text preceding the phone number as usual
		if (first != matchBegin)
		{
			parent->insert_node
				( child
				, store->allocate_node(node_data, L"", first, 0, matchBegin - first)
				);
		}

		// prepend "tel:" to url, automatically recognized by WinCE
		wstring url(L"tel:");
		url.append(matchBegin, matchEnd - matchBegin);

		// add a phone link
		xml_node<wchar_t> * linkNode = store->allocate_node
				(node_element, L"a", matchBegin, 0, matchEnd - matchBegin);
		linkNode->append_attribute
			( store->allocate_attribute
				( L"href"
				, store->allocate_string(url.c_str(), url.size())
				, 0
				, url.size()
				)
			);
		parent->insert_node(child, linkNode);

		first = matchEnd;
	}
	if (hasMatches)
	{
		// append trailing text and remove the original node
		if (first != end)
		{
			parent->insert_node
				( child
				, store->allocate_node(node_data, L"", first, 0, end - first)
				);
		}
		parent->remove_node(child);
	}
}

void EnNoteTranslator::ProcessNode
	( memory_pool<wchar_t> * store
	, xml_node<wchar_t>    * node
	, TransformMap         & transforms
	, bool                   linkify
	)
{
	xml_node<wchar_t> * child(node->first_node());
	while (child)
	{
		xml_node<wchar_t> * sibling(child->next_sibling());

		ProcessNode(store, child, transforms, linkify);

		wstring name(child->name(), child->name_size());
		if (child->type() == node_data)
		{
			if (linkify)
				ProcessDataNode(store, node, child);
		}
		else
		{
			TransformMap::const_iterator transform(transforms.find(name));
			if (transform != transforms.end())
				(this->*transform->second)(store, node, child);
		}

		child = sibling;
	}
}

void EnNoteTranslator::ReplaceCheckbox
	( memory_pool<wchar_t> * store
	, xml_node<wchar_t>    * parent
	, xml_node<wchar_t>    * child
	)
{
	xml_attribute<wchar_t> * typeAttribute(child->first_attribute(L"type"));
	if (!typeAttribute)
		return;
	wstring type(typeAttribute->value(), typeAttribute->value_size());

	if (type != L"checkbox")
		return;

	xml_attribute<wchar_t> * stateAttribute(child->first_attribute(L"checked"));
	bool state(stateAttribute != NULL);

	child->remove_all_attributes();
	child->name(L"en-todo");
	child->append_attribute(store->allocate_attribute(L"checked", state ? L"true" : L"false"));
}

void EnNoteTranslator::ReplaceCrypt
	( memory_pool<wchar_t> * store
	, xml_node<wchar_t>    * parent
	, xml_node<wchar_t>    * child
	)
{
	child->name(L"img");
	child->append_attribute(store->allocate_attribute(L"type", L"en-crypt"));
	child->append_attribute(store->allocate_attribute(L"src", L"encrypt.png"));

	wstring value(child->value(), child->value_size());
	Tools::ReplaceAll(value, L"\r", L"");
	Tools::ReplaceAll(value, L"\n", L"");
	child->append_attribute
		( store->allocate_attribute
			( L"content"
			, store->allocate_string(value.c_str(), value.size())
			, 0
			, value.size()
			)
		);
	child->remove_all_nodes();
	child->value(L"", 0);
}

void EnNoteTranslator::ReplaceDiv
	( memory_pool<wchar_t> * store
	, xml_node<wchar_t>    * parent
	, xml_node<wchar_t>    * child
	)
{
	const wchar_t * whitelist[] =
		{ L"align", L"bgcolor", L"dir", L"lang", L"style"
		, L"text", L"title", L"type", L"xml:lang", L"xmlns"
		};
	FilterAttributes(child, whitelist, whitelist + Tools::GetArraySize(whitelist));

	xml_attribute<wchar_t> * typeAttribute(child->first_attribute(L"type"));
	if (!typeAttribute)
		return;
	wstring type(typeAttribute->value(), typeAttribute->value_size());

	if (type == L"en-note")
		child->name(L"en-note");
	child->remove_attribute(typeAttribute);
}

void EnNoteTranslator::ReplaceEncryptImg
	( memory_pool<wchar_t> * store
	, xml_node<wchar_t>    * parent
	, xml_node<wchar_t>    * child
	)
{
	// convert the "content" attribute to value
	xml_attribute<wchar_t> * contentAttribute(child->first_attribute(L"content"));
	if (contentAttribute)
	{
		child->value
			( store->allocate_string
				( contentAttribute->value()
				, contentAttribute->value_size()
				)
			, contentAttribute->value_size()
			);
		child->remove_attribute(contentAttribute);
	}

	// rename the node
	child->name(L"en-crypt");

	const wchar_t * whitelist[] = { L"cipher", L"hint", L"length" };
	FilterAttributes(child, whitelist, whitelist + Tools::GetArraySize(whitelist));
}

void EnNoteTranslator::ReplaceImg
	( memory_pool<wchar_t> * store
	, xml_node<wchar_t>    * parent
	, xml_node<wchar_t>    * child
	)
{
	// check for special images
	xml_attribute<wchar_t> * typeAttribute(child->first_attribute(L"type"));
	if (typeAttribute)
	{
		wstring type(typeAttribute->value(), typeAttribute->value_size());
		if (type == L"en-crypt")
		{
			ReplaceEncryptImg(store, parent, child);
			return;
		}
	}

	xml_attribute<wchar_t> * srcAttribute(child->first_attribute(L"src"));
	if (!srcAttribute)
		return;
	wstring src(srcAttribute->value(), srcAttribute->value_size());

	int colonPosition(src.find(L':'));
	if (colonPosition == wstring::npos)
		return;
	if (src.substr(0, colonPosition) != L"img")
		return;

	int dotPosition(src.find(L'.'));
	if (dotPosition == wstring::npos)
		return;
	if (dotPosition < colonPosition)
		return;

	typedef map<wstring, const wchar_t *> ImageTypeMap;
	ImageTypeMap imageTypeMap;
	imageTypeMap[L"png"] = L"image/png";
	imageTypeMap[L"jpg"] = L"image/jpeg";
	imageTypeMap[L"gif"] = L"image/gif";
	ImageTypeMap::const_iterator imageType
		(imageTypeMap.find(src.substr(dotPosition + 1)));
	if (imageType == imageTypeMap.end())
		return;

	wstring hash(src.substr(colonPosition + 1, dotPosition - colonPosition - 1));
	const wchar_t * hashString(store->allocate_string(hash.c_str(), hash.size() + 1));

	child->remove_attribute(srcAttribute);
	child->name(L"en-media");
	child->append_attribute(store->allocate_attribute(L"hash", hashString));
	child->append_attribute(store->allocate_attribute(L"type", imageType->second));

	const wchar_t * whitelist[] =
		{ L"align", L"alt", L"border", L"dir", L"hash", L"height"
		, L"hspace", L"lang", L"longdesc", L"style", L"title"
		, L"type", L"usemap", L"vspace", L"width", L"xml:lang"
		};
	FilterAttributes(child, whitelist, whitelist + Tools::GetArraySize(whitelist));
}

void EnNoteTranslator::ReplaceMedia
	( memory_pool<wchar_t> * store
	, xml_node<wchar_t>    * parent
	, xml_node<wchar_t>    * child
	)
{
	xml_attribute<wchar_t> * hashAttribute(child->first_attribute(L"hash"));
	if (!hashAttribute)
		return;

	xml_attribute<wchar_t> * typeAttribute(child->first_attribute(L"type"));
	if (!typeAttribute)
		return;

	typedef map<wstring, const wchar_t *> ImageTypeMap;
	ImageTypeMap imageTypeMap;
	imageTypeMap[L"image/png"]  = L"png";
	imageTypeMap[L"image/jpeg"] = L"jpg";
	imageTypeMap[L"image/gif"]  = L"gif";
	ImageTypeMap::const_iterator imageType(imageTypeMap.find(wstring
		( typeAttribute->value()
		, typeAttribute->value_size()
		)));
	if (imageType == imageTypeMap.end())
		return;

	wstring src(L"img:");
	src.append(hashAttribute->value(), hashAttribute->value_size());
	src.append(L".", 1);
	src.append(imageType->second);
	const wchar_t * srcString(store->allocate_string(src.c_str(), src.size() + 1));

	child->remove_attribute(hashAttribute);
	child->remove_attribute(typeAttribute);
	child->name(L"img");
	child->append_attribute(store->allocate_attribute(L"src", srcString));
}

void EnNoteTranslator::ReplaceParagraph
	( memory_pool<wchar_t> * store
	, xml_node<wchar_t>    * parent
	, xml_node<wchar_t>    * child
	)
{
	if (child->value_size() > 0 || child->first_node() != NULL)
		return;
	xml_node<wchar_t> * node(store->allocate_node(node_element, L"p", L"&nbsp;"));
	parent->insert_node(child, node);
	parent->remove_node(child);
}

void EnNoteTranslator::ReplaceNote
	( memory_pool<wchar_t> * store
	, xml_node<wchar_t>    * parent
	, xml_node<wchar_t>    * child
	)
{
	DeleteNode(store, parent, child);
}

void EnNoteTranslator::ReplaceTodo
	( memory_pool<wchar_t> * store
	, xml_node<wchar_t>    * parent
	, xml_node<wchar_t>    * child
	)
{
	xml_attribute<wchar_t> * attribute = child->first_attribute(L"checked");
	bool checked(attribute && wstring(attribute->value(), attribute->value_size()) == L"true");

	xml_node<wchar_t> * node(store->allocate_node(node_element, L"input"));
	node->append_attribute(store->allocate_attribute(L"type", L"checkbox"));
	if (checked)
		node->append_attribute(store->allocate_attribute(L"checked", L"true"));

	wchar_t checkboxIdStr[10];
	_itow(checkboxId, checkboxIdStr, 10);
	node->append_attribute
		(store->allocate_attribute(L"uid", store->allocate_string(checkboxIdStr)));
	++checkboxId;

	parent->insert_node(child, node);
	parent->remove_node(child);
}

void EnNoteTranslator::ReplaceEditTodo
	( memory_pool<wchar_t> * store
	, xml_node<wchar_t>    * parent
	, xml_node<wchar_t>    * child
	)
{
	xml_attribute<wchar_t> * attribute = child->first_attribute(L"checked");
	bool checked(attribute && wstring(attribute->value(), attribute->value_size()) == L"true");

	if (dirtyCheckboxIds.find(checkboxId) != dirtyCheckboxIds.end())
		attribute->value(checked ? L"false" : L"true");
	++checkboxId;
}

void EnNoteTranslator::SetRootToEnNote(xml_document<wchar_t> * doc)
{
	xml_node<wchar_t> * root(doc->first_node());
	if (!root || wstring(root->name(), root->name_size()) != L"en-note")
	{
		root = doc->allocate_node(node_element, L"en-note");
		xml_node<wchar_t> * node(doc->first_node());
		while (node)
		{
			doc->remove_node(node);
			root->append_node(node);
			node = doc->first_node();
		}
		doc->append_node(root);
	}
}

void EnNoteTranslator::WrapInDiv(wstring & str)
{
	const wchar_t * openDiv  (L"<div>");
	const wchar_t * closeDiv (L"</div>");
	str.reserve(str.size() + wcslen(openDiv) + wcslen(closeDiv));
	str.insert(0, openDiv);
	str.append(closeDiv);
}
