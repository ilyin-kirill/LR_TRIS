#pragma once
#include "pch.h"
#include "Message.h"

class Session
{
private:
	int session_id;
	queue<Message> messages;
	CRITICAL_SECTION cs;
	clock_t  time;
public:
	Session() {
		InitializeCriticalSection(&cs);
	}

	Session(int ID, clock_t t)
		:session_id(ID), time(t) {
		InitializeCriticalSection(&cs);
	}

	~Session() {
		DeleteCriticalSection(&cs);
	}

	int getId() {
		return session_id;
	}

	void setId(int i) {
		session_id = i;
	}

	clock_t getTime() {
		return time;
	}

	void setTime(clock_t t) {
		time = t;
	}

	void Add(Message& m) {
		EnterCriticalSection(&cs);
		messages.push(m);
		LeaveCriticalSection(&cs);
	}

	void Send(CSocket& s) {
		EnterCriticalSection(&cs);
		if (messages.empty())
		{
			Message::SendMessage(s, session_id, MIDDLEWARE, MT_NODATA);
		}
		else
		{
			messages.front().Send(s);
			messages.pop();
		}
		LeaveCriticalSection(&cs);
	}
};

