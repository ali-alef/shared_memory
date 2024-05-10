#include <zmq.hpp>
#include <thread>
#include <mutex>

#include "json.hpp"
#include "utils.f"
#include "settings.f"

#define NUM_MESSAGES 10

using json = nlohmann::json;

std::mutex SHARED_MEMORY_LOCK;

const std::string REQUEST_OPERATION = "request";
const std::string RESPONSE_OPERATION = "response";
const std::string LOCK_REQUEST = "lock";
const std::string UNLOCK_REQUEST = "unlock";

const void *const OK = "200";

const void *const BAD_REQUEST = "400";
const void *const FORBIDDEN = "403";
const void *const METHOD_NOT_ALLOWED = "405";
const void *const IM_A_TEA_POT = "418";
const void *const LOCKED = "423";

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
    std::cout << "Publisher Running on port: " << ENV_MAP["ZMQ_PUBLISHER_PORT"] << std::endl;

    int shm = create_shared_memory();
    TimedSet request_set;

    int index = 0;
    int times_loop_ran = 0;
    while (true) {
        times_loop_ran++;
        if(times_loop_ran == 20) {
            std::cout << "Removing expired data in set" << std::endl;
            request_set.cleanupExpiredItems();
            times_loop_ran = 0;
        }

        lock();
        std::string data = read_from_shared_memory(shm, index);
        index++;

        if(data.empty()) {
            unlock();
            index %= NUM_MESSAGES;
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }


        json parsed_data = json::parse(data);
        std::string service_name = parsed_data["service_name"];
        std::string uuid = parsed_data["uuid"];
        std::string operation = parsed_data["operation"];
        std::stringstream message;
        message << index << ":" << operation << ":" << service_name << ":" << uuid;
        if (request_set.contains(message.str())) {
            unlock();
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }
        request_set.add(message.str(), std::chrono::seconds(2));

        publisher.send(zmq::buffer(message.str()), zmq::send_flags::none);
        unlock();
    }
}

int first_empty_spot(int shm) {
    for(int i = 0; i < NUM_MESSAGES; i++) {
        std::string data = read_from_shared_memory(shm, i);
        if(data.empty()) {
            return i;
        }
    }
    return -1;
}

void handle_shared_lock() {
    zmq::context_t context(1);
    zmq::socket_t socket(context, zmq::socket_type::rep);
    std::string response_route = "tcp://127.0.0.1:" + ENV_MAP["ZMQ_RESPONSE_PORT"];
    socket.bind(response_route);
    std::cout << "Response socket Running  on port: " << ENV_MAP["ZMQ_RESPONSE_PORT"] << std::endl;

    int shm = create_shared_memory();

    while (true) {
        zmq::message_t lock_request;
        socket.recv(lock_request, zmq::recv_flags::none);
        std::string lock_request_str(static_cast<char*>(lock_request.data()), lock_request.size());
        std::vector<std::string> tokens = split_string(lock_request_str, ':');

        if(tokens[0] != LOCK_REQUEST && tokens[0] != UNLOCK_REQUEST) {
            zmq::message_t reply(3);
            memcpy(reply.data(), IM_A_TEA_POT, 3);
            socket.send(reply, zmq::send_flags::none);
            continue;
        }

        if(tokens[0] == UNLOCK_REQUEST) {
            zmq::message_t reply(3);
            memcpy(reply.data(), METHOD_NOT_ALLOWED, 3);
            socket.send(reply, zmq::send_flags::none);
            continue;
        }

        lock();
        int index = first_empty_spot(shm);
        if (index == -1) {
            zmq::message_t reply(3);
            memcpy(reply.data(), METHOD_NOT_ALLOWED, 3);
            socket.send(reply, zmq::send_flags::none);
            continue;
        }

        zmq::message_t reply(5);

        char indexString[2];
        char result[5];
        sprintf(indexString, "%d", index);
        strcpy(result, (const char *)OK);
        strcat(result, ":");
        strcat(result, indexString);
        memcpy(reply.data(), result, 5);
        socket.send(reply, zmq::send_flags::none);

        while (true) {
            zmq::message_t unlock_request;
            socket.recv(unlock_request, zmq::recv_flags::none);
            std::string unlock_request_str(static_cast<char *>(unlock_request.data()), unlock_request.size());
            std::vector<std::string> unlock_tokens = split_string(unlock_request_str, ':');

            if(unlock_tokens[0] == LOCK_REQUEST) {
                zmq::message_t error_reply(3);
                memcpy(error_reply.data(), LOCKED, 3);
                socket.send(error_reply, zmq::send_flags::none);
                continue;
            } else if (unlock_tokens[0] != UNLOCK_REQUEST) {
                zmq::message_t error_reply(3);
                memcpy(error_reply.data(), METHOD_NOT_ALLOWED, 3);
                socket.send(error_reply, zmq::send_flags::none);
                continue;
            }

            if(unlock_tokens[1] != tokens[1]) {
                zmq::message_t error_reply(3);
                memcpy(error_reply.data(), FORBIDDEN, 3);
                socket.send(error_reply, zmq::send_flags::none);
                continue;
            }

            zmq::message_t error_reply(3);
            memcpy(error_reply.data(), OK, 3);
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
