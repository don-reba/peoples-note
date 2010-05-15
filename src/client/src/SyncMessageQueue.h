#pragma once

#include <queue>

class SyncMessageQueue
{
public:

	enum Message
	{
		MessageNotebooksChanged,
		MessageNotesChanged,
		MessageTagsChanged,
		MessageSyncFailed,
		MessageSyncComplete,
	};

private:

	CRITICAL_SECTION lock;

	std::queue<Message> messages;

public:

	SyncMessageQueue();

	void Enqueue(Message message);

	Message Dequeue();

	bool IsEmpty();
};
