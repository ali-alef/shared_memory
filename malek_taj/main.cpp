#include "settings.f"
#include "utils.f"
#include <zmq.hpp>


int main() {
    read_env();
    InitializeSharedObject();
    // Initialize ZeroMQ context
    zmq::context_t context(1);

    // Create a publisher socket
    zmq::socket_t publisher(context, ZMQ_PUB);

    // Bind the socket to the address
    publisher.bind("tcp://*:5555"); // You can change the address/port as needed

    // Publish some messages
    for (int i = 0; i < 10; ++i) {
        std::string message = "Message " + std::to_string(i);
        zmq::message_t zmq_message(message.size());
        memcpy(zmq_message.data(), message.data(), message.size());
        publisher.send(zmq_message);
        std::cout << "Published: " << message << std::endl;
        // Add some delay before publishing the next message
        // std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    return 0;
}
