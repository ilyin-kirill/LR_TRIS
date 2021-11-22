using System;
using System.Collections.Generic;
using System.Text;
using System.Net.Sockets;

namespace ClientCsharp
{
    class Message
    {
        private MessageHeader header;
        private string msgData;

        public MessageHeader getMsgHeader()
        {
            return header;
        }

        public void setMsgData(string msgData)
        {
            this.msgData = msgData;
        }

        public string getMsgData()
        {
            return msgData;
        }

        public Message()
        {
            header.to = 0;
            header.from = 0;
            header.size = 0;
            header.type = 0;
        }

        public Message(int to, int from, MessageTypes type = MessageTypes.MT_DATA, string data = "")
        {
            header.to = to;
            header.from = from;
            header.type = (int)type;
            header.size = data.Length;
            msgData = data;
        }

        public void Send(Socket s)
        {
            s.Send(BitConverter.GetBytes(header.from), sizeof(int), SocketFlags.None);
            s.Send(BitConverter.GetBytes(header.to), sizeof(int), SocketFlags.None);
            s.Send(BitConverter.GetBytes(header.type), sizeof(int), SocketFlags.None);
            s.Send(BitConverter.GetBytes(header.size), sizeof(int), SocketFlags.None);

            if (header.size != 0)
            {
                s.Send(Encoding.UTF8.GetBytes(msgData), header.size, SocketFlags.None);
            }

        }

        public MessageTypes Receive(Socket s)
        {
            byte[] buffer = new byte[4];
            s.Receive(buffer, sizeof(int), SocketFlags.None);
            header.from = BitConverter.ToInt32(buffer, 0);

            buffer = new byte[4];
            s.Receive(buffer, sizeof(int), SocketFlags.None);
            header.to = BitConverter.ToInt32(buffer, 0);

            buffer = new byte[4];
            s.Receive(buffer, sizeof(int), SocketFlags.None);
            header.type = (int)(MessageTypes)BitConverter.ToInt32(buffer, 0);

            buffer = new byte[4];
            s.Receive(buffer, sizeof(int), SocketFlags.None);
            header.size = BitConverter.ToInt32(buffer, 0);

            if (header.size != 0)
            {
                buffer = new byte[header.size];
                s.Receive(buffer, header.size, SocketFlags.None);
                msgData = Encoding.UTF8.GetString(buffer, 0, header.size);
            }

            return (MessageTypes)(int)header.type;
        }
    }
}
