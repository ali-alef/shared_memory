import json

from methods import hello, invalid_function
from utils import create_shared_memory, read_from_shared_memory, write_to_shared_memory, init_request_socket, \
    SharedMemoryLock
from utils import init_subscriber_socket
from utils.validator import validate_request_dict

SERVICE_NAME = "python-service"

subscriber_socket = init_subscriber_socket()
req_socket = init_request_socket()
shared_lock = SharedMemoryLock(req_socket)
shared_memory = create_shared_memory()

function_mapper = {
    "hello": hello,
}

if __name__ == '__main__':
    while True:
        message = subscriber_socket.recv_string()
        operation, service_name, id = message.split(":")

        if operation != "request" or service_name != SERVICE_NAME:
            continue

        shared_lock.get_lock()
        data = json.loads(read_from_shared_memory(shared_memory))
        if not validate_request_dict(data):
            shared_lock.unlock()
            continue

        write_to_shared_memory(shared_memory, "")
        shared_lock.unlock()

        request_function = function_mapper.get(data.pop("function_name"), invalid_function)
        res = request_function(*data.pop("args", []))

        data["result"] = res
        data["operation"] = "response"

        shared_lock.get_lock()
        print(data)
        write_to_shared_memory(shared_memory, json.dumps(data))
        shared_lock.unlock()
