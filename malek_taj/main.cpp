#include "settings.f"
#include "utils.f"
#include <zmq.hpp>
#include <thread>
#include "json.hpp"

using json = nlohmann::json;

const std::string REQUEST_OPERATION = "request";
const std::string RESPONSE_OPERATION = "response";

std::string read_request_from_shm(int shm) {
    while (true) {
        std::string data = read_from_shared_memory(shm);
        std::cout << "read message from shm -> " << data << std::endl;
        if(data.empty()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            continue;
        }

        json parsed_data = json::parse(data);
        std::string service_name = parsed_data["service_name"];

        return service_name;
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
    std::string publisher_route = "tcp://*:" + ENV_MAP["ZMQ_PORT"];
    publisher.bind(publisher_route);

    int shm = create_shared_memory();

    while (true) {
        std::string service_name = read_request_from_shm(shm);
        std::cout << "publishing message -> " << service_name << std::endl;
        publisher.send(zmq::buffer(service_name), zmq::send_flags::none);
    }

    return 0;
}
