#include "stdafx.h"
#include "NotePresenter.h"

#include "Attachment.h"
#include "EnNoteTranslator.h"
#include "Guid.h"
#include "INoteListModel.h"
#include "INoteListView.h"
#include "INoteView.h"
#include "IUserModel.h"
#include "Tools.h"
#include "Transaction.h"

#include <fstream>

using namespace boost;
using namespace std;
using namespace Tools;

//----------
// interface
//----------

NotePresenter::NotePresenter
	( const wstring    & deviceDocumentPath
	, INoteListModel   & noteListModel
	, INoteListView    & noteListView
	, INoteView        & noteView
	, IUserModel       & userModel
	, EnNoteTranslator & enNoteTranslator
	)
	: noteListModel    (noteListModel)
	, noteListView     (noteListView)
	, noteView         (noteView)
	, userModel        (userModel)
	, enNoteTranslator (enNoteTranslator)
{
	noteListView.ConnectOpenNote   (bind(&NotePresenter::OnOpenNote,       this));
	noteView.ConnectAttachment     (bind(&NotePresenter::OnAttachment,     this));
	noteView.ConnectClose          (bind(&NotePresenter::OnCloseNote,      this));
	noteView.ConnectToggleMaximize (bind(&NotePresenter::OnToggleMaximize, this));
}

//---------------
// event handlers
//---------------

void NotePresenter::OnAttachment()
{
	Guid guid(noteView.GetSelecteAttachmentGuid());
	if (guid.IsEmpty())
		return;

	Resource resource;
	userModel.GetResource(guid, resource);
	if (resource.Data.empty())
		return;

	vector<wchar_t> fileName(MAX_PATH);
	copy
		( resource.FileName.begin()
		, resource.FileName.size() <= fileName.size()
		? resource.FileName.end()
		: resource.FileName.begin() + fileName.size()
		, fileName.begin()
		);

	OPENFILENAME openFileName = { sizeof(openFileName) };
	openFileName.lpstrFile       = &fileName[0];
	openFileName.nMaxFile        = MAX_PATH;
	openFileName.lpstrInitialDir = deviceDocumentPath.c_str();
	openFileName.lpstrTitle      = L"Save attachment...";
	openFileName.Flags           = OFN_OVERWRITEPROMPT;
	BOOL result(::GetSaveFileName(&openFileName));
	if (0 == result || 6 == result)
		return;

	ofstream file(&fileName[0], ios::binary);
	if (!file)
		return;
	file.write
		( reinterpret_cast<const char *>(&resource.Data[0])
		, resource.Data.size()
		);
}

void NotePresenter::OnCloseNote()
{
	if (!noteView.IsDirty())
		return;

	wstring bodyHtml;
	noteView.GetBody(bodyHtml);

	wstring bodyXml;
	enNoteTranslator.ConvertToXml(bodyHtml, bodyXml);

	wstring bodyText;
	enNoteTranslator.ConvertToText(bodyXml, bodyText);

	Note note;
	noteView.GetNote(note);
	note.modificationDate = Timestamp::GetCurrentTime();
	note.isDirty = true;

	{
		Transaction transaction(userModel);

		Notebook notebook;
		userModel.GetLastUsedNotebook(notebook);
		userModel.AddNote(note, bodyXml, bodyText, notebook);

		userModel.DeleteNoteThumbnail(note.guid);
		noteListView.UpdateThumbnail(note.guid);
	}

	noteListModel.Reload();
}

void NotePresenter::OnOpenNote()
{
	Transaction transaction(userModel);

	Guid guid(noteListView.GetSelectedNoteGuid());

	Note note;
	userModel.GetNote(guid, note);

	wstring subtitle;
	CreateSubtitle(note, subtitle);

	wstring body;
	wstring html;
	userModel.GetNoteBody(guid, body);
	enNoteTranslator.ConvertToHtml(body, html);

	AttachmentList         attachments;
	AttachmentViewInfoList attachmentViews;
	userModel.GetNoteAttachments(guid, attachments);
	transform
		( attachments.begin()
		, attachments.end()
		, back_inserter(attachmentViews)
		, &NotePresenter::ConvertAttachment
		);

	noteView.SetNote(note, note.name, subtitle, html, attachmentViews, true);
	noteView.Show();
}

void NotePresenter::OnToggleMaximize()
{
	if (noteView.IsMaximized())
		noteView.RestoreWindow();
	else
		noteView.MaximizeWindow();
}

//------------------
// utility functions
//------------------

void NotePresenter::CreateSubtitle(const Note & note, wstring & subtitle)
{
	subtitle = L"created on ";
	subtitle.append(note.creationDate.GetFormattedDateTime());

	TagList tags;
	userModel.GetNoteTags(note, tags);
	if (!tags.empty())
	{
		subtitle.append(L"\ntags: ");
		subtitle.append(tags[0].name);
		for (int i(1); i != tags.size(); ++i)
		{
			subtitle.append(L", ");
			subtitle.append(tags[i].name);
		}
	}
}

const wchar_t * NotePresenter::GetAttachmentImageUrl(const Attachment & attachment)
{
	const wstring::size_type extLocation(attachment.FileName.rfind(L'.'));
	if (extLocation == wstring::npos)
	{
		wstring mime;
		mime.reserve(attachment.Mime.size());
		transform
			( attachment.Mime.begin()
			, attachment.Mime.end()
			, back_inserter(mime)
			, tolower
			);
		if (mime == L"audio/mpeg")
			return L"url(attachment-mp3.png)";
		if (mime == L"application/pdf")
			return L"url(attachment-pdf.png)";
		if (StartsWith(mime, L"audio"))
			return L"url(attachment-sound.png)";
		if (StartsWith(mime, L"image"))
			return L"url(attachment-image.png)";
	}
	else
	{
		wstring ext;
		transform
			( attachment.FileName.begin() + extLocation
			, attachment.FileName.end()
			, back_inserter(ext)
			, tolower
			);

		if (ext == L".mp3")
			return L"url(attachment-mp3.png)";

		if (ext == L".pdf")
			return L"url(attachment-pdf.png)";

		const wchar_t * imageExts[]
			= { L".gif", L".jpg", L".jpeg", L".png", L".bmp", L".tif", L".tiff", L".raw" };
		for (int i(0); i != GetArraySize(imageExts); ++i)
			if (ext == imageExts[i]) return L"url(attachment-image.png)";

		const wchar_t * audioExts[]
			= { L".wav", L".mpg", L".mpeg", L".amr", L".ogg", L".flac", L".ape" };
		for (int i(0); i != GetArraySize(audioExts); ++i)
			if (ext == audioExts[i]) return L"url(attachment-sound.png)";
	}
	return L"url(attachment-misc.png)";
}

AttachmentViewInfo NotePresenter::ConvertAttachment(const Attachment & attachment)
{
	return AttachmentViewInfo
		( attachment.Guid
		, GetAttachmentImageUrl(attachment)
		, attachment.FileName
		);
}
