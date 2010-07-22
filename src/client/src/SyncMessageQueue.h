#pragma once

#include <boost/ptr_container/ptr_deque.hpp>

class SyncMessageQueue
{
public:

	enum MessageType
	{
		MessageNotebooksChanged,
		MessageNotesChanged,
		MessageTagsChanged,
		MessageSyncFailed,
		MessageSyncComplete,
		MessageText,
	};

	class Message
	{
	public:
		
		virtual MessageType GetType() const = 0;
	};

	class PlainMessage : public Message
	{
	private:

		MessageType type;

	public:

		PlainMessage(MessageType type);

		virtual MessageType GetType() const;
	};

	class TextMessage : public Message
	{
	private:

		std::wstring text;

	public:

		TextMessage(const wchar_t * text);

		virtual MessageType GetType() const;

		const wchar_t * GetText() const;
	};

private:

	CRITICAL_SECTION lock;

	boost::ptr_deque<Message> messages;

public:

	SyncMessageQueue();

	void Enqueue(Message * message);

	Message * Dequeue();

	bool IsEmpty();
};
