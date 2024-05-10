import json

from methods import hello, invalid_function
from utils import create_shared_memory, read_from_shared_memory, write_to_shared_memory, init_request_socket, \
    SharedMemoryLock
from utils import init_subscriber_socket

SERVICE_NAME = "python-service"

subscriber_socket = init_subscriber_socket()
req_socket = init_request_socket()
shared_lock = SharedMemoryLock(req_socket)
shared_memory = create_shared_memory()

function_mapper = {
    "hello": hello,
    None: invalid_function,
}

if __name__ == '__main__':
    while True:
        message = subscriber_socket.recv_string()
        operation, service_name, id = message.split(":")

        if operation != "request" or service_name != SERVICE_NAME:
            print("none of my business!")
            continue

        shared_lock.get_lock()
        data = json.loads(read_from_shared_memory(shared_memory))
        write_to_shared_memory(shared_memory, "")
        shared_lock.unlock()

        res = function_mapper[data.pop("function_name", "")](*data.pop("args", None))

        data["response"] = res
        data["operation"] = "response"

        shared_lock.get_lock()
        write_to_shared_memory(shared_memory, json.dumps(data))
        shared_lock.unlock()
