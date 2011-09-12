#pragma once
#include "INoteView.h"

#include "Note.h"

class MockNoteView : public INoteView
{
	MacroTestEvent(Attachment)
	MacroTestEvent(Close)
	MacroTestEvent(Edit)
	MacroTestEvent(ToggleMaximize)

public:

	AttachmentViewInfoList attachments;
	Guid selectedAttachment;

	std::wstring body;
	std::wstring subtitle;
	std::wstring title;
	std::wstring windowTitle;

	bool enableChrome;
	bool isDirty;
	bool isMaximized;
	bool isShown;

	Note note;

	SIZE renderSize;

public:

	MockNoteView();

	virtual void GetBody(std::wstring & html);

	virtual void GetNote(Note & note);

	virtual Guid GetSelecteAttachmentGuid();

	virtual void GetTitle(std::wstring & text);

	virtual void Hide();

	virtual bool IsDirty();

	virtual bool IsMaximized();

	virtual void MaximizeWindow();

	virtual void Render(Thumbnail & thumbnail);

	virtual void RestoreWindow();

	virtual void SetNote
		( const Note                   & note
		, const std::wstring           & titleText
		, const std::wstring           & subtitleText
		, const std::wstring           & bodyHtml
		, const AttachmentViewInfoList & attachments
		, const bool                     enableChrome
		);

	virtual void SetWindowTitle(const std::wstring & text);

	virtual void Show();
};
