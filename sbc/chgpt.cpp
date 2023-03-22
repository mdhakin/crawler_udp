#include <iostream>
#include <boost/asio.hpp>
#include <thread>
#include <mutex>
#include <vector>
#include <string.h>
#include <cstdint>
#include <cstring>

using boost::asio::ip::udp;


std::mutex mutex;
std::vector<std::string> messages; // shared data structure

std::string name = "";
std::string thisip = "";
std::string targetip = "";
int port_no = 0;
int loop_speed = 100;


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

        int64_t lp = (int64_t)loop_speed;
        // send broadcast message in a loop
        while (true)
        {
            socket.send_to(boost::asio::buffer(send_buffer), broadcast_endpoint);
            socket.send_to(boost::asio::buffer(send_buffer2), broadcast_endpoint);
            std::this_thread::sleep_for(std::chrono::milliseconds(lp)); // wait for 1 second before sending next message

            // add message to shared data structure
            std::lock_guard<std::mutex> lock(mutex);
            messages.push_back(message);
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
        std::array<char, 12> receive_buffer;
        udp::endpoint sender_endpoint;

        // receive broadcast message in a loop
        while (true)
        {
            receive_socket.receive_from(boost::asio::buffer(receive_buffer), sender_endpoint);
            // print received message
            std::string str = std::string(receive_buffer.data(), receive_buffer.size());
            if(str.substr(0, receive_buffer.size()) != "Hp  Pavilion" || str.substr(0, receive_buffer.size()) != "Hp  Pavilion")
            {
                std::cout << "Received message: " << std::string(receive_buffer.data(), receive_buffer.size()) << std::endl;
                std::cout << "Size: " << receive_buffer.size() << std::endl;
            }else
            {
                
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

    // wait for threads to finish
    send_thread.join();
    receive_thread.join();

    // print all messages in shared data structure
    std::lock_guard<std::mutex> lock(mutex);
    std::cout << "All messages:" << std::endl;
    for (const auto& message : messages)
    {
        std::cout << message << std::endl;
    }

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