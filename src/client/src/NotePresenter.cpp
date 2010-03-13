#include "stdafx.h"
#include "NotePresenter.h"

#include "Guid.h"
#include "Tools.h"

#include "RapidXml/rapidxml_print.hpp"

#include <iterator>

using namespace boost;
using namespace rapidxml;
using namespace std;
using namespace Tools;

//----------
// interface
//----------

NotePresenter::NotePresenter
	( INoteListView & noteListView
	, INoteView     & noteView
	, IUserModel    & userModel
	)
	: noteListView (noteListView)
	, noteView     (noteView)
	, userModel    (userModel)
{
	noteListView.ConnectOpenNote(bind(&NotePresenter::OnOpenNote, this));
	noteView.ConnectLoadingData(bind(&NotePresenter::OnLoadingData, this, _1, _2));
}

//---------------
// event handlers
//---------------

void NotePresenter::OnLoadingData
	( const wchar_t * uri
	, Blob          & blob
	)
{
	const wchar_t * hash = wcschr(uri, L':');
	if (!hash)
		return;
	++hash;
	userModel.GetImageResource(ConvertToAnsi(hash), blob);
}

void NotePresenter::OnOpenNote()
{
	Guid guid(noteListView.GetSelectedNoteGuid());

	wstring body;
	userModel.GetNoteBody(guid, body);
	Note note(userModel.GetNote(guid));

	wstring subtitle(L"created on ");
	subtitle.append(note.GetCreationDate().GetFormattedDateTime());

	noteView.SetBody(ProcessNote(body));
	noteView.SetSubtitle(subtitle);
	noteView.SetTitle(note.GetTitle());
	noteView.Show();
}

//------------------
// utility functions
//------------------

void NotePresenter::AddXmlAttributes
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

void NotePresenter::GetXmlAttributes
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

wstring NotePresenter::ProcessNote(wstring text)
{
	typedef xml_document<wchar_t> XmlDocument;
	auto_ptr<XmlDocument> doc(new XmlDocument());
	doc->parse<0>(&text[0]);

	TransformMap transforms;
	transforms[L"en-media"] = &NotePresenter::ReplaceMedia;
	transforms[L"en-todo"]  = &NotePresenter::ReplaceTodo;
	ProcessNoteXml(doc.get(), doc.get(), transforms);

	wstring result;
	print(std::back_inserter(result), *doc, print_no_indenting);
	return result;
}

void NotePresenter::ProcessNoteXml
	( memory_pool<wchar_t> * store
	, xml_node<wchar_t>    * node
	, TransformMap         & transforms
	)
{
	xml_node<wchar_t> * child(node->first_node());
	while (child)
	{
		xml_node<wchar_t> * sibling = child->next_sibling();

		TransformMap::iterator transform = transforms.find(child->name());
		if (transform == transforms.end())
			ProcessNoteXml(store, child, transforms);
		else
			(*transform->second)(store, node, child);

		child = sibling;
	}
}

void NotePresenter::ReplaceMedia
	( memory_pool<wchar_t> * store
	, xml_node<wchar_t>    * parent
	, xml_node<wchar_t>    * child
	)
{
	AttributeMap map;
	GetXmlAttributes(child, map);

	wstring type = map[L"type"];
	if (type != L"image/png" && type != L"image/jpeg")
		return;
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

void NotePresenter::ReplaceTodo
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
