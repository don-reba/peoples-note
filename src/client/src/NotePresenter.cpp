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
	( INoteListModel   & noteListModel
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

	vector<wchar_t> folder(MAX_PATH);
	::SHGetSpecialFolderPath
		( NULL           // hwndOwner
		, &folder[0]     // lpszPath
		, CSIDL_PERSONAL // nFolder
		, TRUE           // fCreate
		);

	OPENFILENAME openFileName = { sizeof(openFileName) };
	openFileName.lpstrFile       = &fileName[0];
	openFileName.nMaxFile        = MAX_PATH;
	openFileName.lpstrInitialDir = &folder[0];
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

	wstring body;
	userModel.GetNoteBody(guid, body);
	Note note;
	userModel.GetNote(guid, note);

	// subtitle

	wstring subtitle(L"created on ");
	subtitle.append(note.creationDate.GetFormattedDateTime());

	// tags

	TagList tags;
	userModel.GetNoteTags(note, tags);
	if (!tags.empty())
	{
		subtitle.append(L"\ntags: ");
		subtitle.append(tags[0].name);
		for (int i = 1; i != tags.size(); ++i)
		{
			subtitle.append(L", ");
			subtitle.append(tags[i].name);
		}
	}

	// body

	wstring html;
	enNoteTranslator.ConvertToHtml(body, html);

	// attachments

	struct
	{
		INoteView::Attachment operator () (const Attachment & attachment)
		{
			return INoteView::Attachment
				( attachment.Guid
				, INoteView::MiscAttachment
				, attachment.FileName
				);
		}
	} convertAttachment;

	AttachmentList            attachments;
	INoteView::AttachmentList attachmentViews;
	userModel.GetNoteAttachments(note.guid, attachments);
	transform
		( attachments.begin()
		, attachments.end()
		, back_inserter(attachmentViews)
		, convertAttachment
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
