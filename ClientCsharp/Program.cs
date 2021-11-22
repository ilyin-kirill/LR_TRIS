using System;
using System.Net;
using System.Net.Sockets;
using System.Threading;

namespace ClientCsharp
{
    enum Address
    {
        MIDDLEWARE = 0,
        BROADCAST = -1,
    };

    enum MessageTypes
    {
        MT_INIT,
        MT_EXIT,
        MT_CONFIRM,
        MT_GETDATA,
        MT_DATA,
        MT_NODATA
    };

    struct MessageHeader
    {
        public int from;
        public int to;
        public int type;
        public int size;
    };
    class Program
    {
        private static int clientID;

        static void SendMessage(Socket s, int To, int From, MessageTypes Type = MessageTypes.MT_DATA, string Data = "")
        {
            Message m = new Message(To, From, Type, Data);
            m.Send(s);
        }

        static void connect(Socket s, IPEndPoint endPoint)
        {
            s.Connect(endPoint);
        }

        static void disconnect(Socket s)
        {
            s.Shutdown(SocketShutdown.Both);
            s.Close();
        }

        public static void GetData()
        {
            while (true)
            {
                IPEndPoint endPoint = new IPEndPoint(IPAddress.Parse("127.0.0.1"), 12345);
                Socket s = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
                Message m = new Message();

                connect(s, endPoint);
                SendMessage(s, 0, clientID, MessageTypes.MT_GETDATA);
                if (m.Receive(s) == MessageTypes.MT_DATA)
                {
                    Console.WriteLine("Message from client " + m.getMsgHeader().from + ": " + m.getMsgData());
                }
                disconnect(s);
                Thread.Sleep(1000);
            }
        }


        public static void Main(string[] args)
        {
            Message m = new Message();
            IPEndPoint endPoint = new IPEndPoint(IPAddress.Parse("127.0.0.1"), 12345);
            Socket s = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);

            connect(s, endPoint);
            if (!s.Connected)
                return;

            SendMessage(s, (int)Address.MIDDLEWARE, 0, MessageTypes.MT_INIT);
            if (m.Receive(s) == MessageTypes.MT_CONFIRM)
            {
                clientID = m.getMsgHeader().to;
                Console.WriteLine("Client is starting ...\n");
                DateTime now = DateTime.Now;
                Console.WriteLine("Current Datetime: " + now.ToString("R") + "\n");
                Console.WriteLine("Client is ready...\n");
                Console.WriteLine("Сlient id - " + clientID);
                Thread t = new Thread(GetData);
                t.Start();
            }
            disconnect(s);

            while (true)
            {
                endPoint = new IPEndPoint(IPAddress.Parse("127.0.0.1"), 12345);
                s = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
                Console.WriteLine("1. Send Message\n2. Exit\n");
                int choice = Convert.ToInt32(Console.ReadLine());
                switch (choice)
                {
                    case 1:
                        {
                            int ClientID = (int)Address.BROADCAST;
                            Console.WriteLine("1. For one client\n2.Broadcast\n");
                            int choice2 = Convert.ToInt32(Console.ReadLine());
                            if (choice2 == 1)
                            {
                                Console.WriteLine("\nEnter ID of Client\n");
                                ClientID = Convert.ToInt32(Console.ReadLine());
                            }

                            Console.WriteLine("\nEnter your message\n");
                            string str = Console.ReadLine().ToString();
                            connect(s, endPoint);
                            SendMessage(s, ClientID, clientID, MessageTypes.MT_DATA, str);
                            if (m.Receive(s) == MessageTypes.MT_CONFIRM)
                            {
                                Console.WriteLine("\nsuccess\n");
                            }
                            else
                            {
                                Console.WriteLine("\nerror\n");
                            }
                            disconnect(s);
                            break;

                        }
                    case 2:
                        {
                            connect(s, endPoint);
                            SendMessage(s, (int)Address.MIDDLEWARE, clientID, MessageTypes.MT_EXIT);
                            if (m.Receive(s) == MessageTypes.MT_CONFIRM)
                            {
                                Console.WriteLine("\nsuccess\n");
                            }
                            else
                            {
                                Console.WriteLine("\nerror\n");
                            }
                            disconnect(s);
                            Environment.Exit(0);
                            break;
                        }

                    default:
                        break;
                }
            }
        }
    }
}
