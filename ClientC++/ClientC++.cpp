#include "pch.h"
#include "Message.h"

int clientID;

void connect(CSocket& S) {
    S.Create();
    S.Connect(_T("127.0.0.1"), 12345);
}

void disconnect(CSocket& S) {
    S.Close();
}

void GetData() {
    while (true) {
        CSocket s;
        connect(s);
        Message m;
        Message::SendMessage(s, MIDDLEWARE, clientID, MT_GETDATA);
        if (m.Receive(s) == MT_DATA) {
            cout << "Message from " << m.getHeader().from << ": " << m.getData() << endl;
        }
        disconnect(s);
        Sleep(1000);
    }
}

void Client() {
    AfxSocketInit();
    CSocket client;
    connect(client);
    Message::SendMessage(client, 0, 0, MT_INIT);
    Message m;
    if (m.Receive(client) == MT_CONFIRM) {
        clientID = m.getHeader().to;
        cout << "Client id - " << clientID << endl;
        thread t(GetData);
        t.detach();
    }
    else {
        cout << "error" << endl;
        return;
    }
    disconnect(client);

    while (true) {
        cout << "1. Send Message\n2. Exit\n" << endl;
        int choice;
        cin >> choice;

        switch (choice) {
        case 1: {
            int ClientID = BROADCAST;
            cout << "1. One client\n2. Broadcast\n" << endl;
            int choice2;
            cin >> choice2;
            if (choice2 == 1) {
                cout << "\nEnter ID of client\n";
                cin >> ClientID;
            }


            cout << "\nEnter your Message\n";
            string str;
            cin.ignore();
            getline(cin, str);
            connect(client);
            Message::SendMessage(client, ClientID, clientID, MT_DATA, str);
            if (m.Receive(client) == MT_CONFIRM) {
                cout << "\nsuccess\n" << endl;
            }
            else {
                cout << "\nerror\n" << endl;
            }
            disconnect(client);
            break;
        }

        case 2: {
            connect(client);
            Message::SendMessage(client, MIDDLEWARE, clientID, MT_EXIT);
            if (m.Receive(client) == MT_CONFIRM) {
                cout << "\nsuccess\n" << endl;
            }
            else {
                cout << "\nerror\n" << endl;
            }
            disconnect(client);
            return;
        }
        default:
            break;
        }
    }

}

int main()
{
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
            std::cout << "Client is starting ...\n" << endl;

            time_t rawtime = time(NULL);
            struct tm timeinfo;
            char buffer[1024];
            localtime_s(&timeinfo, &rawtime);
            asctime_s(buffer, &timeinfo);

            cout << "Current Datetime: " << string(buffer) << endl;

            std::cout << "Client is ready ...\n" << endl;

            Client();
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