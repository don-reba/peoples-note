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

void SyncMessageQueue::Enqueue(SyncMessageQueue::Message * message)
{
	ScopedLock lock(lock);
	messages.push_back(message);
}

SyncMessageQueue::Message * SyncMessageQueue::Dequeue()
{
	ScopedLock lock(lock);
	Message * message(&messages.front());
	messages.pop_front();
	return message;
}

bool SyncMessageQueue::IsEmpty()
{
	ScopedLock lock(lock);
	return messages.empty();
}

//-------------
// PlainMessage
//-------------

SyncMessageQueue::PlainMessage::PlainMessage
	( SyncMessageQueue::MessageType type
	)
	: type (type)
{
}

SyncMessageQueue::MessageType SyncMessageQueue::PlainMessage::GetType() const
{
	return type;
}

//------------
// TextMessage
//------------

SyncMessageQueue::TextMessage::TextMessage(const wchar_t * text)
	: text (text)
{
}

SyncMessageQueue::MessageType SyncMessageQueue::TextMessage::GetType() const
{
	return MessageText;
}

const wchar_t * SyncMessageQueue::TextMessage::GetText() const
{
	return text.c_str();
}
