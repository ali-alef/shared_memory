import json

from methods import hello, invalid_function
from utils import create_shared_memory, read_from_shared_memory, write_to_shared_memory
from utils import init_subscriber_socket
from utils.zmq import init_push_socket

SERVICE_NAME = "python-service"

subscriber_socket = init_subscriber_socket()
push_socket = init_push_socket()
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

        data = json.loads(read_from_shared_memory(shared_memory))
        write_to_shared_memory(shared_memory, "")
        push_socket.send_string("ok")

        res = function_mapper[data.pop("function_name", "")](*data.pop("args", None))

        data["response"] = res
        data["operation"] = "response"
        write_to_shared_memory(shared_memory, json.dumps(data))

