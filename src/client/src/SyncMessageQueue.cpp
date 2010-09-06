#include "stdafx.h"
#include "SyncMessageQueue.h"

#include "ScopedLock.h"

//-----------------
// SyncMessageQueue
//-----------------

SyncMessageQueue::SyncMessageQueue()
{
	::InitializeCriticalSection(&lock);
}

void SyncMessageQueue::Enqueue(const SyncMessageQueue::Message & message)
{
	ScopedLock lock(lock);
	messages.push(message);
}

SyncMessageQueue::Message SyncMessageQueue::Dequeue()
{
	ScopedLock lock(lock);
	Message message(messages.front());
	messages.pop();
	return message;
}

bool SyncMessageQueue::IsEmpty()
{
	ScopedLock lock(lock);
	return messages.empty();
}

//--------
// Message
//--------

SyncMessageQueue::Message::Message
	( SyncMessageQueue::MessageType   type
	, const wchar_t                 * text
	, double                          value
	)
	: Text  (text)
	, Type  (type)
	, Value (value)
{
}
