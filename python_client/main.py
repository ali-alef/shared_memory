from utils import (create_shared_memory, Bridge, init_subscriber_socket,
                   init_push_socket, init_request_socket, SharedMemoryLock)
from typing import Tuple


push_socket = init_push_socket()
sub_socket = init_subscriber_socket()
req_socket = init_request_socket()
shared_memory = create_shared_memory()
shared_lock = SharedMemoryLock(req_socket)

bridge = Bridge(shared_memory, sub_socket, shared_lock)


@bridge.decorate(service_name="python-service")
async def hello(name: str, age: int) -> Tuple[str, int]: ...


if __name__ == '__main__':
    response = hello("ali", 21)
    print(f'got response {response}')
