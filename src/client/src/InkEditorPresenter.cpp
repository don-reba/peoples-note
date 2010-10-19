#include "stdafx.h"
#include "InkEditorPresenter.h"

#include "IInkEditorModel.h"
#include "IInkEditorView.h"
#include "INoteListModel.h"
#include "INoteListView.h"
#include "IUserModel.h"
#include "Resource.h"
#include "Timestamp.h"
#include "Tools.h"
#include "Transaction.h"

using namespace boost;
using namespace std;
using namespace Tools;

InkEditorPresenter::InkEditorPresenter
	( IInkEditorModel & inkEditorModel
	, IInkEditorView  & inkEditorView
	, INoteListModel  & noteListModel
	, INoteListView   & noteListView
	, IUserModel      & userModel
	)
	: inkEditorModel (inkEditorModel)
	, inkEditorView  (inkEditorView)
	, noteListModel  (noteListModel)
	, noteListView   (noteListView)
	, userModel      (userModel)
{
	inkEditorView.ConnectAccept     (bind(&InkEditorPresenter::OnAccept,     this));
	inkEditorView.ConnectCancel     (bind(&InkEditorPresenter::OnCancel,     this));
	inkEditorView.ConnectPenChanged (bind(&InkEditorPresenter::OnPenChanged, this));
	noteListView.ConnectNewInkNote  (bind(&InkEditorPresenter::OnNewInkNote, this));
}

//---------------
// event handlers
//---------------

void InkEditorPresenter::OnAccept()
{
	// create note 
	Note note;
	note.name         = L"Ink note";
	note.creationDate = Timestamp::GetCurrentTime();
	note.isDirty      = true;
	note.usn          = 0;

	// create resource with hash
	Resource image;
	inkEditorView.GetImage(image.Data);
	image.Hash = HashWithMD5(image.Data);
	image.Note = note.guid;

	// create note body referencing resource by hash
	wstring body =
		L"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
		L"<!DOCTYPE en-note SYSTEM \"http://xml.evernote.com/pub/enml2.dtd\">\n"
		L"<en-note><en-media type=\"image/jpeg\" hash=\"";
	body.append(ConvertToUnicode(image.Hash));
	body.append(L"\"/></en-note>");

	// add both to the database
	{
		Transaction transaction(userModel);
		Notebook notebook;
		userModel.GetLastUsedNotebook(notebook);
		userModel.AddNote(note, body, L"", notebook);
		userModel.AddResource(image);
	}

	inkEditorView.Hide();

	noteListModel.Reload();
}

void InkEditorPresenter::OnCancel()
{
	inkEditorView.Hide();
}

void InkEditorPresenter::OnNewInkNote()
{
	inkEditorView.Show();

	wstring penWidth;
	wstring penColor;
	inkEditorModel.GetPen(penWidth, penColor);

	inkEditorView.SetPen
		( GetPenWidth(penWidth.c_str())
		, GetPenColor(penColor.c_str())
		);
}

void InkEditorPresenter::OnPenChanged()
{
	InkPenWidth width(inkEditorView.GetPenWidth());
	InkPenColor color(inkEditorView.GetPenColor());

	inkEditorModel.SetPen
		( GetPenWidthName(width)
		, GetPenColorName(color)
		);
	inkEditorView.SetPen
		( inkEditorView.GetPenWidth()
		, inkEditorView.GetPenColor()
		);
}

//------------------
// utility functions
//------------------

InkPenColor InkEditorPresenter::GetPenColor(const wchar_t * name)
{
	if (0 == wcscmp(name, L"black"))  return PenBlack;
	if (0 == wcscmp(name, L"white"))  return PenWhite;
	if (0 == wcscmp(name, L"yellow")) return PenYellow;
	if (0 == wcscmp(name, L"red"))    return PenRed;
	if (0 == wcscmp(name, L"blue"))   return PenBlue;
	if (0 == wcscmp(name, L"green"))  return PenGreen;
	return PenBlack;
}

InkPenWidth InkEditorPresenter::GetPenWidth(const wchar_t * name)
{
	if (0 == wcscmp(name, L"1px")) return Pen1px;
	if (0 == wcscmp(name, L"2px")) return Pen2px;
	if (0 == wcscmp(name, L"4px")) return Pen4px;
	if (0 == wcscmp(name, L"8px")) return Pen8px;
	return Pen1px;
}

const wchar_t * InkEditorPresenter::GetPenColorName(InkPenColor color)
{
	switch (color)
	{
	case PenBlack:  return L"black";
	case PenWhite:  return L"white";
	case PenYellow: return L"yellow";
	case PenRed:    return L"red";
	case PenBlue:   return L"blue";
	case PenGreen:  return L"green";
	}
	return L"";
}

const wchar_t * InkEditorPresenter::GetPenWidthName(InkPenWidth width)
{
	switch (width)
	{
	case Pen1px: return L"1px";
	case Pen2px: return L"2px";
	case Pen4px: return L"4px";
	case Pen8px: return L"8px";
	}
	return L"";
}
