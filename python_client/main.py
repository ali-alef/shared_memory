from utils import create_shared_memory, Bridge, init_subscriber_socket, init_push_socket
from typing import Tuple


push_socket = init_push_socket()
subscriber_socket = init_subscriber_socket()
shared_memory = create_shared_memory()
bridge = Bridge(shared_memory, subscriber_socket)


@bridge.js(service_name="python-service")
async def hello(name: str, age: int) -> Tuple[str, int]: ...


if __name__ == '__main__':
    n, a = hello("ali", 21)
    print(n, a)
