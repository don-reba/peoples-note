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
		MessageSyncProgress,
		MessageText,
	};

	class Message
	{
	public:

		const MessageType  Type;
		const std::wstring Text;
		const double       Value;

		Message(MessageType type, const wchar_t * text, double value);
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
