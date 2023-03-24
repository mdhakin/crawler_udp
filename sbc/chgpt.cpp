#include <iostream>
#include <boost/asio.hpp>
#include <thread>
#include <mutex>
#include <vector>
#include <string.h>
#include <cstdint>
#include <cstring>
#include <chrono>

using boost::asio::ip::udp;


std::mutex mutex;
std::vector<std::string> messages; // shared data structure

std::string name = "";
std::string thisip = "";
std::string targetip = "";
int port_no = 0;
int loop_speed = 100;
bool bEnd = false;

struct data_frame
{
    uint8_t id = 0;
    uint8_t d0 = 255;
    uint8_t d1 = 255;
    uint8_t d2 = 255;
    uint8_t d3 = 255;
    uint8_t d4 = 255;
    uint8_t d5 = 255;
    uint8_t d6 = 255;
    uint8_t d7 = 255;
};

data_frame ddf;
data_frame data1;

char* uint8_tToCharArray(uint8_t* uintArray, size_t size);
uint8_t* charArrayToUint8_t(char* charArray, size_t size);
uint8_t* dataFrameToUint8_tArray(data_frame* df, size_t size);
void mainloop();

void send_thread_func()
{
    try
    {
        boost::asio::io_context io_context;

        // create socket for broadcasting
        udp::socket socket(io_context, udp::v4());
        socket.set_option(boost::asio::socket_base::broadcast(true));
        socket.set_option(boost::asio::socket_base::reuse_address(true));

        // create endpoint for broadcasting
        udp::endpoint broadcast_endpoint(boost::asio::ip::make_address(targetip), 12345);

        /*
        // create buffer to send datadata_frame ddf;
        uint8_t* ue = dataFrameToUint8_tArray(&ddf,9);
        char* chue = uint8_tToCharArray(ue,9);
        std::string sUE = std::string(chue);
        //std::string message = "sbc        i\n";
        std::string message = sUE;
        std::array<char, 9> send_buffer;
        std::copy(message.begin(), message.end(), send_buffer.begin());

        uint8_t* ue2 = dataFrameToUint8_tArray(&data1,9);
        char* chue2 = uint8_tToCharArray(ue2,9);
        std::string sUE2 = std::string(chue2);
        //std::string message = "sbc        i\n";
        std::string message2 = sUE2;
        std::array<char, 9> send_buffer2;
        std::copy(message2.begin(), message2.end(), send_buffer2.begin());
        */
        int64_t lp = (int64_t)loop_speed;
        // send broadcast message in a loop
        while (true)
        {
            if(bEnd)
            {
                break;
            }


            // create buffer to send datadata_frame ddf;
            uint8_t* ue = dataFrameToUint8_tArray(&ddf,9);
            char* chue = uint8_tToCharArray(ue,9);
            std::string sUE = std::string(chue);
            //std::string message = "sbc        i\n";
            std::string message = sUE;
            std::array<char, 9> send_buffer;
            std::copy(message.begin(), message.end(), send_buffer.begin());

            uint8_t* ue2 = dataFrameToUint8_tArray(&data1,9);
            char* chue2 = uint8_tToCharArray(ue2,9);
            std::string sUE2 = std::string(chue2);
            //std::string message = "sbc        i\n";
            std::string message2 = sUE2;
            std::array<char, 9> send_buffer2;
            std::copy(message2.begin(), message2.end(), send_buffer2.begin());

            socket.send_to(boost::asio::buffer(send_buffer), broadcast_endpoint);
            socket.send_to(boost::asio::buffer(send_buffer2), broadcast_endpoint);
            std::this_thread::sleep_for(std::chrono::milliseconds(lp)); // wait for 1 second before sending next message
            
            // add message to shared data structure
            std::lock_guard<std::mutex> lock(mutex);
            //messages.push_back(message2);
        }
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}

void receive_thread_func()
{
    try
    {
        boost::asio::io_context io_context;

        // create socket for receiving broadcasts
        udp::socket receive_socket(io_context, udp::endpoint(udp::v4(), (unsigned short)port_no));

        // create buffer to receive data
        std::array<char, 9> receive_buffer;
        udp::endpoint sender_endpoint;

        // receive broadcast message in a loop
        while (true)
        {
            receive_socket.receive_from(boost::asio::buffer(receive_buffer), sender_endpoint);
            // print received message
            std::string str = std::string(receive_buffer.data(), receive_buffer.size());
            if(str.substr(0, receive_buffer.size()) != "Hp  Pavilion" || str.substr(0, receive_buffer.size()) != "Hp  Pavilion")
            {
                //std::cout << "Received message: " << std::string(receive_buffer.data(), receive_buffer.size()) << std::endl;
                //std::cout << "Size: " << receive_buffer.size() << std::endl;
            }else
            {
                
            }
            if(bEnd)
            {
                break;
            }

            // add message to shared data structure
            std::lock_guard<std::mutex> lock(mutex);
            messages.push_back(std::string(receive_buffer.data(), receive_buffer.size()));
        }
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}

int main(int argc, char* argv[])
{
    
    //std::cout << sizeof(data_frame) << std::endl;

    
    ddf.id = 200;
    ddf.d0 = 100;
    ddf.d1 = 101;
    ddf.d2 = 102;
    ddf.d3 = 103;
    ddf.d4 = 104;
    ddf.d5 = 105;
    ddf.d6 = 106;
    ddf.d7 = 107;

    data1.id = 201;
    data1.d0 = 250;
    data1.d1 = 150;
    data1.d2 = 175;
    data1.d3 = 202;
    data1.d4 = 208;
    data1.d5 = 240;
    data1.d6 = 78;
    data1.d7 = 5;


    uint8_t* uintArray = dataFrameToUint8_tArray(&ddf,9);
    char* chrArray = uint8_tToCharArray(uintArray,9);
    uint8_t* returnedArray = charArrayToUint8_t(chrArray,9);


    //std::cout << static_cast<unsigned int>(uintArray[3]) << std::endl;
    //std::cout << chrArray[3] << std::endl;
    //std::cout << static_cast<unsigned int>(returnedArray[3]) << std::endl;
    


    if(argc == 6)
    {
        name = argv[1];
        thisip = argv[2];
        targetip = argv[3];
        port_no = atoi(argv[4]);
        loop_speed = atoi(argv[5]);
        std::cout << name << std::endl;
        std::cout << thisip << std::endl;
        std::cout << targetip << std::endl;
        std::cout << port_no << std::endl;
        std::cout << loop_speed << std::endl;
    }
    // create threads for sending and receiving broadcasts
    std::thread send_thread(send_thread_func);
    std::thread receive_thread(receive_thread_func);
    std::thread mnloop(mainloop);
    // wait for threads to finish
    send_thread.join();
    receive_thread.join();
    mnloop.join();
    // print all messages in shared data structure
    //std::lock_guard<std::mutex> lock(mutex);
    //std::cout << "All messages:" << std::endl;
    //for (const auto& message : messages)
    //{
      //  std::cout << message << std::endl;
   // }

    return 0;
}


// Convert uint8_t array to char*
char* uint8_tToCharArray(uint8_t* uintArray, size_t size) {
    char* charArray = new char[size];
    std::memcpy(charArray, uintArray, size);
    return charArray;
}

// Convert char* to uint8_t array
uint8_t* charArrayToUint8_t(char* charArray, size_t size) {
    uint8_t* uintArray = new uint8_t[size];
    std::memcpy(uintArray, charArray, size);
    return uintArray;
}

uint8_t *dataFrameToUint8_tArray(data_frame *df, size_t size)
{
    uint8_t* uintArray = new uint8_t[size];
    uintArray[0] = df->id;
    uintArray[1] = df->d0;
    uintArray[2] = df->d1;
    uintArray[3] = df->d2;
    uintArray[4] = df->d3;
    uintArray[5] = df->d4;
    uintArray[6] = df->d5;
    uintArray[7] = df->d6;
    uintArray[8] = df->d7;

    //std::memcpy(uintArray, df, size);
    return uintArray;
}

void mainloop()
{
    

    // Command line loop to modify the data array
    while (true)
    {
        std::string command;
        std::cout << ">> ";
        std::cin >> command;
        //std::cout << command << std::endl;
        if (command == "quit" || command == "q" || command == "e")
        {
            //std::cout << "quit " << std::endl;
            bEnd = true;
            break;
        }
        else if (command == "set")
        {
            unsigned int value;
            std::string sValue = "";

            unsigned int index;
            std::string sNewValue = "";
            unsigned int newValue;

            std::cout << "data1 or data2? >>";
            std::cin >> index;
            std::cout << "What byte 0 - 7? >>";
            std::cin >> sValue;
            value = std::stoul(sValue);
            std::cout << "new value 0 - 255? >>";
            std::cin >> sNewValue;
            newValue = std::stoul(sNewValue);

            if(index == 1 && newValue >=0 && newValue < 256)
            {
               if (value == 0)
               {
                ddf.d0 = newValue;
               }else if(value == 1)
               {
                ddf.d1 = newValue;
               }else if(value == 2)
               {
                ddf.d2 = newValue;
               }else if(value == 3)
               {
                ddf.d3 = newValue;
               }else if(value == 4)
               {
                ddf.d4 = newValue;
               }else if(value == 5)
               {
                ddf.d5 = newValue;
               }else if(value == 6)
               {
                ddf.d6 = newValue;
               }else if(value == 7)
               {
                ddf.d7 = newValue;
               }else
               {
                std::cout << "Invalid bit entry" << std::endl;
                std::cout << std::dec << index << std::endl;
                std::cout << std::dec << value << std::endl;
                std::cout << sNewValue << std::endl;
                std::cout << newValue << std::endl;
               }
               
            }else if (index == 2 && newValue >=0 && newValue < 256)
            {
                if (value == 0)
                {
                    data1.d0 = newValue;
                }else if(value == 1)
                {
                    data1.d1 = newValue;
                }else if(value == 2)
                {
                    data1.d2 = newValue;
                }else if(value == 3)
                {
                    data1.d3 = newValue;
                }else if(value == 4)
                {
                    data1.d4 = newValue;
                }else if(value == 5)
                {
                    data1.d5 = newValue;
                }else if(value == 6)
                {
                    data1.d6 = newValue;
                }else if(value == 7)
                {
                    data1.d7 = newValue;
                }
            }else
            {
                std::cout << "Invalid Datastructure Entry" << std::endl;
                std::cout << std::dec << index << std::endl;
                std::cout << std::dec << value << std::endl;
                std::cout << sNewValue << std::endl;
                std::cout << newValue << std::endl;
            }
            

            command = "";
          
            //std::cout << messages[2] << std::endl;
        }
        else if (command == "get")
        {
            std::size_t index;
            std::cout << "Enter index >>";
            std::cin >> index;
		/*
            if (index < data.size())
            {
                std::cout << static_cast<int>(data[index]) << std::endl;
            }
            else
            {
                std::cerr << "Invalid index: " << index << std::endl;
            }
		*/
            std::cout << messages[index] << std::endl;
            command = "";
            //std::cout << index << std::endl;
        }
        else
        {
            std::cerr << "Invalid command: " << command << std::endl;
            command = "";
        }
    }
    return;
    // Stop the IO context and

}









































/*

C Sharp Console App

using System;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;

class Program
{
    byte[] data1 = { 255, 255, 255, 255, 255, 255, 255, 255, 255 };
    byte[] data2 = { 255, 255, 255, 255, 255, 255, 255, 255, 255 };

    static UdpClient udpClient;
    static TcpListener tcpListener;
    //ipString ipstring = new ipString();
    //static string ipAdder = "192.168.1.";

    static void Main(string[] args)
    {
        
        

        // Start the UDP sender thread
        Thread udpThread = new Thread(new ThreadStart(SendUdpPackets));
        udpThread.Start();

        Thread udpThread2 = new Thread(new ThreadStart(ReceiveUdpPackets));
        udpThread2.Start();

        // Start the TCP listener thread
        //Thread tcpThread = new Thread(new ThreadStart(ListenForTcpConnections));
        //tcpThread.Start();
    }

    static void SendUdpPackets()
    {
        udpClient = new UdpClient();
        

        while (true)
        {
            // Send UDP packets
            byte[] cPacketData = { getu8RandomNumber(), getu8RandomNumber(), getu8RandomNumber(), getu8RandomNumber(), getu8RandomNumber(), getu8RandomNumber(), getu8RandomNumber(), getu8RandomNumber(), getu8RandomNumber() };
            byte[] packetData = Encoding.ASCII.GetBytes("Windows l");
            udpClient.Send(cPacketData, cPacketData.Length, new IPEndPoint(IPAddress.Parse("192.168.1.8"), 1234));
            //Console.WriteLine("Sent Windows   1");
            // udpClient.Send(packetData, packetData.Length, new IPEndPoint(IPAddress.Broadcast, 1234));
            Thread.Sleep(100);
        }
    }

    static void ListenForTcpConnections()
    {
        tcpListener = new TcpListener(IPAddress.Any, 1234);
        tcpListener.Start();

        while (true)
        {
            // Wait for a TCP client to connect
            TcpClient client = tcpListener.AcceptTcpClient();
            NetworkStream stream = client.GetStream();

            // Receive messages from the client
            byte[] buffer = new byte[1024];
            int bytesRead = stream.Read(buffer, 0, buffer.Length);
            string message = Encoding.ASCII.GetString(buffer, 0, bytesRead);
            Console.WriteLine("Received message from TCP client: " + message);

            // Send a response back to the client
            byte[] response = Encoding.ASCII.GetBytes("Message received!");
            stream.Write(response, 0, response.Length);

            client.Close();
        }
    }

    static void ReceiveUdpPackets()
    {
        // Set up a UDP socket to listen for packets on port 1234
        UdpClient udpClient = new UdpClient(12345);
        int oneOrtwo = 1;
        while (true)
        {
            // Receive a UDP packet and print its contents
            IPEndPoint endPoint = new IPEndPoint(IPAddress.Any, 0);
            byte[] data = udpClient.Receive(ref endPoint);
            //Console.WriteLine($"Received packet from {endPoint}: {BitConverter.ToString(data)}");
            if (oneOrtwo == 1)
            {
                oneOrtwo++;
                Console.Clear();
            }
            else
            {
                
                oneOrtwo = 1;
            }
            
            Console.WriteLine("ID:" + data[0].ToString() + " Data:" + data[1].ToString() + " " + " " + data[2].ToString() + " " + data[3].ToString() + " " + data[4].ToString() + " " + data[5].ToString() + " " + data[6].ToString() + " " + data[7].ToString() + " " + data[8].ToString());
        }
    }

    static byte getu8RandomNumber()
    {
        // Create a new Random object
        Random rand = new Random();

        // Generate a random integer between 1 and 100
        int randomInt = rand.Next(1, 120);
        
        return (byte)randomInt;
    }

    public class ipString
    {
        public string ipAdder = "192.168.1.";
        public string stIP()
        {
            Console.WriteLine("Enter IP end");
            string ii = ipAdder + Console.ReadLine();
            ipAdder = ii;
            return ii;
        }
    }

}


*/









/*
#include <iostream>
#include <thread>
#include <chrono>
#include <boost/asio.hpp>

using boost::asio::ip::udp;

// The data to be broadcasted
std::array<uint8_t, 1024> data;

// The UDP broadcast endpoint
udp::endpoint broadcastEndpoint(boost::asio::ip::address_v4::broadcast(), 1234);

// The UDP socket for sending data
udp::socket sendSocket(boost::asio::io_context& ioContext, const udp::endpoint& localEndpoint)
{
    udp::socket socket(ioContext);
    socket.open(udp::v4());
    socket.set_option(boost::asio::socket_base::broadcast(true));
    socket.bind(localEndpoint);
    return socket;
}

// The UDP socket for receiving data
udp::socket receiveSocket(boost::asio::io_context& ioContext, const udp::endpoint& localEndpoint)
{
    udp::socket socket(ioContext);
    socket.open(udp::v4());
    socket.bind(localEndpoint);
    return socket;
}

// The send loop thread function
void sendLoop(boost::asio::io_context& ioContext)
{
    udp::socket socket = sendSocket(ioContext, udp::endpoint(udp::v4(), 0));
    while (true)
    {
        socket.send_to(boost::asio::buffer(data), broadcastEndpoint);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

// The receive loop thread function
void receiveLoop(boost::asio::io_context& ioContext)
{
    udp::socket socket = receiveSocket(ioContext, udp::endpoint(udp::v4(), 1234));
    while (true)
    {
        std::array<uint8_t, 1024> buffer;
        udp::endpoint senderEndpoint;
        std::size_t length = socket.receive_from(boost::asio::buffer(buffer), senderEndpoint);
        std::cout << "Received " << length << " bytes from " << senderEndpoint << std::endl;
    }
}

int main()
{
    // Start the IO context and the send/receive loop threads
    boost::asio::io_context ioContext;
    std::thread sendThread(sendLoop, std::ref(ioContext));
    std::thread receiveThread(receiveLoop, std::ref(ioContext));

    // Command line loop to modify the data array
    while (true)
    {
        std::string command;
        std::cin >> command;
        if (command == "quit")
        {
            break;
        }
        else if (command == "set")
        {
            uint8_t value;
            std::size_t index;
            std::cin >> index >> value;
            if (index < data.size())
            {
                data[index] = value;
            }
            else
            {
                std::cerr << "Invalid index: " << index << std::endl;
            }
        }
        else if (command == "get")
        {
            std::size_t index;
            std::cin >> index;
            if (index < data.size())
            {
                std::cout << static_cast<int>(data[index]) << std::endl;
            }
            else
            {
                std::cerr << "Invalid index: " << index << std::endl;
            }
        }
        else
        {
            std::cerr << "Invalid command: " << command << std::endl;
        }
    }

    // Stop the IO context and

}
*/