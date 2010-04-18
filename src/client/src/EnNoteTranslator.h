#pragma once

#include "RapidXML/rapidxml.hpp"

#include <map>

class EnNoteTranslator
{
// nested types

private:

	typedef void (*NodeTransform)
		( rapidxml::memory_pool<wchar_t> *
		, rapidxml::xml_node<wchar_t>    *
		, rapidxml::xml_node<wchar_t>    *
		);
	typedef std::map<std::wstring, NodeTransform> TransformMap;

	typedef std::map<std::wstring, std::wstring> AttributeMap;

// data

private:

	TransformMap transforms;

// interface

public:

	EnNoteTranslator();

	void ConvertToHtml
		( std::wstring   xml
		, std::wstring & html
		);

// utility functions

private:

	static void AddXmlAttributes
		( rapidxml::memory_pool<wchar_t> * store
		, rapidxml::xml_node<wchar_t>    * node
		, AttributeMap                   & map
		);

	static void GetXmlAttributes
		( rapidxml::xml_node<wchar_t> * node
		, AttributeMap                & map
		);

	static void ProcessNoteXml
		( rapidxml::memory_pool<wchar_t> * store
		, rapidxml::xml_node<wchar_t>    * node
		, const TransformMap             & transforms
		);

	static void ReplaceMedia
		( rapidxml::memory_pool<wchar_t> * store
		, rapidxml::xml_node<wchar_t>    * parent
		, rapidxml::xml_node<wchar_t>    * child
		);

	static void ReplaceNote
		( rapidxml::memory_pool<wchar_t> * store
		, rapidxml::xml_node<wchar_t>    * parent
		, rapidxml::xml_node<wchar_t>    * child
		);

	static void ReplaceTodo
		( rapidxml::memory_pool<wchar_t> * store
		, rapidxml::xml_node<wchar_t>    * parent
		, rapidxml::xml_node<wchar_t>    * child
		);
};
