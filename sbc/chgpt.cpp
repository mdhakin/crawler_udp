#include <iostream>
#include <boost/asio.hpp>
#include <thread>
#include <mutex>
#include <vector>
#include <string.h>

using boost::asio::ip::udp;


std::mutex mutex;
std::vector<std::string> messages; // shared data structure

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
        udp::endpoint broadcast_endpoint(boost::asio::ip::make_address("255.255.255.255"), 1234);

        // create buffer to send data
        std::string message = "Hp  Pavilion\n";
        std::array<char, 13> send_buffer;
        std::copy(message.begin(), message.end(), send_buffer.begin());

        // send broadcast message in a loop
        while (true)
        {
            socket.send_to(boost::asio::buffer(send_buffer), broadcast_endpoint);
            std::this_thread::sleep_for(std::chrono::milliseconds(10)); // wait for 1 second before sending next message

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
        udp::socket receive_socket(io_context, udp::endpoint(udp::v4(), 1234));

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
    std::string name = "";
    std::string thisip = "";
    std::string targetip = "";
    int port_no = 0;
    int loop_speed = 100;
    
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
