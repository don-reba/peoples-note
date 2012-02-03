#pragma once

#include "RapidXML/rapidxml.hpp"

#include <set>
#include <map>

class EnNoteTranslator
{
// nested types

private:

	typedef void (EnNoteTranslator::*NodeTransform)
		( rapidxml::memory_pool<wchar_t> *
		, rapidxml::xml_node<wchar_t>    *
		, rapidxml::xml_node<wchar_t>    *
		);
	typedef std::map<std::wstring, NodeTransform> TransformMap;

	typedef std::map<std::wstring, std::wstring> AttributeMap;

// data

private:

	TransformMap editTransforms;
	TransformMap htmlTransforms;
	TransformMap xmlTransforms;

	int           checkboxId;
	std::set<int> dirtyCheckboxIds;

// interface

public:

	EnNoteTranslator();

	void ApplyXmlModifications
		( std::wstring    srcXml
		, std::wstring  & dstXml
		, std::set<int> & dirtyCheckboxIds
		);

	void ConvertToHtml
		( std::wstring   xml
		, std::wstring & html
		, bool           linkify
		);

	void ConvertToText
		( std::wstring   xml
		, std::wstring & text
		);

	void ConvertToXml
		( std::wstring   html
		, std::wstring & xml
		);

// utility functions

private:

	static void AddXmlAttributes
		( rapidxml::memory_pool<wchar_t> * store
		, rapidxml::xml_node<wchar_t>    * node
		, AttributeMap                   & map
		);

	static bool AttributeNameSortPredicate
		( const wchar_t * name1
		, const wchar_t * name2
		);

	static void ClearEntities(std::wstring & text);

	static void ClearSymbols(std::wstring & text);

	static void DeleteNode
		( rapidxml::memory_pool<wchar_t> * store
		, rapidxml::xml_node<wchar_t>    * parent
		, rapidxml::xml_node<wchar_t>    * child
		);

	// requires a lowercase-alphabetically sorted whitelist
	static void FilterAttributes
		( rapidxml::xml_node<wchar_t> *  node
		, const wchar_t               ** whitelistBegin
		, const wchar_t               ** whitelistEnd
		);

	static void GetXmlAttributes
		( rapidxml::xml_node<wchar_t> * node
		, AttributeMap                & map
		);

	void ProcessDataNode
		( rapidxml::memory_pool<wchar_t> * store
		, rapidxml::xml_node<wchar_t>    * parent
		, rapidxml::xml_node<wchar_t>    * child
		);

	void ProcessNode
		( rapidxml::memory_pool<wchar_t> * store
		, rapidxml::xml_node<wchar_t>    * node
		, TransformMap                   & transforms
		, bool                             linkify
		);

	void ReplaceCheckbox
		( rapidxml::memory_pool<wchar_t> * store
		, rapidxml::xml_node<wchar_t>    * parent
		, rapidxml::xml_node<wchar_t>    * child
		);

	void ReplaceCrypt
		( rapidxml::memory_pool<wchar_t> * store
		, rapidxml::xml_node<wchar_t>    * parent
		, rapidxml::xml_node<wchar_t>    * child
		);

	 void ReplaceDiv
		( rapidxml::memory_pool<wchar_t> * store
		, rapidxml::xml_node<wchar_t>    * parent
		, rapidxml::xml_node<wchar_t>    * child
		);

	void ReplaceEncryptImg
		( rapidxml::memory_pool<wchar_t> * store
		, rapidxml::xml_node<wchar_t>    * parent
		, rapidxml::xml_node<wchar_t>    * child
		);

	void ReplaceImg
		( rapidxml::memory_pool<wchar_t> * store
		, rapidxml::xml_node<wchar_t>    * parent
		, rapidxml::xml_node<wchar_t>    * child
		);

	void ReplaceMedia
		( rapidxml::memory_pool<wchar_t> * store
		, rapidxml::xml_node<wchar_t>    * parent
		, rapidxml::xml_node<wchar_t>    * child
		);

	void ReplaceNote
		( rapidxml::memory_pool<wchar_t> * store
		, rapidxml::xml_node<wchar_t>    * parent
		, rapidxml::xml_node<wchar_t>    * child
		);

	void ReplaceParagraph
		( rapidxml::memory_pool<wchar_t> * store
		, rapidxml::xml_node<wchar_t>    * parent
		, rapidxml::xml_node<wchar_t>    * child
		);

	void ReplaceTodo
		( rapidxml::memory_pool<wchar_t> * store
		, rapidxml::xml_node<wchar_t>    * parent
		, rapidxml::xml_node<wchar_t>    * child
		);

	void ReplaceEditTodo
		( rapidxml::memory_pool<wchar_t> * store
		, rapidxml::xml_node<wchar_t>    * parent
		, rapidxml::xml_node<wchar_t>    * child
		);

	static void SetRootToEnNote
		( rapidxml::xml_document<wchar_t> * doc
		);

	static void WrapInDiv(std::wstring & text);
};
