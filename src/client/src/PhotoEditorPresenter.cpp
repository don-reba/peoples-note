#include "stdafx.h"
#include "PhotoEditorPresenter.h"

#include "IFile.h"
#include "INoteListModel.h"
#include "INoteListView.h"
#include "IPhotoEditorModel.h"
#include "IPhotoEditorView.h"
#include "IUserModel.h"
#include "Note.h"
#include "Notebook.h"
#include "Tools.h"
#include "Transaction.h"

using namespace boost;
using namespace std;
using namespace Tools;

PhotoEditorPresenter::PhotoEditorPresenter
	( IFile             & file
	, INoteListModel    & noteListModel
	, INoteListView     & noteListView
	, IPhotoEditorModel & photoEditorModel
	, IPhotoEditorView  & photoEditorView
	, IUserModel        & userModel
	)
	: file             (file)
	, noteListModel    (noteListModel)
	, noteListView     (noteListView)
	, photoEditorModel (photoEditorModel)
	, photoEditorView  (photoEditorView)
	, userModel        (userModel)
{
	noteListView.ConnectNewPhotoNote (bind(&PhotoEditorPresenter::OnNewPhotoNote, *this));
	photoEditorView.ConnectCancel    (bind(&PhotoEditorPresenter::OnCancel,       *this));
	photoEditorView.ConnectOk        (bind(&PhotoEditorPresenter::OnOk,           *this));
	photoEditorView.ConnectCapture   (bind(&PhotoEditorPresenter::OnCapture,      *this));
}

void PhotoEditorPresenter::OnCancel()
{
	photoEditorView.Hide();
}

void PhotoEditorPresenter::OnCapture()
{
	// create note 
	Note note;
	note.name         = photoEditorView.GetTitle();
	note.creationDate = Timestamp::GetCurrentTime();
	note.isDirty      = true;
	note.usn          = 0;

	if (note.name.empty())
		note.name = L"Photo";

	// create resource with hash
	Resource image;
	if (!file.Read(photoEditorView.GetImagePath(), image.Data))
	{
		photoEditorView.Hide();
		return;
	}
	image.Hash = HashWithMD5(image.Data);
	image.Note = note.guid;
	image.Mime = L"image/jpeg";

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

	photoEditorModel.SetQuality
		( GetQualityName(photoEditorView.GetQuality())
		);
	photoEditorModel.SetResolution
		( GetResolutionName(photoEditorView.GetResolution())
		);

	photoEditorView.Hide();

	noteListModel.Reload();
}

void PhotoEditorPresenter::OnNewPhotoNote()
{
	photoEditorView.Show();

	wstring quality;
	photoEditorModel.GetQuality(quality);
	photoEditorView.SetQuality(GetQuality(quality.c_str()));

	wstring resolution;
	photoEditorModel.GetResolution(resolution);
	photoEditorView.SetResolution(GetResolution(resolution.c_str()));
}

void PhotoEditorPresenter::OnOk()
{
	photoEditorView.InitiateCapture();
}

PhotoQuality PhotoEditorPresenter::GetQuality(const wchar_t * quality)
{
	if (0 == wcscmp(quality, L"default")) return PhotoQualityDefault;
	if (0 == wcscmp(quality, L"low"))     return PhotoQualityLow;
	if (0 == wcscmp(quality, L"normal"))  return PhotoQualityNormal;
	if (0 == wcscmp(quality, L"high"))    return PhotoQualityHigh;
	return PhotoQualityDefault;
}

PhotoResolution PhotoEditorPresenter::GetResolution(const wchar_t * resolution)
{
	if (0 == wcscmp(resolution, L"qvga")) return PhotoResolutionQvga;
	if (0 == wcscmp(resolution, L"vga"))  return PhotoResolutionVga;
	if (0 == wcscmp(resolution, L"1m"))   return PhotoResolution1M;
	if (0 == wcscmp(resolution, L"2m"))   return PhotoResolution2M;
	if (0 == wcscmp(resolution, L"3m"))   return PhotoResolution3M;
	return PhotoResolutionVga;
}

const wchar_t * PhotoEditorPresenter::GetQualityName(PhotoQuality quality)
{
	switch (quality)
	{
	case PhotoQualityDefault: return L"default";
	case PhotoQualityLow:     return L"low";
	case PhotoQualityNormal:  return L"normal";
	case PhotoQualityHigh:    return L"high";
	}
	return L"";
}

const wchar_t * PhotoEditorPresenter::GetResolutionName(PhotoResolution resolution)
{
	switch (resolution)
	{
	case PhotoResolutionQvga: return L"qvga";
	case PhotoResolutionVga:  return L"vga";
	case PhotoResolution1M:   return L"1m";
	case PhotoResolution2M:   return L"2m";
	case PhotoResolution3M:   return L"3m";
	}
	return L"";
}
