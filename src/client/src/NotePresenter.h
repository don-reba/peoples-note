#pragma once

#include "INoteListView.h"
#include "INoteView.h"
#include "IUserModel.h"

#include "RapidXML/rapidxml.hpp"

#include <map>

class NotePresenter
{
private:

	typedef void (*NodeTransform)
		( rapidxml::memory_pool<wchar_t> *
		, rapidxml::xml_node<wchar_t>    *
		, rapidxml::xml_node<wchar_t>    *
		);
	typedef std::map<std::wstring, NodeTransform> TransformMap;

	typedef std::map<std::wstring, std::wstring> AttributeMap;

private:

	INoteListView & noteListView;
	INoteView     & noteView;
	IUserModel    & userModel;

// interface

public:

	NotePresenter
		( INoteListView & noteListView
		, INoteView     & noteView
		, IUserModel    & userModel
		);

// event handlers

private:

	void OnLoadingData
		( const wchar_t * uri
		, Blob          & blob
		);

	void OnOpenNote();

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

	static std::wstring ProcessNote(std::wstring text);

	static void ProcessNoteXml
		( rapidxml::memory_pool<wchar_t> * store
		, rapidxml::xml_node<wchar_t>    * node
		, TransformMap                   & transforms
		);

	static void ReplaceMedia
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
