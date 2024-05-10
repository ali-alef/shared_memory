#include <zmq.hpp>
#include <thread>
#include <mutex>

#include "json.hpp"
#include "utils.f"
#include "settings.f"


using json = nlohmann::json;

std::mutex SHARED_MEMORY_LOCK;

const std::string REQUEST_OPERATION = "request";
const std::string RESPONSE_OPERATION = "response";
const std::string OK = "ok";


std::string read_request_from_shm(int shm) {
    while (true) {
        SHARED_MEMORY_LOCK.lock();
        std::string data = read_from_shared_memory(shm);
        SHARED_MEMORY_LOCK.unlock();

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

void handle_shared_requests() {
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
}

void handle_shared_lock() {
    zmq::context_t context(1);
    zmq::socket_t socket(context, zmq::socket_type::rep);
    socket.bind("tcp://127.0.0.1:5557");

    while (true) {
        zmq::message_t lock_request;
        socket.recv(lock_request, zmq::recv_flags::none);
        std::string lock_request_str(static_cast<char*>(lock_request.data()), lock_request.size());
        std::vector<std::string> tokens = split_string(lock_request_str, ':');

        if(tokens[0] != "lock" && tokens[0] != "unlock") {
            zmq::message_t reply(3);
            memcpy(reply.data(), "418", 3);
            socket.send(reply, zmq::send_flags::none);
            continue;
        }

        if(tokens[0] == "unlock") {
            zmq::message_t reply(3);
            memcpy(reply.data(), "405", 3);
            socket.send(reply, zmq::send_flags::none);
            continue;
        }

        SHARED_MEMORY_LOCK.lock();
        zmq::message_t reply(3);
        memcpy(reply.data(), "200", 3);
        socket.send(reply, zmq::send_flags::none);

        while (true) {
            zmq::message_t unlock_request;
            socket.recv(unlock_request, zmq::recv_flags::none);
            std::string unlock_request_str(static_cast<char *>(unlock_request.data()), unlock_request.size());
            std::vector<std::string> unlock_tokens = split_string(unlock_request_str, ':');

            if(unlock_tokens[0] == "lock") {
                zmq::message_t error_reply(3);
                memcpy(error_reply.data(), "423", 3);
                socket.send(error_reply, zmq::send_flags::none);
                continue;
            } else if (unlock_tokens[0] != "unlock") {
                zmq::message_t error_reply(3);
                memcpy(error_reply.data(), "400", 3);
                socket.send(error_reply, zmq::send_flags::none);
                continue;
            }

            if(unlock_tokens[1] != tokens[1]) {
                zmq::message_t error_reply(3);
                memcpy(error_reply.data(), "403", 3);
                socket.send(error_reply, zmq::send_flags::none);
                continue;
            }

            zmq::message_t error_reply(3);
            memcpy(error_reply.data(), "200", 3);
            socket.send(error_reply, zmq::send_flags::none);
            break;
        }

        SHARED_MEMORY_LOCK.unlock();
    }
}

int main() {
    read_env();
    initialize_shared_object();

    std::thread t2(handle_shared_lock);
    t2.join();
    return 0;
}
