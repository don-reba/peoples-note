#pragma once

#include "Blob.h"
#include "ISignalProvider.h"
#include "Guid.h"
#include "Thumbnail.h"

class Note;

class INoteView : public ISignalProvider
{
	MacroIEvent(Attachment)
	MacroIEvent(Close)
	MacroIEvent(Edit)
	MacroIEvent(ToggleMaximize)

public:

	enum AttachmentType
	{
		AudioAttachment,
		MiscAttachment,
	};

	class Attachment
	{
	public:
		AttachmentType Type;
		std::wstring   Text;
		::Guid         Guid;

		Attachment(const ::Guid & guid, AttachmentType type, const std::wstring & text)
			: Guid(guid), Type(type), Text(text)
		{
		}
	};

	typedef std::vector<Attachment> AttachmentList;

public:

	virtual void GetBody(std::wstring & html) = 0;

	virtual void GetNote(Note & note) = 0;

	virtual Guid GetSelecteAttachmentGuid() = 0;

	virtual void GetTitle(std::wstring & text) = 0;

	virtual void Hide() = 0;

	virtual bool IsDirty() = 0;

	virtual bool IsMaximized() = 0;

	virtual void MaximizeWindow() = 0;

	virtual void Render(Thumbnail & thumbnail) = 0;

	virtual void RestoreWindow() = 0;

	virtual void SetNote
		( const Note           & note
		, const std::wstring   & titleText
		, const std::wstring   & subtitleText
		, const std::wstring   & bodyHtml
		, const AttachmentList & attachments
		, const bool             enableChrome
		) = 0;

	virtual void SetWindowTitle(const std::wstring & text) = 0;

	virtual void Show() = 0;
};
