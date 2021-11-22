#include "pch.h"
#include "Message.h"
#include "Session.h"

map<int, shared_ptr<Session>> clientSessions;
int maxClientID = 100;

void TimeOut() {
    while (true)
    {
        for (auto i = clientSessions.begin(); i != clientSessions.end();) {
            if (clientSessions.find(i->first) != clientSessions.end()) {
                if (double(clock() - i->second->getTime()) > 10000) {
                    cout << "Client " << i->first << " has been disconnected" << endl;
                    i = clientSessions.erase(i);
                }
                else
                    i++;
            }
        }
        Sleep(1000);
    }
}


void ProcessClient(SOCKET hSOCK) {
    CSocket s;
    s.Attach(hSOCK);
    Message m;
    int code = m.Receive(s);
    switch (code) {
    case MT_INIT: 
    {
        int ID = ++maxClientID;
        clientSessions[ID] = make_shared<Session>(ID, clock());
        cout << "Client " << ID << " connected" << endl;
        Message::SendMessage(s, ID, MIDDLEWARE, MT_CONFIRM);
        break;
    }
    case MT_EXIT:
    {
        clientSessions.erase(m.getHeader().from);
        cout << "Client " << m.getHeader().from << " disconnected" << endl;
        Message::SendMessage(s, m.getHeader().from, MIDDLEWARE, MT_CONFIRM);
        break;
    }
    case MT_GETDATA:
    {
        auto iSession = clientSessions.find(m.getHeader().from);
        if (iSession != clientSessions.end()) 
        {
            clientSessions[m.getHeader().from]->Send(s);
            clientSessions[m.getHeader().from]->setTime(clock());
        }
        break;
    }
    default: 
    {
        auto iSessionFrom = clientSessions.find(m.getHeader().from);
        if (iSessionFrom != clientSessions.end()) {
            if (clientSessions.find(m.getHeader().to) != clientSessions.end()) {
                clientSessions[m.getHeader().to]->Add(m);
            }
            else if (m.getHeader().to == BROADCAST) {
                for (auto session : clientSessions) {
                    if (session.first != m.getHeader().from)
                        session.second->Add(m);
                }

            }
            Message::SendMessage(s, m.getHeader().from, MIDDLEWARE, MT_CONFIRM);
            clientSessions[m.getHeader().from]->setTime(clock());
        }
        break;
    }
    }
}


void ServerStart() {
    AfxSocketInit();
    CSocket Server;
    Server.Create(12345);
    thread tt(TimeOut);
    tt.detach();
    while (true)
    {
        if (!Server.Listen())
            break;
        CSocket s;
        Server.Accept(s);
        thread t(ProcessClient, s.Detach());
        t.detach();
    }
}

int main()
{
    cout << "Server is starting ...\n" << endl;

    time_t rawtime = time(NULL);
    struct tm timeinfo;
    char buffer[1024];
    localtime_s(&timeinfo, &rawtime);
    asctime_s(buffer, &timeinfo);
    cout << "Current Datetime: " << string(buffer) << endl;

    cout << "Server is listening ...\n" << endl;

    int nRetCode = 0;

    HMODULE hModule = ::GetModuleHandle(nullptr);

    if (hModule != nullptr)
    {
        // инициализировать MFC, а также печать и сообщения об ошибках про сбое
        if (!AfxWinInit(hModule, nullptr, ::GetCommandLine(), 0))
        {
            // TODO: вставьте сюда код для приложения.
            wprintf(L"Критическая ошибка: сбой при инициализации MFC\n");
            nRetCode = 1;
        }
        else
        {
            // TODO: вставьте сюда код для приложения.
            ServerStart();
        }
    }
    else
    {
        // TODO: измените код ошибки в соответствии с потребностями
        wprintf(L"Критическая ошибка: сбой GetModuleHandle\n");
        nRetCode = 1;
    }

    return nRetCode;
}