#include "settings.f"
#include "utils.f"
#include <zmq.hpp>
#include <thread>
#include "json.hpp"

using json = nlohmann::json;

const std::string REQUEST_OPERATION = "request";
const std::string RESPONSE_OPERATION = "response";
const std::string OK = "ok";


std::string read_request_from_shm(int shm) {
    while (true) {
        std::string data = read_from_shared_memory(shm);

        if(data.empty()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            continue;
        }
        std::cout << "data -> " << data << std::endl;
        json parsed_data = json::parse(data);
        std::string service_name = parsed_data["service_name"];
        std::string uuid = parsed_data["uuid"];
        std::string operation = parsed_data["operation"];

        return operation + ":" + service_name + ":" + uuid;
    }
}

int main() {
    read_env();
    initialize_shared_object();

    // Initialize ZeroMQ context
    zmq::context_t context(1);

    // Create a publisher socket
    zmq::socket_t publisher(context, ZMQ_PUB);

    // Bind the socket to the address
    std::string publisher_route = "tcp://*:" + ENV_MAP["ZMQ_PUBLISHER_PORT"];
    publisher.bind(publisher_route);


    // PAIR socket for receiving messages from a specific subscriber
    zmq::socket_t pull_socket(context, zmq::socket_type::pull);
    pull_socket.bind("tcp://*:5556"); // Bind to an address and port

    int shm = create_shared_memory();

    while (true) {
        std::string message = read_request_from_shm(shm);
        publisher.send(zmq::buffer(message), zmq::send_flags::none);
        std::cout << "publishing message -> " << message << std::endl;

        zmq::pollitem_t items[] = {{pull_socket, 0, ZMQ_POLLIN, 0}};
        zmq::poll(&items[0], 1, 1000); // Timeout: 1000 milliseconds

        if (items[0].revents & ZMQ_POLLIN) {
            // Received message
            zmq::message_t received_message;
            pull_socket.recv(&received_message, 0);

            // Convert the received message to a string
            std::string received_str(static_cast<char*>(received_message.data()), received_message.size());
        } else {
            // No message received within 1 second
            std::cout << "No message received within 1 second." << std::endl;
        }
    }

    return 0;
}
