#include "stdafx.h"
#include "NotePresenter.h"

#include "Attachment.h"
#include "EnNoteTranslator.h"
#include "Guid.h"
#include "INoteListModel.h"
#include "INoteListView.h"
#include "INoteTagListModel.h"
#include "INoteView.h"
#include "IUserModel.h"
#include "Tools.h"
#include "Transaction.h"
#include "WaitCursor.h"

#include <fstream>
#include <set>

using namespace boost;
using namespace std;
using namespace Tools;

//----------
// interface
//----------

NotePresenter::NotePresenter
	( const wstring     & deviceDocumentPath
	, INoteListModel    & noteListModel
	, INoteListView     & noteListView
	, INoteTagListModel & noteTagListModel
	, INoteView         & noteView
	, IUserModel        & userModel
	, EnNoteTranslator  & enNoteTranslator
	)
	: deviceDocumentPath (deviceDocumentPath)
	, noteListModel      (noteListModel)
	, noteListView       (noteListView)
	, noteTagListModel   (noteTagListModel)
	, noteView           (noteView)
	, userModel          (userModel)
	, enNoteTranslator   (enNoteTranslator)
{
	noteListView.ConnectOpenNote (bind(&NotePresenter::OnOpenNote, this));

	noteTagListModel.ConnectCommitted (bind(&NotePresenter::OnTagListCommitted, this));

	noteView.ConnectAttachment     (bind(&NotePresenter::OnAttachment,     this));
	noteView.ConnectClose          (bind(&NotePresenter::OnCloseNote,      this));
	noteView.ConnectDelete         (bind(&NotePresenter::OnDeleteNote,     this));
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
	{
		MacroWaitCursor;
		userModel.GetResource(guid, resource);
		if (resource.Data.empty())
			return;
	}

	wstring path = noteView.GetSavePath
		( L"Save attachment..."
		, resource.FileName
		, deviceDocumentPath
		);
	if (path.empty())
		return;

	MacroWaitCursor;

	ofstream file(path.c_str(), ios::binary);
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

	MacroWaitCursor;

	{
		Transaction transaction(userModel);

		Note note;
		noteView.GetNote(note);
		note.modificationDate = Timestamp::GetCurrentTime();
		note.isDirty = true;

		set<int> dirtyCheckboxIds;
		noteView.GetDirtyCheckboxIds(dirtyCheckboxIds);

		wstring srcXml, dstXml;
		userModel.GetNoteBody(note.guid, srcXml);
		enNoteTranslator.ApplyXmlModifications(srcXml, dstXml, dirtyCheckboxIds);

		wstring bodyText;
		enNoteTranslator.ConvertToText(dstXml, bodyText);

		noteView.SetNote(note, L"", L"", L"", AttachmentViewInfoList(), false); // clear

		Notebook notebook;
		userModel.GetLastUsedNotebook(notebook);
		userModel.AddNote(note, dstXml, bodyText, notebook);

		userModel.DeleteNoteThumbnail(note.guid);
		noteListView.UpdateThumbnail(note.guid);
	}

	noteListModel.Reload();
	noteListModel.NotifyOfNoteChange();
}

void NotePresenter::OnDeleteNote()
{
	Note note;
	noteView.GetNote(note);
	userModel.DeleteNote(note.guid);
	
	noteView.Hide();

	noteListModel.Reload();
}

void NotePresenter::OnOpenNote()
{
	WaitCursor waitCursor;

	Transaction transaction(userModel);

	Guid guid(noteListView.GetSelectedNoteGuid());

	Note note;
	userModel.GetNote(guid, note);

	wstring subtitle;
	CreateSubtitle(note, subtitle);

	wstring body;
	wstring html;
	userModel.GetNoteBody(guid, body);
	enNoteTranslator.ConvertToHtml(body, html, true);

	AttachmentList         attachments;
	AttachmentViewInfoList attachmentViews;
	userModel.GetNoteAttachments(guid, attachments);
	transform
		( attachments.begin()
		, attachments.end()
		, back_inserter(attachmentViews)
		, &NotePresenter::ConvertAttachment
		);

	try
	{
		noteView.SetNote(note, note.name, subtitle, html, attachmentViews, true);
	}
#ifdef WINCE
	catch (const std::exception & e)
	{
		::NKDbgPrintfW
			( L"NotePresenter::OnOpenNote: %s\n"
			, ConvertToUnicode(e.what()).c_str()
			);
	}
#else // WINCE
	catch (const std::exception &)
	{
		// try to continue
	}
#endif // WINCE
	noteView.Show();
}

void NotePresenter::OnTagListCommitted()
{
	WaitCursor waitCursor;

	Transaction transaction(userModel);

	Guid guid(noteListView.GetSelectedNoteGuid());

	Note note;
	userModel.GetNote(guid, note);
	note.isDirty = true;

	wstring subtitle;
	CreateSubtitle(note, subtitle);

	noteView.SetSubtitle(subtitle);

	userModel.UpdateNote(note.guid, note);
	noteListModel.Reload();
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
