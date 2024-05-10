#include <zmq.hpp>
#include <thread>
#include <mutex>
#include <set>

#include "json.hpp"
#include "utils.f"
#include "settings.f"


using json = nlohmann::json;

std::mutex SHARED_MEMORY_LOCK;

const std::string REQUEST_OPERATION = "request";
const std::string RESPONSE_OPERATION = "response";
const std::string OK = "ok";

void lock() {
    SHARED_MEMORY_LOCK.lock();
}

void unlock() {
    SHARED_MEMORY_LOCK.unlock();
}

void handle_shared_requests() {
    // Initialize ZeroMQ context
    zmq::context_t context(1);

    // Create a publisher socket
    zmq::socket_t publisher(context, ZMQ_PUB);

    // Bind the socket to the address
    std::string publisher_route = "tcp://*:" + ENV_MAP["ZMQ_PUBLISHER_PORT"];
    publisher.bind(publisher_route);
    std::cout << "Running publisher socket on: " << publisher_route << std::endl;

    int shm = create_shared_memory();
    TimedSet request_set;

    while (true) {
        lock();
        std::string data = read_from_shared_memory(shm);

        if(data.empty()) {
            unlock();
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }


        json parsed_data = json::parse(data);
        std::string service_name = parsed_data["service_name"];
        std::string uuid = parsed_data["uuid"];
        std::string operation = parsed_data["operation"];
        std::string message = operation + ":" + service_name + ":" + uuid;
        if (request_set.contains(message)) {
            unlock();
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            std::cout << "Removing expired data in set" << std::endl;
            request_set.cleanupExpiredItems();
            continue;
        }
        request_set.add(message, std::chrono::seconds(2));

        publisher.send(zmq::buffer(message), zmq::send_flags::none);
        unlock();
    }
}

void handle_shared_lock() {
    zmq::context_t context(1);
    zmq::socket_t socket(context, zmq::socket_type::rep);
    std::string response_route = "tcp://127.0.0.1:" + ENV_MAP["ZMQ_RESPONSE_PORT"];
    socket.bind(response_route);
    std::cout << "Running response socket on: " << response_route << std::endl;

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

        lock();
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
        unlock();
    }
}

int main() {
    read_env();
    initialize_shared_object();

    std::thread shared_lock_thread(handle_shared_lock);
    std::thread shared_request_thread(handle_shared_requests);

    shared_lock_thread.join();
    shared_request_thread.join();

    return 0;
}
