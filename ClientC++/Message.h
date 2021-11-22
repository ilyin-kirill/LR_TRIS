#pragma once
#include "pch.h"


enum Address
{
	MIDDLEWARE = 0,
	BROADCAST = -1,
};

enum MessageTypes {
	MT_INIT,
	MT_EXIT,
	MT_CONFIRM,
	MT_GETDATA,
	MT_DATA,
	MT_NODATA
};

struct MessageHeader
{
	int from;
	int to;
	int type;
	int size;
};

class Message
{
private:
	MessageHeader header;
	string msgData;
public:

	Message() {
		header = { 0 };
		msgData = "";
	}

	Message(int to, int from, int type = MT_DATA, const string& data = "") {
		header.from = from;
		header.to = to;
		header.type = type;
		header.size = data.length();
		msgData = data;
	}

	MessageHeader getHeader() {
		return header;
	}

	string getData() {
		return msgData;
	}

	void Send(CSocket& s) {
		s.Send(&header, sizeof(MessageHeader));
		if (header.size) {
			s.Send(msgData.c_str(), header.size + 1);
		}
	}

	int Receive(CSocket& s) {
		s.Receive(&header, sizeof(MessageHeader));
		if (header.size) {
			vector <char> v(header.size);
			s.Receive(&v[0], header.size);
			msgData = string(&v[0], header.size);
		}

		return header.type;
	}

	static void SendMessage(CSocket& s, int to, int from, int type = MT_DATA, const string& data = "") {
		Message m(to, from, type, data);
		m.Send(s);
	}
};

