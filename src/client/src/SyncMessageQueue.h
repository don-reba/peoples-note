#pragma once

#include <queue>

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

		const MessageType  Type;
		const std::wstring Text;

		Message(MessageType type, const wchar_t * text);
	};

private:

	CRITICAL_SECTION lock;

	std::queue<Message> messages;

public:

	SyncMessageQueue();

	void Enqueue(const Message & message);

	Message Dequeue();

	bool IsEmpty();
};
