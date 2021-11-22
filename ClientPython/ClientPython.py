import socket, struct
from enum import IntEnum
from threading import Thread
from time import sleep
from datetime import datetime
import sys

HOST = 'localhost'
PORT = 12345

MT_INIT = 0
MT_EXIT = 1
MT_CONFIRM = 2
MT_GETDATA = 3
MT_DATA = 4
MT_NODATA = 5

class Addresses(IntEnum):
    MIDDLEWARE = 0,
    BROADCAST = -1,

class MsgHeader():
    def __init__(self, msgFrom=0, msgTo=0, msgType=0, msgSize=0):
        self.msgFrom=msgFrom
        self.msgTo=msgTo
        self.msgType=msgType
        self.msgSize=msgSize
    def HeaderInit(self, header):
        self.msgFrom=header[0]
        self.msgTo=header[1]
        self.msgType=header[2]
        self.msgSize=header[3]

class Message():
    def __init__(self, From=0, To=0, Type=MT_DATA, Data=''):
        self.msgHeader=MsgHeader()
        self.msgHeader.msgFrom=From;
        self.msgHeader.msgTo=To;
        self.msgHeader.msgType=Type;
        self.msgHeader.msgSize=int(len(Data))
        self.msgData=Data
    def SendData(self, s):
        s.send(struct.pack('i', self.msgHeader.msgFrom))
        s.send(struct.pack('i', self.msgHeader.msgTo))
        s.send(struct.pack('i', self.msgHeader.msgType))
        s.send(struct.pack('i', self.msgHeader.msgSize))
        if self.msgHeader.msgSize>0:
            s.send(struct.pack(f'{self.msgHeader.msgSize}s', self.msgData.encode('utf-8')))
    def ReceiveData(self, s):
        self.msgHeader=MsgHeader()
        self.msgHeader.HeaderInit(struct.unpack('iiii', s.recv(16)))
        if self.msgHeader.msgSize>0:
            self.msgData=struct.unpack(f'{self.msgHeader.msgSize}s', s.recv(self.msgHeader.msgSize))[0]
        return self.msgHeader.msgType

# Отправка сообщения
def SendMessage(Socket, From, To, Type=MT_DATA, Data=''):
    m=Message(From, To, Type, Data)
    m.SendData(Socket)

# Прием сообщения
def Receive(Socket):
    m=Message()
    res=m.ReceiveData(Socket)
    return res

def GetData(ID):
    while True:
        clientSock=socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        clientSock.connect((HOST, PORT))
        SendMessage(clientSock, ID, 0, MT_GETDATA)
        msg=Message()
        if (msg.ReceiveData(clientSock)==MT_DATA):
            print('Message from client '+str(msg.msgHeader.msgFrom)+': '+msg.msgData.decode('utf-8'))
        clientSock.close()

clientID=0

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.connect((HOST, PORT))
    SendMessage(s, 0, 0, MT_INIT)
    msg=Message()
    if (msg.ReceiveData(s) == MT_CONFIRM):
        clientID=msg.msgHeader.msgTo
        print("Client is starting ...\n");
        print("Client is ready ...\n");
        print('Client id - '+str(clientID))
        t=Thread(target=GetData, args=(clientID,), daemon=True)
        t.start()      
        s.close()
    else:
        print('Error')
        sys.exit()
while True:
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        choice=int(input('1. Send Message\n2. Exit\n'))
        if choice==1:
            ClientID=Addresses.BROADCAST
            choice2=int(input('1. For one client\n2.Broadcast\n'))
            if choice2==1:
                ClientID=int(input('\nEnter ID of client\n'))
            str=input('\nEnter your Message\n')
            s.connect((HOST, PORT))
            SendMessage(s, clientID, ClientID, MT_DATA, str)
            if msg.ReceiveData(s) == MT_CONFIRM:
                print('\nsuccess\n')
            else: print('\nerror\n')
            s.close()
        elif choice==2:
           s.connect((HOST, PORT))
           SendMessage(s,clientID, 0, MT_EXIT)
           if msg.ReceiveData(s) == MT_CONFIRM:
               print('\nsuccess\n')
           else: print('\nerror\n')
           s.close()
        else:
            print('Please, print 1 or 2')




