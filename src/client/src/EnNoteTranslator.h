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

	TransformMap htmlTransforms;
	TransformMap xmlTransforms;

// interface

public:

	EnNoteTranslator();

	void ConvertToHtml
		( std::wstring   xml
		, std::wstring & html
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

	static void GetXmlAttributes
		( rapidxml::xml_node<wchar_t> * node
		, AttributeMap                & map
		);

	void ProcessNode
		( rapidxml::memory_pool<wchar_t> * store
		, rapidxml::xml_node<wchar_t>    * node
		, TransformMap                   & transforms
		);

	static void ReplaceCheckbox
		( rapidxml::memory_pool<wchar_t> * store
		, rapidxml::xml_node<wchar_t>    * parent
		, rapidxml::xml_node<wchar_t>    * child
		);

	static void ReplaceCrypt
		( rapidxml::memory_pool<wchar_t> * store
		, rapidxml::xml_node<wchar_t>    * parent
		, rapidxml::xml_node<wchar_t>    * child
		);

	static void ReplaceDiv
		( rapidxml::memory_pool<wchar_t> * store
		, rapidxml::xml_node<wchar_t>    * parent
		, rapidxml::xml_node<wchar_t>    * child
		);

	static void ReplaceEncryptImg
		( rapidxml::memory_pool<wchar_t> * store
		, rapidxml::xml_node<wchar_t>    * parent
		, rapidxml::xml_node<wchar_t>    * child
		);

	static void ReplaceImg
		( rapidxml::memory_pool<wchar_t> * store
		, rapidxml::xml_node<wchar_t>    * parent
		, rapidxml::xml_node<wchar_t>    * child
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
